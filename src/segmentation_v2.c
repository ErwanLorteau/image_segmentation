#include "Util.h"
#include <time.h>
#include <stdlib.h>
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

void init_centers(int* centers, int k, int cols, int rows, int maxval){

    for(int i=0; i<k; i++){
        //Check if center coordinates are not repeated
        centers[i*5] = rand()%rows;
        centers[i*5+1] = rand()%cols;
        centers[i*5+2] = rand()%maxval;
        centers[i*5+3] = rand()%maxval;
        centers[i*5+4] = rand()%maxval;
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

double intensity_distance(gray r2, gray r1, gray g2, gray g1, gray b2, gray b1){
    return sqrt(pow((r2-r1),2)+ pow((g2-g1),2)+pow((b2-b1),2));
}

double spatial_distance(int a2, int a1, int b2, int b1){
    return sqrt(pow((a2-a1),2)+ pow((b2-b1),2));
}

void compute_association(gray* rgb_map, int* centers, int* cluster_assoc, int rows, int cols, int k){

    gray r,g,b;
    int center_i, center_j; 
    gray center_r, center_g, center_b;
    int i, j, m, min_index;
    double curr_instensity_distance, curr_spatial_distance, min;

    for(i=0; i < rows; i++){
      for(j=0; j < cols; j++){
        r = rgb_map[(i * cols + j)*3];
        g = rgb_map[(i * cols + j)*3+1];
        b = rgb_map[(i * cols + j)*3+2];
        min = -1;
        min_index = -1;
        for(m = 0; m < k; m++){
            center_i = centers[m*5];
            center_j = centers[m*5+1];
            center_r = centers[m*5+2];
            center_g = centers[m*5+3];
            center_b = centers[m*5+4];
            curr_instensity_distance = intensity_distance(center_r,r,center_g,g,center_b,b);
            curr_spatial_distance = spatial_distance(center_i,i,center_j,j);
            if (curr_instensity_distance+curr_spatial_distance < min || min == -1){
                min_index = m;
                min = curr_instensity_distance+curr_spatial_distance;
            }
        }
        cluster_assoc[i * cols + j] = min_index;
      }
    }     
}

int compute_means(int* centers, int* cluster_assoc, gray* rgb_map, int k, int cols, int rows){

    int m, i, j, imoy, jmoy, rmoy, gmoy, bmoy, nb, break_iteration;

    break_iteration = 1;

    for(m=0; m<k; m++){
        imoy = 0;
        jmoy = 0;
        rmoy = 0;
        gmoy = 0;
        bmoy = 0;
        nb = 0;
        for(i=0; i<rows; i++){
            for(j=0; j<cols; j++){
                if (cluster_assoc[i * cols + j] == m){
                    nb++;
                    imoy += i;
                    jmoy += j;
                    rmoy += rgb_map[(i * cols + j)*3];
                    gmoy += rgb_map[(i * cols + j)*3+1];
                    bmoy += rgb_map[(i * cols + j)*3+2];
                }
            }
        }
        // The cluster is ignored if it does not contain any pixels
        if(nb != 0){
            if (centers[m*5] != imoy/nb){
                centers[m*5] = imoy/nb;
                break_iteration = 0;
            }
            if (centers[m*5+1] != jmoy/nb){
                centers[m*5+1] = jmoy/nb;
                break_iteration = 0;
            }
            if (centers[m*5+2] != rmoy/nb){
                centers[m*5+2] = rmoy/nb;
                break_iteration = 0;
            }
            if (centers[m*5+3] != gmoy/nb){
                centers[m*5+3] = gmoy/nb;
                break_iteration = 0;
            }
            if (centers[m*5+4] != bmoy/nb){
                centers[m*5+4] = bmoy/nb;
                break_iteration = 0;
            }
        }
    }
    return break_iteration;
}

void writeInFile(FILE *fp, int* centers, int* cluster_assoc, int nbCols, int nbRows, int maxval) {

    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d \n", nbCols, nbRows);
    fprintf(fp, "%d\n",maxval);

    int cluster_index;

    for(int i=0; i < nbRows; i++)
      for(int j=0; j < nbCols ; j++){
        cluster_index = cluster_assoc[i * nbCols + j];
        fprintf(fp,"%c", centers[cluster_index*5+2]);
        fprintf(fp,"%c", centers[(cluster_index)*5+3]);
        fprintf(fp,"%c", centers[(cluster_index)*5+4]);
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
    time_t seed;

    init_guard(argc);

    /* Fixed seed for testing purposes */
    //srand(1668869384); 
    
    /* Comment out if want to use fixed seed */
    seed = time(NULL);
    printf("seed: %ld\n",seed);
    srand(seed);

    image = open_image(argv[1]);

    rgb_map = readPPM(image, &cols, &rows, &maxval);

    k = atoi(argv[2]);
    max_iterations = atoi(argv[3]);

    int* centers = (int*)malloc(sizeof(int)*5*k); //each center will contain: col, row, r, g, b
    init_centers(centers, k, cols, rows, maxval);

    int* cluster_assoc = (int*)malloc(sizeof(int)*cols*rows);

    i = 0;
    break_iteration = 0;
    while(i<max_iterations && !break_iteration){
        compute_association(rgb_map, centers, cluster_assoc, rows, cols, k);
        break_iteration = compute_means(centers, cluster_assoc, rgb_map, k, cols, rows);
        i++;
    }
    printf("number of iterations: %d\n",i);

    /**Creating an output file**/
    FILE* newImage = createFile("result.ppm");

    writeInFile(newImage, centers, cluster_assoc, cols, rows, maxval);

    return 0;
}