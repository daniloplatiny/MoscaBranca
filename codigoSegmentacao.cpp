#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int, char** argv)
{
    // Carrega a imagem - com argumentos do Bash
    Mat src = imread(argv[1]);
	// Checagem se a imagem foi carregada
    if (!src.data)
        return -1;
    // Mostra a imagem original
    imshow("Imagem geral", src);

 //Embaçamento da imagem para diminuir detalhes
	Mat dst1; 
	for (int i=1; i<51; i=i+2){ 
      //smooth the image in the "src" and save it to "dst"
      blur(src, dst1, Size(i,i));
}
      //show the blurred image with the text
      imshow( "Smoothing by avaraging", dst1 );
src = dst1;
   
   // Aqui definimos uma máscara correspondente a derivada segunda para o filtro Laplaciano
   //Estudar: Converte em algo "mais profundo" do que CV_8U (???) porque o kernel tem alguns valores negativos então o Laplaciano terá valores negativos que serão truncados ao usarmos unsigned int   
    Mat kernel = (Mat_<float>(3,3) <<
            1,  1, 1,
            1, -8, 1,
            1,  1, 1); 
    Mat imgLaplacian;
    Mat sharp = src; // copia da imagem original
	
	//CV_8U
	//CV_32F
	//CV_8UC3
	
    //ler referência
    //filter2D(sharp, imgLaplacian, CV_32F, kernel);
    //src.convertTo(sharp, CV_32F);
    //Mat imgResult = sharp - imgLaplacian;

    // convert back to 8bits gray scale
    //imgResult.convertTo(imgResult, CV_8UC3);
    //imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
    //imshow( "Laplace Filtered Image", imgLaplacian );
    //imshow( "New Sharped Image", imgResult );
    //src = imgResult; // copy back
    // Create binary image from source image
    Mat bw;
//ler referência
    cvtColor(src, bw, CV_BGR2GRAY);
    threshold(bw, bw, 40, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
Mat LaplacianoBinario;

filter2D(bw, LaplacianoBinario, CV_32F, kernel);
bw.convertTo(bw, CV_32F);
Mat ret = bw - LaplacianoBinario;
ret.convertTo(ret, CV_8UC3);
bw.convertTo(bw,CV_8UC3);
LaplacianoBinario.convertTo(LaplacianoBinario, CV_8UC3);
    //imshow( "Laplace Filtered Image", LaplacianoBinario );
	//imshow("Binario - Lapaciano",ret);
    //imshow("Binary Image", bw);
//src = ret;


    // Perform the distance transform algorithm
    Mat dist;
//ler muito
    distanceTransform(bw, dist, CV_DIST_L2, 3);
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0, 1., NORM_MINMAX);
    imshow("Distance Transform Image", dist);
    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    threshold(dist, dist, .59, 1., CV_THRESH_BINARY);
    // Dilate a bit the dist image
    Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
//um pouco de morfologia pra que??
    dilate(dist, dist, kernel1);
    imshow("Picos", dist);
    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    // Find total markers
    vector<vector<Point> > contours;
    findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    // Create the marker image for the watershed algorithm
    Mat markers = Mat::zeros(dist.size(), CV_32SC1);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++)
        drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), -1);
    // Draw the background marker
    circle(markers, Point(5,5), 3, CV_RGB(255,255,255), -1);
    imshow("Markers", markers*10000);


    // Perform the watershed algorithm
//ret.convertTo(ret, CV_8UC1);
	
Mat water = Mat::zeros(markers.size(), src.type());
printf("O Tipo de src é %c. \n", src.type());
    watershed(src, markers);
    Mat mark = Mat::zeros(markers.size(), CV_8UC1);
    markers.convertTo(mark, CV_8UC1);
    bitwise_not(mark, mark);
    imshow("Markers_v2", mark); // uncomment this if you want to see how the mark
                                  // image looks like at that point
    // Generate random colors
    vector<Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++)
    {
        int b = theRNG().uniform(0, 255);
        int g = theRNG().uniform(0, 255);
        int r = theRNG().uniform(0, 255);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
    // Create the result image
    Mat dst = Mat::zeros(markers.size(), CV_8UC3);
    // Fill labeled objects with random colors
int qtdSegmentos = 0;
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i,j);
            if (index > 0 && index <= static_cast<int>(contours.size())){
                dst.at<Vec3b>(i,j) = colors[index-1];
		
		}
            else
                dst.at<Vec3b>(i,j) = Vec3b(0,0,0);
        }
    }
	qtdSegmentos = colors.size();
    // Visualize the final image
    printf("Temos %d Bolinhas. \n",qtdSegmentos); 
    imshow("Final Result", dst);

    waitKey(0);
    return 0;
}
