/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <TAxis.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TDecompBK.h>
#include <TEventList.h>
#include <TF1.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TLorentzVector.h>
#include <TMatrixDSym.h>
#include <TMatrixTSparse.h>
#include <TPaveText.h>
#include <TString.h>
#include <TTree.h>
#include <TVectorD.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "MuonFixedId.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLine.h"
#include "TROOT.h"
#include "TStyle.h"
#include "Tools.h"

using namespace MuonCalib;

map<TString, TH1*> getHistos(TChain* tree, TString combinetype);

MuonFixedId* mid;

int m_xbins;
double m_xmax;
double m_xmin;

void help(const char* argv0) {
    cerr << "Usage: " << argv0 << " [options] " << endl
         << "Options:" << endl
         << " -o :\tOutput path" << endl
         << " -i :\tInputfiles" << endl
         << " -l :\tLegend labels" << endl
         << " -c :\tHistogram colors " << endl
         << " -t :\tCombination type (Chamber,Tower,Sector,Side,BarrelEC,MS)"
         << " -h :\tShow this message and quit" << endl
         << "Multiple arguments for one option are passed via comma seperated lists (e.g. '-c 3,4')" << endl
         << endl;
    exit(1);
}

//_____________________________________________________________________________
int main(int argc, char* argv[]) {
    //./plotres_x -c 3,4 -i MuonAlign_BXS2A4_notshifted_1.root,MuonAlign_BXS2A4_a1notshifted_1.root -o pics -l Perfect,Misaligned

    //----------------------------
    //-- Parse the command line --
    //----------------------------

    extern char* optarg;
    char c;
    TString infilestr("");
    TString outputstr("");
    TString labelstr("");
    TString colorstr("");
    TString combinetype("Chamber");  // Chamber,Tower,Sector,Side,BarrelEC,MS

    while ((c = getopt(argc, argv, "-ho:i:l:c:t:")) != -1) {
        switch (c) {
            case 'o': outputstr = optarg; break;
            case 'i': infilestr = optarg; break;
            case 'l': labelstr = optarg; break;
            case 'c': colorstr = optarg; break;
            case 't': combinetype = optarg; break;
            default:
                cerr << "Help or unknown argument!" << endl;
                help(argv[0]);
                break;
        }
    }

    //---------------------------
    //-- Set and verify inputs --
    //---------------------------

    vector<TString> filenames = Tools::getOptionVector<TString>(infilestr);
    vector<TString> legendlabels = Tools::getOptionVector<TString>(labelstr);
    vector<int> colors = Tools::getOptionVector<int>(colorstr);
    TString directory = outputstr;

    if (filenames.size() == 0) {
        cerr << "no imput files given, exiting..." << endl;
        exit(1);
    }
    if (filenames.size() != legendlabels.size()) {
        cerr << "different number of inpufiles and legendnames," << endl << "exiting..." << endl;
        exit(1);
    }
    if (filenames.size() != colors.size()) {
        cerr << "different number of inpufiles and colors," << endl << "exiting..." << endl;
        exit(1);
    }

    //---------
    //-- Run --
    //---------

    TH1::SetDefaultSumw2();
    Tools::setStyle();
    // gStyle->SetOptStat(111);
    // gStyle->SetOptStat(0);

    // unsigned int m_combType = Chamber;
    m_xbins = 100;
    m_xmax = 0.5;
    m_xmin = -1 * m_xmax;

    bool saveplot = 1;

    //  std::string directory="";
    //   if (argc>1) {
    //     directory=string(argv[1]);
    //   }
    //   std::string iteration="0";
    //   if (argc>2) {
    //     iteration=string(argv[2]);
    //   }
    //   std::cout<<"using input directory "<<directory<<std::endl;

    //  stringstream rootname;
    //   if (BATCH)
    //     rootname<<directory<<"/MuonAlign_"<<directory<<"_b8_iter"<<iteration<<".root";
    //   else
    //     rootname<<directory<<"/MuonAlign_"<<directory<<"_"<<iteration<<".root";

    mid = new MuonFixedId(0);

    //  vector<TString> filenames;
    //   vector<TString> legendlabels;
    //   vector<int> colors;

    //   TString directory = "pics";

    //   //filenames.push_back("BXS2A4_notshifted/MuonAlign_BXS2A4_notshifted_1.root");
    //   //filenames.push_back("BXS2A4_a1notshifted/MuonAlign_BXS2A4_a1notshifted_1.root");
    //   filenames.push_back("MuonAlign_BXS2A4_notshifted_1.root");
    //   filenames.push_back("MuonAlign_BXS2A4_a1notshifted_1.root");

    //   legendlabels.push_back("Perfect");
    //   legendlabels.push_back("Misaligned");

    //   colors.push_back(3);
    //   colors.push_back(4);

    // get histos
    vector<map<TString, TH1*> > histMaps;
    vector<TCanvas*> cans;
    for (unsigned int k = 0; k < filenames.size(); k++) {
        TChain* tree = new TChain("MuonAlign");
        std::cout << "adding " << filenames[k] << std::endl;
        tree->Add(filenames[k]);
        int m_ntrees = tree->GetNtrees();
        std::cout << "have " << m_ntrees << " trees" << std::endl;

        histMaps.push_back(getHistos(tree, combinetype));
        delete tree;
    }

    // create legend
    double xmin = 0.65;
    double xmax = xmin + 0.3;
    double ymax = 0.87;
    double ymin = ymax - ((gStyle->GetTextSize() + 0.01) * (legendlabels.size() + 1));
    TLegend* leg = new TLegend(xmin, ymin, xmax, ymax);
    leg->SetFillStyle(0);
    leg->SetFillColor(0);
    for (unsigned int k = 0; k < legendlabels.size(); k++) {
        TH1* hist = histMaps[k].begin()->second;
        if (!hist) continue;
        leg->AddEntry(hist, legendlabels[k], "L");
    }
    // leg->SetTitle("test");
    // iterate over histos of first file
    TCanvas* can = new TCanvas("canv", "canv");
    map<TString, TH1*>::const_iterator histIt = histMaps[0].begin();
    for (; histIt != histMaps[0].end(); ++histIt) {
        can->Clear();

        TH1* firsthist = histIt->second;
        firsthist->SetLineColor(colors[0]);
        firsthist->SetTitle("");
        firsthist->Draw("Hist");
        TString regionname = histIt->first;

        // if(regionname!="BMS2A4") continue;
        // cout << "first hist entries: " <<  firsthist->GetMean() << endl;

        // get histos for other files and normalize to first histo
        for (unsigned int k = 1; k < filenames.size(); k++) {
            TH1* hist = histMaps[k][regionname];
            if (!hist) continue;
            // cout << "hist entries: " <<  hist->GetMean() << endl;
            // hist->Scale(firsthist->GetEntries()/hist->GetEntries());
            hist->SetLineColor(colors[k]);
            hist->Draw("Hist Same");
        }
        leg->SetHeader(regionname);
        leg->AppendPad();
        TString canvname = directory + "/residuals_" + regionname + ".eps";
        if (saveplot) can->Print(canvname);
    }

    // delete histos
    for (unsigned int k = 0; k < histMaps.size(); k++) {
        map<TString, TH1*>::const_iterator histIt = histMaps[k].begin();
        for (; histIt != histMaps[k].end(); ++histIt) { delete histIt->second; }
    }

    // rootname<<directory<<"/MuonAlign_"<<directory<<"_"<<iteration<<".root";
    // rootname<<directory<<"/MuonAlign_L1_MC_b0_iter0.root";

    // getDataFromTree(tree,fullArrayMap,xarrayMap,numTracks,chamberNames,goodParams);

    //   TCanvas* canv=new TCanvas("canv","canv");
    //   map<TString,TH1*>::const_iterator histIt = histMap.begin();
    //   for (; histIt!=histMap.end(); ++histIt) {
    //     canv->Clear();
    //     TH1* hist = histIt->second;
    //     hist->Draw();
    //     stringstream canvname;
    //     canvname<<directory<<"/"<<directory<<"_"<<hist->GetName()<<".eps";
    //     if(saveplot)  canv->Print(canvname.str().c_str());
    //   }
}

