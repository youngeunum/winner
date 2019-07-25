#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int convcal(int image_rows,int image_cols, double filter_rows, double filter_cols, int padding, int stride, int ***pad, double ***filt, int output_rows, int output_cols, double ***c, int cha);
int max_pooling(int image_rows,int image_cols, double filter_rows,double filter_cols, int stride, int ***a, int output_rows, int output_cols, int ***max, int cha);
//double getActivation(int image_rows,int image_cols, int ***a, double ***at, int cha);

int main()
{
    int i,j,fi,wj,ni,nj,padding,stride;
    int n=0;
    int cha=0;
    int ***a;
    int ***pad;
    double ***filt;
    double filter_rows;
    double filter_cols;
    double ***c;
    int ***max;
    double ***at;
    int image_rows;
    int image_cols;
    int output_rows;
    int output_cols;
    int outmax_rows;
    int outmax_cols;
    Mat image;
    image=imread("test.jpg", IMREAD_COLOR);    
    
    if(image.empty())
    {
        cout<<"Could not open or find the image"<<endl;
        return -1;
    }

        cout<<"channel 입력:"<<endl;
        scanf("%d", &cha);
        cout<<"filter_rows 값입력:"<<endl;
		scanf("%lf", &filter_rows);
        cout<<"filter_cols 값입력:"<<endl;
		scanf("%lf", &filter_cols);
		cout<< "padding 입력:"<<endl;
		scanf("%d", &padding);
		cout<< "stride 입력:"<<endl;
		scanf("%d", &stride);

    //////////////////행렬, padding////////////////////////////////////// ///////////////////////////////////////////
    a=(int***)malloc(cha * sizeof(int**));
    for(i=0; i<cha; i++){
        *(a+i)=(int**)malloc(image.rows * sizeof(int*));
        for(j=0; j<image.rows; j++){
            *(*(a+i)+j)=(int*)malloc(image.cols * sizeof(int));
        }
    }

    //padding  할당 
    
    pad=(int***)malloc(cha * sizeof(int**));
    for(i=0; i<cha; i++){
        *(pad+i)=(int**)malloc((image.rows+padding*2) * sizeof(int*));
        for(j=0; j<(image.rows+padding*2); j++){
            *(*(pad+i)+j)=(int*)malloc((image.cols+padding*2) * sizeof(int));
        }
    }

    //padding 초기화
    for(n=0; n<cha; n++){
        for(i= 0; i<(image.rows+padding*2); i++){
            for(j=0; j<(image.cols+padding*2); j++){
                pad[n][i][j]=0;
            }
        }
    }
    //이미지 입력
    for(n=0; n<cha; n++){
        for(i=0; i<image.rows; i++){
            for(j=0; j<image.cols; j++){
                a[n][i][j]=image.at<cv::Vec3b>(i,j)[n];            
            }
        }
    }
    //padding에 입력
    for(n=0; n<cha; n++){
        for(i=padding; i<(image.rows+padding); i++){
            for(j=padding; j<(image.cols+padding); j++){
                pad[n][i][j]=a[n][i-padding][j-padding];
            }
        }
    }

    
    ///////////////////////////////////////필터///////////////////////////////////////////////////////////////////////
    //필터 할당
       
        filt=(double***)malloc(cha * sizeof(double**));
        for(i=0; i<cha; i++) {
            *(filt+i)=(double**)malloc(filter_rows * sizeof(double*));
            for(j=0; j<filter_rows; j++){
                *(*(filt+i)+j)=(double*)malloc((filter_cols)*sizeof(double));
            }
        }

    //필터 입력
    for(n=0; n<cha; n++){
        for(i=0; i<filter_rows; i++){
            for(j=0; j<filter_cols; j++){
                      printf("%d행 %d열 입력 : ",i+1,j+1);
                        scanf("%lf", &filt[n][i][j]);
  
            }
        }
        printf("\n");
    }
          
    //필터 출력
    printf("필터는 : \n");
    for(n=0; n<cha; n++){
    for(i= 0; i<filter_rows; i++){
        for(j=0; j<filter_cols; j++){
            printf("%lf   ", filt[n][i][j]);
        }
        printf("\n");
    }
    }
/////////////////////////////////////////////////컨볼,맥스,엑트 동적 할당////////////////////////////////////////////////
    //conv 할당
    output_rows=(image.rows-filter_rows+ 2*padding)/stride + 1;
    output_cols=(image.cols-filter_cols+ 2*padding)/stride + 1;
    c=(double***)malloc(cha * sizeof(double**));
    for(i=0; i<cha; i++){
        *(c+i)=(double **)malloc(output_rows * sizeof(double*));
        for(j=0; j<output_rows; j++){
            *(*(c+i)+j)=(double*)malloc(output_cols * sizeof(double));
        }
    }
    for(n=0; n<cha; n++){
        for(i=0; i<output_rows; i++){
            for(j=0; j<output_cols; j++){
                c[n][i][j]=0;           
             }
        }
    }
    
    //max 할당
    outmax_rows=(image.rows-filter_rows)/stride + 1; 
    outmax_cols=(image.cols-filter_cols)/stride + 1; 
    max=(int***)malloc(cha * sizeof(int**));
    for(i=0; i<cha; i++){
        *(max+i)=(int **)malloc(outmax_rows * sizeof(int*));
        for(j=0; j<outmax_rows; j++){
            *(*(max+i)+j)=(int*)malloc(outmax_cols * sizeof(int));
        }
    }
    
    for(n=0; n<cha; n++){
        for(i=0; i<outmax_rows; i++){
            for(j=0; j<outmax_cols; j++){
                max[n][i][j]=0;           
             }
        }
    }

        //getActivation
    
    /*at=(double***)malloc(cha * sizeof(double**));
    for(i=0; i<cha; i++){
        *(at+i)=(double**)malloc(image.rows * sizeof(double*));
        for(j=0; j<image.rows; j++){
            *(*(at+i)+j)=(double*)malloc(image.cols * sizeof(double));
        }
    }*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //conv 시간
    clock_t begin=clock();
     
    convcal(image.rows,image.cols, filter_rows,filter_cols, padding, stride, pad, filt, output_rows,output_cols, c, cha);

    clock_t end=clock();

    //conv 시간 출력
    double elapsed_secs=double (end-begin)/CLOCKS_PER_SEC;
    printf("conv 연산 시간 : %lf 초\n", elapsed_secs);
    
    

    max_pooling(image.rows,image.cols, filter_rows, filter_cols, stride, a, outmax_rows, outmax_cols, max, cha);
   // getActivation(image.rows, image.cols,a, at, cha); printf("%d %d %d %d %d\n", image.rows,image.cols,stride,outmax_rows,outmax_cols);

    
    
    //이미지 convolution
    Mat n_image(output_rows, output_cols, image.type());

    for(n=0; n<cha; n++){
        for(i=0; i<output_rows; i++){
            for(j=0; j<output_cols; j++){
                if(c[n][i][j]<0){
                    c[n][i][j]=0;
                }
                else if(c[n][i][j]>255){
                    c[n][i][j]=255;
                }
                else c[n][i][j]=c[n][i][j];
                n_image.at<cv::Vec3b>(i,j)[n]=c[n][i][j];
            }
        }
    }           
    imwrite("n_image.jpg", n_image);


    //이미지 max
    /*Mat n_image(outmax_rows, outmax_cols, image.type());

    for(n=0; n<cha; n++){
        for(i=0; i<outmax_rows; i++){
            for(j=0; j<outmax_cols; j++){
                if(max[n][i][j]<0){
                    max[n][i][j]=0;
                }
                else if(max[n][i][j]>255){
                    max[n][i][j]=255;
                }
                else max[n][i][j]=c[n][i][j];
                n_image.at<cv::Vec3b>(i,j)[n]=max[n][i][j];
            }
        }
    }           
   

    imwrite("n_image.jpg", n_image);*/

    //////////////////////////////////////////free//////////////////////////////////////////////////////////////////   
    for(i=0; i<cha; i++){
        for(j=0; j<image.rows; j++){
            free(*(*(a+i)+j));
        }
        free(*(a+i));
    }
        
    for(i=0; i<cha; i++){
        for(j=0; j<(image.rows+padding*2); j++){
            free(*(*(pad+i)+j));
        }
        free(*(pad+i));
    }

    for(i=0; i<cha; i++) {
        for(j=0; j<filter_rows; j++){
        free(*(*(filt+i)+j));
    }    
        free(*(filt+i));
    }
    for(i=0; i<cha; i++) {
        for(j=0; j<output_rows; j++){
            free(*(*(c+i)+j));
        }
        free(*(c+i));
    }

    for(i=0; i<cha; i++) {
        for(j=0; j<outmax_rows; j++){
            free(*(*(max+i)+j));
        }
        free(*(max+i));
    }

    /*for(i=0; i<cha; i++){
        for(j=0; j<image.rows; j++){
            free(*(*(at+i)+j));
        }
        free(*(at+i));
    }*/

    free(a);
    free(pad);
    free(filt);
    free(c);
    free(max);
    //free(at);

    return 0;
}


