


# include <stdio.h>

/*Global variable declarations*/
int buf[50],buft[17],buftv[17],bc,bct;
char filename[100],encfile[100];
unsigned int txtbuf[4100],lkbuf[20];
int txtbc,lkbc;
int count,pos;
int i,j;
FILE *p,*q;
int check,countold,posold;
int ch;
/*Global variable for determining the end of file */
int end=0;
long int infilelen=0,outfilelen=0;

/*Functio prototype declarations*/
int fillphrase2buff();
unsigned char oddchar();
void refreshbuffer();
void intro();
void add2buff(unsigned int);
void check4phrase();
void compress();
int lkfill(int);
void exhaustlkbuff();
char *getname(char *);

/*Function definitions*/
int fillphrase2buff()
{
	int countbak=count;
	buf[bc]=1;
	bc++;
	for(i=0;i<12;i++)
	{
		buft[i]=pos%2;pos/=2;
	}
	for(i=0;i<12;i++)
	{
		buftv[11-i]=buft[i];
	}
	for(i=0;i<12;i++)
	{
		buf[bc+i]=buftv[i];
	}
	bc+=12;
	for(i=0;i<4;i++)
	{
		buft[i]=countbak%2;countbak/=2;
	}
	for(i=0;i<4;i++)
	{
		buftv[3-i]=buft[i];
	}
	for(i=0;i<4;i++)
	{
		buf[bc+i]=buftv[i];
	}
	bc+=4;
	refreshbuffer();
	return 0;
}
unsigned char oddchar()
{
	int i;
	for(i=bc;i<8;i++)
	{
		buf[i]=0;
	}
	return ((1*buf[7])+(2*buf[6])+(4*buf[5])+(8*buf[4])+(16*buf[3])+(32*buf[2])+(64*buf[1])+(128*buf[0]));
}
void refreshbuffer()
{
	unsigned char r;
	int i;
	while(bc>=8)
	{
		r=(1*buf[7])+(2*buf[6])+(4*buf[5])+(8*buf[4])+(16*buf[3])+(32*buf[2])+(64*buf[1])+(128*buf[0]);
		if(fputc(r,q)!=(unsigned)r || r<0 || r>255)printf("\nError");
		outfilelen++;
		for(i=8;i<bc;i++)
		{
			buf[i-8]=buf[i];
		}
		bc-=8;
	}
}
void add2buff(unsigned int c)
{
	int i;
	bct=-1;
	while(c!=0)
	{
		buft[++bct]=(c%2);
		c/=2;
	}
	for(i=(bct+1);i<8;i++)
	{
		buft[i]=0;
	}
	for(i=(0);i<8;i++)
	{
		buftv[7-i]=buft[i];
	}
	for(i=0;i<8;i++)
	{
		buf[bc+i]=buftv[i];
	}
	bc+=8;
}
//int check,countold,posold;
void check4phrase()
{
	check=0;
	countold=0;
	posold=0;
	count=0;
	for(i=0;i<txtbc;i++)
	{
		if(check==0 && lkbuf[0]==txtbuf[i])
		{
			check=1;
			count=1;
			pos=i;
		}
		else if(check==1 && lkbuf[count]==txtbuf[i])
		{
			count++;
			if(count==(lkbc-1))/*modification for debugging*/
				break;
		}
		else if(check==1 && lkbuf[count]!=txtbuf[i])
		{
			check=0;
			if(count==(lkbc-1))/*Modification for debugging*/
				break;
			if(count>countold)
			{
				countold=count;
				posold=pos;
			}
		}
	}
	if(countold>count)
	{
		count=countold;
		pos=posold;
	}
	return;
	/*Now count and phrase store the values of the phrases*/
}

