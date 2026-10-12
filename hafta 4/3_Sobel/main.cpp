#include "../ortak.hpp"

int main(int argc, char** argv) {
    try {
        Mat gri = griOku(argc, argv);
        Mat gauss = yumusat(gri);
        Mat x = sobel(gauss, true);
        Mat y = sobel(gauss, false);
        kaydet("gri", gri);
        kaydet("gauss", gauss);
        kaydet("sobel_x", x, 0.25);
        kaydet("sobel_y", y, 0.25);
        kaydet("sobel_bileske", buyukluk(x, y), 0.25);
        cout << "Sonuclar sonuclar klasorune kaydedildi.\n";
        return 0;
    } catch (const exception& hata) {
        cerr << hata.what() << '\n';
        return 1;
    }
}
