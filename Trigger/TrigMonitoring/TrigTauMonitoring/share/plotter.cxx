//
//  plotter.cxx
//
//  Created by Daniele Zanzi on 5/03/2015.
//
// to run, launch root and
// .L plotter.cxx
// plotter("physval.root","sample_type","plot_dir_name") sample_type == DC14 or MC15 or see code...
// directory for plots to be already created
// add/remove trigger items in the vector<string> as you like


#include <TROOT.h>
#include <TStyle.h>
#include "TH1F.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TLine.h"
#include "TLatex.h"
#include "TLegend.h"
#include <TCanvas.h>
#include <TGraph.h>
#include <TEfficiency.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <iomanip>


using namespace std;


int draw_plot(vector<TEfficiency*> histos, vector<string> names, std::string label="", std::string name_plot="", std::string dir_plot="" )
{
    
    if(histos.size()<1) return 1;
    if(names.size()!=histos.size())
    {
        cout<<"name size diff from histos size\n"; return 1;
    }
    
    for(int i = 0;i<histos.size();i++)
    {
        TEfficiency* histo = histos.at(i);
        
        histo->SetMarkerColor(2+i);
        histo->SetMarkerStyle(20+i);
        histo->SetLineColor(2+i);
        histo->SetFillStyle(0);
        histo->SetLineStyle(1+i);
        histo->SetMarkerStyle(20+i);
    }
    
    TH1* tmp = histos.at(0)->GetCopyPassedHisto();
    tmp->SetMinimum(0.);
    tmp->SetMaximum(1.1);
    tmp->GetYaxis()->SetTitle("Eff.");
    
    TCanvas canvas("can","can",800,600);
    tmp->Reset();
    tmp->Draw();
    for(int i = 0;i<histos.size();i++) histos.at(i)->Draw("SAME");
    
    TLegend* leg = new TLegend(0.6,0.1,0.9,0.3);
    leg->SetBorderSize(0.);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    //leg-> SetNColumns(2);
    for(int i = 0;i<histos.size();i++)
    {
        stringstream leg_label;
        leg->AddEntry(histos.at(i),names.at(i).c_str(),"lp");
    }
    leg->Draw();
    
    TLatex l1; //l.SetTextAlign(12);
    l1.SetTextSize(0.05);
    l1.SetNDC();
    l1.SetTextColor(1);
    l1.DrawLatex(0.1,0.95,label.c_str());
    
    canvas.cd();
    std::string canvas_name(dir_plot);
    canvas_name += "/trigger_";
    canvas_name = canvas_name + name_plot.c_str()+".eps";
    canvas.Print(canvas_name.c_str());
    
    return 0;
}

