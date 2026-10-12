#include "../ortak.hpp"

int main(int argc, char** argv) {
    try {
        Mat gri = griOku(argc, argv);
        Mat gauss = yumusat(gri);
        Mat sonuc = laplacian(gauss);
        kaydet("gri", gri);
        kaydet("gauss", gauss);
        kaydet("laplacian_mutlak", sonuc);
        kaydet("laplacian_kenarlar", sifirGecisleri(sonuc, 20));
        karsilastir(gri, {"Prewitt", "Sobel", "Laplacian"});
        cout << "Sonuclar sonuclar klasorune kaydedildi.\n";
        return 0;
    } catch (const exception& hata) {
        cerr << hata.what() << '\n';
        return 1;
    }
}
