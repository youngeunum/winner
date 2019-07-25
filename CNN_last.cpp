#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
using namespace std;
using namespace cv;

int conv_output(int input_size_r, int input_size_c, int filter_size, int padding, int stride, int ***pp, double **pf, int output_size_r, int output_size_c, double ***po, int channel);
int max_pooling(int input_size_r, int input_size_c, int filter_size, int stride, int ***pi, int output_size_pr, int output_size_pc, int ***pmax, int channel);
int avg_pooling(int input_size_r, int input_size_c, int filter_size, int stride, int ***pi, int output_size_pr, int output_size_pc, int ***pavg, int channel);
double Activation(int input_size_r, int input_size_c, int ***pi, double ***act, int channel);

int main()
{
    int i;
    int j;
    int n=0;
    int channel=3;
    int fi;
    int fj;
    int ni;
    int nj;
    Mat image;
    image=imread("test.jpg", IMREAD_COLOR);    
    
    if(image.empty())
    {
        cout<<"Could not open or find the image"<<endl;
        return -1;
    }

    //행렬 크기 입력
    int input_size_r=image.rows;
    int input_size_c=image.cols;

    //필터 사이즈 입력 받기
    int filter_size;
    printf("filter의 크기를 입력하시오:");
    scanf("%d", &filter_size);

    //padding 입력 받기
    int padding;
    printf("padding을 입력하시오:");
    scanf("%d", &padding);

    //stride 입력 받기
    int stride;
    printf("stride를 입력하시오:");
    scanf("%d", &stride);

    //행렬 동적 메모리 할당
    int ***pi;
    pi=(int***)malloc(channel * sizeof(int**));
    for(i=0; i<channel; i++){
        *(pi+i)=(int**)malloc(input_size_r * sizeof(int*));
        for(j=0; j<input_size_r; j++){
            *(*(pi+i)+j)=(int*)malloc(input_size_c * sizeof(int));
        }
    }

    //padding 메모리 할당 
    int ***pp;
    pp=(int***)malloc(channel * sizeof(int**));
    for(i=0; i<channel; i++){
        *(pp+i)=(int**)malloc((input_size_r+padding*2) * sizeof(int*));
        for(j=0; j<(input_size_r+padding*2); j++){
            *(*(pp+i)+j)=(int*)malloc((input_size_c+padding*2) * sizeof(int));
        }
    }

    //padding 초기화
    for(n=0; n<channel; n++){
        for(i= 0; i<(input_size_r+padding*2); i++){
            for(j=0; j<(input_size_c+padding*2); j++){
                pp[n][i][j]=0;
            }
        }
    }

    //필터 동적 메모리 할당
    double **pf;
    pf=(double**)malloc(filter_size * sizeof(double*));
    for(i=0; i<filter_size; i++) {
        *(pf+i)=(double*)malloc(filter_size * sizeof(double));
    }

    //이미지 입력 받기
    for(n=0; n<channel; n++){
        for(i=0; i<input_size_r; i++){
            for(j=0; j<input_size_c; j++){
                pi[n][i][j]=image.at<cv::Vec3b>(i,j)[n];            
            }
        }
    }

    //padding에 입력 받기
    for(n=0; n<channel; n++){
        for(i=padding; i<(input_size_r+padding); i++){
            for(j=padding; j<(input_size_c+padding); j++){
                pp[n][i][j]=pi[n][i-padding][j-padding];
            }
        }
    }

    //필터 입력 받기
    for(i=0; i<filter_size; i++){
        for(j=0; j<filter_size; j++){
        printf("필터의 성분을 입력하시오. : ");
        scanf("%lf", &pf[i][j]);
        }
        printf("\n");
    }
          
    //필터 출력 하기
    printf("필터는 : \n");
    for(i= 0; i<filter_size; i++){
        for(j=0; j<filter_size; j++){
            printf("%lf   ", pf[i][j]);
        }
        printf("\n");
    }

    //conv 계산을 위한 출력 사이즈 결정
    printf("\n\n");    
    int output_size_c; 
    int output_size_r;
    output_size_c=(input_size_c-filter_size+ 2*padding)/stride + 1;
    output_size_r=(input_size_r-filter_size+ 2*padding)/stride + 1;

    //conv 계산 출력 메모리 할당
    double ***po;
    po=(double***)malloc(channel * sizeof(double**));
    for(i=0; i<channel; i++){
        *(po+i)=(double **)malloc(output_size_r * sizeof(double*));
        for(j=0; j<output_size_r; j++){
            *(*(po+i)+j)=(double*)malloc(output_size_c * sizeof(double));
        }
    }

    //max, avg 계산 출력 사이즈 결정
    int output_size_pr;
    int output_size_pc;
    output_size_pr=(input_size_r-filter_size)/stride + 1;    
    output_size_pc=(input_size_c-filter_size)/stride + 1; 
    
    
    //max 계산 출력 메모리 할당
    int ***pmax;
    pmax=(int***)malloc(channel * sizeof(int**));
    for(i=0; i<channel; i++){
        *(pmax+i)=(int **)malloc(output_size_pr * sizeof(int*));
        for(j=0; j<output_size_pr; j++){
            *(*(pmax+i)+j)=(int*)malloc(output_size_pc * sizeof(int));
        }
    }
    
    //avg 계산 출력 메모리 할당
    int ***pavg;
    pavg=(int***)malloc(channel * sizeof(int**));
    for(i=0; i<channel; i++){
        *(pavg+i)=(int **)malloc(output_size_pr * sizeof(int*));
        for(j=0; j<output_size_pr; j++){
            *(*(pavg+i)+j)=(int*)malloc(output_size_pc * sizeof(int));
        }
    }
    
    //Activation 메모리 할당
    double ***act;
    act=(double***)malloc(channel * sizeof(double**));
    for(i=0; i<channel; i++){
        *(act+i)=(double**)malloc(input_size_r * sizeof(double*));
        for(j=0; j<input_size_r; j++){
            *(*(act+i)+j)=(double*)malloc(input_size_c * sizeof(double));
        }
    }
    
    //conv 연산 시간 계산
    clock_t begin=clock();
    
    //conv계산    
    conv_output(input_size_r, input_size_c, filter_size, padding, stride, pp, pf, output_size_r, output_size_c, po, channel);

    //conv 연산 시간 계산 종료
    clock_t end=clock();
    double elapsed_secs=double (end-begin)/CLOCKS_PER_SEC;

    //conv 연산 시간 출력
    printf("conv 연산 시간 : %lf 초\n", elapsed_secs);

    //max, avg 계산
    //max_pooling(input_size_r, input_size_c, filter_size, stride, pi, output_size_pr, output_size_pc, pmax, channel);
    //avg_pooling(input_size_r, input_size_c, filter_size, stride, pi, output_size_pr, output_size_pc, pavg, channel);

    //sat
    for(n=0; n<channel; n++){
        for(i=0; i<output_size_r; i++){
            for(j=0; j<output_size_c; j++){
                if(po[n][i][j]>255){
                    po[n][i][j]=255;
                }
                else if(po[n][i][j]<0){
                    po[n][i][j]=0;
                }
            }
        }
    }
 
    //이미지 출력
    Mat new_image(output_size_r, output_size_c, CV_8UC3, Scalar(0,1,2));

    for(n=0; n<channel; n++){
        for(i=0; i<new_image.rows; i++){
            for(j=0; j<new_image.cols; j++){
                new_image.at<cv::Vec3b>(i,j)[n]=po[n][i][j];
            }
        }
    }

    imwrite("new_image.jpg", new_image);

    //동적 메모리 free    
    for(i=0; i<channel; i++){
        for(j=0; j<input_size_r; j++){
            free(*(*(pi+i)+j));
        }
        free(*(pi+i));
    }
        
    for(i=0; i<channel; i++){
        for(j=0; j<(input_size_r+padding*2); j++){
            free(*(*(pp+i)+j));
        }
        free(*(pp+i));
    }

    for(i=0; i<filter_size; i++) {
        free(*(pf+i));
    }    
    
    for(i=0; i<channel; i++) {
        for(j=0; j<output_size_r; j++){
            free(*(*(po+i)+j));
        }
        free(*(po+i));
    }
    
    for(i=0; i<channel; i++) {
        for(j=0; j<output_size_pr; j++){
            free(*(*(pmax+i)+j));
        }
        free(*(pmax+i));
    }

    for(i=0; i<channel; i++) {
        for(j=0; j<output_size_pr; j++){
            free(*(*(pavg+i)+j));
        }
        free(*(pavg+i));
    }
    
    for(i=0; i<channel; i++){
        for(j=0; j<input_size_r; j++){
            free(*(*(act+i)+j));
        }
        free(*(act+i));
    }

    free(pi);
    free(pp);
    free(pf);
    free(po);
    free(pmax);
    free(pavg);
    free(act);

    return 0;
}


