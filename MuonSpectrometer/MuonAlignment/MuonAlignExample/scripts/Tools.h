/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOOLS_H
#define TOOLS_H

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <valarray>

#include "MuonFixedId.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TStyle.h"

class Tools {
public:
    /** default constructor */
    Tools(){};
    static std::string getChamberName(MuonCalib::MuonFixedId* mid);
    static double* getPosition(std::string filename, MuonCalib::MuonFixedId* mid);
    static void setStyle();

    template <class MyType> static inline std::vector<MyType> getOptionVector(TString);

private:
};

inline std::string Tools::getChamberName(MuonCalib::MuonFixedId* mid) {
    int eta = mid->eta();
    int phi = mid->phi();
    int station = mid->stationName();
    string statname = mid->stationNumberToFixedStationString(station);

    stringstream chambername;
    chambername << statname << std::abs(eta);
    if (eta < 0)
        chambername << "C";
    else
        chambername << "A";
    int detphi = 2 * phi;
    if (statname.substr(2, 1) == "L")
        detphi--;
    else if (statname.substr(2, 1) != "S")
        detphi = phi;
    chambername << detphi;

    return chambername.str();
}

inline double* Tools::getPosition(std::string filename, MuonCalib::MuonFixedId* mid) {
    double* pos = new double[6];
    for (int i = 0; i < 6; i++) { pos[i] = 0.; }

    ifstream file(filename.c_str());
    if (!file) {
        cout << "Can't open AlignmentConstantsFile" << endl;
        return 0;
    }

    int eta = mid->eta();
    int phi = mid->phi();
    int station = mid->stationName();
    string name = mid->stationNumberToFixedStationString(station);

    char in_str[255], fname[3];
    int fphi, feta, fjob;
    float fpos0, fpos1, fpos2, fpos3, fpos4, fpos5;
    while (file) {
        file.getline(in_str, 255);
        if (file) {
            sscanf(in_str, "%*c %s %i%i%i %f%f%f%f%f%f", fname, &fphi, &feta, &fjob, &fpos0, &fpos1, &fpos2, &fpos3, &fpos4, &fpos5);

            if (fphi != phi) continue;
            if (feta != eta) continue;
            if (fname != name) continue;

            pos[0] = fpos0;
            pos[1] = fpos1;
            pos[2] = fpos2;
            pos[3] = fpos3 * 1000;
            pos[4] = fpos4 * 1000;
            pos[5] = fpos5 * 1000;
        }
    }
    file.close();

    return pos;
}

inline void Tools::setStyle() {
    // The original rootlogon.c
    // from: http://atlas.web.cern.ch/Atlas/GROUPS/GENERAL/SCINOTES/templates/root/rootlogon.C

    TStyle* atlasStyle = new TStyle("ATLAS", "Atlas style");

    // use plain black on white colors
    Int_t icol = 0;
    atlasStyle->SetFrameBorderMode(icol);
    atlasStyle->SetCanvasBorderMode(icol);
    atlasStyle->SetPadBorderMode(icol);
    atlasStyle->SetPadColor(icol);
    atlasStyle->SetCanvasColor(icol);
    atlasStyle->SetStatColor(icol);

    // atlasStyle->SetFillColor(icol); //no hitso fill color if this is set
    atlasStyle->SetLegendBorderSize(icol);

    // set the paper & margin sizes
    atlasStyle->SetPaperSize(20, 26);
    atlasStyle->SetPadTopMargin(2.);  // 0.05
    atlasStyle->SetPadRightMargin(0.05);
    atlasStyle->SetPadBottomMargin(0.16);
    atlasStyle->SetPadLeftMargin(0.12);

    Int_t font = 42;
    // Double_t tsize=0.05;
    // increased size a bit
    Double_t tsize = 0.06;

    atlasStyle->SetTextFont(font);

    atlasStyle->SetTextSize(tsize);
    atlasStyle->SetLabelFont(font, "x");
    atlasStyle->SetTitleFont(font, "x");
    atlasStyle->SetLabelFont(font, "y");
    atlasStyle->SetTitleFont(font, "y");
    atlasStyle->SetLabelFont(font, "z");
    atlasStyle->SetTitleFont(font, "z");

    atlasStyle->SetLabelSize(tsize, "x");
    atlasStyle->SetTitleSize(tsize, "x");
    atlasStyle->SetLabelSize(tsize, "y");
    atlasStyle->SetTitleSize(tsize, "y");
    atlasStyle->SetLabelSize(tsize, "z");
    atlasStyle->SetTitleSize(tsize, "z");

    // use bold lines and markers
    atlasStyle->SetMarkerSize(1.2);                // 0.8,1.2
    atlasStyle->SetHistLineWidth(2);               // 2.
    atlasStyle->SetLineStyleString(2, "[12 12]");  // postscript dashes

    atlasStyle->SetFrameFillStyle(0);
    atlasStyle->SetHistFillStyle(0);
    atlasStyle->SetStatStyle(0);
    atlasStyle->SetTitleFillColor(0);
    atlasStyle->SetTitleBorderSize(1);
    // atlasStyle->SetHistFillColor(0);

    // get rid of X error bars and y error bar caps
    // atlasStyle->SetErrorX(0.001);

    // do not display any of the standard histogram decorations
    // atlasStyle->SetOptTitle(0);
    // atlasStyle->SetOptStat(1111);
    atlasStyle->SetOptStat(0);
    // atlasStyle->SetOptFit(1111);
    atlasStyle->SetOptFit(0);

    // put tick marks on top and RHS of plots
    atlasStyle->SetPadTickX(1);
    atlasStyle->SetPadTickY(1);

    // atlasStyle->SetPalette(1);
    // atlasStyle->SetPadBottomMargin(0.12);
    // atlasStyle->SetOptStat(221112211);
    // atlasStyle->SetOptFit(1111);
    // 0.20 instead of 0.2, same number of digits in axis
    // atlasStyle->SetStripDecimals(false);
    // change default canvas size
    // atlasStyle->SetCanvasDefH(500);
    // atlasStyle->SetCanvasDefW(500);
    // atlasStyle->SetPadLeftMargin(0.15);
    // atlasStyle->SetTitleYOffset(1.2);
    // atlasStyle->SetTitleXOffset(1.1);
    // atlasStyle->SetNdivisions(507,"xyz");
    // atlasStyle->SetNdivisions(508,"xyz");
    // atlasStyle->SetPadLeftMargin(0.14);

    // gROOT->SetStyle("Plain");
    gROOT->SetStyle("ATLAS");
}

template <class MyType> inline std::vector<MyType> Tools::getOptionVector(TString optstring) {
    optstring = optstring.ReplaceAll(" ", "");
    TObjArray* optarray = optstring.Tokenize(",");

    std::vector<MyType> optvec;
    for (int k = 0; k < optarray->GetEntries(); k++) {
        stringstream optstream;
        optstream << ((TObjString*)optarray->At(k))->GetString().Data();

        MyType opt;
        optstream >> opt;
        optvec.push_back(opt);
    }

    delete optarray;
    return optvec;
}

#endif  // TOOLS_H
