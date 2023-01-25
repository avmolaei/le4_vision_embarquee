//--------------------------BIBLIOTHEQUES-------------------------
#include <stdio.h>
#include <stdlib.h>
#include "highgui.h"
#include "cv.h"
//----------------------------------------------------------------

//----------------------DOCUMENTATION OPENCV----------------------
        //https://docs.opencv.org/3.4/d2/df8/group__core__c.html
//----------------------------------------------------------------


//---------------------------PROTOTYPES---------------------------
void rkSabreSobel(IplImage* src, IplImage* dst, int kernel_size);
void rkSabreMedian(IplImage* src, IplImage* dst, int kernel_size);
//----------------------------------------------------------------

/**
 * main(); fonction principale
 * utilise les fonctions rkSabreMedian et rkSabreSobel
 * pour filtrer une image en niveaux de gris, et en 
 * en détecter les contours
 * @return Code de retour 0 si l'application s'est temrinée correctement
*/
int main() {
    //acquisition du flux vidéo	
    CvCapture* capture = cvCreateCameraCapture(0);
    //on cree un objet capture pour recup les images de la camera par defaut (0)
    if (!capture) {
        //si on ne peut pas ouvrir la cémara, on affiche un message d'erreur et l'on termine le programme
        printf("La caméra n'a pas pu être ouverte\n");
        return -1;
    }

    //on récupère une frame de la caméro
    IplImage* frame = cvQueryFrame(capture);

    //on créé une image en niveau de gris 
    IplImage* gray_frame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);

    //on créé une image pour stocker l'image après l'application du filtre médian
    IplImage* median_frame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);

    //on créé une image pour stocker l'image après l'application du filtre de sobel
    IplImage* sobel_frame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);


    //NB:
    //pour toutes les images on utilise un 8 bits pour stocker la valeur de chaque pixel
    //(valeurs de 0 à 255).
    //j'avais pas trop compris cet argument donc je le note en 
    //commentaire pour référence future: IPL_DEPTH_8U est le paramètre qui indique ceci.
    //le troisième param est 1 car on a un seul canal de couleur (niv de gris)
    //CF la doc de opencv: https://docs.opencv.org/3.4/d2/df8/group__core__c.html#gad83bfadf567039148caaf60d8ce9978a

    
    //on capture les images de la caméra jusqu'a ce que l'utilisateur appuie sur une touche
    while (frame) {
        //conversion de l'image en niveaux de gris
        cvCvtColor(frame, gray_frame, CV_BGR2GRAY);

        // Utilisation de la fonction rkSabreMedian pour le filtrage médian
        rkSabreMedian(gray_frame, median_frame, 3);

        //Utilisation de la fonction rkSabreSobel pour le filtre de Sobel
        rkSabreSobel(median_frame, sobel_frame, 3);

        //Enfin, on affiche notre image 
        cvShowImage("Contours", sobel_frame);

        //on attend que l'utilisateur appuie sur une touche pendant 30ms, si 
        //c'est le cas on break
        if (cvWaitKey(30) >= 0) {
            break;
        }

        //on capture la frame suivante.
        frame = cvQueryFrame(capture);
    }

    //enfin, on libère la mémoire allouée pour toutes les images définies et utilisées. 
    cvReleaseImage(&gray_frame);
    cvReleaseImage(&median_frame);
    cvReleaseImage(&sobel_frame);
    cvReleaseCapture(&capture);

    return 0;
}

