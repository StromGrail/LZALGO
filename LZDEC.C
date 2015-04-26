

# include <stdio.h>
# include <io.h>

//Global variable declarations
int buf[50],bc;//buffer and counter
unsigned int txtbuf[4100],txtbc;
unsigned int ch[20],cc;
char fname[30];
int fcount;
FILE *p,*q;
int count,pos;
unsigned int qt;
int buft[20],buftv[20];
int end=0;
int i,j,ct;
long int redlen=0,filelen=0;

//Function prototypr declarations
void decompress();
int isalphas();
unsigned int getcharfromp();
void fillbuffer();
void getphrasefromp();
void intro();

void decompress()
{
	int i;
	int h;
	long int th=0;
	printf("\n\nStarting operation...");
	printf("\nBuffering....");
	fillbuffer();//Filles the buf to 20 or more
	printf("done");
	printf("\nDecompression starts...\n");
	while((end==0)||(end==1 && bc!=0))
	{
		if(redlen%10000==0)
			printf("*");
		if(buf[0]==0)
		{
			h=getcharfromp();
			fputc(h,q);th++;
		}
		else if(buf[0]==1)
		{
			getphrasefromp();
			//Now count pos and ch have values of the phrase
			for(i=0;i<count;i++)
			{
				fputc(ch[i],q);th++;
			}
			//Add ch count times to the buffer also
		}
	}
	printf("\nDecompression over");
	printf("\nPerforming finalization tasks...");
	fcloseall();
	printf("done");
	printf("\nOperation finished");
	printf("\n\nCreated output file %s",fname);
	printf("\nThank you");
	return;
}
unsigned int getcharfromp()
{
	unsigned int qt;
	fillbuffer();
	if(bc<9)
	 printf("Error..");
	qt=(1*buf[8])+(2*buf[7])+(4*buf[6])+(8*buf[5])+(16*buf[4])+(32*buf[3])+(64*buf[2])+(128*buf[1]);
	for(i=0;i<(bc-9);i++)
	{
		buf[i]=buf[i+9];
	}
	bc-=9;
	if(txtbc==4096)
	{
		for(i=0;i<4095;i++)
		{
			txtbuf[i]=txtbuf[i+1];
		}
		txtbuf[4095]=qt;
	}
	else
	{
		txtbuf[txtbc]=qt;
		txtbc++;
	}
	fillbuffer();
	return qt;
}
void fillbuffer()
{
	int t,tb;
	int tst=0;
	ct=-1;
	while(bc<20 && end==0)
	{
		if(redlen==(filelen-2))
		{
			t=fgetc(p);
			ct=fgetc(p);
			//end=1;
			tst=1;
		}
		else
		{
			t=fgetc(p);redlen++;
		}
		tb=t;
		if(tb==EOF)
		{
			end=1;
			break;
		}
		for(i=0;i<8;i++)
		{
			buft[i]=tb%2;
			tb/=2;
		}
		for(i=0;i<8;i++)
		{
			buftv[7-i]=buft[i];
		}
		for(i=0;i<8;i++)
		{
			buf[bc+i]=buftv[i];
		}
		if(tst==1)
		{
			bc+=ct;
			break;
			//end=1;
		}
		else
		{
			bc+=8;
		}
	}
	if(tst==1)end=1;
	return;
}
void getphrasefromp()
{
	int i;
	fillbuffer();
	pos=(1*buf[12])+(2*buf[11])+(4*buf[10])+(8*buf[9])+(16*buf[8])+(32*buf[7])+(64*buf[6])+(128*buf[5])+(256*buf[4])+(512*buf[3])+(1024*buf[2])+(2048*buf[1]);
	count=(1*buf[16])+(2*buf[15])+(4*buf[14])+(8*buf[13]);
	for(i=0;i<count;i++)
	{
		ch[i]=txtbuf[pos+i];
	}
	for(i=0;i<bc;i++)
	{
		buf[i]=buf[17+i];
	}
	bc-=17;
	i=0;
	while(txtbc<4096 && i<count)
	{
		txtbuf[txtbc]=ch[i];
		txtbc++;
		i++;
	}
	if(i!=count)
	{
		for(j=0;j<(4096-(count-i));j++)
		{
			txtbuf[j]=txtbuf[j+(count-i)];
		}
		txtbc-=(count-i);
		for(j=0;j<(count-i);j++)
		{
			txtbuf[txtbc+j]=ch[i+j];
		}
		txtbc+=(count-i);
		if(txtbc!=4096){printf("\nError");}
	}
	return;
}
int isalphas(char outfile[])
{
	int i=0;
	while(outfile[i]!='\0')
	{
		if((int)outfile[i]>128)
			return 1;
		i++;
	}
	return 0;
}

main()
{
	char infile[100],outfile[100];
	intro();
	txtbc=0;
	bc=0;
	printf("\nEnter input filename:");scanf("%s",infile);
	p=fopen(infile,"rb");
	if(p==NULL)
	{
		printf("\nCould not open input file..Aborting operation");
		return -1;
	}
	fcount=fgetc(p);
	fread(outfile,fcount,1,p);
	if(isalphas(outfile)!=0)
	{
		printf("\n\nThis does not appear to be a file encoded by Encoder");
		printf("\nHence decompression cannot proceed\nOperation aborted");
		fcloseall();
		return -1;
	}
	printf("\nThis file has in it an encoded file named %s",outfile);
	printf("\nYou may choose to create the output file of this name");
	printf("\nor specify another name.");
	printf("\nSpecify another name(y/any other key):");
	if(tolower(getch())=='y')
	{
		printf("\n\nEnter name for output file:");
		scanf("%s",outfile);
	}
	strcpy(fname,outfile);
	q=fopen(outfile,"wb");
	if(q==NULL)
	{
		fcloseall();
		printf("\nCould not open output file..Aborting operation");
		return -1;
	}
	filelen=filelength(fileno(p));redlen=fcount+1;
	decompress();
	return 0;
}
void intro()
{
	clrscr();
	printf("Elzed Decoder 1.0");
	printf("\nDemonstration of data compression by the LZ algorithm");

	return;
}

