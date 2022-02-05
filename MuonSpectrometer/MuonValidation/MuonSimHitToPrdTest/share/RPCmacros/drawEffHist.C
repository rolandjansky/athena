#include <TAxis.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TStyle.h>
void drawEffHist(TH1F* h, float xmin = -999, float xmax = 999) {
    // TCanvas* c = new TCanvas();
    if (xmin < -998. || xmax > 998.) {
        xmin = h->GetXaxis()->GetXmin();
        xmax = h->GetXaxis()->GetXmax();
    }
    gStyle->SetOptFit(111111);
    gStyle->SetOptStat(111111);
    h->SetFillColor(5);
    h->SetLineWidth(2);
    h->SetLineColor(kOrange);
    h->SetFillColor(5);
    h->Fit("gaus", "", "", xmin, xmax);
    //  c->SaveAs("hfit.png");
}