int plotter(char * input_file_name, const std::string & sample_type, const std::string & dirplots)
{
    
    TFile* ifile =  new TFile(input_file_name);
    if(!ifile->IsOpen())
    {
        cout<<"cannot open file "<<input_file_name<<endl;
        return 0;
    }
    
    // fix style:
    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
    gStyle->SetEndErrorSize(0.);
    gStyle->SetMarkerStyle(20);
    gStyle->SetMarkerSize(1.2);
    gStyle->SetHistLineWidth(2.);
    gStyle->SetLineStyleString(2,"[12 12]");
    
    Int_t icol=0; // WHITE
    gStyle->SetFrameBorderMode(icol);
    gStyle->SetFrameFillColor(icol);
    gStyle->SetCanvasBorderMode(icol);
    gStyle->SetCanvasColor(icol);
    gStyle->SetPadBorderMode(icol);
    gStyle->SetPadColor(icol);
    gStyle->SetStatColor(icol);
    
    Int_t font=42; // Helvetica
    gStyle->SetTextFont(font);
    gStyle->SetLabelFont(font,"x");
    gStyle->SetTitleFont(font,"x");
    gStyle->SetLabelFont(font,"y");
    gStyle->SetTitleFont(font,"y");
    gStyle->SetLabelFont(font,"z");
    gStyle->SetTitleFont(font,"z");
    
    Double_t tsize=0.06;
    gStyle->SetLabelSize(tsize,"x");
    gStyle->SetTitleSize(tsize,"x");
    gStyle->SetLabelSize(tsize,"y");
    gStyle->SetTitleSize(tsize,"y");
    gStyle->SetLabelSize(tsize,"z");
    gStyle->SetTitleSize(tsize,"z");
    
    gStyle->SetPadTopMargin(0.07);
    gStyle->SetPadRightMargin(0.05);
    gStyle->SetPadBottomMargin(0.1);
    gStyle->SetPadLeftMargin(0.1);
    
    // set title offsets (for axis label)
    gStyle->SetTitleXOffset(0.8);
    
    vector<string> trigItems;
    if(sample_type == "DC14")
    {
        //DC14 TrigItems
        //trigItems.push_back("Dump");
        //trigItems.push_back("tau20_r1medium1");
        trigItems.push_back("tau20_r1perf");
        //trigItems.push_back("tau20_r1_idperf");
        trigItems.push_back("tau25_medium1_calo");
        //trigItems.push_back("tau25_medium1_calo_L1TAU12");
        trigItems.push_back("tau25_medium1_track");
        trigItems.push_back("tau25_medium1_tracktwo");
        trigItems.push_back("tau25_medium1_ptonly");
        //trigItems.push_back("tau25_medium1_ptonly_L1TAU12");
        trigItems.push_back("tau25_medium1_caloonly");
        trigItems.push_back("tau25_medium1_trackonly");
        trigItems.push_back("tau25_medium1_mvonly");
        trigItems.push_back("tau25_r1perf");
        //trigItems.push_back("tau25_r1perf_L1TAU8");
        //trigItems.push_back("tau25_medium1_mvonly_L1TAU8");
        trigItems.push_back("tau25_perf");
        //trigItems.push_back("tau25_perf_L1TAU8");
        //trigItems.push_back("tau25_ptonly_idperf");
        //trigItems.push_back("tau29_r1medium1");
        trigItems.push_back("tau29_r1perf");
        trigItems.push_back("tau35_medium1_calo");
        //trigItems.push_back("tau35_medium1_calo_L1TAU20");
        trigItems.push_back("tau35_medium1_track");
        trigItems.push_back("tau35_medium1_ptonly");
        //trigItems.push_back("tau35_medium1_ptonly_L1TAU20");
        trigItems.push_back("tau35_perf");
        trigItems.push_back("tau80_medium1_calo");
        trigItems.push_back("tau80_medium1_track");
        //trigItems.push_back("tau125_r1medium1");
        trigItems.push_back("tau125_medium1_calo");
        trigItems.push_back("tau125_medium1_track");
        trigItems.push_back("tau125_r1perf");
        trigItems.push_back("tau125_perf");
    }
    else if(sample_type == "MC15")
    {
        //MC15 trigItems
        trigItems.push_back("tau5_perf_ptonly_L1TAU8");
        trigItems.push_back("tau0_perf_ptonly_L1TAU12");
        trigItems.push_back("tau0_perf_ptonly_L1TAU60");
        // Run-II - No BDT: main track-based items
        trigItems.push_back("tau25_perf_tracktwo");
        trigItems.push_back("tau25_perf_tracktwo_L1TAU12");
        trigItems.push_back("tau25_perf_track");
        trigItems.push_back("tau25_perf_track_L1TAU12");
        // Run-II - No BDT: variations
        trigItems.push_back("tau25_perf_calo");
        trigItems.push_back("tau25_perf_calo_L1TAU12");
        trigItems.push_back("tau25_perf_ptonly");
        trigItems.push_back("tau25_perf_ptonly_L1TAU12");
        trigItems.push_back("tau25_perf_trackcalo");
        trigItems.push_back("tau25_perf_tracktwocalo");
        // Run-II - With BDT: main track-based items
        trigItems.push_back("tau25_medium1_tracktwo");
        trigItems.push_back("tau25_medium1_track");
        trigItems.push_back("tau25_medium1_tracktwo_L1TAU12");
        trigItems.push_back("tau25_medium1_tracktwo_L1TAU12IL");
        trigItems.push_back("tau25_medium1_tracktwo_L1TAU12IT");
        // Run-II - With BDT: main calo-based items
        trigItems.push_back("tau25_medium1_calo");
        trigItems.push_back("tau25_medium1_trackcalo");
        trigItems.push_back("tau25_medium1_tracktwocalo");
        // Run-II - With BDT: no pre-selection
        trigItems.push_back("tau25_medium1_mvonly");
        // Run-II - High-pT variations
        trigItems.push_back("tau35_medium1_tracktwo");
        //trigItems.push_back("tau35_medium1_tracktwo_L1TAU20");
        //trigItems.push_back("tau35_medium1_tracktwo_L1TAU20IL");
        //trigItems.push_back("tau35_medium1_tracktwo_L1TAU20IT");
        trigItems.push_back("tau35_medium1_track");
        trigItems.push_back("tau35_medium1_calo");
        trigItems.push_back("tau35_medium1_ptonly");
        //trigItems.push_back("tau35_medium1_ptonly_L1TAU20");
        trigItems.push_back("tau35_perf_tracktwo");
        trigItems.push_back("tau35_perf_ptonly");
        trigItems.push_back("tau80_medium1_calo");
        trigItems.push_back("tau80_medium1_tracktwo");
        trigItems.push_back("tau80_medium1_track");
        //trigItems.push_back("tau80_medium1_tracktwo_L1TAU60");
        trigItems.push_back("tau50_medium1_tracktwo_L1TAU12");
        trigItems.push_back("tau125_medium1_calo");
        trigItems.push_back("tau125_medium1_tracktwo");
        trigItems.push_back("tau125_medium1_track");
        trigItems.push_back("tau125_perf_tracktwo");
        trigItems.push_back("tau125_perf_ptonly");
        trigItems.push_back("tau160_medium1_tracktwo");
    }
    else if(sample_type == "MC15_25perf")
    {
        //MC15 trigItems
        // Run-II - No BDT: main track-based items
        trigItems.push_back("tau25_perf_tracktwo");
        trigItems.push_back("tau25_perf_track");
        // Run-II - No BDT: variations
        trigItems.push_back("tau25_perf_calo");
        trigItems.push_back("tau25_perf_ptonly");
        trigItems.push_back("tau25_perf_trackcalo");
        trigItems.push_back("tau25_perf_tracktwocalo");
    }
    else if(sample_type == "MC15_25medium1")
    {
        //MC15 trigItems
        // Run-II - With BDT: main track-based items
        trigItems.push_back("tau25_medium1_tracktwo");
        trigItems.push_back("tau25_medium1_track");
        // Run-II - With BDT: main calo-based items
        trigItems.push_back("tau25_medium1_calo");
        trigItems.push_back("tau25_medium1_trackcalo");
        trigItems.push_back("tau25_medium1_tracktwocalo");
        // Run-II - With BDT: no pre-selection
        trigItems.push_back("tau25_medium1_mvonly");
    }
    else if(sample_type == "MC15_25medium1_iso")
    {
        trigItems.push_back("tau25_medium1_tracktwo");
        trigItems.push_back("tau25_medium1_tracktwo_L1TAU12");
        trigItems.push_back("tau25_medium1_tracktwo_L1TAU12IL");
        trigItems.push_back("tau25_medium1_tracktwo_L1TAU12IT");
    }
    else
    {
        cout << "Wrong sample type " << endl;
        return 0;
    }
    
    vector<string> effCurvesNames;
    effCurvesNames.push_back("Pt");
    effCurvesNames.push_back("Pt1P");
    effCurvesNames.push_back("Pt3P");
    effCurvesNames.push_back("Eta");
    effCurvesNames.push_back("Phi");
    effCurvesNames.push_back("NTrack");
    effCurvesNames.push_back("NVtx");
    effCurvesNames.push_back("Mu");
    
    vector< vector<TEfficiency> > effCurvesTruthL1;
    vector< vector<TEfficiency> > effCurvesTruthHLT;
    vector< vector<TEfficiency> > effCurvesRecoL1;
    vector< vector<TEfficiency> > effCurvesRecoHLT;
    vector< vector<TEfficiency> > effCurvesTruthRecoL1;
    vector< vector<TEfficiency> > effCurvesTruthRecoHLT;
    
    for(int i=0;i<trigItems.size();i++)
    {
        string dirname("run_1/HLT/TauMon/Expert/");
        dirname += trigItems.at(i);
        cout<<"accessing dir "<<dirname<<endl;
        ifile->cd(dirname.c_str());
        
        TH1F* hnumL1 = 0;
        TH1F* hnumHLT = 0;
        TH1F* hdenum = 0;
        
        effCurvesTruthL1.push_back(vector<TEfficiency>());
        effCurvesTruthHLT.push_back(vector<TEfficiency>());
        effCurvesRecoL1.push_back(vector<TEfficiency>());
        effCurvesRecoHLT.push_back(vector<TEfficiency>());
        effCurvesTruthRecoL1.push_back(vector<TEfficiency>());
        effCurvesTruthRecoHLT.push_back(vector<TEfficiency>());
        
        gDirectory->cd("TurnOnCurves/TruthEfficiency");
        for(int j=0;j<effCurvesNames.size();j++)
        {
            string numNameL1("hTrueL1"), numNameHLT("hTrueHLT"), denumName("hTrueTau");
            numNameL1   += effCurvesNames.at(j) + "Num";
            numNameHLT   += effCurvesNames.at(j) + "Num";
            denumName += effCurvesNames.at(j) + "Denom";
            
            gDirectory->GetObject(numNameL1.c_str(),hnumL1);
            if(!hnumL1){ cout<< "cannot get "<< numNameL1.c_str()<<endl; return 0; }
            gDirectory->GetObject(numNameHLT.c_str(),hnumHLT);
            if(!hnumHLT){ cout<< "cannot get "<< numNameHLT.c_str()<<endl; return 0; }
            gDirectory->GetObject(denumName.c_str(),hdenum);
            if(!hdenum){ cout<< "cannot get "<< denumName.c_str()<<endl; return 0; }
            
            string effNameL1 = trigItems.at(i);
            effNameL1 += "_TruthL1_" + effCurvesNames.at(j);
            TEfficiency effTruthL1(*hnumL1,*hdenum);
            effTruthL1.SetName(effNameL1.c_str());
            effTruthL1.GetPassedHistogram()->GetXaxis()->SetTitle(effCurvesNames.at(j).c_str());
            effTruthL1.SetDirectory(0);
            effCurvesTruthL1.back().push_back(effTruthL1);
            
            string effNameHLT = trigItems.at(i);
            effNameHLT += "_TruthHLT_" + effCurvesNames.at(j);
            TEfficiency effTruthHLT(*hnumHLT,*hdenum);
            effTruthHLT.SetName(effNameHLT.c_str());
            effTruthHLT.GetPassedHistogram()->GetXaxis()->SetTitle(effCurvesNames.at(j).c_str());
            effTruthHLT.SetDirectory(0);
            effCurvesTruthHLT.back().push_back(effTruthHLT);
            
            vector<TEfficiency*> histos;
            histos.push_back(&effCurvesTruthL1.back().back());
            histos.push_back(&effCurvesTruthHLT.back().back());
            vector<string> names;
            names.push_back("L1");
            names.push_back("HLT");
            string label = trigItems.at(i); label += ", truth only";
            string name = trigItems.at(i); name += "_Truth_" + effCurvesNames.at(j);
            draw_plot(histos,names,label,name,dirplots);
        }
        
        hnumL1 = 0;
        hnumHLT = 0;
        hdenum = 0;
        ifile->cd(dirname.c_str());
        gDirectory->cd("TurnOnCurves/Truth+RecoEfficiency");
        for(int j=0;j<effCurvesNames.size();j++)
        {
            string numNameL1("hTruthRecoL1"), numNameHLT("hTruthRecoHLT"), denumName("hTruthRecoTau");
            numNameL1   += effCurvesNames.at(j) + "Num";
            numNameHLT   += effCurvesNames.at(j) + "Num";
            denumName += effCurvesNames.at(j) + "Denom";
            
            gDirectory->GetObject(numNameL1.c_str(),hnumL1);
            if(!hnumL1){ cout<< "cannot get "<< numNameL1.c_str()<<endl; return 0; }
            gDirectory->GetObject(numNameHLT.c_str(),hnumHLT);
            if(!hnumHLT){ cout<< "cannot get "<< numNameHLT.c_str()<<endl; return 0; }
            gDirectory->GetObject(denumName.c_str(),hdenum);
            if(!hdenum){ cout<< "cannot get "<< denumName.c_str()<<endl; return 0; }
            
            string effNameL1 = trigItems.at(i);
            effNameL1 += "_Truth+RecoL1_" + effCurvesNames.at(j);
            TEfficiency effRecoL1(*hnumL1,*hdenum);
            effRecoL1.SetName(effNameL1.c_str());
            effRecoL1.GetPassedHistogram()->GetXaxis()->SetTitle(effCurvesNames.at(j).c_str());
            effRecoL1.SetDirectory(0);
            effCurvesTruthRecoL1.back().push_back(effRecoL1);
            
            string effNameHLT = trigItems.at(i);
            effNameHLT += "_Truth+RecoHLT_" + effCurvesNames.at(j);
            TEfficiency effRecoHLT(*hnumHLT,*hdenum);
            effRecoHLT.SetName(effNameHLT.c_str());
            effRecoHLT.GetPassedHistogram()->GetXaxis()->SetTitle(effCurvesNames.at(j).c_str());
            effRecoHLT.SetDirectory(0);
            effCurvesTruthRecoHLT.back().push_back(effRecoHLT);
            
            vector<TEfficiency*> histos;
            histos.push_back(&effCurvesTruthRecoL1.back().back());
            histos.push_back(&effCurvesTruthRecoHLT.back().back());
            vector<string> names;
            names.push_back("L1");
            names.push_back("HLT");
            string label = trigItems.at(i); label += ", truth+reco only";
            string name = trigItems.at(i); name += "_Truth+Reco_" + effCurvesNames.at(j);
            draw_plot(histos,names,label,name,dirplots);
        }
        
        hnumL1 = 0;
        hnumHLT = 0;
        hdenum = 0;
        ifile->cd(dirname.c_str());
        gDirectory->cd("TurnOnCurves/RecoEfficiency");
        for(int j=0;j<effCurvesNames.size();j++)
        {
            string numNameL1("hRecoL1"), numNameHLT("hRecoHLT"), denumName("hRecoTau");
            numNameL1   += effCurvesNames.at(j) + "Num";
            numNameHLT   += effCurvesNames.at(j) + "Num";
            denumName += effCurvesNames.at(j) + "Denom";
            
            gDirectory->GetObject(numNameL1.c_str(),hnumL1);
            if(!hnumL1){ cout<< "cannot get "<< numNameL1.c_str()<<endl; return 0; }
            gDirectory->GetObject(numNameHLT.c_str(),hnumHLT);
            if(!hnumHLT){ cout<< "cannot get "<< numNameHLT.c_str()<<endl; return 0; }
            gDirectory->GetObject(denumName.c_str(),hdenum);
            if(!hdenum){ cout<< "cannot get "<< denumName.c_str()<<endl; return 0; }
            
            string effNameL1 = trigItems.at(i);
            effNameL1 += "_RecoL1_" + effCurvesNames.at(j);
            TEfficiency effRecoL1(*hnumL1,*hdenum);
            effRecoL1.SetName(effNameL1.c_str());
            effRecoL1.GetPassedHistogram()->GetXaxis()->SetTitle(effCurvesNames.at(j).c_str());
            effRecoL1.SetDirectory(0);
            effCurvesRecoL1.back().push_back(effRecoL1);
            
            string effNameHLT = trigItems.at(i);
            effNameHLT += "_RecoHLT_" + effCurvesNames.at(j);
            TEfficiency effRecoHLT(*hnumHLT,*hdenum);
            effRecoHLT.SetName(effNameHLT.c_str());
            effRecoHLT.GetPassedHistogram()->GetXaxis()->SetTitle(effCurvesNames.at(j).c_str());
            effRecoHLT.SetDirectory(0);
            effCurvesRecoHLT.back().push_back(effRecoHLT);
            
            vector<TEfficiency*> histos;
            histos.push_back(&effCurvesRecoL1.back().back());
            histos.push_back(&effCurvesRecoHLT.back().back());
            vector<string> names;
            names.push_back("L1");
            names.push_back("HLT");
            string label = trigItems.at(i); label += ", reco only";
            string name = trigItems.at(i); name += "_Reco_" + effCurvesNames.at(j);
            draw_plot(histos,names,label,name,dirplots);
        }
    }
    
    for(int j=0;j<effCurvesNames.size();j++)
    {
        vector<TEfficiency*> TruthhistosL1;
        vector<TEfficiency*> TruthhistosHLT;
        vector<TEfficiency*> TruthRecohistosL1;
        vector<TEfficiency*> TruthRecohistosHLT;
        vector<TEfficiency*> RecohistosL1;
        vector<TEfficiency*> RecohistosHLT;
        
        vector<string> names;
        
        for(int i=0;i<trigItems.size();i++)
        {
            TruthhistosL1.push_back(&effCurvesTruthL1.at(i).at(j));
            TruthhistosHLT.push_back(&effCurvesTruthHLT.at(i).at(j));
            TruthRecohistosL1.push_back(&effCurvesTruthRecoL1.at(i).at(j));
            TruthRecohistosHLT.push_back(&effCurvesTruthRecoHLT.at(i).at(j));
            RecohistosL1.push_back(&effCurvesRecoL1.at(i).at(j));
            RecohistosHLT.push_back(&effCurvesRecoHLT.at(i).at(j));
        }
        
        string TruthlabelL1 = "L1, truth only";
        string TruthlabelHLT = "HLT, truth only";
        string TruthnameL1("Truth_L1_"); TruthnameL1 += effCurvesNames.at(j).c_str();
        string TruthnameHLT("Truth_HLT_"); TruthnameHLT += effCurvesNames.at(j).c_str();
        
        string TruthRecolabelL1 = "L1, truth+reco";
        string TruthRecolabelHLT = "HLT, truth+reco";
        string TruthReconameL1("Truth+Reco_L1_"); TruthReconameL1 += effCurvesNames.at(j).c_str();
        string TruthReconameHLT("Truth+Reco_HLT_"); TruthReconameHLT += effCurvesNames.at(j).c_str();
        
        string RecolabelL1 = "L1, reco only";
        string RecolabelHLT = "HLT, reco only";
        string ReconameL1("Reco_L1_"); ReconameL1 += effCurvesNames.at(j).c_str();
        string ReconameHLT("Reco_HLT_"); ReconameHLT += effCurvesNames.at(j).c_str();
        
        draw_plot(TruthhistosL1,trigItems,TruthlabelL1,TruthnameL1,dirplots);
        draw_plot(TruthhistosHLT,trigItems,TruthlabelHLT,TruthnameHLT, dirplots);
        draw_plot(TruthRecohistosL1,trigItems,TruthRecolabelL1,TruthReconameL1,dirplots);
        draw_plot(TruthRecohistosHLT,trigItems,TruthRecolabelHLT,TruthReconameHLT, dirplots);
        draw_plot(RecohistosL1,trigItems,RecolabelL1,ReconameL1, dirplots);
        draw_plot(RecohistosHLT,trigItems,RecolabelHLT,ReconameHLT, dirplots);
    }
    
    ifile->Close();
    
    return 0;
}


