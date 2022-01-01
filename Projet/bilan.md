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
que le processus 0 est fini