int conv_output(int input_size_r, int input_size_c, int filter_size, int padding, int stride, int ***pp, double **pf, int output_size_r, int output_size_c, double ***po, int channel)
{
    int i;
    int j;
    int n;
    int fi;
    int fj;
    int ni;
    int nj;

    //conv 출력
    for(n=0; n<channel; n++){
        for(ni=0; ni<output_size_r; ni++){
            for(nj=0; nj<output_size_c; nj++){
                for(fi=0; fi<filter_size; fi++){
                    for(fj=0; fj<filter_size; fj++){
                        po[n][ni][nj] += pp[n][fi+(ni*stride)][fj+(nj*stride)] * pf[fi][fj];
                        
                        /* sat
                        if(po[n][ni][nj]>255){
                            po[n][ni][nj]=255;
                        }
                        else if(po[n][ni][nj]<0){
                            po[n][ni][nj]=0;
                        }
                        */
                    }
                }
            }
        }
    }

    return 0;
}


int max_pooling(int input_size_r, int input_size_c, int filter_size, int stride, int ***pi, int output_size_pr, int output_size_pc, int ***pmax, int channel)
{
    int i=0;
    int j=0;
    int n;
    int ni;
    int nj;
    int fi;
    int fj;

    for(n=0; n<channel; n++){
        for(ni=0; ni<output_size_pr; ni++){
            for(nj=0; nj<output_size_pc; nj++){
                for(fi=0; fi<filter_size; fi++){
                    for(fj=0; fj<filter_size; fj++){
                        if(pmax[n][ni][nj]<pi[n][fi+(ni*stride)][fj+(nj*stride)]){
                            pmax[n][ni][nj]=pi[n][fi+(ni*stride)][fj+(nj*stride)];
                        }
                    }
                }
            }
        }
    } 

    return 0;
}