/**
 * rkSabreMedian(); fonction qui applique le filtre médian
 * @param src l'image d'entrée
 * @param dst l'image de sortie
 * @param kernel_size la taille du kernel 
*/
void rkSabreMedian(IplImage* src, IplImage* dst, int kernel_size) {
    int width = src->width;         //largeur
    int height = src->height;       //hauteur
    int step = src->widthStep;      //le step (nombre d'octets par ligne)

    // Vérification de la taille du kernel pour s'assurer qu'elle est impaire
    if (kernel_size % 2 == 0) {
        printf("La taille du kernel doit être impaire\n");

        //Si la taille est paire, il n'y a pas de pixel central défini et cela 
        //pourrait causer des problèmes lors de l'application du filtre. 
        //si la taille est paire, on affiche un message d'erreur et on termine le programme
        return;
    }

    // Boucle pour parcourir tous les pixels de l'image. 
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // Création d'un tableau pour stocker les valeurs des pixels voisins
            int values[kernel_size * kernel_size];
            int k = 0;

            //on veut parcourir les pixels autour du pixel courant aux coordonnées i, j dans
            //un rayon égal a la moitié de la taille du kernel. ainsi, tous les pixels qui sont
            //a l'intérieur du kernel sont inclus dans la boucle, et ceux à l'extérieur du kernel
            //sont exclus.

            //boucle pour parcourir les pixels voisins de chaque pixels de l'image
            for (int x = i - kernel_size / 2; x <= i + kernel_size / 2; x++) {
                for (int y = j - kernel_size / 2; y <= j + kernel_size / 2; y++) {
                    //vérification que les coordonnées sont valides (comprises dans la taille de l'image)
                    if (x >= 0 && x < height && y >= 0 && y < width) {
                        //ajout de la valeur du pixel courant dans l'image d'input dans le tableau des valeurs voisines
                        values[k++] = src->imageData[x * step + y];
                    }
                }
            }

            // Tri du tableau pour trouver la valeur médiane
            // tri "trivial" plutot classique, piste d'opti : ) 
            for (int x = 0; x < k; x++) {
                for (int y = x + 1; y < k; y++) {
                    if (values[x] > values[y]) {
                        int temp = values[x];
                        values[x] = values[y];
                        values[y] = temp;
                    }
                }
            }

            // Applique la valeur médiane sur l'image de sortie (dst)
            dst->imageData[i * step + j] = (uchar)values[k / 2];
                //on caste values[k/2] en un entier 8 bit non signé,
                //valeur que l'on affecte à l'image d'output pour chaque indice de
                //(i*step, j) (on est encore dans la double boucle qui itère sur la height/width)
        }
    }
}

/**
 * rkSabreSobel(); fonction qui applique le filtre de Sobel
 * @param src l'image d'entrée
 * @param dst l'image de sortie
 * @param kernel_size la taille du kernel 
*/
void rkSabreSobel(IplImage* src, IplImage* dst, int kernel_size) {
    int width = src->width;         //largeur
    int height = src->height;       //hauteur
    int step = src->widthStep;      //le step (nombre d'octets par ligne)



    // Création des kernels pour les gradients en x et en y

    //on utilise les kernels de sobel "classiques":
    //                |-1     0     1 |                  |-1     -2     -1 |     
    //  kernelx=      |-2     0     2 | ,  kernely=      | 0      0      0 |  
    //                |-1     0     1 |                  | 1      2      1 |

            //POUR KERNELX:
    //sur la première ligne, ces valeurs amplifient les diff de niv de gris entre les
    //pixels adjacents: ca met en valeur les contours
    //-1 amplifie les bords noirs sur fond blanc, 0 annule les pixels qui ne sont pas sur un contour, et 1 amplifie les bords blancs sur fond noir
    //idem pour -2, 0, 2 et -1, 0, 1; on varie les valeurs sur la deuxieme ligne pour amplifier davantage les contours
    //pour augementer la sensibilité du filtre

            //POUR KERNELY
    //même raisonnement mais transposé!
    int kernel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int kernel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};


    // Vérification de la taille du kernel = 3 (taille des matrices)
    if (kernel_size != 3) {
        printf("La taille du kernel doit être 3\n");
        return;
    }


    //Boucle pour parcourir tous les pixels de l'image
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            //gradients en x et y 
            int gradx = 0, grady = 0;

            //Boucles pour parcourir les pixels voisins
            //un peu tricky, si on met les mauvais indices ca pisse des segmentation fault:
            //k et l sont les indicent qui parcourent les pixels autour du pixel courant (plage de -1 à 1)
            //
            //donc les pixels a gauche et a droite (pour la boucle sur k), et en haut et en bas (pour la boucle sur l) du pixel courant.
            for (int k = -1; k <= 1; k++) {
                for (int l = -1; l <= 1; l++) {
                    //on vérifie que les coordonnées du pixel voisin sont valides (comprises dans les dimensions de l'image)
                    if (i + k >= 0 && i + k < height && j + l >= 0 && j + l < width) {
                        //calcul du gradient 
                        gradx += src->imageData[(i + k) * step + (j + l)] * kernel_x[k + 1][l + 1];
                        grady += src->imageData[(i + k) * step + (j + l)] * kernel_y[k + 1][l + 1];
                    }
                }
            }

            // Calcul de la magnitude du gradient
            //on fait la racine de gradx²+grady², on a pas importé math.h donc on 
            //doit faire x*x = x²
            int magnitude = (int)sqrt(gradx * gradx + grady * grady);

            if (magnitude > 255) {
                magnitude = 255;
            }

            // Application de la magnitude sur l'image de sortie
            dst->imageData[i * step + j] = (uchar)magnitude;
        }
    }
}