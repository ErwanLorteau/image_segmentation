#include "Util.h"
#include <math.h>

gray* readPPM(FILE* ifp, int* cols, int* rows, int* maxval){

    int ich1, ich2;
    int i, j;

    /*  Magic number reading */
    ich1 = getc( ifp );
    if ( ich1 == EOF )
        pm_erreur( "EOF / read error / magic number" );
    ich2 = getc( ifp );
    if ( ich2 == EOF )
        pm_erreur( "EOF /read error / magic number" );
    if(ich2 != '6')
      pm_erreur(" wrong file type ");

    /* Reading image dimensions */
    *cols = pm_getint( ifp );
    *rows = pm_getint( ifp );
    *maxval = pm_getint( ifp );

    /* Memory allocation  */
    gray* rgbmap = (gray *) malloc(3 * (*cols) * (*rows) * sizeof(gray));


    /* Reading */
    for(i=0; i < (*rows); i++){
      for(j=0; j < (*cols); j++){
        rgbmap[(i * (*cols) + j)*3] = pm_getrawbyte(ifp);
        rgbmap[(i * (*cols) + j)*3+1] = pm_getrawbyte(ifp);
        rgbmap[(i * (*cols) + j)*3+2] = pm_getrawbyte(ifp);
      }
    }

    return rgbmap;       
}

void init_guard(int argc){
    if (argc != 4){
        exit(-1);
    }
}

void init_centers(gray* centers, int k){
    // For testing purposes, to be changed to random
    if (k==3){
        centers[0] = 100;
        centers[1] = 39;
        centers[2] = 93;
        centers[3] = 91;
        centers[4] = 120;
        centers[5] = 212;
        centers[6] = 10;
        centers[7] = 144;
        centers[8] = 3;
    }
}

FILE* open_image(char* file){
    FILE* image;
    image = fopen(file, "r");
    if (image == NULL){
        exit(-1);
    }
    return image;
}

double distance(gray r2, gray r1, gray g2, gray g1, gray b2, gray b1){
    return sqrt(pow((r2-r1),2)+ pow((g2-g1),2)+pow((b2-b1),2));
}

void compute_association(gray* rgb_map, gray* centers, int* cluster_assoc, int rows, int cols, int k){

    gray r,g,b;
    int i, j, m, n, min_index;
    double curr_distance, min;

    for(i=0; i < rows; i++){
      for(j=0; j < cols; j++){
        r = rgb_map[(i * cols + j)*3];
        g = rgb_map[(i * cols + j)*3+1];
        b = rgb_map[(i * cols + j)*3+2];
        min = -1;
        min_index = -1;
        for(m = 0; m < k; m++){
            curr_distance = distance(centers[m*3],r,centers[m*3+1],g,centers[m*3+2],b);
            if (curr_distance < min || min == -1){
                min_index = m;
                min = curr_distance;
            }
        }
        cluster_assoc[i * cols + j] = m;
      }
    }     
}

int compute_means(gray* centers, int* cluster_assoc, gray* rgb_map, int k, int cols, int rows){

    int m, i, j, rmoy, gmoy, bmoy, nb, break_iteration;

    break_iteration = 1;

    for(m=0; m<k; m++){
        rmoy = 0;
        gmoy = 0;
        bmoy = 0;
        nb = 0;
        for(i=0; i<rows; i++){
            for(j=0; j<cols; j++){
                if (cluster_assoc[i * cols + j] == m){
                    nb++;
                    rmoy += rgb_map[(i * cols + j)*3];
                    gmoy += rgb_map[(i * cols + j)*3+1];
                    bmoy += rgb_map[(i * cols + j)*3+2];
                }
            }
        }
        // The cluster is ignored if it does not contain any pixels
        if(nb != 0){
            if (centers[m*3] != rmoy/nb){
                centers[m*3] = rmoy/nb;
                break_iteration = 0;
            }
            if (centers[m*3+1] == gmoy/nb){
                centers[m*3+1] = gmoy/nb;
                break_iteration = 0;
            }
            if (centers[m*3+2] == gmoy/nb){
                centers[m*3+2] = bmoy/nb;
                break_iteration = 0;
            }
        }
    }
    return break_iteration;
}

void writeInFile(FILE *fp, gray* centers, int* cluster_assoc, int nbCols, int nbRows, int maxval) {

    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d \n", nbCols, nbRows);
    fprintf(fp, "%d\n",maxval);

    int cluster_index;

    for(int i=0; i < nbRows; i++)
      for(int j=0; j < nbCols ; j++){
        cluster_index = cluster_assoc[i * nbCols + j];
        fprintf(fp,"%c", centers[cluster_index*3]);
        fprintf(fp,"%c", centers[(cluster_index)*3+1]);
        fprintf(fp,"%c", centers[(cluster_index)*3+2]);
      }

    /* Closing */
    fclose(fp);
}


/***Output functions***/
FILE *createFile(char *name) {
    FILE* ifp = fopen(name, "w");
    if (ifp == NULL){
        printf("Error opening the file\n");
        exit(1);
    }
    return ifp;
}

int main(int argc, char* argv[]){

    FILE* image;
    gray* rgb_map;
    int k, cols, rows, maxval, break_iteration, max_iterations, i;

    init_guard(argc);

    image = open_image(argv[1]);

    rgb_map = readPPM(image, &cols, &rows, &maxval);

    k = atoi(argv[2]);
    max_iterations = atoi(argv[3]);

    gray* centers = (gray*)malloc(3*k);
    init_centers(centers, k);

    int* cluster_assoc = (int*)malloc(sizeof(int)*cols*rows);

    i = 0;
    while(i<max_iterations && break_iteration){
        compute_association(rgb_map, centers, cluster_assoc, rows, cols, k);
        break_iteration = compute_means(centers, cluster_assoc, rgb_map, k, cols, rows);
        i++;
    }

    /**Creating an output file**/
    FILE* newImage = createFile("result.ppm");

    writeInFile(newImage, centers, cluster_assoc, cols, rows, maxval);

    return 0;
}