int convcal(int image_rows,int image_cols, double filter_rows,double filter_cols, int padding, int stride, int ***pad, double ***filt, int output_rows, int output_cols, double ***c, int cha)
{
    int i,j,n,fi,wj,ni,nj;

    for(n=0; n<cha; n++){
        for(ni=0; ni<output_rows; ni++){
            for(nj=0; nj<output_cols; nj++){
                for(fi=0; fi<filter_rows; fi++){
                    for(wj=0; wj<filter_cols; wj++){
                        c[n][ni][nj] += pad[n][fi+(ni*stride)][wj+(nj*stride)] * filt[n][fi][wj];
                        
                    }
                }
            }
        }
    }
    
    
}


int max_pooling(int image_rows, int image_cols, double filter_rows, double filter_cols, int stride, int ***a, int outmax_rows, int outmax_cols, int ***max, int cha)
{


    int n,fi,wj,ni,nj;

    for(n=0; n<cha; n++){
        for(ni=0; ni<outmax_rows; ni++){
            for(nj=0; nj<outmax_cols; nj++){
                for(fi=0; fi<filter_rows; fi++){
                    for(wj=0; wj<filter_cols; wj++){
                        if(max[n][ni][nj]<a[n][fi+(ni*stride)][wj+(nj*stride)]){
                            max[n][ni][nj]=a[n][fi+(ni*stride)][wj+(nj*stride)];
                            
                            
                        }
                       
                    }
                } 
            }
        } 
    }  

}

/*double getActivation(int image_rows, int image_cols, int ***a, double ***at, int cha)
{
    int i,j,n;
    
    for(n=0; n<cha; n++){
        for(i=0; i<image_rows; i++){
            for(j=0; j<image_cols; j++){  
                 at[n][i][j]=1/(1+exp(-a[n][i][j]));
            }
        }
    }

}*/