# image segmentation


Notes

Image pgm en 2D (x/y) + 3 channel RGB
Mapper sur un espace de point  3D (R G B) (a ne pas implementer, juste pour la visualisation)
Prendre K en entrée de l’algo
Avoir un tableau de taille k*3 qui stocke les rgb (coordonée) de chaque centre de cluster
Generer les centre au hasard au debut et les fixé pour l’expérience
Avoir une matrice de taille y*x qui stocker a chaque index de pixel le cluster auquel il apparteint

Algo
1) prendre K en entrée (e nombre de cluster) et l’image (X,Y, RGB)

2)Générer les centres et les placer dans un tableau de taille 3K. (stocker leurs coordonnée, et le nom = index).

3)Générer un tableau de la taille de l’image sans l’initialiser

4)Compute : itérer dans l’image et regarder les coordonnée RGB, pour chaque pixel, calculer a quel cluster il appartient (Calculer la distance a chaque centre de K et prendre le minimum). Sauvegarder l’index du cluster associé dans le tableau précédent.

5) Itérer dans ce tableau : pour chaque ensemble de point (cluster), calculer la position moyenne dans l’espace RGB.

6) Modifier le tableau de centre avec la nouveau moyenne de chaque cluster
