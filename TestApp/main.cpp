#include <rtgopro.h>

#include <Windows.h>

#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>

void* image = NULL;

void* respimage = NULL;
bool has = false;
int size = 0;

int w = 1920;
int h = 1080;

unsigned int t0;
unsigned int t1;

unsigned int GetTime() 
{
	struct _timeb timebuffer;

	_ftime(&timebuffer);

	return (unsigned int)timebuffer.time * 1000 + timebuffer.millitm;
}

void response(void*output, void*input)
{
	memcpy(respimage, output, size);
	has = true;	
}

void message(const char*message)
{
	printf("%s\n", message);
}

void loadImage(const char * filename)
{	
	unsigned char* datBuff[2] = {NULL, NULL};

	BITMAPFILEHEADER* bmpHeader = NULL; 
	BITMAPINFOHEADER* bmpInfo   = NULL; 

	FILE *fp = fopen(filename, "rb");

	datBuff[0] = new unsigned char[sizeof(BITMAPFILEHEADER)];
	datBuff[1] = new unsigned char[sizeof(BITMAPINFOHEADER)];

	fread((char*)datBuff[0], sizeof(BITMAPFILEHEADER), 1, fp);
	fread((char*)datBuff[1], sizeof(BITMAPINFOHEADER), 1, fp);

	bmpHeader = (BITMAPFILEHEADER*) datBuff[0];
	bmpInfo   = (BITMAPINFOHEADER*) datBuff[1];

	if(bmpHeader->bfType != 0x4D42)
	{
		return;
	}

	image = new unsigned char[bmpInfo->biSizeImage];

	fseek(fp, bmpHeader->bfOffBits, SEEK_SET);
	fread((char*)image, bmpInfo->biSizeImage, 1, fp);

	w = bmpInfo->biWidth;
	h = bmpInfo->biHeight;

	delete [] datBuff[0];
	delete [] datBuff[1];

	fclose(fp);
}

static int getuint(unsigned short *uint, FILE *input)
{
  int got;
  unsigned char temp[2];
  unsigned short tempuint;

  got = (int) fread(&temp, 1, 2, input);
  if (got != 2) return 0;

  tempuint = ((unsigned short)(temp[1])<<8) | ((unsigned short)(temp[0]));

  *uint = tempuint;

  return 1;
}

static int putuint(unsigned short uint, FILE *output)
{
  int put;
  unsigned char temp[2];

  temp[0] = uint & 0xff;
  temp[1] = (uint >> 8) & 0xff;

  put = (int) fwrite(&temp, 1, 2, output);
  if (put != 2) return 0;
 
  return 1;
}

void saveImage(const char *filename)
{
   unsigned char imageType=2;
   unsigned char bitDepth=32;

   FILE         *filePtr;
   long        imageIdx; 
   unsigned char colorSwap;

   unsigned char byteZero=0;
   short int     shortZero=0;


   filePtr = fopen(filename, "wb");

   putc(byteZero,filePtr); 
   putc(byteZero,filePtr);     
   putc(imageType,filePtr);    
   putuint(shortZero,filePtr); 
   putuint(shortZero,filePtr);  
   putc(byteZero,filePtr);      
   putuint(shortZero,filePtr);  
   putuint(shortZero,filePtr);  
   putuint((unsigned int)rtgGetResultWidth(), filePtr);
   putuint((unsigned int)rtgGetResultHeight(), filePtr);
   putc(bitDepth,filePtr);  
   putc(byteZero, filePtr); 

   unsigned char *buf = (unsigned char *)respimage;

   for (imageIdx = 0; imageIdx < size ; imageIdx += 4) 
   {
      colorSwap = buf[imageIdx];
      buf[imageIdx] = buf[imageIdx + 2];
      buf[imageIdx + 2] = colorSwap;
   }

   fwrite(respimage, sizeof(unsigned char), size, filePtr); 

   fclose(filePtr);
}

int main(int argc, char*argv)
{
	loadImage("IMGv.bmp");

	rtgSetMessageCallback(message);

	if( rtgInitialize("stitch180.rtx", 64, 64, w, h, false, true, false, rt90Degrees, false, response) != 0)
	{
		return -1;
	}

	size = rtgGetResultWidth() * rtgGetResultHeight() * 4;

	respimage = new unsigned char [size];

	rtgStartEngine();		

	char Temp[200] = "";

	while(true)
	{
		rtgPushImage(image);

		Sleep(1000);

		saveImage("img_s.tga");

		printf("> ");
		scanf("%s", Temp);

		if(strcmp(Temp, "exit") == 0)
			break;		
	}	

	rtgStopEngine();

	delete [] respimage;
	delete [] image;

	return 0;
}