int avg_pooling(int input_size_r, int input_size_c, int filter_size, int stride, int ***pi, int output_size_pr, int output_size_pc, int ***pavg, int channel)
{
    int i=0;
    int j=0;
    int n;
    int ni;
    int nj;
    int fi;
    int fj;

    for(n=0; n<channel; n++){
        for(ni=0; ni<output_size_pr; ni++){
            for(nj=0; nj<output_size_pc; nj++){
                for(fi=0; fi<filter_size; fi++){
                    for(fj=0; fj<filter_size; fj++){
                        pavg[n][ni][nj]=pavg[n][ni][nj]+pi[n][fi+(ni*stride)][fj+(nj*stride)];
                    }
                }        
                if( (pavg[n][ni][nj]%(filter_size*filter_size)) / (filter_size*filter_size) >=5){
                    pavg[n][ni][nj] = pavg[n][ni][nj]/(filter_size*filter_size)+1; 
                }

                else{
                    pavg[n][ni][nj] = pavg[n][ni][nj]/(filter_size*filter_size); 
                }
            }
        }
    } 

    return 0;
}

double Activation(int input_size_r, int input_size_c, int ***pi, double ***act, int channel)
{
    int i;
    int j;
    int n;

    for(n=0; n<channel; n++){
        for(i=0; i<input_size_r; i++){
            for(j=0; j<input_size_c; j++){  
                 act[n][i][j]=1/(1+exp(-pi[n][i][j]));
            }
        }
    }

    return  0.0;
}