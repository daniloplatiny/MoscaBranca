#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>


int main(int argc, char *argv[])
{
  IplImage* img = 0; 
  int height,width,step,channels;
  uchar *data;
  int i,j,k;

  if(argc<2){
    printf("Uso: NomeDoExecutavel  <NomeDaImagen.formato>\n\7");
    exit(0);
  }

  // load an image  
  img=cvLoadImage(argv[1]);
  if(!img){
    printf("Não foi possível carregar a imagem: %s\n",argv[1]);
    exit(0);
  }

  // get the image data
  height    = img->height;
  width     = img->width;
  step      = img->widthStep;
  channels  = img->nChannels;
  data      = (uchar *)img->imageData;
  printf("Processando uma imagem %dx%d com  %d canais \n",height,width,channels); 

  // Criar uma janela
  cvNamedWindow("Imagem de Saida", CV_WINDOW_AUTOSIZE);
  cvMoveWindow("Imagem de Saida", 100, 100);

  // Inverte a imagem
  for(i=0;i<height;i++) for(j=0;j<width;j++) for(k=0;k<channels;k++)
    data[i*step+j*channels+k]=255-data[i*step+j*channels+k]; //Num entendi muito bem direito

  // Mostrar a Imagem
  cvShowImage("Imagem de Saida", img );

  // wait for a key
  cvWaitKey(0);
	
  // release the image
  cvReleaseImage(&img );
  return 0;
}