map<TString, TH1*> getHistos(TChain* tree, TString combineType) {
    static int histcounter;

    int evt;
    int nchambers;
    int* chamberId = new int[10];
    int nhits;
    double* res = new double[100];
    int* fixedid = new int[100];

    tree->SetBranchAddress("evt", &evt);
    tree->SetBranchAddress("nchambers", &nchambers);
    tree->SetBranchAddress("chamberid", chamberId);
    tree->SetBranchAddress("nhits", &nhits);
    tree->SetBranchAddress("fixedid", fixedid);
    tree->SetBranchAddress("res", res);

    map<TString, TH1*> histMap;

    for (int ientry = 0; ientry < tree->GetEntries(); ientry++) {
        tree->GetEntry(ientry);
        for (int ihit = 0; ihit < nhits; ihit++) {
            mid->setMuonIdentifier(fixedid[ihit]);
            string chamberName = Tools::getChamberName(mid);

            TString regionname = chamberName;
            if (combineType == "Tower")
                regionname.Replace(1, 1, "");
            else if (combineType == "Sector") {
                regionname.Replace(0, 5, "");
                regionname.Prepend("Sector");
            } else if (combineType == "Side") {
                regionname.Replace(0, 4, "");
                regionname.Resize(1);
                regionname.Prepend("Side");
            } else if (combineType == "BarrelEC")
                regionname.Resize(1);
            else if (combineType == "MS")
                regionname = "MS";

            // cout << "ihit: " << ihit << ", " << res[ihit]
            //    << ", regionname: " << regionname << endl;

            if (!histMap[regionname]) {
                TH1* hist = new TH1F("residuals_" + regionname + Form("_%i", histcounter), "Residuals (" + regionname + ")", m_xbins,
                                     m_xmin, m_xmax);
                hist->GetXaxis()->SetTitle("Residual [mm]");
                hist->GetYaxis()->SetTitle("Entries");
                histMap[regionname] = hist;
                histcounter++;
            }
            histMap[regionname]->Fill(res[ihit]);
        }
    }

    delete[] chamberId;
    delete[] res;
    delete[] fixedid;

    return histMap;
}
