#include "../ortak.hpp"

int main(int argc, char** argv) {
    try {
        Mat gri = griOku(argc, argv);
        Mat gauss = yumusat(gri);
        Mat kenarlar;
        Canny(gauss, kenarlar, 50, 150, 3, true);
        kaydet("gri", gri);
        kaydet("gauss", gauss);
        kaydet("canny_kenarlar", kenarlar);
        karsilastir(gri, {"Prewitt", "Sobel", "Laplacian", "LoG", "Canny"});
        cout << "Sonuclar sonuclar klasorune kaydedildi.\n";
        return 0;
    } catch (const exception& hata) {
        cerr << hata.what() << '\n';
        return 1;
    }
}
