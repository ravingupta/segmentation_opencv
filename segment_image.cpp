#include "opencv\cv.h"
#include "opencv\highgui.h"
#include <stdio.h>

const double PI = 3.14159265;

double c1 = 0;
double c2 = 0;

double epsilon = 1.0;


double lambda1 = 1.0;
double lambda2 = 3.0;

int counter1 = 0;

double num1 = 0;
double den1 = 0;
double num2 = 0;
double den2 = 0;
double H_phi;

double x,y;
int i,j;
double tempc1 = 0, tempc2 = 0;

void ChanVeseSegmentation(void);
void GetRegionAverages(void);

int k, step, channels, gray, height, width;
double phi[1200][1200];
uchar *data1;
IplImage *img0 = NULL;

int main() {
  img0 = cvLoadImage("test.jpg", -1);

  data1 = (uchar *) img0->imageData;
  height = img0->height;
  width = img0->width;
  step = (img0->widthStep);
  channels = (img0->nChannels);

  cvNamedWindow("image0", 1);

  for (i = 0; i < img0->height; i++) {
    for (j = 0; j < img0->width; j++) {
      gray = (int) (0.114 * data1[i * step + j * channels + 0]) +
      (0.587 * data1[i * step + j * channels + 1]) + 
      (0.299 * data1[i * step + j * channels + 2]);
      data1[i * step + j * channels + 0] = gray;
      data1[i * step + j * channels + 1] = gray;
      data1[i * step + j * channels + 2] = gray;
    }
  }

  for (i = 0; i < img0->height; i++) {
    for (j = 0; j < img0->width; j++) {
      x = (i - (img0->height) / 2.0);
      y = (j - (img0->width) / 2.0);
      // phi[i][j]=900.0/(900.0+x*x+y*y)-0.5;
      phi[i][j] = -sqrt((x - 20) * (x - 20) + (y - 20) * (y - 20)) + 30;
    }
  }

  ChanVeseSegmentation();

  for (i = 0; i < img0->height; i++) {
    for (j = 0; j < img0->width; j++) {
      // if((phi[i][j])>0)
      if (phi[i][j] < 0 && phi[i + 1][j] > 0 || phi[i][j] > 0 && phi[i][j - 1] < 0 || phi[i][j] < 0 && phi[i + 1][j - 1] > 0 || phi[i][j] > 0 && phi[i][j + 1] < 0) {
        data1[i * step + j * channels + 0] = 0;
        data1[i * step + j * channels + 1] = 0;
        data1[i * step + j * channels + 2] = 255;
      }
    }
  }
  cvShowImage("image0", img0);
  cvWaitKey(0);
  cvReleaseImage(&img0);
  return (0);
} // main end

void ChanVeseSegmentation() {
  do {
    tempc1 = c1;
    tempc2 = c2;

    GetRegionAverages();
    counter1++;

    for (i = 1; i < height; ++i) {
      for (j = 1; j < width; ++j) {
        // GetChanVeseCoefficients (); Not Required!
        phi[i][j] = (double)-((lambda1 * pow(data1[i * step + j * channels + 1] - c1, 2) - lambda2 * pow(data1[i * step + j * channels + 1] - c2, 2)));
        // printf("\nphi=%f",phi[i][j]);
      }
    }
  // printf("\ntemp = %f %f>",tempc1,tempc2);
  } while (((tempc1 - c1) != 0.0) && ((tempc2 - c2) != 0.0)); // while(counter1<13);
}

void GetRegionAverages() {
  num1 = 0;
  num2 = 0;
  den1 = 0;
  den2 = 0;

  for (i = 0; i < img0->height; ++i) {
    for (j = 0; j < img0->width; ++j) {
      H_phi = 0.5 * (1 + (2 / PI) * atan(phi[i][j] / epsilon));
      num1 += (data1[i * step + j * channels + 1]) * H_phi;
      den1 += H_phi;
      num2 += (data1[i * step + j * channels + 1]) * (1 - H_phi);
      den2 += 1 - H_phi;
    }
  }
  c1 = num1 / den1;
  c2 = num2 / den2;

  // c1=abs(c1);
  // c2=abs(c2);

  printf("\n c1c2=%f  %f>", c1, c2);
}
