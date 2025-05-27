// Projet903 Traitement et Analyse d'images.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <cstdio>
#include <iostream>
#include <algorithm>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

struct ColorDistribution {
    float data[8][8][8]; // l'histogramme
    int nb = 0;              // le nombre d'échantillons

    ColorDistribution() { reset(); }
    ColorDistribution& operator=(const ColorDistribution& other) = default;
    // Met à zéro l'histogramme    
    void reset() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                for (int k = 0; k < 8; k++) {
                    data[i][j][k] = 0;
                }
            }
        }
    };
    // Ajoute l'échantillon color à l'histogramme:
    // met +1 dans la bonne case de l'histogramme et augmente le nb d'échantillons
    void add(Vec3b color) {
        // Vec3b contient 3 entiers entre 0 et 255 la couleur du pixel courant, dans le carré blanc de l'image
        // On souhaite convertir ces valeurs entre 0 et 8, pour incrémenter de 1 la case correspondante dans data.
        data[int(floor(color[0] / 32))][int(floor(color[1] / 32))][int(floor(color[2] / 32))]++;
        nb++;
    };
    // Indique qu'on a fini de mettre les échantillons:
    // divise chaque valeur du tableau par le nombre d'échantillons
    // pour que case représente la proportion des picels qui ont cette couleur.
    void finished() {
        //normalisation des valeurs des histogrammes, en divisant par le nombre d'échantillons:
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                for (int k = 0; k < 8; k++) {
                    data[i][j][k] = data[i][j][k] / nb;
                }
            }
        }
    };
    // Retourne la distance entre cet histogramme et l'histogramme other
    float distance(const ColorDistribution& other) const {
        float resultat = 0;
        // Calcul de la distance du CHI-2 entre les histogrammes:
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                for (int k = 0; k < 8; k++) {
                    if ((data[i][j][k] + other.data[i][j][k]) > 0) {
                        resultat = resultat + ((data[i][j][k] - other.data[i][j][k]) * (data[i][j][k] - other.data[i][j][k])) / (data[i][j][k] + other.data[i][j][k]);
                    }
                }
            }
        }
        return resultat;
    };
};

ColorDistribution getColorDistribution(Mat input, Point pt1, Point pt2) {
    ColorDistribution cd;
    for (int y = pt1.y; y < pt2.y; y++)
        for (int x = pt1.x; x < pt2.x; x++)
            cd.add(input.at<Vec3b>(y, x));
    cd.finished();
    return cd;
};

float minDistance(const ColorDistribution& h, const std::vector< ColorDistribution >& hists) {
    float dMin = h.distance(hists[0]);
    for (int i = 1; i < hists.size(); i++) {
        float newD = h.distance(hists[i]);
        if (newD < dMin) {
            dMin = newD;
        }
    }
    return dMin;
};

Mat recoObject(Mat input,
    const std::vector< ColorDistribution >& col_hists, /*< les distributions de couleurs du fond */
    const std::vector<std::vector< ColorDistribution >>& all_col_hists_objects, /*< les distributions de couleurs de l'objet */
    const std::vector< Vec3b >& colors, /*< les couleurs pour fond/objet */
    const int bloc_taille /*< taille de chaque bloc, 16 si 16x16 */) {
    Mat img_output = Mat::zeros(input.size(), input.type());
    for (int y = 0; y <= input.rows - bloc_taille; y += bloc_taille) { // On met -bloc_taille, afin de s'assurer qu'aucun bloc ne sort du cadre.
        for (int x = 0; x <= input.cols - bloc_taille; x += bloc_taille) {
            //calcule ColorDistribution sur le bloc(x, y) -> (x + bbloc_taille, y + bloc_taille):
            Rect bloc(x, y, bloc_taille, bloc_taille);
            ColorDistribution cdBloc = getColorDistribution(input, Point(x, y), Point(x + bloc_taille, y + bloc_taille));

            //On calcule la distance minimale pour chaque distributions, du fond et des objets, par rapport au bloc courant:
            float dFond = minDistance(cdBloc, col_hists);
            int IndexClosestObject = -1;
            float distMin = dFond;
            for (int i = 0; i < all_col_hists_objects.size(); i++) {
                float dist = minDistance(cdBloc, all_col_hists_objects[i]);
                if (dist < distMin) {
                    distMin = dist;
                    IndexClosestObject = i;
                }
            }
            //on range la couleur correspondant à l'étiquetage du bloc dans le vecteur de triplet RGB colors:
            if (IndexClosestObject == -1) {
                //cout<< "rouge" << endl;
                rectangle(img_output, bloc, colors[0], FILLED);
            }
            else {
                if (all_col_hists_objects.size() <= colors.size() + 1) {
                    rectangle(img_output, bloc, colors[IndexClosestObject + 1], FILLED);
                }
                else {
                    cout << "Pas assez de couleurs enregistres, par rapport au nombre d'objets en mémoire." << endl;
                }
            }
        }
    }
    return img_output;
};