void compress()
{
	int ch,i;
	char fname[30];
	txtbc=0;
	lkbc=0;
	bc=0;
	printf("\n\nStarting operation...");
	strcpy(fname,getname(filename));
	fputc(strlen(fname)+1,q);/*Filling filelname length indicator*/
	fwrite(fname,strlen(fname)+1,1,q);
	outfilelen=strlen(fname)+2;
	for(i=0;i<2;i++)
	{
		buf[bc]=0;bc++;
		ch=fgetc(p);
		add2buff(ch);
                refreshbuffer();
		txtbuf[i]=(unsigned int)ch;
	}
	txtbc=2;infilelen=2;
	refreshbuffer();
	printf("\nBuffering....");
	for(i=0;i<16;i++)
	{
		lkbuf[i]=(unsigned int)fgetc(p);
	}
	lkbc=16;infilelen=18;
	printf("done");
	printf("\nCompression starts..\n");
	while(1)
	{
		if((infilelen%10000)==0)
			printf("*");
		check4phrase();
		if(count==0 || count==1)
		{
			buf[bc]=0;bc++;
			add2buff(lkbuf[0]);
			refreshbuffer();
			if(lkfill(1)==-1)
				break;
		}
		else
		{
			fillphrase2buff();
			if(lkfill(count)==-1)
				break;
		}
	}
	printf("\nCompression over.\nPerforming finalization tasks....");
	exhaustlkbuff();
	refreshbuffer();
	fputc(oddchar(),q);
	fputc(bc,q);
	printf("done\nOperation finished");
	printf("\n\nInput filelength :%ld",infilelen+1);
	printf("\nOutput filelength:%ld",outfilelen+2);
	printf("\nCompressed to %.1f%% of original size",(float)(outfilelen*100/infilelen));
	printf("\n\nThank you.");
	fcloseall();
	return;
}

/*These depend on the global variable end*/
int c[18],cc;
int lkfill(int a)
{
	int i;
	for(i=0;i<a;i++)
	{
		c[i]=lkbuf[i];
	}
	cc=a;
	//Now stored the desired no of characters in c*/
	for(i=0;i<(lkbc-a);i++)
	{
		lkbuf[i]=lkbuf[i+a];
	}
	//Shifted the desired no of bytes to the left*/
	lkbc-=a;
	//Now the hard part to fill into txtbuf
	i=0;
	while(txtbc<4096 && i<a)
	{
		txtbuf[txtbc]=c[i];
		txtbc++;
		i++;
	}
	if(i!=a)
	{
		for(j=0;j<(4096-(a-i));j++)
		{
			txtbuf[j]=txtbuf[j+(a-i)];
		}
		txtbc-=(a-i);
		for(j=0;j<(a-i);j++)
		{
			txtbuf[txtbc+j]=c[i+j];
		}
		txtbc+=(a-i);
	}
	//Now to fill intolkbuf
	if(end!=1)
	{
		for(j=lkbc;j<16;j++)
		{
			ch=fgetc(p);infilelen++;
			if(ch==EOF)
			{
				lkbc=j;
				end=1;
				return -1;
			}
			lkbuf[j]=(unsigned int)ch;
		}
		lkbc=16;
	}
	return 0;
}
/*This function was a trial by me to encode the last 16bytes of the
input file too.But it was in vain cos i could not debug it to
exclude the last some extra bytes that it wrote to the output file
during decompression.Hence i used a brute force method by not encoding
the last 16bytes of the original input file
void exhaustlkbuff()
{
	while(lkbc>0)
	{
		check4phrase();
		if(count==1 || count==0)
		{
			buf[bc]=0;
			bc++;
			add2buff(lkbuf[0]);
			refreshbuffer();
			lkfill(1);
		}
		else
		{
			fillphrase2buff();
			refreshbuffer();
			lkfill(count);
		}
	}
	return;
} */
/*The new version of exhaustlkbuff that i use*/
void exhaustlkbuff()
{
	int i;
	for(i=0;i<lkbc;i++)
	{
		buf[bc]=0;bc++;
		add2buff(lkbuf[i]);
		refreshbuffer();
	}
	return;
}

main()
{
	intro();
	printf("\nEnter input file:");scanf("%s",filename);
	printf("\nEnter output file:");scanf("%s",encfile);
	p=fopen(filename,"rb");
	q=fopen(encfile,"wb");
	if(p==NULL || q==NULL)
	{
		printf("\nCould not open one or more files");
		fcloseall();
		return 0;
	}
	compress();
	return 0;
}
char *getname(char *filepath)
{
	char drive[4],dir[67],file[15],ext[5];
	fnsplit(filepath,drive,dir,file,ext);
	strcat(file,ext);
	return file;
}
void intro()
{
	clrscr();
	printf("Elzed Encoder 1.0");
	printf("\nDemonstration of data compression by the LZ algorithm");

	return;
}
