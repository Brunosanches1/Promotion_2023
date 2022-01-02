# Bilan pour le projet réalisé 

## Mesure du temps en sequentiel
Avec affichage = 120ms par pas de temp
Sans affichage = 29ms par pas de temp

Affichage correspond à 75% du temps de la simulation

## Mesure du temps avec affichage synchrone et MPI
Avec affichage = 124ms par pas de temp
Sans affichage = 40ms par pas de temp

Le temps utilisé est plus importante que pour la simulation sans MPI, c'est à cause de l'échange de messages necessaires dans le cas avec ou sans affichage


## Mesure du temps avec affichage asynchrone et MPI
Avec affichage = 37ms par pas de temp
Sans affichage = 31ms pas pas de temp

Mantenant, le temp avec l'affichage est reduit puis que c'est pas necessaire d'attendre que le processus 0
finisse son affichage pour envoyer des donnés, le processus 1 continue la simulation jusqu'au moment 
que le processus 0 est fini, le speedup c'est de 3,3

## Mesure du temps avec affichage asynchrone, MPI et OMP
Avec affichage = 17ms en utilisant 24 OMP threads
Sans affichage = 16ms en utilisant 24 OMP threads

Mantenant on peut voir que le temps d'affichage n'est plus important dans la simulation et la difference entre la
simulation avec et sans affichage est minimale, au contraire de quand ça est en sequentiel

## Mesure du temps avec affichage asynchrone et plusieurs processeurs MPI
Avec affichage = 25ms par pas de temp avec 4 processus MPI
Sans affichage = 21ms par pas de temps avec 4 processus MPI

## Mesure du temps avec MPI et OMP finale
Meilleur temp obtenu avec 3 threads OMP et 2 processus MPI (Un pour la simulation et autre pour l'affichage)
En utilisant un seule ordinateur, ce n'est pas profitable d'utiliser MPI, mais peut-être que dans une architecture NUMA ou 
en utilisant plusieurs ordinateurs ce résultat peut être changer pour plus que 2 processus