// Vérifie si la distribution de couleur du nouvel objet est suffisamment distincte de celles des autres objets
bool isDistinct(const ColorDistribution& newHist, const vector<ColorDistribution>& existingObjectHists, float threshold) {
    for (const auto& hist : existingObjectHists) {
        if (newHist.distance(hist) < threshold) {
            // La nouvelle distribution est trop proche d'une distribution existante
            return false;
        }
    }
    // La nouvelle distribution est suffisamment distincte
    return true;
};

bool isDistinctInObject(const ColorDistribution& newHist, const vector<ColorDistribution>& currentObjectHists, float threshold) {
    for (const auto& hist : currentObjectHists) {
        if (newHist.distance(hist) < threshold) {
            // La nouvelle distribution est trop proche d'une distribution existante de l'objet courant
            return false;
        }
    }
    // La nouvelle distribution est suffisamment distincte des distributions de l'objet courant
    return true;
};

int main(int argc, char** argv)
{
    Mat img_input, img_seg, img_d_bgr, img_d_hsv, img_d_lab;
    VideoCapture* pCap = nullptr;
    const int width = 640;
    const int height = 480;
    const int size = 50;
    // Ouvre la camera
    pCap = new VideoCapture(0);
    if (!pCap->isOpened()) {
        cout << "Couldn't open image / camera ";
        return 1;
    }
    // Force une camera 640x480 (pas trop grande).
    pCap->set(CAP_PROP_FRAME_WIDTH, 640);
    pCap->set(CAP_PROP_FRAME_HEIGHT, 480);
    (*pCap) >> img_input;
    if (img_input.empty()) return 1; // probleme avec la camera
    Point pt1(width / 2 - size / 2, height / 2 - size / 2);
    Point pt2(width / 2 + size / 2, height / 2 + size / 2);
    std::vector<ColorDistribution> col_hists; // histogrammes du fond
    std::vector<std::vector<ColorDistribution>> all_col_hists_objects; // ensemble des histogrammes des objets
    std::vector<ColorDistribution> newObjectHist; // Histogramme de l'objet courant.
    // Pour le mode reconnaissance:
    std::vector< Vec3b > colors;
    colors.push_back(Vec3b(0, 0, 0)); // Noir pour le Fond.
    colors.push_back(Vec3b(0, 0, 255)); // Rouge pour l'Objet 1.
    colors.push_back(Vec3b(0, 255, 0)); // Vert pour l'Objet 2.
    colors.push_back(Vec3b(255, 0, 0)); // Bleu pour l'Objet 3.
    colors.push_back(Vec3b(255, 0, 255)); // Violet pour l'Objet 4.
    const float threshold = 0.05;
    namedWindow("input", 1);
    imshow("input", img_input);
    bool freeze = false;
    bool reconnaissance = false;
    while (true)
    {
        char c = (char)waitKey(50); // attend 50ms -> 20 images/s
        if (pCap != nullptr && !freeze)
            (*pCap) >> img_input;     // récupère l'image de la caméra
        if (c == 27 || c == 'q')  // permet de quitter l'application
            break;
        if (c == 'f') // permet de geler l'image
            freeze = !freeze;
        cv::rectangle(img_input, pt1, pt2, Scalar({ 255.0, 255.0, 255.0 }), 1);
        imshow("input", img_input); // affiche le flux video
        if (c == 'v' && !freeze) { // test le calcule de la distance chi2
            Point p1(0, 0);
            Point p2(width / 2, height - 1);
            Point p3(width / 2, 0);
            Point p4(width - 1, height - 1);
            ColorDistribution cd1 = getColorDistribution(img_input, p1, p2);
            ColorDistribution cd2 = getColorDistribution(img_input, p3, p4);
            //ICI ajouter la suite pour calculer la distance entre les histogrammes.
            float res = cd1.distance(cd2);
            cout << "distance du chi2:" << res << endl;
        }
        if (c == 'b' && !freeze) {//Calcule des histogrammes de couleurs, sur les différentes parties de l'image et les mémorizes.
            const int bbloc = 128;
            for (int y = 0; y <= height - bbloc; y += bbloc) {
                for (int x = 0; x <= width - bbloc; x += bbloc) {
                    //calcule ColorDistribution sur le bloc(x, y) -> (x + bbloc, y + bbloc):
                    Point ptHautGauche(x, y);
                    Point ptBasDroite(x + bbloc, y + bbloc);
                    ColorDistribution cdBloc = getColorDistribution(img_input, ptHautGauche, ptBasDroite);
                    //le mémorise dans col_hists:
                    col_hists.push_back(cdBloc);
                }
            }
            cout << "Histogrammes de fond, taille: " << col_hists.size() << endl;
            int nb_hists_background = col_hists.size();
        }
        if (c == 'a' && !freeze) { // Touche pour ajouter une distribution à l'objet courant
            // création d'une nouvelle distribution de couleur pour l'objet courant
            ColorDistribution newHist = getColorDistribution(img_input, pt1, pt2); // Supposons que vous avez déjà calculé la nouvelle distribution

            // Première étape : vérifier par rapport aux autres objets mémorisés
            bool isDistinctFromOthers = true;
            for (const auto& otherObjectHists : all_col_hists_objects) {
                if (!isDistinct(newHist, otherObjectHists, threshold)) {
                    isDistinctFromOthers = false;
                    break; // La nouvelle distribution est trop proche d'une des distributions des autres objets
                }
            }

            if (isDistinctFromOthers) {
                // Seconde étape : vérifier par rapport aux distributions de l'objet courant
                if (isDistinctInObject(newHist, newObjectHist, threshold)) {
                    // Si la distribution est distincte, on l'ajoute à l'objet courant
                    newObjectHist.push_back(newHist);
                    cout << "Distribution ajoutee a l objet courant." << endl;
                }
                else {
                    cout << "La distribution est trop similaire a une existante dans l'objet courant. Pas d'ajout." << endl;
                }
            }
            else {
                cout << "La distribution est trop similaire à une distribution d'un autre objet. Pas d'ajout." << endl;
            }
        }
        Mat output = img_input;
        if (c == 'r' && !freeze) {
            reconnaissance = !reconnaissance;
        }
        if (c == 'n') { // Touche pour ajouter un nouvel histogramme d’objet
            // S'il n'est pas vide, on ajoute le nouveau vecteur de distribution de couleurs, correspondant au nouvel objet.
            if (newObjectHist.size() > 0) {
                all_col_hists_objects.push_back(newObjectHist);
                cout << "Ajout du nouvel objet" << endl;
                cout << "Nombres d'objets memorises :" << all_col_hists_objects.size() << endl;
            }
            // On vide l'objet newObjectHist:
            newObjectHist.clear();
            cout << "contenu de newObjectHist :" << newObjectHist.size() << endl;
        }
        if (c == 'd') { // Touche pour supprimer le dernier objet enregistre.
            // Check s'il y a au moins un objet memorise
            if (all_col_hists_objects.size() > 0) {
                // Supprime le dernier objet memorise
                all_col_hists_objects.pop_back();
                cout << "Dernier objet supprime." << endl;
            }
            else {
                cout << "Aucun objet à supprimer." << endl;
            }
        }
        if (reconnaissance == true) { // mode reconnaissance
            Mat reco = recoObject(img_input, col_hists, all_col_hists_objects, colors, 16);
            output = 0.5 * reco + 0.5 * img_input; // mélange reco + caméra
            imshow("input", output);
        }
        else {
            cv::rectangle(img_input, pt1, pt2, Scalar({ 255.0, 255.0, 255.0 }), 1);
            imshow("input", output);
        }
    }
    return 0;
}