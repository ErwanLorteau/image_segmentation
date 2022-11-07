#include "Util.h"

gray* readPPM(FILE* ifp){

    int ich1, ich2, cols, rows, maxval;
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
    cols = pm_getint( ifp );
    rows = pm_getint( ifp );
    maxval = pm_getint( ifp );

    /* Memory allocation  */
    gray* graymap = (gray *) malloc(3 * cols * rows * sizeof(gray));

    /* Reading */
    for(i=0; i < rows; i++){
      for(j=0; j < cols; j++){
        graymap[(i * cols + j)] = pm_getrawbyte(ifp);
      }
    }


       
}

int main(int argc, char* argv[]){

    



    return 0;
}