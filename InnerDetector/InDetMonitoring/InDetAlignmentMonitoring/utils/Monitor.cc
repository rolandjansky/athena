/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////
#include <include/Monitor.h>
#include <include/style.cc>
#include <macros/residuals.cc>
#include <macros/hits.cc>
#include <macros/hitCorrelations.cc>
#include <macros/hitsPerLayer.cc>
#include <macros/pixelHitmap.cc>
#include <macros/sctBarrelHitmap.cc>
#include <macros/sctECHitmap.cc>
#include <macros/pixelResiduals.cc>
#include <macros/sct_trt_Residuals.cc>
#include <macros/pixelBarrelRes.cc>
#include <macros/pixelBarrelResVsPt.cc>
#include <macros/pixelECRes.cc>
#include <macros/pixelECResVsPt.cc>
#include <macros/sctBarrelRes.cc>
#include <macros/sctBarrelResVsPt.cc>
#include <macros/sctECRes.cc>
#include <macros/sctECResVsPt.cc>
#include <macros/pixelResmaps.cc>
#include <macros/sctBarrelResmaps.cc>
#include <macros/sctECResmaps.cc>
#include <macros/trkParameters.cc>
#include <macros/trkMomentum.cc>
#include <macros/errD0.cc>
#include <macros/errD0vsPhi.cc>
#include <macros/errPt.cc>
#include <macros/errPhi0.cc>
#include <macros/errCotTheta.cc>
#include <macros/errZ0.cc>
#include <macros/CosmicSegments.cc>
#include <macros/SymmetricYaxis.cc>
//=====================================================================
//
//  monitor
//
//=====================================================================

void Monitor(TString inputFileName, TString inputFirstTrackCollection, TString inputSecondTrackCollection) {
    
    gROOT->SetBatch(kTRUE);
    AtlasStyle = false;
    normalizePlots = false;
    showFits = true;
    PrintLevel = 3;
    
    FileName = inputFileName;
    FirstTrackCollection = inputFirstTrackCollection;
    SecondTrackCollection = inputSecondTrackCollection;
    CosmicSegmentsCollection = ""; //"Tracks_NoTriggerSelection";
    
    cout << "File selected: " << FileName << endl;
    cout << "Track collection 1: " << FirstTrackCollection << endl;
    if (SecondTrackCollection != "") cout << "Track collection 2: " << SecondTrackCollection << endl;
    if (CosmicSegmentsCollection != "") 	cout << " Cosmic Segments collection " << CosmicSegmentsCollection.Data() << endl;
    
    
    if (!OpenFile()) return;
    setStyle();
    cout << " <Monitor> calling fillSummary() " << endl;
    fillSummary();
    cout << " <Monitor> calling drawPlots() " << endl;
    drawPlots();
    cout << " <Monitor> calling printPS() " << endl;
    printPS();
    
    return;
}

///////////////////////////////////////////////////////////////////
bool OpenFile(){
    cout << "Opening file" << endl;
    file = new TFile(FileName);
    TDirectory *IDAlignMonDir = file->GetDirectory("IDAlignMon");
    TString tmpName;
    tmpName = "IDAlignMon/"+FirstTrackCollection;
    TDirectory *FirstTrkSelDir = file->GetDirectory(tmpName);
    tmpName = "IDAlignMon/"+SecondTrackCollection;
    TDirectory *SecondTrkSelDir = file->GetDirectory(tmpName);
    
    if (IDAlignMonDir==NULL) {
        cout << "Error opening file!!" << endl;
        return false;
    }
    if  (FirstTrkSelDir==NULL) {
        cout << "Error opening track collection " <<  FirstTrackCollection << endl;
        return false;
    }
    if  (SecondTrkSelDir==NULL) {
        cout << "Error opening track collection " <<  SecondTrackCollection << endl;
        return false;
    }
    cout << "File opened successfully." << endl;
    return true;
}

