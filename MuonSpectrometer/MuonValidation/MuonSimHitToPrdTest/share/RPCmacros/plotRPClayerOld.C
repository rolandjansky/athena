/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <TBox.h>
#include <TCanvas.h>
#include <TH2F.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

void plotRPClayerOld(Int_t inputLayer) {
    TCanvas* cLayer = new TCanvas("", "", 900, 700);
    Double_t hphimin = -M_PI - M_PI / 10.;
    Double_t hphimax = M_PI + M_PI / 10.;
    TH2F* hLayer = new TH2F("hLayer", "hLayer", 100, -1.07, 1.07, 100, hphimin, hphimax);
    hLayer->Draw();
    TBox* t = new TBox();
    t->SetLineWidth(2);
    t->SetLineColor(kBlack);
    t->SetFillStyle(0);

    std::string STRING;
    stringstream ss;
    ss << inputLayer;
    std::string laySTRING = ss.str();
    std::cout << " Will use only lines with Layer = " << laySTRING << std::endl;
    ss.clear();
    ifstream infile(
        "/afs/le.infn.it/user/s/spagnolo/atlas/rpcPerf/code/MuonSpectrometer/MuonGeoModelTest/share/rpc_current_panelID_R.07.01");
    int nPanels = 0;
    float etamin, etamax, phimin, phimax, zmin, zmax;
    std::string stName;
    std::string viewName;
    std::string stringId;
    int layerRPC, msIndex, stNameIndex, stEtaIndex, stPhiIndex, techIndex, dbrIndex, dbzIndex, dbpIndex, ggIndex, measphiIndex, stripIndex;
    unsigned long panelID;
    if (infile.is_open())  // if the file is open
    {
        while (!infile.eof())  // To get you all the lines.
        {
            ss.clear();
            getline(infile, STRING);  // Saves the line in STRING.
            if (STRING.substr(0, 1) != laySTRING) {
                continue;  // skip comments
            } else {
                ++nPanels;
                ss << STRING;
                std::cout << " STRING = <" << STRING << ">" << std::endl;

                ss >> layerRPC >> stName >> viewName >> stringId >> panelID >> etamin >> etamax >> phimin >> phimax >> zmin >> zmax;

                if (nPanels < 10)
                    std::cout << " Variables are <" << layerRPC << ",  " << stName << ", " << viewName << ", " << stringId << ", "
                              << panelID << " eta,phi range [" << etamin << ", " << etamax << "][" << phimin << ", " << phimax << "]"
                              << std::endl;

                t->DrawBox(etamin, phimin, etamax, phimax);
            }
            //	    if (nPanels>1) break;
        }

    } else {
        std::cout << "a problem with input file" << std::endl;
    }
    std::cout << "Finished; n. of panels found is " << nPanels << std::endl;
    cLayer->SaveAs((std::string("RPClayer") + laySTRING + std::string(".png")).c_str());
}
