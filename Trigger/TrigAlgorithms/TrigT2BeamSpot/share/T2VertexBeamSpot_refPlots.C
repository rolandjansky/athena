#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "TBranch.h"
#include "TLegend.h"
#include "TChain.h"
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>

void MakeRefHistos(TString inFile = "expert-monitoring.root", TCanvas *canv, char *psFile,char* type, bool savePlots = false )
{
    
    // Draw plots and save them
    TH2F *h2D;
    TH1F *h1D;
    
    char histoName[100];
    char canvName[100];
    char plotType[100];
    
    char* plot1DNames[69] = { 
                             "HiPTTracksPerTE",
                             "HiPTTracksPerROI",
                             "Ncluster",
                             "Ntrack",
                             "Nvtx",
                             "NvtxPass",
                             "ROIperTE",
                             "RoIEta",
                             "RoIPhi",
                             "RoIID",
                             "RunID",
                             "TotalROIs",
                             "TotalTEs",
                             "TotalTracks",
                             "TotalTracksPass",
                             "SiTrackD0",
                             "SiTrackD0err",
                             "SiTrackEta",
                             "SiTrackPhi",
                             "SiTrackPt",
                             "SiTrackQual",
                             "SiTrackZ0",
                             "SiTrackZ0err",
                             "SiTrackSiHits",
                             "SiTrackTRTHits",
                             "IdScanD0",
                             "IdScanD0err",
                             "IdScanEta",
                             "IdScanPhi",
                             "IdScanPt",
                             "IdScanQual",
                             "IdScanZ0",
                             "IdScanZ0err",
                             "IdScanSiHits",
                             "IdScanTRTHits",
                             "TrackD0Pass",
                             "TrackD0errPass",
                             "TrackEtaPass",
                             "TrackPhiPass",
                             "TrackPtPass",
                             "TrackQualPass",
                             "TrackZ0Pass",
                             "TrackZ0errPass",
                             "TracksPerROI",
                             "TracksPerROIPass",
                             "TracksPerTE",
                             "TrackPerTEPass",
                             "TrackSiHitsPass",
                             "TrackTRTHitsPass",
                             "VertexNTrks",
                             "VertexMass",
                             "VertexQual",
                             "VertexSumPt",
                             "VertexX",
                             "VertexY",
                             "VertexZ",
                             "VertexXerr",
                             "VertexYerr",
                             "VertexZerr",
                             "VertexNTrksPass",
                             "VertexMassPass",
                             "VertexQualPass",
                             "VertexSumPtPass",
                             "VertexXPass",
                             "VertexYPass",
                             "VertexZPass",
                             "VertexXerrPass",
                             "VertexYerrPass",
                             "VertexZerrPass"
                           };
                        
    char* plot2DNames[17] = { 
                             "IdScanPhi_vs_IdScanD0",
                             "SiTrackPhi_vs_SiTrackD0",
                             "IdScanPhi_vs_IdScanSiHits",
                             "IdScanPhi_vs_IdScanTRTHits",
                             "SiTrackPhi_vs_SiTrackSiHits",
                             "TrackPt_vs_TrackD0err",
                             "TrackPt_vs_TrackQual",
                             "VertexX_vs_VertexY",
                             "VertexNTrks_vs_VertexXerr",
                             "VertexNTrks_vs_VertexYerr",
                             "VertexNTrks_vs_VertexZerr",
                             "VertexNTrks_vs_VertexQual",
                             "VertexXPass_vs_VertexYPass",
                             "VertexNTrksPass_vs_VertexXerrPass",
                             "VertexNTrksPass_vs_VertexYerrPass",
                             "VertexNTrksPass_vs_VertexZerrPass",
                             "VertexNTrksPass_vs_VertexQualPass"
                           };
                      
    /// Set plot type
    sprintf(plotType,"pdf");
    
    /// Open Files
    TFile *file = TFile::Open(inFile);
                      
    ///=========================
    /// Loop on 1D Histograms
    ///=========================
    
    for (unsigned int i = 0; i < 69; i++) {
    
      sprintf(histoName,"T2VertexBeamSpot_Fex/%s",plot1DNames[i]);
      sprintf(canvName,"%s_%s.%s",plot1DNames[i],type,plotType);
      
      h1D = (TH1F*)gDirectory->Get(histoName);
      if (h1D) {
        h1D->SetFillColor(kBlue);
        h1D->Draw();
        if (savePlots) canv->SaveAs(canvName);
        canv->Print(psFile);
      }
    }
    
    ///=========================
    /// Loop on 2D Histograms
    ///=========================
    
    for (unsigned int i = 0; i < 17; i++) {
    
      sprintf(histoName,"T2VertexBeamSpot_Fex/%s",plot2DNames[i]);
      sprintf(canvName,"%s_%s.%s",plot2DNames[i],type,plotType);
      
      h2D = (TH2F*)gDirectory->Get(histoName);
      if (h2D) {
        h2D->Draw("colz");
        if (savePlots) canv->SaveAs(canvName);
        canv->Print(psFile);
      }
    }

    ///=========================
    ///     SPECIAL PLOTS    ///
    ///=========================
    
    // event statistics
    sprintf(canvName,"EventStatistics_%s.%s",type,plotType);
      
    TH1F *h1D = (TH1F*)gDirectory->Get("T2VertexBeamSpot_Fex/EventStatistics");
    char* statNames[10] = {"All Input","Has TE","Has ROI","Has tracks","Has seed track","Enough Tracks","Has Cluster","Has Vertex","Accepted Vtx"};
    if (h1D) {
      for (unsigned int i = 1; i<h1D->GetNbinsX()+1; i++) {
        h1D->GetXaxis()->SetBinLabel(i,statNames[i-1]);
      }
      h1D->SetMinimum(0.0);
      h1D->Draw();
      h1D->SetFillColor(kYellow);
      if (savePlots) canv->SaveAs(canvName);
      canv->Print(psFile);
    }

}

void T2VertexBeamSpot_refPlots(TString inFile = "expert-monitoring.root", char* type = "", bool savePlots = false )
{
    // Set style
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1,0);
    gStyle->SetMarkerStyle(6);
   
    char rootOut[200];
    sprintf(rootOut,"T2VertexBeamSpot.%s.output.root",type);
 
    TFile *outfile = new TFile(rootOut,"UPDATE"); 
    
    //  The canvas for our fitted spectrum
    TCanvas *canv = new TCanvas("canv","canv",8,49,501,375);
  
    char psoutfile[200];
    char psbegin[200];
    char psend[200];
 
    //  Setup the output PostScript file
    sprintf(psoutfile,"T2VertexBeamSpot.%s.plots.ps",type);
    sprintf(psbegin,"%s[",psoutfile);
    sprintf(psend,"%s]",psoutfile);
   
    // Open the output .ps file
    canv->Print(psbegin);
    
    // Draw histograms
    MakeRefHistos(inFile,canv,psoutfile,type,savePlots);
    
    // Close the .ps file
    canv->Print(psend);
    outfile->Close();

}


