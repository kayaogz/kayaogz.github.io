#include <math.h>

/** Creer une matrice de taille nrows x ncols.
 *  Si init="random", les elements sont initialises aleatoirement. 
 *  Si init="zero", les elements sont initialises a zero.
 */
void createMatrix(
    double **pmat,
    int nrows,
    int ncols,
    char *init);

/** Multiplier deux matrices A et B de taille MxM, mettre le resultat dans la matrice C.
 */
void multiplyMatrix(
    double *A,
    double *B,
    double *C,
    int M);

/** Algorithme parallele SUMMA pour effectuer la multiplication matrix-matrix C = A x B.
 */
void summa(
  double *Aloc, // Morceau locale de la matrice A.
  double *Bloc, // Morceau locale de la matrice B.
  double *Cloc, // Morceau locale de la matrice C. Initialise deja a zero.
  int M) // Le nombre de lignes/de colonnes de Aloc, Bloc et Cloc.
{
  int procRank, numProcs;
  double *Atemp, *Btemp; // Le blocs a recuperer dans la boucle principale
  // Recuperer procRank et numProcs, puis creer des communicateurs de ligne et de colonne
  
  // Broadcaster le morceau k (k = 1 ... sqrt(numProcs)) de chaque bloc de ligne de A, et bloc de colonne de B

  // Effectuer la multiplication locale 

  // Eventuellement creer un tableau locale dans le processus 0 de taille (M x sqrt(numProcs)) x (M x sqrt(numProcs)), et mettre ensemble le resultat avec MPI_Gather.
}
