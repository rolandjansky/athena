/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// #include <iostream>
#include <vector>
//#include <map>
#include "TH1F.h"
// #include "TString.h"
// #include "TFile.h"
// #include "TKey.h"
// #include "TList.h"

#include "/afs/cern.ch/user/l/limbach/ATLAS-Style/AtlasUtils.C"
#include "/afs/cern.ch/user/l/limbach/ATLAS-Style/AtlasLabels.C"

void ScanDirectory(TList* keyList, std::vector<TH1F*>* HistListToFill);


void dumpRTHContents() {
    gROOT->LoadMacro("/afs/cern.ch/user/l/limbach/ATLAS-Style/AtlasStyle.C");
    SetAtlasStyle();
    std::cout << "Setup..." << std::endl;
    
    TString InFile_NoVicinity   = "PanTau_RunTimeHists_NoVicinity.root";
    TString InFile_DoVicinityP  = "PanTau_RunTimeHists_DoVicinityP0.4.root";
    TString InFile_DoVicinityM  = "PanTau_RunTimeHists_DoVicinityM0.4.root";
    
    std::vector<TString>    Types;
    Types.push_back("NoVic_");
    Types.push_back("DoVicP_");
    Types.push_back("DoVicM_");
    
    std::vector<int> LineColors;
    LineColors.push_back(417);
    LineColors.push_back(632);
    LineColors.push_back(596);
    
    std::vector<int> LineWidths;
    LineWidths.push_back(2);
    LineWidths.push_back(2);
    LineWidths.push_back(2);
    
    std::vector<int> LineStyles;
    LineStyles.push_back(1);
    LineStyles.push_back(1);
    LineStyles.push_back(1);
    
    //For each inputfile, contains all histograms
    std::vector<TH1F*> Histograms_NoVicinity;//    = std::vector<TH1F*>(0);
    std::vector<TH1F*> Histograms_DoVicinityP;// = std::vector<TH1F*>(0);
    std::vector<TH1F*> Histograms_DoVicinityM;// = std::vector<TH1F*>(0);
    
    std:vector<TFile*> OpenInputFiles;//           = std::vector< TFile* >(0);
    OpenInputFiles.push_back( TFile::Open(InFile_NoVicinity) );
    OpenInputFiles.push_back( TFile::Open(InFile_DoVicinityP) );
    OpenInputFiles.push_back( TFile::Open(InFile_DoVicinityM) );
    
    
    //gather histograms
    std::cout << "Begin loop over inputfiles to gather histograms..." << std::endl;
    
    for(unsigned int iFile==0; iFile<OpenInputFiles.size(); iFile++) {
        
        TFile* curFile = OpenInputFiles[iFile];
        std::cout << "\tInFile opened at " << curFile << std::endl;
        if(curFile == 0) {
            std::cout << "Pointer to 0! Skip" << std::endl;
            continue;
        }
        
        //set right vector to fill
        std::vector<TH1F*>* HistListToFill = 0;
        if(iFile == 0) HistListToFill = &Histograms_NoVicinity;
        if(iFile == 1) HistListToFill = &Histograms_DoVicinityP;
        if(iFile == 2) HistListToFill = &Histograms_DoVicinityM;
        std::cout << "\tFilling histvector at " << HistListToFill << std::endl;
        
        //get list of keys and loop recursively through file contents
        TList* keyList = curFile->GetListOfKeys();
        
        ScanDirectory(keyList, HistListToFill);
        
        //finalize single hists
        //std::cout << "\tDumping histos in list: " << std::endl;
        for(int iHist=0; iHist<HistListToFill->size(); iHist++) {
            TH1F* curHist = HistListToFill->at(iHist);
            TString newName = Types.at(iFile) + curHist->GetName();
            curHist->SetName(newName);
            curHist->SetTitle(newName);
//             std::cout << "\t\t" << curHist->GetName() << std::endl;
            curHist->SetLineColor(LineColors[iFile]);
            curHist->SetLineWidth(LineWidths[iFile]);
            curHist->SetLineStyle(LineStyles[iFile]);
        }
        
    }//end loop over files
    
    
    //put histograms into canvasses
    double xStart = 0.54;
    double yStart = 0.70;
    TLegend* theLegend = new TLegend(xStart, yStart, 0.93, 0.93);
    theLegend->SetFillColor(0);
    theLegend->SetFillStyle(0);
    theLegend->SetBorderSize(0);
    std::vector<TCanvas*> CanvasList;
    std::cout << "Begin loop over histograms to fill canvasses..." << std::endl;
    for(unsigned int iCan=0; iCan<Histograms_NoVicinity.size(); iCan++) {
        TString curHistName = HistListToFill->at(iCan)->GetName();
        std::cout << "\tFilling canvas for hist: " << curHistName << std::endl;
        
        //fill the canvas
        TCanvas* curCan = new TCanvas(curHistName, curHistName);
        curCan->cd();
        Histograms_NoVicinity.at(iCan)->Draw();
        Histograms_DoVicinityM.at(iCan)->Draw("same");
        Histograms_DoVicinityP.at(iCan)->Draw("same");
        
        //complete the legend
        if(iCan == 0) {
            theLegend->AddEntry(Histograms_NoVicinity.at(iCan),     "All tauRec candidates", "l");
            theLegend->AddEntry(Histograms_DoVicinityM.at(iCan),    "Isolated tauRecs (in #Delta R = 0.4)", "l");
            theLegend->AddEntry(Histograms_DoVicinityP.at(iCan),    "tauRec with other tauRec in 0.4", "l");
        }
        
        CanvasList.push_back(curCan);
        
    }
    
    
    std::cout << "Saving to outputfile" << std::endl;
    TFile* outfile = new TFile("dumpedRTHHists.root", "RECREATE");
    outfile->cd();
    for(unsigned int iSave=0; iSave<CanvasList.size(); iSave++) {
        CanvasList.at(iSave)->cd();
        theLegend->Draw();
        CanvasList.at(iSave)->Write();
    }
    
    return;
}





void ScanDirectory(TList* keyList, std::vector<TH1F*>* HistListToFill) {
    //loop through key list
    for(int iKey=0; iKey<keyList->GetEntries(); iKey++) {
        TKey* curKey      = (TKey*)keyList->At(iKey);
        TString itsClass  = curKey->GetClassName();
//         std::cout << "DEBUG\t\tKey: " << curKey->GetName() << " / " << curKey->GetTitle() << " ClassName: |" << itsClass << "|" << std::endl;

        //if a key is a directory, get its list of keys and loop through that list
        if( itsClass == "TDirectoryFile") {
            TDirectoryFile* nextDir = (TDirectoryFile*)curKey->ReadObj();
//             std::cout << "DEBUG\t\t\tThis key is a TDirectory! It is named/titled: " << nextDir->GetName() << " / " << nextDir->GetTitle() << std::endl;

            TList* nextKeyList = nextDir->GetListOfKeys();
//             std::cout << "DEBUG\t\t\tKeys in the next dir " << nextKeyList->GetEntries() << std::endl;

            //scan next subdir
            ScanDirectory(nextKeyList, HistListToFill);
        }

        //if it is a TH1, put it into list of histograms
        if( itsClass == "TH1F") {
            TH1F* curHist = (TH1F*)curKey->ReadObj();

//             std::cout << "DEBUG\t\t\tThis key is a TH1! It is named/titled: " << curHist->GetName() << " / " << curHist->GetTitle() << std::endl;
            HistListToFill->push_back(curHist);
        }

    } // end loop over keys
    return;
}













