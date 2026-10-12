#include "../ortak.hpp"

int main(int argc, char** argv) {
    try {
        Mat gri = griOku(argc, argv);
        Mat gauss = yumusat(gri);
        kaydet("prewitt_bileske", buyukluk(prewitt(gauss, true), prewitt(gauss, false)), 1.0 / 3.0);
        kaydet("sobel_bileske", buyukluk(sobel(gauss, true), sobel(gauss, false)), 0.25);
        karsilastir(gri, {"Prewitt", "Sobel"});
        cout << "Sonuclar sonuclar klasorune kaydedildi.\n";
        return 0;
    } catch (const exception& hata) {
        cerr << hata.what() << '\n';
        return 1;
    }
}
