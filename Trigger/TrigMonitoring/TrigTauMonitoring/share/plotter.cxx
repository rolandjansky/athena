//
//  plotter.cxx
//
//  Created by Daniele Zanzi on 5/03/2015.
//
// to run, launch root and
// .L plotter.cxx
// plotter("physval.root")
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

string replaceString(string input, string replace_old, string replace_new){
    
    size_t pos,length;
    while(input.find(replace_old)!=std::string::npos){
        pos = input.find(replace_old);
        length = replace_old.length();
        input.replace(pos,length,replace_new);
    }
    //cout<<input.c_str()<<endl;
    return input;
}


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
    
    TLegend* leg = new TLegend(0.75,0.1,0.9,0.3);
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

int plotter(char * input_file_name)
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
    
    // draw chain counts
    string dirname("run_1/HLT/TauMon/Expert");
    cout<<"accessing dir "<<dirname<<endl;
    ifile->cd(dirname.c_str());
    TH1F* hL1counts = 0;
    TH1F* hHLTcounts = 0;
    gDirectory->GetObject("hL1Counts",hL1counts);
    if(!hL1counts){ cout<< "cannot get L1 counts"<<endl; return 0; }
    gDirectory->GetObject("hHLTCounts",hHLTcounts);
    if(!hHLTcounts){ cout<< "cannot get HLT counts"<<endl; return 0; }
    TCanvas canvas("can_counts","can_counts",800,600);
    gPad->SetLogy();
    canvas.SetBottomMargin(0.6);
    hL1counts->SetMinimum(hHLTcounts->GetMinimum()*0.8);
    hL1counts->GetXaxis()->SetTitle("");
    hL1counts->SetLineColor(kBlue);
    hL1counts->SetLineWidth(3);
    hHLTcounts->SetLineColor(kRed);
    hHLTcounts->SetLineWidth(3);
    TLegend* leg_count = new TLegend(0.8,0.8,0.95,0.9);
    leg_count->SetBorderSize(0.);
    leg_count->SetFillColor(0);
    leg_count->SetFillStyle(0);
    leg_count->AddEntry(hL1counts,"L1 counts","lp");
    leg_count->AddEntry(hHLTcounts,"HLT counts","lp");
    hL1counts->Draw();
    leg_count->Draw();
    hHLTcounts->Draw("same");
    canvas.Print("counts.eps");
    
    vector<string> dirplots;
    //dirplots.push_back("MC15_25perf");
    //dirplots.push_back("MC15_25perf_L1TAU12");
    //dirplots.push_back("MC15_25medium1");
    //dirplots.push_back("MC15_25medium1_iso");
    //dirplots.push_back("MC15_25medium1_perf");
    //dirplots.push_back("MC15_high");
    dirplots.push_back("all_singleTau");
    
    for(int dir_i=0;dir_i<dirplots.size();dir_i++){
        stringstream create_dir;
        create_dir << "rm -r " << dirplots.at(dir_i) <<"; mkdir " << dirplots.at(dir_i);
        system(create_dir.str().c_str());
        
        vector<string> trigItems;
        if(dirplots.at(dir_i) == "all_singleTau")
        {
            trigItems.push_back("tau0_perf_ptonly_L1TAU12");
            trigItems.push_back("tau0_perf_ptonly_L1TAU60");
            trigItems.push_back("tau25_perf_tracktwo");
            trigItems.push_back("tau25_perf_tracktwo_L1TAU12");
            trigItems.push_back("tau25_perf_ptonly");
            trigItems.push_back("tau25_perf_ptonly_L1TAU12");
            trigItems.push_back("tau25_loose1_tracktwo");
            trigItems.push_back("tau25_medium1_tracktwo");
            trigItems.push_back("tau25_medium1_tracktwo_L1TAU12");
            trigItems.push_back("tau25_medium1_tracktwo_L1TAU12IL");
            trigItems.push_back("tau25_medium1_tracktwo_L1TAU12IT");
            trigItems.push_back("tau25_tight1_tracktwo");
            trigItems.push_back("tau25_loose1_ptonly");
            trigItems.push_back("tau25_medium1_ptonly");
            trigItems.push_back("tau25_tight1_ptonly");
            trigItems.push_back("tau35_medium1_tracktwo");
            trigItems.push_back("tau35_medium1_tracktwo_L1TAU20");
            trigItems.push_back("tau35_medium1_ptonly");
            trigItems.push_back("tau35_loose1_tracktwo");
            trigItems.push_back("tau35_tight1_tracktwo");
            trigItems.push_back("tau35_perf_tracktwo");
            trigItems.push_back("tau35_perf_ptonly");
            trigItems.push_back("tau80_medium1_tracktwo");
            trigItems.push_back("tau80_medium1_tracktwo_L1TAU60");
            trigItems.push_back("tau50_medium1_tracktwo_L1TAU12");
            trigItems.push_back("tau125_medium1_tracktwo");
            trigItems.push_back("tau125_perf_tracktwo");
            trigItems.push_back("tau125_perf_ptonly");
            trigItems.push_back("tau160_medium1_tracktwo");
            
        } else if(dirplots.at(dir_i) == "MC15_25perf")
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
        else if(dirplots.at(dir_i) == "MC15_25perf_L1TAU12")
        {
            //MC15 trigItems
            // Run-II - No BDT: main track-based items
            trigItems.push_back("tau25_perf_tracktwo_L1TAU12");
            trigItems.push_back("tau25_perf_track_L1TAU12");
            // Run-II - No BDT: variations
            trigItems.push_back("tau25_perf_calo_L1TAU12");
            trigItems.push_back("tau25_perf_ptonly_L1TAU12");
        }
        else if(dirplots.at(dir_i) == "MC15_25medium1")
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
        else if(dirplots.at(dir_i) == "MC15_25medium1_iso")
        {
            trigItems.push_back("tau25_medium1_tracktwo");
            trigItems.push_back("tau25_medium1_tracktwo_L1TAU12");
            trigItems.push_back("tau25_medium1_tracktwo_L1TAU12IL");
            trigItems.push_back("tau25_medium1_tracktwo_L1TAU12IT");
        }
        else if(dirplots.at(dir_i) == "MC15_25medium1_perf")
        {
            trigItems.push_back("tau25_perf_ptonly_L1TAU12");
            trigItems.push_back("tau25_perf_tracktwo_L1TAU12");
            trigItems.push_back("tau25_medium1_tracktwo_L1TAU12");
            trigItems.push_back("tau25_medium1_tracktwo");
        }
        else if(dirplots.at(dir_i) == "MC15_high")
        {
            trigItems.push_back("tau125_perf_ptonly");
            trigItems.push_back("tau125_perf_tracktwo");
            trigItems.push_back("tau125_medium1_tracktwo");
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
            
            
            // plot L1 TauClus, TauClus vs EMIso, HLT Preselection Et and FTF #Trk
            TH1F* hL1tauCLus = 0;
            gDirectory->GetObject("L1RoI/hL1RoITauClus",hL1tauCLus);
            if(!hL1tauCLus){ cout<< "cannot get hL1tauCLus"<<endl; return 0; }
            string name_can("can_tauClus");
            name_can += trigItems.at(i);
            TCanvas canvas_tauClus(name_can.c_str(),name_can.c_str(),800,600);
            hL1tauCLus->SetLineColor(kBlue);
            hL1tauCLus->SetLineWidth(2);
            hL1tauCLus->Draw();
            std::string canvas_name(dirplots.at(dir_i).c_str());
            canvas_name += "/trigger_";
            canvas_name = canvas_name + trigItems.at(i).c_str() + "_tauClus.eps";
            canvas_tauClus.Print(canvas_name.c_str());
            
            TH2F* hHLTiso = 0;
            gDirectory->GetObject("L1RoI/hL1RoITauClusEMIso",hHLTiso);
            if(!hHLTiso){ cout<< "cannot get hHLTiso"<<endl; return 0; }
            name_can = "can_iso";
            name_can += trigItems.at(i);
            TCanvas canvas_iso(name_can.c_str(),name_can.c_str(),800,600);
            canvas_iso.SetRightMargin(0.1);
            hHLTiso->Draw("COLZ");
            TLine line(0.,3.,60.,9.);
            line.SetLineColor(2);
            line.SetLineStyle(2);
            line.SetLineWidth(2);
            line.Draw();
            TLine line_loose(0.,4.,60.,10.);
            line_loose.SetLineColor(1);
            line_loose.SetLineStyle(2);
            line_loose.SetLineWidth(2);
            line_loose.Draw();
            TLine line_tight(0.,2.5,60.,8.5);
            line_tight.SetLineColor(1);
            line_tight.SetLineStyle(2);
            line_tight.SetLineWidth(2);
            line_tight.Draw();
            canvas_name = dirplots.at(dir_i).c_str();
            canvas_name += "/trigger_";
            canvas_name = canvas_name + trigItems.at(i).c_str() + "_L1Iso.eps";
            canvas_iso.Print(canvas_name.c_str());
            
            TH1F* hHLTEt = 0;
            gDirectory->GetObject("PreselectionTau/hEFEt",hHLTEt);
            if(!hHLTEt){ cout<< "cannot get hEFEt"<<endl; return 0; }
            name_can = "can_Et";
            name_can += trigItems.at(i);
            TCanvas canvas_et(name_can.c_str(),name_can.c_str(),800,600);
            hHLTEt->SetLineColor(kBlue);
            hHLTEt->SetLineWidth(2);
            hHLTEt->Draw();
            canvas_name = dirplots.at(dir_i).c_str();
            canvas_name += "/trigger_";
            canvas_name = canvas_name + trigItems.at(i).c_str() + "_PreselEt.eps";
            canvas_et.Print(canvas_name.c_str());
            
            TH1F* hHLTnTrk = 0;
            gDirectory->GetObject("PreselectionTau/hFTFnTrack",hHLTnTrk);
            if(!hHLTnTrk){ cout<< "cannot get hFTFnTrack"<<endl; return 0; }
            name_can = "can_nTrk";
            name_can += trigItems.at(i);
            TCanvas canvas_nTrk(name_can.c_str(),name_can.c_str(),800,600);
            hHLTnTrk->SetLineColor(kBlue);
            hHLTnTrk->SetLineWidth(2);
            hHLTnTrk->Draw();
            canvas_name = dirplots.at(dir_i).c_str();
            canvas_name += "/trigger_";
            canvas_name = canvas_name + trigItems.at(i).c_str() + "_PreselNTrk.eps";
            canvas_nTrk.Print(canvas_name.c_str());
            
            
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
                
                if(dirplots.at(dir_i)=="all_singleTau") continue;
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
                draw_plot(histos,names,label,name,dirplots.at(dir_i).c_str());
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
                draw_plot(histos,names,label,name,dirplots.at(dir_i).c_str());
            }
            
            hnumL1 = 0;
            hnumHLT = 0;
            hdenum = 0;
            ifile->cd(dirname.c_str());
            gDirectory->cd("TurnOnCurves/RecoEfficiency");
            for(int j=0;j<effCurvesNames.size();j++)
            {
                
                //if(dirplots.at(dir_i)=="all_singleTau") continue;
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
                draw_plot(histos,names,label,name,dirplots.at(dir_i).c_str());
            }
        }
        
        for(int j=0;j<effCurvesNames.size();j++)
        {
            
            if(dirplots.at(dir_i)=="all_singleTau") continue;
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
            
            draw_plot(TruthhistosL1,trigItems,TruthlabelL1,TruthnameL1,dirplots.at(dir_i).c_str());
            draw_plot(TruthhistosHLT,trigItems,TruthlabelHLT,TruthnameHLT, dirplots.at(dir_i).c_str());
            draw_plot(TruthRecohistosL1,trigItems,TruthRecolabelL1,TruthReconameL1,dirplots.at(dir_i).c_str());
            draw_plot(TruthRecohistosHLT,trigItems,TruthRecolabelHLT,TruthReconameHLT, dirplots.at(dir_i).c_str());
            draw_plot(RecohistosL1,trigItems,RecolabelL1,ReconameL1, dirplots.at(dir_i).c_str());
            draw_plot(RecohistosHLT,trigItems,RecolabelHLT,ReconameHLT, dirplots.at(dir_i).c_str());
        }
    }
    
    ifile->Close();
    
    std::fstream latex_file;
    std:: string latex_file_name;
    latex_file_name="plotbook.tex";
    latex_file.open (latex_file_name.c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
    
    latex_file << "\\documentclass[12pt,a4paper]{article} \n";
    latex_file << "\\usepackage[a4paper,hmargin={1cm,1cm},vmargin={1cm,1.5cm}]{geometry}\n \\usepackage[applemac]{inputenc} \n \\usepackage{indentfirst}\n \\usepackage{graphicx}\n \\usepackage[english]{babel}\n \\usepackage[english]{hyperref}\n \\usepackage{pdfpages}\n \\usepackage{subfigure}\n \\usepackage{mathrsfs}\n \\usepackage{lscape}\n";
    latex_file << "\\begin{document} \n";
    latex_file << "\\listoffigures \n";
    
    //add counts
    latex_file << "\\begin{landscape} \n";
    latex_file << "\\begin{figure}[htbp]\n";
    latex_file << "\\begin{center}\n";
    latex_file << "\\includegraphics[width=1.1\\textwidth]{counts.eps} \n";
    latex_file << "\\end{center} \n";
    latex_file << "\\end{figure} \n";
    latex_file << "\\end{landscape} \n";
    //latex_file << "\\newpage \n";
    
    for(int i=0;i<trigItems.size();i++){
        string trig_name = replaceString(trigItems.at(i),"_"," ");
        latex_file << "\\begin{figure}[htbp]\n";
        latex_file << "\\begin{center}\n";
        latex_file << "\\caption{"<<trig_name.c_str()<<"}\n";
        latex_file << "\\label{"<<trigItems.at(i).c_str()<<"}\n";
        latex_file << "\\begin{tabular}{cc}\n";
        latex_file << "\\includegraphics[width=0.45\\textwidth]{all_singleTau/trigger_"<<trigItems.at(i).c_str()<<"_tauClus.eps} & \n";
        latex_file << "\\includegraphics[width=0.45\\textwidth]{all_singleTau/trigger_"<<trigItems.at(i).c_str()<<"_L1Iso.eps} \\\\ \n";
        latex_file << " L1 TauClus & L1 EMiso vs TauClus \\\\ \n";
        latex_file << "\\includegraphics[width=0.45\\textwidth]{all_singleTau/trigger_"<<trigItems.at(i).c_str()<<"_PreselEt.eps} & \n";
        latex_file << "\\includegraphics[width=0.45\\textwidth]{all_singleTau/trigger_"<<trigItems.at(i).c_str()<<"_PreselNTrk.eps} \\\\ \n";
        latex_file << " HLT Presel Et & HLT Presel FTF nTrk \\\\ \n";
        latex_file << "\\includegraphics[width=0.45\\textwidth]{all_singleTau/trigger_"<<trigItems.at(i).c_str()<<"_Truth+Reco_Pt.eps} & \n";
        latex_file << "\\includegraphics[width=0.45\\textwidth]{all_singleTau/trigger_"<<trigItems.at(i).c_str()<<"_Truth+Reco_NVtx.eps} \\\\ \n";
        latex_file << " Online vs Truth+Offline Reco Eff & Online vs Truth+Offline Reco Eff \\\\ \n";
        latex_file << "\\includegraphics[width=0.45\\textwidth]{all_singleTau/trigger_"<<trigItems.at(i).c_str()<<"_Truth+Reco_NTrack.eps} & \n";
        latex_file << "\\includegraphics[width=0.45\\textwidth]{all_singleTau/trigger_"<<trigItems.at(i).c_str()<<"_Reco_Pt.eps} \\\\ \n";
        latex_file << " Online vs Truth+Offline Reco Eff & Online vs Truth+OfflineReco+HLTPresel Eff \\\\ \n";
        latex_file << "\\end{tabular} \n";
        latex_file << "\\end{center} \n";
        latex_file << "\\end{figure} \n";
        if(i%10==0)latex_file << "\\clearpage \n";
    }
    
    //latex_file << "\\end{landscape} \n";
    latex_file << "\\end{document} \n";
    latex_file.close();
    
    
    return 0;
}


