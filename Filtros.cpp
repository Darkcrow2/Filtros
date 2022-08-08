#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
    Mat imagen;
    imagen = imread("lena.jpg", 1);

    uchar Azul;
    uchar Verde;
    uchar Rojo;

    RNG numero;

    cv::Vec3b pixel;

    int filas = imagen.rows;
    int columnas = imagen.cols;
    int sumaTotal = 0;

    float sumaKernel = 0;
    float KernelGaussiano = 0;
    float kernel[5][5];
    float kernelNormalizado[5][5];
    float sigma = 2.0;

    Mat gris(filas, columnas, CV_8UC1);
    Mat imagenRuidoGaussiano(filas, columnas, CV_8UC1);
    Mat imagenBordesGaussiano(filas + 4, columnas + 4, CV_8UC1);
    Mat imagenSinRuidoGaussiano(filas, columnas, CV_8UC1);
    Mat imagenBordes(filas + 2, columnas + 2, CV_8UC1);
    Mat imagenMedia(filas, columnas, CV_8UC1);
    Mat imagenPasaAltas(filas, columnas, CV_8UC1);
    Mat imagenRealce(filas, columnas, CV_8UC1);

    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            pixel = imagen.at<Vec3b>(i, j);

            Azul = pixel[0];
            Verde = pixel[1];
            Rojo = pixel[2];

            gris.at<uchar>(i, j) = (Azul + Verde + Rojo) / 3;
            imagenRuidoGaussiano.at<uchar>(i, j) = gris.at<uchar>(i, j) * (numero.uniform(0.f, 1.f));
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

             /////////////////////////////////   FILTRO DE MEDIA     //////////////////////////////////

    for (int i = 0; i < filas + 2; i++)
    {
        for (int j = 0; j < columnas + 2; j++)
        {
            if ((i == 0) || (j == 0))
            {
                imagenBordes.at<uchar>(i, j) = 0;
            }
            else if ((i == filas + 1) || (j == columnas + 1))
            {
                imagenBordes.at<uchar>(i, j) = 0;
            }
            else
            {
                imagenBordes.at<uchar>(i, j) = gris.at<uchar>(i - 1, j - 1);
            }
        }
    }

    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            for (int p = 0; p < 3; p++)
            {
                for (int q = 0; q < 3; q++)
                {
                    sumaTotal = sumaTotal + imagenBordes.at<uchar>(p + i, q + j);
                }
            }

            imagenMedia.at<uchar>(i, j) = sumaTotal / 9;

            sumaTotal = 0;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

             ////////////////////////////////   FILTRO PASA ALTOS ////////////////////////////////////

    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            imagenPasaAltas.at<uchar>(i, j) = gris.at<uchar>(i, j) - imagenMedia.at<uchar>(i, j);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

            ////////////////////////////////   FILTRO DE REALCE ////////////////////////////////////

    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            imagenRealce.at<uchar>(i, j) = 2*(gris.at<uchar>(i, j)) - imagenMedia.at<uchar>(i, j);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

            ////////////////////////////////   FILTRO GAUSSIANO ////////////////////////////////////

                            //////////////////// KERNEL  //////////////////
    for (int a = 0; a < 5; a++)
    {
        for (int b = 0; b < 5; b++)
        {
            kernel[a][b] = ( 1 / (2 * CV_PI * sigma * sigma) ) *
                ( exp ( - ( ( (a * a) + (b * b) ) / (2 * sigma * sigma) ) ) );
            sumaKernel = sumaKernel + kernel[a][b];
        }

    }

    for (int a = 0; a < 5; a++)
    {
        for (int b = 0; b < 5; b++)
        {
            kernelNormalizado[a][b] = (kernel[a][b]) / (sumaKernel);
        }
    }

    for (int i = 0; i < filas + 4; i++)
    {
        for (int j = 0; j < columnas + 4; j++)
        {
            if ((i < 2) || (j < 2))
            {
                imagenBordesGaussiano.at<uchar>(i, j) = 0;
            }
            else if ((i > filas + 1) || (j > columnas + 1))
            {
                imagenBordesGaussiano.at<uchar>(i, j) = 0;
            }
            else
            {
                imagenBordesGaussiano.at<uchar>(i, j) = imagenRuidoGaussiano.at<uchar>(i - 2, j - 2);
            }
        }
    }

    //////////////////// APLICANDO KERNEL PARA EL FILTRO  //////////////////

    for (int w = 0; w < filas; w++)
    {
        for (int x = 0; x < columnas; x++)
        {
            for (int y = 0; y < 5; y++)
            {
                for (int z = 0; z < 5; z++)
                {
                    KernelGaussiano = KernelGaussiano + 
                        (imagenBordesGaussiano.at<uchar>(w + y, x + z)*kernelNormalizado[y][z]);
                }
            }
            imagenSinRuidoGaussiano.at<uchar>(w, x) = KernelGaussiano;
            KernelGaussiano = 0;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //imshow("Imagen gris", gris);
    //imshow("Imagen gris con filtro pasa altas", imagenPasaAltas);
    //imshow("Imagen gris con filtro de realce", imagenRealce);
    imshow("Imagen gris con filtro gaussiano", imagenSinRuidoGaussiano);

    waitKey();
    return 0;
}