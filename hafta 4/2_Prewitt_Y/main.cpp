#include "../ortak.hpp"

int main(int argc, char** argv) {
    try {
        Mat gri = griOku(argc, argv);
        Mat gauss = yumusat(gri);
        Mat sonuc = prewitt(gauss, false);
        kaydet("gri", gri);
        kaydet("gauss", gauss);
        kaydet("prewitt_y", sonuc, 1.0 / 3.0);
        cout << "Sonuclar sonuclar klasorune kaydedildi.\n";
        return 0;
    } catch (const exception& hata) {
        cerr << hata.what() << '\n';
        return 1;
    }
}
