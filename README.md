# Embed_Qr_Code
 Permet de fusionner des images avec un Qr Code que l'on génère également.
Bibliothèque utilisée : 
- OPENCV-VC15
- STL

Ce projet prend en entrée plusieurs choses à plusieurs moment :
- La phrase de l'utilisateur qui doit être transformé en QR_Code.
- Le nom de l'image à fusionner.

Plusieurs paramètres rentre en jeu :
- La taille du module qui correspond à la taille d'une valeur du QR Code (0 ou 1)
- La taille "px" qui correspond à la taille du centre du module voulue et qui aura une luminance plus ou moins élevée en fonction des zones mais qui sera globalement représentatif du QR Code.
- La taille du carré "Select Local" qui correspond à la taille de la zone dans laquelle on va venir chercher le maximum et le minimum de luminance pour les appliquer au pixel de cette zone carrée. Plus la zone est petite, plus la luminance sera proche de celle de l'image d'origine mais cela peut poser problème pour la détection du QR Code.

29-11-2020 : Cette version est finie et tout fonctionne. Des modifications sont suceptibles d'être apportées.

02-12-2020 : Version MaJ pour ne plus avoir d'inversion des couleurs entre bleu et vert (lié au système BGR utilisé par OpenCV)

02-12-2020 V2 : Pas mal de légers bugs ont été mis à jour, voir version "Correction".

16-12-2020 : Probablement la dernière MaJ de ce projet. On a rajouté plusieurs fonctions de "test" comme la disparition de la zone silencieuse ou encore des références mais cela nuit gravement à la lisibilité du QR-Code.
