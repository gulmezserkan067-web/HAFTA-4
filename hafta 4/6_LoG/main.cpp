#include "../ortak.hpp"

int main(int argc, char** argv) {
    try {
        Mat gri = griOku(argc, argv);
        Mat cekirdek = logCekirdegi();
        Mat sonuc;
        filter2D(gri, sonuc, CV_32F, cekirdek, Point(-1, -1), 0, BORDER_REFLECT_101);
        kaydet("gri", gri);
        kaydet("log_mutlak", sonuc, 4.0);
        kaydet("log_kenarlar", sifirGecisleri(sonuc, 3));
        ofstream dosya("sonuclar/log_7x7.txt");
        dosya << fixed << setprecision(8) << cekirdek << '\n';
        karsilastir(gri, {"Prewitt", "Sobel", "Laplacian", "LoG"});
        cout << "Sonuclar sonuclar klasorune kaydedildi.\n";
        return 0;
    } catch (const exception& hata) {
        cerr << hata.what() << '\n';
        return 1;
    }
}
