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

## Overview
Here is the Net game,

This game uses a 5x5 grid, where each cell contains pieces representing pipes or cables. These pieces can be rotated a quarter turn to the right or left. They come in different shapes: terminations, straight segments, elbows, T-junctions, or crosses, with orientations facing north, east, south, or west. Each piece is built from arms extending from the center to its edges, called half-edges.
Good luck!

## Play

There are different ways to play the game. To generate executables for playing, run the following commands:
```
mkdir build
cd build
cmake ..
make
```
This will create two executables:

- To play the base game:
```./game_text [game_to_load]```

- To create a random game:
```./game_random <nb_rows> <nb_cols> <wrapping> <nb_empty> <nb_extra> <shuffle> [<filename>]```

## Commands

The available commands are:
- ```c I J``` Rotates the piece at row I and column J clockwise
- ```a I J``` Rotates the piece at row I and column J counterclockwise
- ```r``` Resets the game grid
- ```z``` Undo the last move
- ```y``` Redo the undone move
- ```q``` Quit the game in disgrace
- ```h``` Display help

## Authors (A17):

- Siméon Descorps
- Aala Ajmaime
- Eliott Peaudeau

