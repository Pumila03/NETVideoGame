```
                               ,----, 
         ,--.                ,/   .`| 
       ,--.'|    ,---,.    ,`   .'  : 
   ,--,:  : |  ,'  .' |  ;    ;     / 
,`--.'`|  ' :,---.'   |.'___,/    ,'  
|   :  :  | ||   |   .'|    :     |   
:   |   \ | ::   :  |-,;    |.';  ;   
|   : '  '; |:   |  ;/|`----'  |  |   
'   ' ;.    ;|   :   .'    '   :  ;   
|   | | \   ||   |  |-,    |   |  '   
'   : |  ; .''   :  ;/|    '   :  |   
|   | '`--'  |   |    \    ;   |.'    
'   : |      |   :   .'    '---'      
;   |.'      |   | ,'                 
'---'        `----'                   
```                 
<img src="res/img_presa.png"  width="200" height="200">  

## Présentation 
Voici le jeu Net,

Ce jeu utilise une grille de 5 cases sur 5, où chaque case contient des pièces représentant des tuyaux ou des câbles. Ces pièces peuvent être pivotées d'un quart de tour à droite ou à gauche. Elles se déclinent en différentes formes : des terminaisons, des segments droits, des coudes, des jonctions en T ou des croix, avec des orientations vers le nord, l'est, le sud ou l'ouest. Chaque pièce est construite à partir de bras partant du centre vers ses bords, appelés demi-arêtes.
Bon courage !

<hr style="border: 5px solid gray;" color="gray"> 
<br>
<br>

# Version texte

## Jouer

Il vous est proposé différentes manières de jouer au jeu. Afin de pouvoir obtenir des exécutables pour jouer, il faudra exécuter les commandes suivantes :
```
mkdir build
cd build
cmake ..
make
```
À partir de là, deux exécutables seront créés :

- Pour créer le jeu de base :

```./game_text [game_to_load]```

- Pour créer un jeu aléatoire :

```./game_random <nb_rows> <nb_cols> <wrapping> <nb_empty> <nb_extra> <shuffle> [<filename>]```

## Les commandes

Les différentes commandes sont :
- ```c I J``` Tourne dans le sens horaire la pièce à la ligne I et à la colonne J
- ```a I J``` Tourne dans le sens anti-horaire la pièce à la ligne I et à la colonne J
- ```r``` Permet de réinitialiser la grille de jeu
- ```z``` Revenir en arrière (Undo)
- ```y``` Revenir en avant (Redo)
- ```q``` Permet de quitter à votre grande honte
- ```h``` Permet d'afficher l'aide

<br>
<br>
<hr style="border: 5px solid gray;" color="gray">
<br>
<br>

# Version Graphique  

Nous avons maintenant une interface graphique pour profiter de notre jeu.

Dans notre interface graphique, nous utilisons à plusieurs reprises des effets sonores ainsi qu'une musique d'ambiance. Si vous n'avez pas les bibliothèques nécessaires pour leur utilisation, le jeu se jouera sans son. Sinon, pour accéder à ces fonctionnalités, il est nécessaire de télécharger la bibliothèque SDL_mixer. Voici la commande à entrer dans le terminal Linux :

```sudo apt install libsdl2-mixer-dev```

Il est toujours possible de couper le son du jeu en cliquant sur le bouton audio une fois le jeu lancé.

- Pour jouer à la version graphique de NET : 
```
mkdir build
cd build
cmake ..
make
./game_sdl
```

## Jouer 

Pour jouer, il faut connecter toutes les pièces ensemble.
- Quand deux pièces sont connectées, leurs couleurs changent, passant du rose au bleu.
- Une option wrapping est appliquée si la lumière en haut à droite du tableau (notée "w") est allumée en vert.
- Lorsque le jeu est gagné, toutes les pièces sont bleues et l'information "You win" vous est renvoyée.
- Il est possible de relancer une partie en générant un nouveau aléatoire avec la touche R !


## Comment jouer  

Voici les différentes commandes pour jouer :

- ```clique GAUCHE``` Tourner la pièce sélectionnée d'un quart de tour vers la gauche
- ```clique DROIT``` Tourner la pièce sélectionnée d'un quart de tour vers la droite
- ```Z``` ou **Bouton Z** Revenir en arrière (Undo)
- ```Y``` ou **Bouton Y** Revenir en avant (Redo)
- ```H``` ou **Bouton H** Permet d'afficher l'aide
- ```R```  Permet de réinitialiser la grille de jeu (random)
- ```S``` Permet d'enregistrer la partie en cours (save) 
- ```L``` Permet de charger une partie enregistrée (load)
- ```K``` Permet d'avoir la solution de la partie actuelle
- ```M``` Permet de mélanger le jeu en cours
- ```ESC``` Permet de quitter le jeu

La touche ```K``` permet de résoudre le jeu. Cependant, certains jeux sont un peu trop complexes pour être résolus. Si, au bout de 10 s, le jeu n'est pas résolu, alors la résolution est arrêtée et un message est affiché. Le message disparaîtra une fois que vous jouerez un nouveau coup."
Toutes ces fonctionnalités, excepté pour bouger les pièces, sont aussi accessibles comme dans la version texte de notre jeu (touches du clavier).

## Les auteurs(A17) :

- Siméon Descorps
- Aala Ajmaime
- Eliott Peaudeau