///////////////////////////////////////////////////////////////////
void drawPlots(){
    bool printprogressreport = true;
    
    if (PrintLevel>0) printprogressreport = true;
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling:  residuals(" << FirstTrackCollection << "); " << endl;
    residuals(FirstTrackCollection);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling:  hits(" << FirstTrackCollection
        << ", " << SecondTrackCollection << "); " << endl;
    hits(FirstTrackCollection, SecondTrackCollection);
    
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling:  hitCorrelations(" << FirstTrackCollection
        << ", " << SecondTrackCollection << "); " << endl;
    hitCorrelations(FirstTrackCollection,SecondTrackCollection);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelHitMaps(" << FirstTrackCollection <<") " << endl;
    cPIXHitMap1 = new TCanvas("cPIXHitMap1","Pixel Hitmaps 1",CanvasSizeX6[0],CanvasSizeX6[1]);
    pixelHitmaps(FirstTrackCollection,cPIXHitMap1,"");
    
    if (SecondTrackCollection != "") {
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelHitMaps(" << SecondTrackCollection <<")  " << endl;
        cPIXHitMap2 = new TCanvas("cPIXHitMap2","Pixel Hitmaps 2",CanvasSizeX6[0],CanvasSizeX6[1]);
        pixelHitmaps(SecondTrackCollection,cPIXHitMap2,"");
    }
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelHitMap(" << FirstTrackCollection <<") " << endl;
    cSCTHitMapBa1 = new TCanvas("cSCTHitMapBa1","SCT Barrel Hitmaps 1",CanvasSizeX6[0],CanvasSizeX6[1]);
    sctBarrelHitmap(FirstTrackCollection,cSCTHitMapBa1,"");
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelHitMap(" << SecondTrackCollection <<") " << endl;
        cSCTHitMapBa2 = new TCanvas("cSCTHitMapBa2","SCT Barrel Hitmaps 2",CanvasSizeX6[0],CanvasSizeX6[1]);
        sctBarrelHitmap(SecondTrackCollection,cSCTHitMapBa2,"");
    }
        
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctHitMapEC(" << FirstTrackCollection <<") " << endl;
    cSCTHitMapEC1[0] = new TCanvas("cSCTHitMapECA_1","SCT Hitmaps ECA 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                   CanvasSizeX9[2], CanvasSizeX9[3]);
    sctECHitmap(FirstTrackCollection,0,cSCTHitMapEC1[0]);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctHitMapEC(" << FirstTrackCollection <<") " << endl;
    cSCTHitMapEC1[1] = new TCanvas("cSCTHitMapECC_1","SCT Hitmaps ECC 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                   CanvasSizeX9[2], CanvasSizeX9[3]);
    sctECHitmap(FirstTrackCollection,1,cSCTHitMapEC1[1]);
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctHitMapEC(" << SecondTrackCollection <<") " << endl;
        cSCTHitMapEC2[0] = new TCanvas("cSCTHitMapECA_2","SCT Hitmaps ECA 2",CanvasSizeX9[0],CanvasSizeX9[1],
                                   CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECHitmap(SecondTrackCollection,0,cSCTHitMapEC2[0]);
    }
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctHitMapEC(" << SecondTrackCollection <<") " << endl;
        cSCTHitMapEC2[1] = new TCanvas("cSCTHitMapECC_2","SCT Hitmaps ECC 2",CanvasSizeX9[0],CanvasSizeX9[1],
                                   CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECHitmap(SecondTrackCollection,1,cSCTHitMapEC2[1]);
    }
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: hitsPerLayer(" << FirstTrackCollection << ", " << SecondTrackCollection <<") " << endl;
    hitsPerLayer(FirstTrackCollection,SecondTrackCollection);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelResiduals(" << FirstTrackCollection << ") " << endl;
    cPixRes_1 = new TCanvas("PixResiduals_1","Pixel Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
    pixelResiduals(FirstTrackCollection, cPixRes_1);
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelResiduals(" << SecondTrackCollection << ") " << endl;
        cPixRes_2 = new TCanvas("PixResiduals_2","Pixel Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
        pixelResiduals(SecondTrackCollection, cPixRes_2);
    }
    
    // cPixRes_3 = new TCanvas("PixResiduals_3","Pixel Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
    // pixelResiduals(FirstTrackCollection, cPixRes_3, SecondTrackCollection);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sct_trt_Residuals(" << FirstTrackCollection << ") " << endl;
    cSctTRTRes_1 = new TCanvas("cSctTRTRes_1","SCT & TRT Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
    sct_trt_Residuals(FirstTrackCollection, cSctTRTRes_1);
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelResiduals(" << SecondTrackCollection << ") " << endl;
        cSctTRTRes_2 = new TCanvas("cSctTRTRes_2","SCT & TRT Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
        sct_trt_Residuals(SecondTrackCollection, cSctTRTRes_2);
    }
    
    // cSctTRTRes_3 = new TCanvas("cSctTRTRes_3","SCT & TRT Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
    // sct_trt_Residuals(FirstTrackCollection, cSctTRTRes_3, SecondTrackCollection);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelBarrelResiduals(" << FirstTrackCollection << ") " << endl;
    cPIXBarrelRes_1 = new TCanvas("cPIXBarrelRes_1","Pixel Barrel Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
    pixelBarrelResiduals(FirstTrackCollection, cPIXBarrelRes_1);
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelBarrelResiduals(" << SecondTrackCollection << ") " << endl;
        cPIXBarrelRes_2 = new TCanvas("cPIXBarrelRes_2","Pixel Barrel Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
        pixelBarrelResiduals(SecondTrackCollection, cPIXBarrelRes_2);
    }
    
    // cPIXBarrelRes_3 = new TCanvas("cPIXBarrelRes_3","Pixel Barrel Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
    // pixelBarrelResiduals(FirstTrackCollection, cPIXBarrelRes_3, SecondTrackCollection);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelBarrelResidualsVsPt(" << FirstTrackCollection << ") " << endl;
    cPIXBarrelResVsPt_1 = new TCanvas("cPIXBarrelResVsPt_1","Pixel Barrel Residuals vs Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
    pixelBarrelResidualsVsPt(FirstTrackCollection, cPIXBarrelResVsPt_1);
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelBarrelResidualsVsPt(" << SecondTrackCollection << ") " << endl;
        cPIXBarrelResVsPt_2 = new TCanvas("cPIXBarrelResVsPt_2","Pixel Barrel Residuals vs Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
        pixelBarrelResidualsVsPt(SecondTrackCollection, cPIXBarrelResVsPt_2);
    }
    
    // cPIXBarrelResVsPt_3 = new TCanvas("cPIXBarrelResVsPt_3","Pixel Barrel Residuals vs Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
    // pixelBarrelResidualsVsPt(FirstTrackCollection, cPIXBarrelResVsPt_3, SecondTrackCollection);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelECResiduals(" << FirstTrackCollection << ") " << endl;
    cPIXECARes_1 = new TCanvas("cPIXECARes_1","Pixel Endcap A Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
    pixelECResiduals(FirstTrackCollection, 0, cPIXECARes_1);
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelECResiduals(" << SecondTrackCollection << ") " << endl;
        cPIXECARes_2 = new TCanvas("cPIXECARes_2","Pixel Endcap A Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
        pixelECResiduals(SecondTrackCollection, 0, cPIXECARes_2);
    }
    
    // cPIXECARes_3 = new TCanvas("cPIXECARes_3","Pixel Endcap A Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
    // pixelECResiduals(FirstTrackCollection, 0, cPIXECARes_3, SecondTrackCollection);
    
    if (false) {
        cPIXECCRes_1 = new TCanvas("cPIXECCRes_1","Pixel Endcap C Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
        pixelECResiduals(FirstTrackCollection, 1, cPIXECCRes_1);
        if (SecondTrackCollection !=""){
            cPIXECCRes_2 = new TCanvas("cPIXECCRes_2","Pixel Endcap C Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
            pixelECResiduals(SecondTrackCollection, 1, cPIXECCRes_2);
        }
        if (SecondTrackCollection !=""){
            cPIXECCRes_3 = new TCanvas("cPIXECCRes_3","Pixel Endcap C Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
            pixelECResiduals(FirstTrackCollection, 1, cPIXECCRes_3, SecondTrackCollection);
        }
        if (SecondTrackCollection !=""){
        cPIXECAResVsPt_1 = new TCanvas("cPIXECAResVsPt_1","Pixel Endcap A Residuals Vs Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
        pixelECResidualsVsPt(FirstTrackCollection, 0, cPIXECAResVsPt_1);
        }
        if (SecondTrackCollection !=""){
            cPIXECAResVsPt_2 = new TCanvas("cPIXECAResVsPt_2","Pixel Endcap A Residuals Vs Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
            pixelECResidualsVsPt(SecondTrackCollection, 0, cPIXECAResVsPt_2);
        }
        if (SecondTrackCollection !=""){
            cPIXECAResVsPt_3 = new TCanvas("cPIXECAResVsPt_3","Pixel Endcap A Residuals Vs Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
            pixelECResidualsVsPt(FirstTrackCollection, 0, cPIXECAResVsPt_3, SecondTrackCollection);
        }
        cPIXECCResVsPt_1 = new TCanvas("cPIXECCResVsPt_1","Pixel Endcap C Residuals Vs Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
        pixelECResidualsVsPt(FirstTrackCollection, 1, cPIXECCResVsPt_1);
        if (SecondTrackCollection !=""){
            cPIXECCResVsPt_2 = new TCanvas("cPIXECCResVsPt_2","Pixel Endcap C Residuals Vs Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
            pixelECResidualsVsPt(SecondTrackCollection, 1, cPIXECCResVsPt_2);
        }
        if (SecondTrackCollection !=""){
            cPIXECCResVsPt_3 = new TCanvas("cPIXECCResVsPt_3","Pixel Endcap C Residuals Vs Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
            pixelECResidualsVsPt(FirstTrackCollection, 1, cPIXECCResVsPt_3, SecondTrackCollection);
        }
    }
    
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelResiduals(" << FirstTrackCollection << ") " << endl;
    cSCTBarrelRes_1 = new TCanvas("cSCTBarrelRes_1","SCT Barrel Residuals",CanvasSizeX4[0],CanvasSizeX4[1]);
    sctBarrelResiduals(FirstTrackCollection, cSCTBarrelRes_1);
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelResiduals(" << SecondTrackCollection << ") " << endl;
        cSCTBarrelRes_2 = new TCanvas("cSCTBarrelRes_2","SCT Barrel Residuals",CanvasSizeX4[0],CanvasSizeX4[1]);
        sctBarrelResiduals(SecondTrackCollection, cSCTBarrelRes_2);
    }
    
    // cSCTBarrelRes_3 = new TCanvas("cSCTBarrelRes_3","SCT Barrel Residuals",CanvasSizeX4[0],CanvasSizeX4[1]);
    // sctBarrelResiduals(FirstTrackCollection, cSCTBarrelRes_3, SecondTrackCollection);
    
    if (false) {
        cSCTBarrelResVsPt_1 = new TCanvas("cSCTBarrelResVsPt_1","SCT Barrel Residuals vs Pt",CanvasSizeX4[0],CanvasSizeX4[1]);
        sctBarrelResidualsVsPt(FirstTrackCollection, cSCTBarrelResVsPt_1);
        if (SecondTrackCollection !=""){
            cSCTBarrelResVsPt_2 = new TCanvas("cSCTBarrelResVsPt_2","SCT Barrel Residuals vs Pt",CanvasSizeX4[0],CanvasSizeX4[1]);
            sctBarrelResidualsVsPt(SecondTrackCollection, cSCTBarrelResVsPt_2);
        }
        if (SecondTrackCollection !=""){
            cSCTBarrelResVsPt_3 = new TCanvas("cSCTBarrelResVsPt_3","SCT Barrel Residuals vs Pt",CanvasSizeX4[0],CanvasSizeX4[1]);
            sctBarrelResidualsVsPt(FirstTrackCollection, cSCTBarrelResVsPt_3, SecondTrackCollection);
        }
        cSCTECARes_1 = new TCanvas("cSCTECARes_1","SCT Endcap A Residuals", CanvasSizeX9[0],CanvasSizeX9[1],
                                   CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResiduals(FirstTrackCollection, 0, cSCTECARes_1);
        if (SecondTrackCollection !=""){
            cSCTECARes_2 = new TCanvas("cSCTECARes_2","SCT Endcap A Residuals", CanvasSizeX9[0],CanvasSizeX9[1],
                                       CanvasSizeX9[2], CanvasSizeX9[3]);
            sctECResiduals(SecondTrackCollection, 0, cSCTECARes_2);
        }
        if (SecondTrackCollection !=""){
            cSCTECARes_3 = new TCanvas("cSCTECARes_3","SCT Endcap A Residuals", CanvasSizeX9[0],CanvasSizeX9[1],
                                       CanvasSizeX9[2], CanvasSizeX9[3]);
            sctECResiduals(FirstTrackCollection, 0, cSCTECARes_3, SecondTrackCollection);
        }
        cSCTECCRes_1 = new TCanvas("cSCTECCRes_1","SCT Endcap C Residuals", CanvasSizeX9[0],CanvasSizeX9[1],
                                   CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResiduals(FirstTrackCollection, 1, cSCTECCRes_1);
        if (SecondTrackCollection !=""){
            cSCTECCRes_2 = new TCanvas("cSCTECCRes_2","SCT Endcap C Residuals", CanvasSizeX9[0],CanvasSizeX9[1],
                                       CanvasSizeX9[2], CanvasSizeX9[3]);
            sctECResiduals(SecondTrackCollection, 1, cSCTECCRes_2);
        }
        if (SecondTrackCollection !=""){
            cSCTECCRes_3 = new TCanvas("cSCTECCRes_3","SCT Endcap C Residuals", CanvasSizeX9[0],CanvasSizeX9[1],
                                       CanvasSizeX9[2], CanvasSizeX9[3]);
            sctECResiduals(FirstTrackCollection, 1, cSCTECCRes_3, SecondTrackCollection);
        }
        cSCTECAResVsPt_1 = new TCanvas("cSCTECAResVsPt_1","SCT Endcap A Residuals Vs Pt", CanvasSizeX9[0],CanvasSizeX9[1],
                                       CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResidualsVsPt(FirstTrackCollection, 0, cSCTECAResVsPt_1);
        if (SecondTrackCollection !=""){
            cSCTECAResVsPt_2 = new TCanvas("cSCTECAResVsPt_2","SCT Endcap A Residuals Vs Pt", CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
            sctECResidualsVsPt(SecondTrackCollection, 0, cSCTECAResVsPt_2);
        }
        if (SecondTrackCollection !=""){
            cSCTECAResVsPt_3 = new TCanvas("cSCTECAResVsPt_3","SCT Endcap A Residuals Vs Pt", CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
            sctECResidualsVsPt(FirstTrackCollection, 0, cSCTECAResVsPt_3, SecondTrackCollection);
        }
        cSCTECCResVsPt_1 = new TCanvas("cSCTECCResVsPt_1","SCT Endcap C Residuals Vs Pt", CanvasSizeX9[0],CanvasSizeX9[1],
                                       CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResidualsVsPt(FirstTrackCollection, 1, cSCTECCResVsPt_1);
        if (SecondTrackCollection !=""){
            cSCTECCResVsPt_2 = new TCanvas("cSCTECCResVsPt_2","SCT Endcap C Residuals Vs Pt", CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
            sctECResidualsVsPt(SecondTrackCollection, 1, cSCTECCResVsPt_2);
        }
        if (SecondTrackCollection !=""){
            cSCTECCResVsPt_3 = new TCanvas("cSCTECCResVsPt_3","SCT Endcap C Residuals Vs Pt", CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
            sctECResidualsVsPt(FirstTrackCollection, 1, cSCTECCResVsPt_3, SecondTrackCollection);
        }
    }
    
    // PIX Residual maps -  First track collection
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelResmaps(" << FirstTrackCollection << ") --> local x mean " << endl;
    cPIXMeanPhiResMap_1 = new TCanvas("cPIXMeanPhiResMap_1","Pixel Mean local x Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                      CanvasSizeX9[2], CanvasSizeX9[3]);
    pixelResmaps(FirstTrackCollection,cPIXMeanPhiResMap_1,true,false,25);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelResmaps(" << FirstTrackCollection << ") --> local x RMS" << endl;
    cPIXRMSPhiResMap_1 = new TCanvas("cPIXRMSPhiResMap_1","Pixel RMS local x Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                     CanvasSizeX9[2], CanvasSizeX9[3]);
    pixelResmaps(FirstTrackCollection,cPIXRMSPhiResMap_1,true,true,25);
    
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelResmaps(" << FirstTrackCollection << ") --> local y mean " << endl;
    cPIXMeanEtaResMap_1 = new TCanvas("cPIXMeanEtaResMap_1","Pixel Mean local Y Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                      CanvasSizeX9[2], CanvasSizeX9[3]);
    pixelResmaps(FirstTrackCollection,cPIXMeanEtaResMap_1,false,false,25);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelResmaps(" << FirstTrackCollection << ") --> local y RMS " << endl;
    cPIXRMSEtaResMap_1 = new TCanvas("cPIXRMSEtaResMap_1","Pixel RMS localy Residual maps 2",CanvasSizeX9[0],CanvasSizeX9[1],
                                     CanvasSizeX9[2], CanvasSizeX9[3]);
    pixelResmaps(FirstTrackCollection,cPIXRMSEtaResMap_1,false,true,50);
    
    // PIX Residual maps -  Second track collection
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelResmaps(" << SecondTrackCollection << ") --> local x mean " << endl;
        cPIXMeanPhiResMap_2 = new TCanvas("cPIXMeanPhiResMap_2","Pixel Mean Phi Residual maps 2",CanvasSizeX9[0],CanvasSizeX9[1],
                                      CanvasSizeX9[2], CanvasSizeX9[3]);
        pixelResmaps(SecondTrackCollection,cPIXMeanPhiResMap_2,true,false,25);
    }
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelResmaps(" << SecondTrackCollection << ") --> local x RMS " << endl;
        cPIXRMSPhiResMap_2 = new TCanvas("cPIXRMSPhiResMap_2","Pixel RMS Phi Residual maps 2",CanvasSizeX9[0],CanvasSizeX9[1],
                                     CanvasSizeX9[2], CanvasSizeX9[3]);
        pixelResmaps(SecondTrackCollection,cPIXRMSPhiResMap_2,true,true,25);
    }
    
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelResmaps(" << SecondTrackCollection << ") --> local y mean " << endl;
        cPIXMeanEtaResMap_2 = new TCanvas("cPIXMeanEtaResMap_2","Pixel Mean local y Residual maps 2",CanvasSizeX9[0],CanvasSizeX9[1],
                                      CanvasSizeX9[2], CanvasSizeX9[3]);
        pixelResmaps(SecondTrackCollection,cPIXMeanEtaResMap_2,false,false,25);
    }
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: pixelResmaps(" << SecondTrackCollection << ") --> local y RMS " << endl;
        cPIXRMSEtaResMap_2 = new TCanvas("cPIXRMSEtaResMap_2","Pixel RMS localy Residual maps 2",CanvasSizeX9[0],CanvasSizeX9[1],
                                     CanvasSizeX9[2], CanvasSizeX9[3]);
        pixelResmaps(SecondTrackCollection,cPIXRMSEtaResMap_2,false,true,50);
    }
    
    
    
    // SCT Residual maps -  First track collection
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << FirstTrackCollection << ") --> EndCap C side 0 mean " << endl;
    cSCTECCMeanSide0ResMap_1 = new TCanvas("cSCTECCMeanSide0ResMap_1","SCT ECC Mean Side0 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
    sctECResmaps(FirstTrackCollection,cSCTECCMeanSide0ResMap_1,true,false,false,25);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << FirstTrackCollection << ") --> EndCap C side 0 RMS " << endl;
    cSCTECCRMSSide0ResMap_1 = new TCanvas("cSCTECCRMSSide0ResMap_1","SCT ECC RMS Side0 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                          CanvasSizeX9[2], CanvasSizeX9[3]);
    sctECResmaps(FirstTrackCollection,cSCTECCRMSSide0ResMap_1,true,false,true,25);
    
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << FirstTrackCollection << ") --> EndCap C side 1 mean " << endl;
    cSCTECCMeanSide1ResMap_1 = new TCanvas("cSCTECCMeanSide1ResMap_1","SCT ECC Mean Side1 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
    sctECResmaps(FirstTrackCollection,cSCTECCMeanSide1ResMap_1,true,true,false,25);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << FirstTrackCollection << ") --> EndCap C side 1 RMS " << endl;
    cSCTECCRMSSide1ResMap_1 = new TCanvas("cSCTECCRMSSide1ResMap_1","SCT ECC RMS Side1 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                          CanvasSizeX9[2], CanvasSizeX9[3]);
    sctECResmaps(FirstTrackCollection,cSCTECCRMSSide1ResMap_1,true,true,true,25);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelResmaps(" << FirstTrackCollection << ") --> Barrel side 0 mean " << endl;
    cSCTBaMeanSide0ResMap_1 = new TCanvas("cSCTBaMeanSide0ResMap_1","SCT Barrel Mean Side0 Residual maps 1",CanvasSizeX4[0],CanvasSizeX4[1]);
    sctBarrelResmaps(FirstTrackCollection,cSCTBaMeanSide0ResMap_1,false,false,25);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelResmaps(" << FirstTrackCollection << ") --> Barrel side 0 RMS " << endl;
    cSCTBaRMSSide0ResMap_1 = new TCanvas("cSCTBaRMSSide0ResMap_1","SCT Barrel RMS Side0 Residual maps 1",CanvasSizeX4[0],CanvasSizeX4[1]);
    sctBarrelResmaps(FirstTrackCollection,cSCTBaRMSSide0ResMap_1,false,true,25);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelResmaps(" << FirstTrackCollection << ") --> Barrel side 1 mean " << endl;
    cSCTBaMeanSide1ResMap_1 = new TCanvas("cSCTBaMeanSide1ResMap_1","SCT Barrel Mean Side1 Residual maps 1",CanvasSizeX4[0],CanvasSizeX4[1]);
    sctBarrelResmaps(FirstTrackCollection,cSCTBaMeanSide1ResMap_1,true,false,25);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelResmaps(" << FirstTrackCollection << ") --> Barrel side 1 RMS " << endl;
    cSCTBaRMSSide1ResMap_1 = new TCanvas("cSCTBaRMSSide1ResMap_1","SCT Barrel RMS Side1 Residual maps 1",CanvasSizeX4[0],CanvasSizeX4[1]);
    sctBarrelResmaps(FirstTrackCollection,cSCTBaRMSSide1ResMap_1,true,true,25);
    
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << FirstTrackCollection << ") --> EndCap A side 0 mean " << endl;
    cSCTECAMeanSide0ResMap_1 = new TCanvas("cSCTECAMeanSide0ResMap_1","SCT ECA Mean Side0 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
    sctECResmaps(FirstTrackCollection,cSCTECAMeanSide0ResMap_1,false,false,false,25);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << FirstTrackCollection << ") --> EndCap A side 0 RMS " << endl;
    cSCTECARMSSide0ResMap_1 = new TCanvas("cSCTECARMSSide0ResMap_1","SCT ECA RMS Side0 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                          CanvasSizeX9[2], CanvasSizeX9[3]);
    sctECResmaps(FirstTrackCollection,cSCTECARMSSide0ResMap_1,false,false,true,25);
    
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << FirstTrackCollection << ") --> EndCap A side 1 mean " << endl;
    cSCTECAMeanSide1ResMap_1 = new TCanvas("cSCTECAMeanSide1ResMap_1","SCT ECA Mean Side1 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
    sctECResmaps(FirstTrackCollection,cSCTECAMeanSide1ResMap_1,false,true,false,25);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << FirstTrackCollection << ") --> EndCap A side 1 RMS " << endl;
    cSCTECARMSSide1ResMap_1 = new TCanvas("cSCTECARMSSide1ResMap_1","SCT ECA RMS Side1 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                          CanvasSizeX9[2], CanvasSizeX9[3]);
    sctECResmaps(FirstTrackCollection,cSCTECARMSSide1ResMap_1,false,true,true,25);
    
    
    // SCT Residual maps -  Second track collection
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << SecondTrackCollection << ") --> EndCap C side 0 mean " << endl;
        cSCTECCMeanSide0ResMap_2 = new TCanvas("cSCTECCMeanSide0ResMap_2","SCT ECC Mean Side0 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResmaps(SecondTrackCollection,cSCTECCMeanSide0ResMap_2,true,false,false,25);
    }
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << SecondTrackCollection << ") --> EndCap C side 0 RMS " << endl;
        cSCTECCRMSSide0ResMap_2 = new TCanvas("cSCTECCRMSSide0ResMap_2","SCT ECC RMS Side0 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                          CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResmaps(SecondTrackCollection,cSCTECCRMSSide0ResMap_2,true,false,true,25);
    }
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << SecondTrackCollection << ") --> EndCap C side 1 mean " << endl;
        cSCTECCMeanSide1ResMap_2 = new TCanvas("cSCTECCMeanSide1ResMap_2","SCT ECC Mean Side1 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResmaps(SecondTrackCollection,cSCTECCMeanSide1ResMap_2,true,true,false,25);
    }
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << SecondTrackCollection << ") --> EndCap C side 1 RMS " << endl;
        cSCTECCRMSSide1ResMap_2 = new TCanvas("cSCTECCRMSSide1ResMap_2","SCT ECC RMS Side1 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                          CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResmaps(SecondTrackCollection,cSCTECCRMSSide1ResMap_2,true,true,true,25);
    }
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelResmaps(" << SecondTrackCollection << ") --> Barrel side 0 mean " << endl;
        cSCTBaMeanSide0ResMap_2 = new TCanvas("cSCTBaMeanSide0ResMap_2","SCT Barrel Mean Side0 Residual maps 1",CanvasSizeX4[0],CanvasSizeX4[1]);
        sctBarrelResmaps(SecondTrackCollection,cSCTBaMeanSide0ResMap_2,false,false,25);
    }
        
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelResmaps(" << SecondTrackCollection << ") --> Barrel side 0 RMS " << endl;
        cSCTBaRMSSide0ResMap_2 = new TCanvas("cSCTBaRMSSide0ResMap_2","SCT Barrel RMS Side0 Residual maps 1",CanvasSizeX4[0],CanvasSizeX4[1]);
        sctBarrelResmaps(SecondTrackCollection,cSCTBaRMSSide0ResMap_2,false,true,25);
    }
        
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelResmaps(" << SecondTrackCollection << ") --> Barrel side 1 mean " << endl;
        cSCTBaMeanSide1ResMap_2 = new TCanvas("cSCTBaMeanSide1ResMap_2","SCT Barrel Mean Side1 Residual maps 1",CanvasSizeX4[0],CanvasSizeX4[1]);
        sctBarrelResmaps(SecondTrackCollection,cSCTBaMeanSide1ResMap_2,true,false,25);
    }
        
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctBarrelResmaps(" << SecondTrackCollection << ") --> Barrel side 1 RMS " << endl;
        cSCTBaRMSSide1ResMap_2 = new TCanvas("cSCTBaRMSSide1ResMap_2","SCT Barrel RMS Side1 Residual maps 1",CanvasSizeX4[0],CanvasSizeX4[1]);
        sctBarrelResmaps(SecondTrackCollection,cSCTBaRMSSide1ResMap_2,true,true,25);
    }
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << SecondTrackCollection << ") --> EndCap A side 0 mean " << endl;
        cSCTECAMeanSide0ResMap_2 = new TCanvas("cSCTECAMeanSide0ResMap_2","SCT ECA Mean Side0 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResmaps(SecondTrackCollection,cSCTECAMeanSide0ResMap_2,true,false,false,25);
    }
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << SecondTrackCollection << ") --> EndCap A side 0 RMS " << endl;
        cSCTECARMSSide0ResMap_2 = new TCanvas("cSCTECARMSSide0ResMap_2","SCT ECA RMS Side0 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                          CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResmaps(SecondTrackCollection,cSCTECARMSSide0ResMap_2,false,false,true,25);
    }
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << SecondTrackCollection << ") --> EndCap A side 1 mean " << endl;
        cSCTECAMeanSide1ResMap_2 = new TCanvas("cSCTECAMeanSide1ResMap_2","SCT ECA Mean Side1 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                           CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResmaps(SecondTrackCollection,cSCTECAMeanSide1ResMap_2,false,true,false,25);
    }
        
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: sctECResmaps(" << SecondTrackCollection << ") --> EndCap A side 1 RMS " << endl;
        cSCTECARMSSide1ResMap_2 = new TCanvas("cSCTECARMSSide1ResMap_2","SCT ECA RMS Side1 Residual maps 1",CanvasSizeX9[0],CanvasSizeX9[1],
                                          CanvasSizeX9[2], CanvasSizeX9[3]);
        sctECResmaps(SecondTrackCollection,cSCTECARMSSide1ResMap_2,false,true,true,25);
    }
    
    //////////////////////////////////
    // Track parameters
    //////////////////////////////////
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: trkParameters(" << FirstTrackCollection << ") " << endl;
    cTrkParameters_1 = new TCanvas("cTrkParameters_1","Track Parameters",CanvasSizeX6[0],CanvasSizeX6[1]);
    trkParameters(FirstTrackCollection, cTrkParameters_1);
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: trkParameters(" << SecondTrackCollection << ") " << endl;
        cTrkParameters_2 = new TCanvas("cTrkParameters_2","Track Parameters",CanvasSizeX6[0],CanvasSizeX6[1]);
        trkParameters(SecondTrackCollection, cTrkParameters_2);
    }
    
    // 	cTrkParameters_3 = new TCanvas("cTrkParameters_3","Track Parameters",CanvasSizeX6[0],CanvasSizeX6[1]);
    //	trkParameters(FirstTrackCollection, cTrkParameters_3, SecondTrackCollection);
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: trkMomentum(" << FirstTrackCollection << ") " << endl;
    cTrkMomentum_1 = new TCanvas("cTrkMomentum_1","Track Momentum",CanvasSizeX6[0],CanvasSizeX6[1]);
    trkMomentum(FirstTrackCollection, cTrkMomentum_1);
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: trkMomentum(" << SecondTrackCollection << ") " << endl;
        cTrkMomentum_2 = new TCanvas("cTrkMomentum_2","Track Momentum",CanvasSizeX6[0],CanvasSizeX6[1]);
        trkMomentum(SecondTrackCollection, cTrkMomentum_2);
    }
    
    // cTrkMomentum_3 = new TCanvas("cTrkMomentum_3","Track Momentum",CanvasSizeX6[0],CanvasSizeX6[1]);
    // trkMomentum(FirstTrackCollection, cTrkMomentum_3, SecondTrackCollection);
    
    
    if (printprogressreport) cout << " <Monitor> <drawPlots> calling: errD0(" << FirstTrackCollection << ") " << endl;
    cErrD0_1 = new TCanvas("cErrD0_1","Error D0",CanvasSizeX6[0],CanvasSizeX6[1]);
    errD0(FirstTrackCollection, cErrD0_1);
    
    if (SecondTrackCollection !=""){
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: errD0(" << SecondTrackCollection << ") " << endl;
        cErrD0_2 = new TCanvas("cErrD0_2","Error D0",CanvasSizeX6[0],CanvasSizeX6[1]);
        errD0(SecondTrackCollection, cErrD0_2);
    }
    
    // cErrD0_3 = new TCanvas("cErrD0_3","Error D0",CanvasSizeX6[0],CanvasSizeX6[1]);
    // errD0(FirstTrackCollection, cErrD0_3, SecondTrackCollection);
    
    if (false) {
        cErrD0VsPhi0_1 = new TCanvas("cErrD0VsPhi0_1","Error D0 Vs Phi0",CanvasSizeX6[0],CanvasSizeX6[1]);
        errD0VsPhi0(FirstTrackCollection, cErrD0VsPhi0_1);
        if (SecondTrackCollection !=""){
            cErrD0VsPhi0_2 = new TCanvas("cErrD0VsPhi0_2","Error D0 Vs Phi0",CanvasSizeX6[0],CanvasSizeX6[1]);
            errD0VsPhi0(SecondTrackCollection, cErrD0VsPhi0_2);
        }
        if (SecondTrackCollection !=""){
            cErrD0VsPhi0_3 = new TCanvas("cErrD0VsPhi0_3","Error D0 Vs Phi0",CanvasSizeX6[0],CanvasSizeX6[1]);
            errD0VsPhi0(FirstTrackCollection, cErrD0VsPhi0_3, SecondTrackCollection);
        }
        cErrPt_1 = new TCanvas("cErrPt_1","Error Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
        errPt(FirstTrackCollection, cErrPt_1);
        if (SecondTrackCollection !=""){
            cErrPt_2 = new TCanvas("cErrPt_2","Error Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
            errPt(SecondTrackCollection, cErrPt_2);
        }
        if (SecondTrackCollection !=""){
            cErrPt_3 = new TCanvas("cErrPt_3","Error Pt",CanvasSizeX6[0],CanvasSizeX6[1]);
            errPt(FirstTrackCollection, cErrPt_3, SecondTrackCollection);
        }
        cErrPhi0_1 = new TCanvas("cErrPhi0_1","Error Phi0",CanvasSizeX6[0],CanvasSizeX6[1]);
        errPhi0(FirstTrackCollection, cErrPhi0_1);
        if (SecondTrackCollection !=""){
            cErrPhi0_2 = new TCanvas("cErrPhi0_2","Error Phi0",CanvasSizeX6[0],CanvasSizeX6[1]);
            errPhi0(SecondTrackCollection, cErrPhi0_2);
        }
        if (SecondTrackCollection !=""){
            cErrPhi0_3 = new TCanvas("cErrPhi0_3","Error Phi0",CanvasSizeX6[0],CanvasSizeX6[1]);
            errPhi0(FirstTrackCollection, cErrPhi0_3, SecondTrackCollection);
        }
        cErrCotTheta_1 = new TCanvas("cErrCotTheta_1","Error Cot Theta",CanvasSizeX6[0],CanvasSizeX6[1]);
        errCotTheta(FirstTrackCollection, cErrCotTheta_1);
        if (SecondTrackCollection !=""){
            cErrCotTheta_2 = new TCanvas("cErrCotTheta_2","Error Cot Theta",CanvasSizeX6[0],CanvasSizeX6[1]);
            errCotTheta(SecondTrackCollection, cErrCotTheta_2);
        }
        if (SecondTrackCollection !=""){
            cErrCotTheta_3 = new TCanvas("cErrCotTheta_3","Error Cot Theta",CanvasSizeX6[0],CanvasSizeX6[1]);
            errCotTheta(FirstTrackCollection, cErrCotTheta_3, SecondTrackCollection);
        }
        cErrZ0_1 = new TCanvas("cErrZ0_1","Error Z0",CanvasSizeX6[0],CanvasSizeX6[1]);
        errZ0(FirstTrackCollection, cErrZ0_1);
        if (SecondTrackCollection !=""){
            cErrZ0_2 = new TCanvas("cErrZ0_2","Error Z0",CanvasSizeX6[0],CanvasSizeX6[1]);
            errZ0(SecondTrackCollection, cErrZ0_2);
        }
        if (SecondTrackCollection !=""){
            cErrZ0_3 = new TCanvas("cErrZ0_3","Error Z0",CanvasSizeX6[0],CanvasSizeX6[1]);
            errZ0(FirstTrackCollection, cErrZ0_3, SecondTrackCollection);
        }
    }
    
    /////////////
    // cosmic ray segments
    if (iCosmics) {
        if (printprogressreport) cout << " <Monitor> <drawPlots> calling: CosmicSegments(" << FirstTrackCollection << ") " << endl;
        cCosmicSegments[0] = new TCanvas("cCosmicSegmentsHits","Hits of cosmic rays segments", CanvasSizeX6[0],CanvasSizeX6[1]);
        cCosmicSegments[1] = new TCanvas("cCosmicSegmentsUpperTrkPar","Cosmic Rays Upper segment: Trk pars", CanvasSizeX6[0],CanvasSizeX6[1]);
        cCosmicSegments[2] = new TCanvas("cCosmicSegmentsLowerTrkPar","Cosmic Rays Lower segment: Trk pars", CanvasSizeX6[0],CanvasSizeX6[1]);
        cCosmicSegments[3] = new TCanvas("cCosmicSegmentsDeltaTrkPar","Cosmic Rays segment: delta Trk pars", CanvasSizeX6[0],CanvasSizeX6[1]);
        cCosmicSegments[4] = new TCanvas("cCosmicSegmentsDeltaTrkParVsPt","Cosmic Rays segment: delta Trk pars vs pt", CanvasSizeX6[0],CanvasSizeX6[1]);
        //TString segmentcollection = "AlignTracks_NoTriggerSelection";
        CosmicSegments(CosmicSegmentsCollection, cCosmicSegments);
    }
    
    return;
}

void fillSummary(){
    TString tmpName;
    tmpName = "IDAlignMon/"+FirstTrackCollection+"/GenericTracks/ntracks";
    iEvents = (int)((TH1F*)file->Get(tmpName))->GetEntries();
    if(PrintLevel>1) cout << "Events: " << iEvents << endl;
    tmpName = "IDAlignMon/"+FirstTrackCollection+"/GenericTracks/Nhits_per_track";
    iTracks_1 = (int)((TH1F*)file->Get(tmpName))->GetEntries();
    if(PrintLevel>1) cout << "Tracks 1: " << iTracks_1 << endl;
    if (SecondTrackCollection != "") {
        tmpName = "IDAlignMon/"+SecondTrackCollection+"/GenericTracks/Nhits_per_track";
        iTracks_2 = (int)((TH1F*)file->Get(tmpName))->GetEntries();
        if(PrintLevel>1) cout << "Tracks 2: " << iTracks_2 << endl;
    }
    
    tmpName = "IDAlignMon/"+FirstTrackCollection+"/Residuals/pix_ecc_residualx";
    iPixelHits_1 =  (int)((TH1F*)file->Get(tmpName))->GetEntries();
    tmpName = "IDAlignMon/"+FirstTrackCollection+"/Residuals/pix_eca_residualx";
    iPixelHits_1 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
    tmpName = "IDAlignMon/"+FirstTrackCollection+"/Residuals/pix_b_residualx";
    iPixelHits_1 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
    if(PrintLevel>1) cout << "PIX Hits 1: " << iPixelHits_1 << endl;
    if (SecondTrackCollection != "") {
        tmpName = "IDAlignMon/"+SecondTrackCollection+"/Residuals/pix_ecc_residualx";
        iPixelHits_2 =  (int)((TH1F*)file->Get(tmpName))->GetEntries();
        tmpName = "IDAlignMon/"+SecondTrackCollection+"/Residuals/pix_eca_residualx";
        iPixelHits_2 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
        tmpName = "IDAlignMon/"+SecondTrackCollection+"/Residuals/pix_b_residualx";
        iPixelHits_2 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
        if(PrintLevel>1) cout << "PIX Hits 2: " << iPixelHits_2<< endl;
    }
    
    tmpName = "IDAlignMon/"+FirstTrackCollection+"/Residuals/sct_ecc_residualx";
    iSCTHits_1 =  (int)((TH1F*)file->Get(tmpName))->GetEntries();
    tmpName = "IDAlignMon/"+FirstTrackCollection+"/Residuals/sct_eca_residualx";
    iSCTHits_1 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
    tmpName = "IDAlignMon/"+FirstTrackCollection+"/Residuals/sct_b_residualx";
    iSCTHits_1 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
    if(PrintLevel>1) cout << "SCT Hits 1: " << iSCTHits_1 << endl;
    if (SecondTrackCollection != "") {
        tmpName = "IDAlignMon/"+SecondTrackCollection+"/Residuals/sct_ecc_residualx";
        iSCTHits_2 =  (int)((TH1F*)file->Get(tmpName))->GetEntries();
        tmpName = "IDAlignMon/"+SecondTrackCollection+"/Residuals/sct_eca_residualx";
        iSCTHits_2 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
        tmpName = "IDAlignMon/"+SecondTrackCollection+"/Residuals/sct_b_residualx";
        iSCTHits_2 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
        if(PrintLevel>1) cout << "SCT Hits 2: " << iSCTHits_2 << endl;
    }
    
    tmpName = "IDAlignMon/"+FirstTrackCollection+"/Residuals/trt_ec_residualR_Endcap_A";
    iTRTHits_1 =  (int)((TH1F*)file->Get(tmpName))->GetEntries();
    tmpName = "IDAlignMon/"+FirstTrackCollection+"/Residuals/trt_ec_residualR_Endcap_C";
    iTRTHits_1 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
    tmpName = "IDAlignMon/"+FirstTrackCollection+"/Residuals/trt_b_residualR";
    iTRTHits_1 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
    if(PrintLevel>1) cout << "TRT Hits 1: " << iTRTHits_1 << endl;
    if (SecondTrackCollection != "") {
        tmpName = "IDAlignMon/"+SecondTrackCollection+"/Residuals/trt_ec_residualR_Endcap_A";
        iTRTHits_2 =  (int)((TH1F*)file->Get(tmpName))->GetEntries();
        tmpName = "IDAlignMon/"+SecondTrackCollection+"/Residuals/trt_ec_residualR_Endcap_C";
        iTRTHits_2 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
        tmpName = "IDAlignMon/"+SecondTrackCollection+"/Residuals/trt_b_residualR";
        iTRTHits_2 += (int)((TH1F*)file->Get(tmpName))->GetEntries();
        if(PrintLevel>1) cout << "TRT Hits 2: " << iTRTHits_2 << endl;
    }
    iHits_1 = iPixelHits_1 + iSCTHits_1 + iTRTHits_1;
    iHits_2 = iPixelHits_2 + iSCTHits_2 + iTRTHits_2;
    
    
    if(FileName.Contains("CosmicsBField")){
        aSetup = "Cosmics BField";
        iCosmics = true;
        iBField = 2.0;
    }
    else if (FileName.Contains("CosmicsNoBField")) {
        aSetup = "Cosmics w/0 BField";
        iCosmics = true;
        iBField = 0;
    }
    else if (FileName.Contains("Collision")){
        aSetup = "Collisions";
        iCosmics = false;
        iBField = 2.0;
    }
    else {
        aSetup = "Unknown";
        iCosmics = false;
        iBField = 2.0;
    }
    
    
    if(PrintLevel>1) cout << "Setup: " << aSetup << endl;
    iCosmics = true;
    if(PrintLevel>1) cout << "Cosmics?: " << iCosmics << " --- FORCED --- Salva --- " <<  endl;
    tmpName = FileName(FileName.Index("Iter")+4);
    IterNumber = tmpName.Atoi();
    if(PrintLevel>1) cout << "Iteration: " << IterNumber << endl;
    
}


///////////////////////////////////////////////////////////////////
void printPS(){
    cout << endl << endl << " <Monitor> <printPs> Storing the plots in a ps file..." << endl;
    TString psname, command;
    psname = FileName(0,FileName.Length()-12)+"_plots.pdf";
    psname = "monitor.pdf";
    TCanvas c0("c0","c0",CanvasSizeX6[0],CanvasSizeX6[1]);
    command = psname+"[";
    c0.Print(command);
    if (cInfoRes)             {cInfoRes->Print(psname); cout << " <printPS> include canvas \"cInfoRes\" " << endl;}
    if (cHits)                {cHits->Print(psname); cout << " <printPS> include canvas \"cHits\" " << endl;}
    if (cHitCorr)		  {cHitCorr->Print(psname); cout << " <printPS> include canvas \"cHitCorr\" " << endl;}
    if (cHitsPerLayer)	  {cHitsPerLayer->Print(psname); cout << " <printPS> include canvas \"cHitsPerLayer\" " << endl;}
    
    if (cPIXHitMap)		  {cPIXHitMap->Print(psname); cout << " <printPS> include canvas \"cPIXHitMap\" " << endl;}
    if (cPIXHitMap1)	  {cPIXHitMap1->Print(psname); cout << " <printPS> include canvas \"cPIXHitMap1\" " << endl;}
    if (cPIXHitMap2)	  {cPIXHitMap2->Print(psname); cout << " <printPS> include canvas \"cPIXHitMap2\" " << endl;}
    
    if (cSCTHitMapEC[1])      {cSCTHitMapEC[1]->Print(psname); cout << " <printPS> include canvas \"cSCTHitMapEC[1]\" " << endl;}
    if (cSCTHitMapBa)	  {cSCTHitMapBa->Print(psname); cout << " <printPS> include canvas \"cSCTHitMapBA\" " << endl;}
    if (cSCTHitMapEC[0])      {cSCTHitMapEC[0]->Print(psname); cout << " <printPS> include canvas \"cSCTHitMapEC[0]\" " << endl;}
    
    if (cSCTHitMapEC1[1])     cSCTHitMapEC1[1]->Print(psname);
    if (cSCTHitMapBa1)		  cSCTHitMapBa1->Print(psname);
    if (cSCTHitMapEC1[0])     cSCTHitMapEC1[0]->Print(psname);
    
    if (cSCTHitMapEC2[1])     cSCTHitMapEC2[1]->Print(psname);
    if (cSCTHitMapBa2)	  cSCTHitMapBa2->Print(psname);
    if (cSCTHitMapEC2[0])     cSCTHitMapEC2[0]->Print(psname);
    
    
    if (cPixRes)			  cPixRes->Print(psname);
    if (cPixRes_1)            cPixRes_1->Print(psname);
    if (cPixRes_2)            cPixRes_2->Print(psname);
    if (cPixRes_3)            cPixRes_3->Print(psname);
    
    if (cSctTRTRes)           cSctTRTRes->Print(psname);
    if (cSctTRTRes_1)         cSctTRTRes_1->Print(psname);
    if (cSctTRTRes_2)         cSctTRTRes_2->Print(psname);
    if (cSctTRTRes_3)         cSctTRTRes_3->Print(psname);
    
    if (cPIXBarrelRes)        cPIXBarrelRes->Print(psname);
    if (cPIXBarrelRes_1)      cPIXBarrelRes_1->Print(psname);
    if (cPIXBarrelRes_2)      cPIXBarrelRes_2->Print(psname);
    if (cPIXBarrelRes_3)      cPIXBarrelRes_3->Print(psname);
    
    if (cPIXBarrelResVsPt)    cPIXBarrelResVsPt->Print(psname);
    if (cPIXBarrelResVsPt_1)  cPIXBarrelResVsPt_1->Print(psname);
    if (cPIXBarrelResVsPt_2)  cPIXBarrelResVsPt_2->Print(psname);
    if (cPIXBarrelResVsPt_3)  cPIXBarrelResVsPt_3->Print(psname);
    
    
    if (cPIXECRes)   	      cPIXECRes->Print(psname);
    if (cPIXECResVsPt)   	  cPIXECResVsPt->Print(psname);
    if (cPIXECCRes_1)         cPIXECCRes_1->Print(psname);
    if (cPIXECCRes_2)         cPIXECCRes_2->Print(psname);
    if (cPIXECCRes_3)         cPIXECCRes_3->Print(psname);
    if (cPIXECCResVsPt_1)     cPIXECCResVsPt_1->Print(psname);
    if (cPIXECCResVsPt_2)     cPIXECCResVsPt_2->Print(psname);
    if (cPIXECCResVsPt_3)     cPIXECCResVsPt_3->Print(psname);
    
    if (cPIXECARes_1)         cPIXECARes_1->Print(psname);
    if (cPIXECARes_2)         cPIXECARes_2->Print(psname);
    if (cPIXECARes_3)         cPIXECARes_3->Print(psname);
    if (cPIXECAResVsPt_1)     cPIXECAResVsPt_1->Print(psname);
    if (cPIXECAResVsPt_2)     cPIXECAResVsPt_2->Print(psname);
    if (cPIXECAResVsPt_3)     cPIXECAResVsPt_3->Print(psname);
    
    
    if (cSCTBarrelRes)        cSCTBarrelRes->Print(psname);
    if (cSCTBarrelRes_1)      cSCTBarrelRes_1->Print(psname);
    if (cSCTBarrelRes_2)      cSCTBarrelRes_2->Print(psname);
    if (cSCTBarrelRes_3)      cSCTBarrelRes_3->Print(psname);
    
    if (cSCTBarrelResVsPt)    cSCTBarrelResVsPt->Print(psname);
    if (cSCTBarrelResVsPt_1)  cSCTBarrelResVsPt_1->Print(psname);
    if (cSCTBarrelResVsPt_2)  cSCTBarrelResVsPt_2->Print(psname);
    if (cSCTBarrelResVsPt_3)  cSCTBarrelResVsPt_3->Print(psname);
    
    if (cSCTECRes)   	      cSCTECRes->Print(psname);
    if (cSCTECARes_1)         cSCTECARes_1->Print(psname);
    if (cSCTECARes_2)         cSCTECARes_2->Print(psname);
    if (cSCTECARes_3)         cSCTECARes_3->Print(psname);
    if (cSCTECCRes_1)         cSCTECCRes_1->Print(psname);
    if (cSCTECCRes_2)         cSCTECCRes_2->Print(psname);
    if (cSCTECCRes_3)         cSCTECCRes_3->Print(psname);
    
    if (cSCTECResVsPt)   	  cSCTECResVsPt->Print(psname);
    if (cSCTECAResVsPt_1)     cSCTECAResVsPt_1->Print(psname);
    if (cSCTECAResVsPt_2)     cSCTECAResVsPt_2->Print(psname);
    if (cSCTECAResVsPt_3)     cSCTECAResVsPt_3->Print(psname);
    if (cSCTECCResVsPt_1)     cSCTECCResVsPt_1->Print(psname);
    if (cSCTECCResVsPt_2)     cSCTECCResVsPt_2->Print(psname);
    if (cSCTECCResVsPt_3)     cSCTECCResVsPt_3->Print(psname);
    
    if (cPIXResMap)		  cPIXResMap->Print(psname);
    if (cPIXMeanPhiResMap_1)  {cPIXMeanPhiResMap_1->Print(psname); cout << " <printPS> include canvas \"cPIXMeanPhiResMap_1\" " << endl;}
    if (cPIXRMSPhiResMap_1)   {cPIXRMSPhiResMap_1->Print(psname); cout << " <printPS> include canvas \"cPIXRMSPhiResMap_1\" " << endl;}
    if (cPIXMeanEtaResMap_1)  {cPIXMeanEtaResMap_1->Print(psname); cout << " <printPS> include canvas \"cPIXMeanEtaResMap_1\" " << endl;}
    if (cPIXRMSEtaResMap_1)   {cPIXRMSEtaResMap_1->Print(psname); cout << " <printPS> include canvas \"cPIXRMSEtaResMap_1\" " << endl;}
    
    if (cPIXMeanPhiResMap_2)  {cPIXMeanPhiResMap_2->Print(psname);  cout << " <printPS> include canvas \"cPIXMeanPhiResMap_2\" " << endl;}
    if (cPIXRMSPhiResMap_2)   {cPIXRMSPhiResMap_2->Print(psname);   cout << " <printPS> include canvas \"cPIXRMSPhiResMap_2\" " << endl;}
    if (cPIXMeanEtaResMap_2)  {cPIXMeanEtaResMap_2->Print(psname); cout << " <printPS> include canvas \"cPIXMeanEtaResMap_2\" " << endl;}
    if (cPIXRMSEtaResMap_2)   {cPIXRMSEtaResMap_2->Print(psname); cout << " <printPS> include canvas \"cPIXRMSEtaResMap_2\" " << endl;}
    
    
    if (cSCTBaResMap)	       {cSCTBaResMap->Print(psname); cout << " <printPS> include canvas \"cSCTBarResMap\" " << endl;}
    if (cSCTResMapEC)  	       {cSCTResMapEC->Print(psname); cout << " <printPS> include canvas \"cSCTResMapEC\" " << endl;}
    if (cSCTECCMeanSide0ResMap_1)  {cSCTECCMeanSide0ResMap_1->Print(psname); cout << " <printPS> include canvas \"cSCTECCMeanSide0ResMap_1\" " << endl;}
    if (cSCTECCRMSSide0ResMap_1)   {cSCTECCRMSSide0ResMap_1->Print(psname);  cout << " <printPS> include canvas \"cSCTECCRMSSide0ResMap_1\" " << endl;}
    if (cSCTECCMeanSide1ResMap_1)  {cSCTECCMeanSide1ResMap_1->Print(psname); cout << " <printPS> include canvas \"cSCTECCMeanSide1ResMap_1\" " << endl;}
    if (cSCTECCRMSSide1ResMap_1)   {cSCTECCRMSSide1ResMap_1->Print(psname); cout << " <printPS> include canvas \"cSCTECCRMSSid10ResMap_1\" " << endl;}
    if (cSCTBaMeanSide0ResMap_1)   {cSCTBaMeanSide0ResMap_1->Print(psname); cout << " <printPS> include canvas \"cSCTBaMeanSide0ResMap_1\" " << endl;}
    if (cSCTBaRMSSide0ResMap_1)    {cSCTBaRMSSide0ResMap_1->Print(psname); cout << " <printPS> include canvas \"cSCTBaRMSSide0ResMap_1\" " << endl;}
    if (cSCTBaMeanSide1ResMap_1)   {cSCTBaMeanSide1ResMap_1->Print(psname); cout << " <printPS> include canvas \"cSCTBaMeanSide1ResMap_1\" " << endl;}
    if (cSCTBaRMSSide1ResMap_1)    {cSCTBaRMSSide1ResMap_1->Print(psname); cout << " <printPS> include canvas \"cSCTBaRMSSide1ResMap_1\" " << endl;}
    if (cSCTECAMeanSide0ResMap_1)  {cSCTECAMeanSide0ResMap_1->Print(psname); cout << " <printPS> include canvas \"cSCTECAMeanSide0ResMap_1\" " << endl;}
    if (cSCTECARMSSide0ResMap_1)   {cSCTECARMSSide0ResMap_1->Print(psname); cout << " <printPS> include canvas \"cSCTECARMSSide0ResMap_1\" " << endl;}
    if (cSCTECAMeanSide1ResMap_1)  {cSCTECAMeanSide1ResMap_1->Print(psname); cout << " <printPS> include canvas \"cSCTECAMeanSide1ResMap_1\" " << endl;}
    if (cSCTECARMSSide1ResMap_1)   {cSCTECARMSSide1ResMap_1->Print(psname); cout << " <printPS> include canvas \"cSCTECARMSSide1ResMap_1\" " << endl;}
    
    if (cSCTECCMeanSide0ResMap_2)  {cSCTECCMeanSide0ResMap_2->Print(psname); cout << " <printPS> include canvas \"cSCTECCMeanSide0ResMap_2\" " << endl;}
    if (cSCTECCRMSSide0ResMap_2)   {cSCTECCRMSSide0ResMap_2->Print(psname);  cout << " <printPS> include canvas \"cSCTECCRMSSide0ResMap_2\" " << endl;}
    if (cSCTECCMeanSide1ResMap_2)  {cSCTECCMeanSide1ResMap_2->Print(psname); cout << " <printPS> include canvas \"cSCTECCMeanSide1ResMap_2\" " << endl;}
    if (cSCTECCRMSSide1ResMap_2)   {cSCTECCRMSSide1ResMap_2->Print(psname); cout << " <printPS> include canvas \"cSCTECCRMSSid10ResMap_2\" " << endl;}
    if (cSCTBaMeanSide0ResMap_2)   {cSCTBaMeanSide0ResMap_2->Print(psname); cout << " <printPS> include canvas \"cSCTBaMeanSide0ResMap_2\" " << endl;}
    if (cSCTBaRMSSide0ResMap_2)    {cSCTBaRMSSide0ResMap_2->Print(psname); cout << " <printPS> include canvas \"cSCTBaRMSSide0ResMap_2\" " << endl;}
    if (cSCTBaMeanSide1ResMap_2)   {cSCTBaMeanSide1ResMap_2->Print(psname); cout << " <printPS> include canvas \"cSCTBaMeanSide1ResMap_2\" " << endl;}
    if (cSCTBaRMSSide1ResMap_2)    {cSCTBaRMSSide1ResMap_2->Print(psname); cout << " <printPS> include canvas \"cSCTBaRMSSide1ResMap_2\" " << endl;}
    if (cSCTECAMeanSide0ResMap_2)  {cSCTECAMeanSide0ResMap_2->Print(psname); cout << " <printPS> include canvas \"cSCTECAMeanSide0ResMap_2\" " << endl;}
    if (cSCTECARMSSide0ResMap_2)   {cSCTECARMSSide0ResMap_2->Print(psname); cout << " <printPS> include canvas \"cSCTECARMSSide0ResMap_2\" " << endl;}
    if (cSCTECAMeanSide1ResMap_2)  {cSCTECAMeanSide1ResMap_2->Print(psname); cout << " <printPS> include canvas \"cSCTECAMeanSide1ResMap_2\" " << endl;}
    if (cSCTECARMSSide1ResMap_2)   {cSCTECARMSSide1ResMap_2->Print(psname); cout << " <printPS> include canvas \"cSCTECARMSSide1ResMap_2\" " << endl;}
    
    
    if (cTrkParameters)       cTrkParameters->Print(psname);
    if (cTrkParameters_1)     cTrkParameters_1->Print(psname);
    if (cTrkParameters_2)     cTrkParameters_2->Print(psname);
    if (cTrkParameters_3)     cTrkParameters_3->Print(psname);
    
    if (cTrkMomentum)       cTrkMomentum->Print(psname);
    if (cTrkMomentum_1)     cTrkMomentum_1->Print(psname);
    if (cTrkMomentum_2)     cTrkMomentum_2->Print(psname);
    if (cTrkMomentum_3)     cTrkMomentum_3->Print(psname);
    
    if (cErrD0)       cErrD0->Print(psname);
    if (cErrD0_1)     cErrD0_1->Print(psname);
    if (cErrD0_2)     cErrD0_2->Print(psname);
    if (cErrD0_3)     cErrD0_3->Print(psname);
    
    if (cErrD0VsPhi0)       cErrD0VsPhi0->Print(psname);
    if (cErrD0VsPhi0_1)     cErrD0VsPhi0_1->Print(psname);
    if (cErrD0VsPhi0_2)     cErrD0VsPhi0_2->Print(psname);
    if (cErrD0VsPhi0_3)     cErrD0VsPhi0_3->Print(psname);
    
    if (cErrPt)       cErrPt->Print(psname);
    if (cErrPt_1)     cErrPt_1->Print(psname);
    if (cErrPt_2)     cErrPt_2->Print(psname);
    if (cErrPt_3)     cErrPt_3->Print(psname);
    
    if (cErrPhi0)       cErrPhi0->Print(psname);
    if (cErrPhi0_1)     cErrPhi0_1->Print(psname);
    if (cErrPhi0_2)     cErrPhi0_2->Print(psname);
    if (cErrPhi0_3)     cErrPhi0_3->Print(psname);
    
    if (cErrCotTheta)       cErrCotTheta->Print(psname);
    if (cErrCotTheta_1)     cErrCotTheta_1->Print(psname);
    if (cErrCotTheta_2)     cErrCotTheta_2->Print(psname);
    if (cErrCotTheta_3)     cErrCotTheta_3->Print(psname);
    
    if (cErrZ0)       cErrZ0->Print(psname);
    if (cErrZ0_1)     cErrZ0_1->Print(psname);
    if (cErrZ0_2)     cErrZ0_2->Print(psname);
    if (cErrZ0_3)     cErrZ0_3->Print(psname);
    
    if (cCosmicSegments[0]) {cCosmicSegments[0]->Print(psname); cout << " <printPS> include canvas \"cCosmicSegments:Hits\" " << endl;}
    if (cCosmicSegments[1]) {cCosmicSegments[1]->Print(psname); cout << " <printPS> include canvas \"cCosmicSegments:Upper\" " << endl;}
    if (cCosmicSegments[2]) {cCosmicSegments[2]->Print(psname); cout << " <printPS> include canvas \"cCosmicSegments:Lower\" " << endl;}
    if (cCosmicSegments[3]) {cCosmicSegments[3]->Print(psname); cout << " <printPS> include canvas \"cCosmicSegments:Delta\" " << endl;}
    if (cCosmicSegments[4]) {cCosmicSegments[4]->Print(psname); cout << " <printPS> include canvas \"cCosmicSegments:DeltaVsPt\" " << endl;}
    
    command = psname + "]";
    c0.Print(command);
    // Compress it!
    command = ".!gzip -f " + psname;
    // gROOT->ProcessLine(command);
    
    cout << " - Plots stored successfully!" << endl;
    
}
