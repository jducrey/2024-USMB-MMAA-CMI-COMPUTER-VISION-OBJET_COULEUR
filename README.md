# 2024-USMB-MMAA-CMI-COMPUTER-VISION-OBJET_Couleur – Reconnaissance d’objets par distribution de couleurs

Projet en **Traitement et Analyse d’Images**, réalisé dans le cadre du Master MMAA et du CMI en Mathématiques Appliquées – Université Savoie Mont Blanc.

🎯 Objectif : Construire une **application temps réel de reconnaissance d’objets** basée sur leur **distribution de couleurs**, à partir d’un flux vidéo capturé en direct.  
L’utilisateur montre à l’application des objets à reconnaître ; le système apprend leurs signatures colorimétriques et les utilise ensuite pour les identifier dans le flux vidéo.

---

## 🧠 Principes clés du projet

- Utilisation d’**histogrammes de couleurs** comme base de la représentation des objets.
- Calcul de **distances entre distributions** pour distinguer les objets du fond.
- **Mode d’apprentissage progressif**, permettant à l’utilisateur d’ajouter, supprimer et tester des objets en live.

---

## 🧩 Structure du projet

### I. Initialisation : capture vidéo et setup de l’interface
- Création d’une mini-application capable de lire un flux vidéo en temps réel.
- Interface utilisateur basée sur des raccourcis clavier (`f`, `v`, `b`, `a`, `r`, `n`, `d`).

### II. Histogrammes de couleurs
- Représentation des couleurs par histogramme dans l’espace RGB.
- Calcul de **distance** entre deux histogrammes pour comparaison.
- Mise en place de fonctions de **vérification** de cohérence.

### III. Comparaison fond / objet
- Apprentissage d’une distribution pour le **fond**.
- Apprentissage d’une distribution pour un **objet** (ex : visage).
- Détection des différences par comparaison d’histogrammes.

### IV. Mode reconnaissance
- Passage en **mode "détection"** après apprentissage.
- Identification des zones de l’image correspondant aux objets connus.
- Vérification visuelle dans l’interface.

### V. Développements supplémentaires
- Ajout de plusieurs objets avec chacun leur distribution propre.
- Optimisation des performances pour **un meilleur framerate**.
- Gestion dynamique de la mémoire (possibilité de **supprimer le dernier objet enregistré**).

---

## 🧪 Mode d’utilisation (contrôles clavier)

| Touche | Fonction                                 |
|--------|------------------------------------------|
| `f`    | Apprendre la distribution du fond        |
| `v`    | Apprendre la distribution du visage      |
| `b`    | Apprendre un nouvel objet                |
| `a`    | Ajouter un objet supplémentaire          |
| `r`    | Reconnaître les objets appris            |
| `n`    | Afficher l’objet suivant                 |
| `d`    | Supprimer le dernier objet mémorisé      |

---

## 🛠️ Technologies & outils utilisés

- **Langage :** C/C++ 
- **IDE :** Visual Studio Solution
- **Capture vidéo :** OpenCV 
- **Gestion des couleurs :** Histogrammes RGB

---

## 💡 Compétences démontrées

- Programmation et **gestion mémoire en C**
- Implémentation de fonctionnalités temps réel en traitement d’image
- Maîtrise des notions de **segmentation par histogrammes**
- Création d’une **interface utilisateur interactive sans surcouche graphique externe**
- Optimisation de code pour l’exécution en live

---

## 📸 Captures d’écran & résultats

Tout est disponible dans le rapport.

---

## 🎓 Contexte académique

Projet réalisé dans le cadre du **Master Modélisation Mathématique et Analyse Appliquée** (MMAA)  
et du **CMI en Mathématiques Appliquées** à l’Université Savoie Mont Blanc.

---

## 🤝 Envie d’en discuter ?

Intéressé par les algos de reconnaissance temps réel, la segmentation couleur ou les projets C de bas niveau ? N'hésitez pas à me contacter ou à ouvrir une *issue* !
