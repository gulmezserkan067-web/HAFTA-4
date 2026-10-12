#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#ifndef __VERSION__
#define __VERSION__ "C++17 derleyicisi"
#endif

using namespace cv;
using namespace std;

Mat griOku(int argc, char** argv) {
    string yol = argc > 1 ? argv[1] : "duvar1.jpeg";
    Mat resim = imread(yol, IMREAD_GRAYSCALE);
    if (resim.empty()) throw runtime_error("Goruntu okunamadi: " + yol);
    return resim;
}

Mat yumusat(const Mat& resim) {
    Mat sonuc;
    GaussianBlur(resim, sonuc, Size(3, 3), 1.0, 1.0, BORDER_REFLECT_101);
    return sonuc;
}

Mat prewitt(const Mat& resim, bool xYonu) {
    Mat kx = (Mat_<float>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
    Mat ky = (Mat_<float>(3, 3) << -1, -1, -1, 0, 0, 0, 1, 1, 1);
    Mat sonuc;
    filter2D(resim, sonuc, CV_32F, xYonu ? kx : ky, Point(-1, -1), 0, BORDER_REFLECT_101);
    return sonuc;
}

Mat sobel(const Mat& resim, bool xYonu) {
    Mat sonuc;
    Sobel(resim, sonuc, CV_32F, xYonu ? 1 : 0, xYonu ? 0 : 1, 3, 1, 0, BORDER_REFLECT_101);
    return sonuc;
}

Mat buyukluk(const Mat& x, const Mat& y) {
    Mat sonuc;
    magnitude(x, y, sonuc);
    return sonuc;
}

Mat laplacian(const Mat& resim) {
    Mat cekirdek = (Mat_<float>(3, 3) << 0, 1, 0, 1, -4, 1, 0, 1, 0);
    Mat sonuc;
    filter2D(resim, sonuc, CV_32F, cekirdek, Point(-1, -1), 0, BORDER_REFLECT_101);
    return sonuc;
}

Mat logCekirdegi() {
    Mat k(7, 7, CV_32F);
    double sigma = 1.0;
    for (int y = -3; y <= 3; y++) {
        for (int x = -3; x <= 3; x++) {
            double r2 = x * x + y * y;
            k.at<float>(y + 3, x + 3) = float((r2 - 2 * sigma * sigma) /
                pow(sigma, 4) * exp(-r2 / (2 * sigma * sigma)));
        }
    }
    k -= mean(k)[0];
    k /= sum(abs(k))[0];
    return k;
}

Mat sifirGecisleri(const Mat& yanit, float esik) {
    Mat sonuc = Mat::zeros(yanit.size(), CV_8U);
    for (int y = 1; y < yanit.rows - 1; y++) {
        for (int x = 1; x < yanit.cols - 1; x++) {
            float merkez = yanit.at<float>(y, x);
            bool kenar = false;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    float komsu = yanit.at<float>(y + dy, x + dx);
                    if (((merkez <= 0 && komsu > 0) || (merkez >= 0 && komsu < 0)) &&
                        abs(merkez - komsu) >= esik && abs(merkez) <= abs(komsu)) kenar = true;
                }
            }
            if (kenar) sonuc.at<uchar>(y, x) = 255;
        }
    }
    return sonuc;
}

void kaydet(const string& ad, const Mat& resim, double olcek = 1.0) {
    filesystem::create_directories("sonuclar");
    Mat goruntu;
    if (resim.type() == CV_8U && olcek == 1.0) goruntu = resim;
    else convertScaleAbs(resim, goruntu, olcek);
    if (!imwrite("sonuclar/" + ad + ".png", goruntu)) throw runtime_error("Sonuc kaydedilemedi.");
}

Mat kenarHaritasi(const Mat& gri, const string& yontem, bool gauss) {
    Mat giris = gauss ? yumusat(gri) : gri;
    Mat yanit, kenar;
    if (yontem == "Prewitt" || yontem == "Sobel") {
        Mat x = yontem == "Prewitt" ? prewitt(giris, true) : sobel(giris, true);
        Mat y = yontem == "Prewitt" ? prewitt(giris, false) : sobel(giris, false);
        yanit = buyukluk(x, y) / (yontem == "Prewitt" ? 3.0 : 4.0);
        compare(yanit, 40, kenar, CMP_GE);
    } else if (yontem == "Laplacian") {
        kenar = sifirGecisleri(laplacian(giris), 20);
    } else if (yontem == "LoG") {
        static Mat k = logCekirdegi();
        filter2D(giris, yanit, CV_32F, k, Point(-1, -1), 0, BORDER_REFLECT_101);
        kenar = sifirGecisleri(yanit, 3);
    } else if (yontem == "Canny") {
        Canny(giris, kenar, 50, 150, 3, true);
    } else throw runtime_error("Bilinmeyen yontem.");
    return kenar;
}

double sureOlc(const function<Mat()>& islem) {
    Mat sonuc;
    for (int i = 0; i < 10; i++) sonuc = islem();
    vector<double> sureler;
    for (int i = 0; i < 101; i++) {
        auto baslangic = chrono::steady_clock::now();
        sonuc = islem();
        auto bitis = chrono::steady_clock::now();
        sureler.push_back(chrono::duration<double, milli>(bitis - baslangic).count());
    }
    sort(sureler.begin(), sureler.end());
    return sureler[sureler.size() / 2];
}

void karsilastir(const Mat& gri, const vector<string>& yontemler) {
    setNumThreads(1);
    filesystem::create_directories("sonuclar");
    ofstream tablo("sonuclar/olcumler.csv");
    tablo << "yontem,gauss,medyan_ms,kenar_pikseli,kenar_orani_yuzde\n";
    tablo << fixed << setprecision(6);
    for (const string& yontem : yontemler) {
        for (bool gauss : {false, true}) {
            auto islem = [&]() { return kenarHaritasi(gri, yontem, gauss); };
            Mat kenar = islem();
            double sure = sureOlc(islem);
            int sayi = countNonZero(kenar);
            string ad = yontem + (gauss ? "_gauss" : "_ham");
            kaydet(ad, kenar);
            tablo << yontem << ',' << (gauss ? "var" : "yok") << ',' << sure << ','
                  << sayi << ',' << 100.0 * sayi / gri.total() << '\n';
            cout << ad << ": " << sure << " ms\n";
        }
    }
    ofstream ortam("sonuclar/ortam.txt");
    ortam << "OpenCV: " << CV_VERSION << "\nDerleyici: " << __VERSION__
          << "\nGoruntu: " << gri.cols << "x" << gri.rows
          << "\nIs parcacigi: 1\nIsinma: 10\nTekrar: 101\nOzet: medyan\n";
}
