//
// RK_Sabre.cpp
//
// Compilation : 
// >> g++ `pkg-config opencv --cflags` RK_Sabre.cpp -o RK_Projet `pkg-config opencv --libs`
//
// pour trouver le /dev/video? de la WebCam faire :
// sudo rm /dev/video*
// brancher la WebCam
// >> debian@SabreLite:~/Desktop$ ls /dev/video*
// ==>/dev/video4
//
// adapter la ligne :
//     capture = cvCreateCameraCapture( 4 );
//

/*
 * Code written by Vinz (GeckoGeek.fr) 14/03/2010
 * http://www.geckogeek.fr/lire-le-flux-dune-webcam-camera-video-avec-opencv.html
 * And modified by Rostom Kachouri; 16/02/2016
 * IN5E23
 */
 
#include <stdio.h>
#include <stdlib.h>
// OpenCV header

#include "opencv/highgui.h"
#include "opencv/cv.h"


int main()
{ 
    // Touche clavier pour quitter
    char ESC_keyboard;
	// intialisation de la variable ESC_keyboard
	ESC_keyboard = cvWaitKey(5);

    // Images
     IplImage *Image_IN;
     IplImage *Image_OUT;
	 
    // Capture vidéo
    CvCapture *capture;
	
	// variables images
	int height,width,step,channels;		// Taille de l'image
	uchar *Data_in;						// pointeur des données Image_IN
	uchar *Data_out;					// pointeur des données Image_OUT
	int i,j,k;							// indices
 
    // Ouvrir le flux vidéo
    //capture = cvCreateFileCapture("/path/to/your/video/test.avi"); // chemin pour un fichier
    //capture = cvCreateFileCapture("/dev/v4l/by-id/usb-046d_HD_Pro_Webcam_C920_*");
    //capture = cvCreateFileCapture("/dev/v4l/by-path/platform-ci_hdrc.1-usb-0\:1.1\:1.0-video-index0");
    capture = cvCreateFileCapture("/dev/v4l/by-path/platform-ci_hdrc.1-usb-0\:1.1\:1.0-video-index0");
    //capture = cvCreateCameraCapture( CV_CAP_ANY );
    capture = cvCreateCameraCapture( 0 );
    
    // Vérifier si l'ouverture du flux est ok
    if (!capture) {
 
       printf("Ouverture du flux vidéo impossible !\n");
       return 1;
 
    }
 
    // Définition des fenêtres
	cvNamedWindow("Image_IN_Window", CV_WINDOW_AUTOSIZE);   // Image_IN
	cvNamedWindow("Image_OUT_Window", CV_WINDOW_AUTOSIZE); 	// Image_OUT
	
	// Positionnement des fenêtres
    cvMoveWindow("Image_IN_Window", 0,0);
    cvMoveWindow("Image_OUT_Window", 0,700);
 
	// Première acquisition
   Image_IN = cvQueryFrame(capture); 
	
   // Création de l'image de sortie
   Image_OUT = cvCreateImage(cvSize(Image_IN->width,Image_IN->height),  IPL_DEPTH_8U, 1); 	
   int step_gray = Image_OUT->widthStep/sizeof(uchar);
   
    // Boucle tant que l'utilisateur n'appuie pas sur la touche q (ou Q)
    while(ESC_keyboard != 'q' && ESC_keyboard != 'Q') {
 
		// On récupère une Image_IN
		Image_IN = cvQueryFrame(capture);
		// Dimension
		height    = Image_IN->height;
		width     = Image_IN->width;
		// distance entre les deux premiers pixels de lignes successives
		step      = Image_IN->widthStep;
		channels  = Image_IN->nChannels;
		// initialisation des pointeurs de donnée
		Data_in      = (uchar *)Image_IN->imageData; 
		Data_out = (uchar *) Image_OUT->imageData;
	  
		//conversion RGB en niveau de gris
		for(i=0;i<height;i++) 
			for(j=0;j<width;j++)
				{ Data_out[i*step_gray+j]=0.114*Data_in[i*step+j*channels+0]+ 0.1*Data_in[i*step+j*channels+1] + 0.499*Data_in[i*step+j*channels+2];}

		// On affiche l'Image_IN dans une fenêtre
		cvShowImage( "Image_IN_Window", Image_IN);
		// On affiche l'Image_OUT dans une deuxième fenêtre
		cvShowImage( "Image_OUT_Window", Image_OUT);
 
		// On attend 5ms
		ESC_keyboard = cvWaitKey(5);
 
    }
 
    // Fermeture de l'acquisition Vidéo
	cvReleaseCapture(&capture);
   
	// Fermeture des fenêtres d'affichage
	cvDestroyWindow("Image_IN_Window");
    cvDestroyWindow("Image_OUT_Window");
 
    return 0;
 
}
