# Bilan projet
## Introduction

Ce projet a été proposé comment activité pour la discipline IN203 - Programmation Parallélé dans le majeur de Sciences et Technologies de l'Information et de la Communication de l'École Nationale Supérieure des
Techniques Avancées (ENSTA Paris). Le code base utilisé a été developpé par le groupe des professeurs responsables pour le cours et peut-être vu sur le [repositoire](https://github.com/JuvignyEnsta/Promotion_2023) des activités du cours.

## Objectifs

L'objectif du projet est de simuler la co-circulation d'un virus et d'un agent pathogène. Plus de détails peuvent être obtenus dans le [sujet du projet](https://github.com/Brunosanches1/Promotion_2023/blob/main/Projet/Sujet.pdf). La simulation sera faite en parties et en utilisant les bibliothèques MPI et Open Mp.

## Exécution et résultats

Cette activité será divisé en parties d'accord avec le niveau de parallelisation et des outils utilisés, l'ordre definie est la suivant:

1. Mesure du temps en sequéntiel du code
2. Parallélisation avec MPI en divisant l'affichage et la simulation entre deux processus, affichage synchrone (affiche chaque jour)
3. Modification pour implementer l'affichage asynchrone, le processus ne va plus afficher tous les jours, l'autre processus va envoyer les donnés du dérnier jours qu'il a calculé quand ça soit solicité et a mesure que ils sont affichés
4. Utilisation d'OpenMP pour créer des threads dans le code
5. Permettre d'utiliser plus de deux processus MPI pour paralleliser plus la simulation
6. Jonction de les modifications 4 et 5 pour la parallélisation finale du code

### Programme sequéntiel

Ce code est sur la branch [main](https://github.com/Brunosanches1/Promotion_2023/tree/main/Projet/sources).

Le programme sequéntiel sera utilisé comme base pour calculer les différents accelerations et certifiquer que le résultat soit toujours le même independant de l'implementation. Le temps calculé est le temps necessaire moyen pour le calcul de chaque jour.

| Avec affichage | Sans Affichage |
| :------------: | :------------: |
| 120ms          | 29ms           |

C'est possible de noter que dans un permier cas, le temps d'affichage est très importante dans le contexte de la simulation, ça correspond à aproximativement 77% du temps utilisé du programme dans chaque jour.

### Parallélisation avec MPI et affichage synchrone

Ce code est sur la branch [sync_affiche_mpi](https://github.com/Brunosanches1/Promotion_2023/tree/sync_affiche_mpi/Projet/sources).

La premier modification proposé c'est de utiliser MPI et deux processus pour separer la affichage de la simulation, le processus responsable pour la simulation doit envoyer les messages MPI avec les donnés utilisés pour l'affichage. Les résultat obtenus sont les suivants.

| Avec affichage | Sans Affichage |
| :------------: | :------------: |
| 124ms          | 40ms           |

Dans ce cas, on note que le temps par jour est augmenté dans les deux cas, c'est à cause de l'échange de messages avec MPI. Si le difference de temps est moins dans le cas avec affichage, c'est à cause de que avec affichage, l'augmentation du temps causé par MPI est en part compensé par la creation d'un "pipeline" de l'affichage, comme la simulation est plus rapide que l'affichage, au moment que le processus 0 termine d'afficher, le processus 1 a déjà les donnés de le prochaine jour.

### Parallélisation avec MPI et affichage asynchrone

Ce code est sur la branch [async_affiche_mpi](https://github.com/Brunosanches1/Promotion_2023/tree/async_affiche_mpi/Projet/sources).

Aprés les dernières parties, on note que l'affichage a un temps très important dans le total de chaque jour, un modification possible c'est de faire l'affichage asynchrone, ça soit, quand le processus 0 termine d'afficher un jour, il peut envoyer une message au processus 1 pour le dire qu'il a fini et que pourtant le processus 1 peut l'envoyer un autre jour pour l'afficher, pas necessairement le prochaine jour, dans ce cas le processus 1 ne doit pas attendre que l'affichage soit complete pour calculer des autres jours. Les resultats obtenus sont les suivants.

| Avec affichage | Sans Affichage |
| :------------: | :------------: |
| 37ms           | 31ms           |

Aprés la modification on peut noter que la difference de temps entre la simulation sans et avec affichage n'est plus grand et est causé pour léchange de messages MPI necessaire. L'acceleration obtenue en comparaison à la partie anterieur c'est de 3.3.

### Parallélisation avec OpenMP

Ce code est sur la branch [async_ompt](https://github.com/Brunosanches1/Promotion_2023/tree/async_omp/Projet/sources).

Pour cette partie on prendre le code antérieur et ajoute le support à la parallélisation avec threads de la bibliothèque OpenMP. On utilise les *for-loops* des individus, pourtant chaque thread va calculer une partie des individus du ensemble de la simulation. Les résultats suivants sont faites avec 24 threads.

| Avec affichage | Sans Affichage |
| :------------: | :------------: |
| 17ms           | 16ms           |

L'acceleration obtenue à cause de cette modification est de 7.3, un résultat déjà très bon. La creation des threads OpenMP est pas cher et donc même avec plus de threads que disponibles dans le ordinateur, on obtien une acceleration bonne.

### Parallélisation avec plus de deux processus MPI

Ce code est sur la branch [async_mpi](https://github.com/Brunosanches1/Promotion_2023/tree/async_mpi/Projet/sources).

Un des cas de utilisation de le MPI est de possibiliter l'utilisation de plusieurs ordinateurs sur un réseau pour le programme, chaque ordinateur peut correspondre à 1 ou plus processus MPI et la bonne implementation d'un algorithme que profite de ça peut donner des bonnes accelerations du code. Pour cette modification c'est necessaire de distribuer les individus par groupes dont un va être responsable, c'est necessaire de prendre soin avec la creation des individus en raison des graines aleatoires. Les meilleures resultats ont été obtenus avec 4 processus MPI, 1 responsable pour l'affichage et 3 pour la simulation.

| Avec affichage | Sans Affichage |
| :------------: | :------------: |
| 25ms           | 21ms           |

Ce résultat est plus grande que l'item anterieur en raison de l'échange des messages, Au contraire de l'OpenMP, la creation de processus MPI a des côuts, surtout dans une architecture d'un seule ordinateur. L'ordinateur utilisé a seulement 4 CPUs, dans un ordinateur avec plus de CPUs le résultat peut être meilleur.

### Parallélisation finale avec OpenMP et MPI

Pour finaliser, on ajout les solutions crées dans les deux dernières items, on divise les individus par groupe et dans les iterations de ces groupes on utilise OpenMP pour le paralleliser, cette solutions est très interessant dans un contexte de plusieurs ordinateurs utilisés pour faire le calcul, dans les experiments, malhereusement, ce n'était pas possible de utiliser plus d'un ordinateur. 

Les résultats obtenues ont été meilleures avec l'utilisation de deux processus MPI et 3 processus OpenMP, le temps pour une simulation sans affichage avec cette configuration a été 18ms. Dans une simulation avec plus d'ordinateur je pense que le résultat peut être meilleure en utilisant plus de processus MPI divisés par les ordinateurs.

## Difficultés

Les difficultés encontrés sont liées a des caracteristiques aleatoires de la simulation. 

La première difficulté encontré était la parallélisation avec OpenMP sans déranger les résultats de la simulation, le méthode estContaminé() de les individus utilisait de moteur des nombres aleatóires des variables grippe et agent, ça ne permettre pas de paralléliser cette partie du code, en vue que les résultats seront differénts en raison que l'ordre d'analyse des individus est différent. La solution était de affecter la generation des nombres aleatoires aux individus et ne plus aux variables grippe et agent.

L'autre difficulté était avec MPI et la definition de groupes d'invidus pour chaque processus, c'était necessaire de definir des calculs pour determiner les groupes de façon balancé entre les processus.

## Bilan d'apprentissage

Dans ce projet, j'ai pu mettre en pratique les concepts apprentis dans le cours de programmation parallèle ainsi comme pratiquer plus de programmation, orientation objet et de connaissances des bibliothèques OpenMP et MPI.

# Prerequis

    sudo apt install g++-10 
    sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev

Et si besoin modifier Makefile et Make_linux.inc pour utiliser g++-10. 


# Compiler mpi avec g++-10

Le plus simple : 

    export OMPI_CXX=g++-10


Ajouter les CFLAGS et LDFLAGS dans le Make_linux.inc

    CXXFLAGS+=$(mpic++ --showme:compile)
    LIBS+=$(mpic++ --showme:link)
    
Autre option, faire un lien symbolique g++ -> /usr/bin/g++10 dans $HOME/bin et le mettre en premier dans PATH) :

     mkdir $HOME/bin
     cd $HOME/bin
     ln -s /usr/bin/g++-10 g++
     export PATH=$HOME/bin:$PATH   # dans votre .bashrc
     which -a g++
     g++ --version   # afficher 10.xx


    
