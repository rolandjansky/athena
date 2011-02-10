
#include "TH1.h"
#include "TVirtualPad.h"
#include "TList.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TDirectory.h"
#include "TStyle.h"
#include <iostream>


void fillAccordingToLabels( TH1* dest, TH1* source ) {
  for ( unsigned k = 1; k <= source->GetNbinsX(); ++k ) {
    dest->Fill(source->GetXaxis()->GetBinLabel(k), source->GetBinContent(k));
  }
}

void normalizeLabel(TString& label, const TString& sel) {
  label.Replace(0,label.Last('/')+1, "");
  label.ReplaceAll("TrigSteer_L2", "");
  label.ReplaceAll("TrigSteer_EF", "");
  ////  label.ReplaceAll("TrigSteer_L2", "");
  label.ReplaceAll(sel+"_","");
  label.Remove(TString::kBoth, '_');
}

TObject* drawSequencesTimingGlobal( TVirtualPad* pad,
				    TList* list, const TString& sel, int bins=30 ) {
  const unsigned N = (list->GetSize());

  TH1 *total = new TH1F("total",";;time[ms]", N, 0, N);  
  total->SetFillColor(kRed);
  double totalTime = 0.;
    
  for ( unsigned i = 0; i < N; ++i ) {
    //     
    TH1* h = (TH1*) list->At(i);
    TString label = TString(h->GetName());
    
    //label.Replace(0, 22, "");
    normalizeLabel(label, sel);
    unsigned bin = i+1;
    
    double entry = 0.0;
    entry = h->GetMean()*h->GetEntries();
    
    total->GetXaxis()->SetBinLabel(bin, label);
    total->SetBinContent(bin, entry);    
    totalTime += entry;
  }
  // drawing
  pad->cd();
  total->GetXaxis()->SetRangeUser(0, bins);
  total->GetXaxis()->LabelsOption("v>");
  total->GetYaxis()->SetLabelSize(0.02);
  total->SetStats(0);
  total->Draw("hbar1");

  std::cout << "/*****************************************/ " << endl; 
  std::cout << "Total summed up time for " << sel << " is " << totalTime << endl; 
  std::cout << "/*****************************************/ " << endl; 
  return total;
}

TObject* drawSequencesTimingGlobalFromTH2( TVirtualPad* pad,
				    TList* list, const TString& file_prefix, const TString& sel, int bins=30 ) {

  TH2F* hprof = (TH2F*) list->FindObject("TrigSteer_"+file_prefix+"_"+sel);
  
  unsigned Nbinsx=hprof->GetNbinsX();
  unsigned Nbinsy=hprof->GetNbinsY();

  TH1 *total = new TH1F("total",";;time[ms]", Nbinsx, 0, Nbinsx);  
  total->SetFillColor(kRed);

  TH1 *htemp = new TH1F("htemp",";;time[ms]", Nbinsy, 0, Nbinsy);
  htemp->SetDirectory(0);

  for ( unsigned i = 0; i < Nbinsx; ++i ) {
    
    unsigned binx = i+1;
    
    TString label = TString(hprof->GetXaxis()->GetBinLabel(binx));
    label.Replace(0, 22, "");
      
      
    total->GetXaxis()->SetBinLabel(binx, label);

    htemp->Reset();
    htemp=hprof->ProjectionY("",binx,binx);
    double entry=htemp->GetMean()*htemp->GetEntries();
      
    total->SetBinContent(binx, entry );   
        
  }

  delete htemp;
  // drawing
  pad->cd();
  total->GetXaxis()->SetRangeUser(0, bins);
  total->GetXaxis()->LabelsOption("v>");
  total->GetYaxis()->SetLabelSize(0.02);
  total->SetStats(0);
  total->Draw("hbar1");
  return total;
  
}

TObject* drawSequencesTimingPerEvent( TVirtualPad* pad,
				      TList* list,  const TString& sel, int bins=30 ) {
  
  // sort objects in the list by highest mean value
  const unsigned N = (list->GetSize());

  TH1 *mean = new TH1F("mean",sel+"Per Event;;time [ms]", N, 0, N);
  TH1 *rms  = new TH1F("rms","RMS", N, 0, N);
  //  TH1D mean_plus_rms("mean_plus_rms","RMS+", N, 0, N);
  //  TH1D mean_minus_rms("mean_minus_rms","RMS-", N, 0, N);
  
  TH1 *min  = new TH1F ("min","Min", N, 0, N);
  TH1 *max  = new TH1F("max","Max", N, 0, N);
  TH1 *overflows = new TH1I("overflow","Overflows", N, 0, N);

  
  cerr << "size " << N << endl;
  
  for ( unsigned i = 0; i < N; ++i ) {
    // 

    TH1* h = (TH1*) list->At(i);
    TString label = TString(h->GetName());

    normalizeLabel(label, sel);

    unsigned bin = i+1;
    mean->SetBinContent(bin, h->GetMean());
    mean->GetXaxis()->SetBinLabel(bin, label);

    rms->SetBinContent(bin, h->GetRMS());
    rms->GetXaxis()->SetBinLabel(bin, label);

    overflows->SetBinContent(bin, h->GetBinContent(h->GetNbinsX()+1));
    overflows->GetXaxis()->SetBinLabel(bin, label);


    // find first bin != 0
    for ( unsigned j = 1; j <= h->GetNbinsX(); ++j ) {
      if (  h->GetBinContent(j) != 0.0 ) {
	min->SetBinContent(bin, h->GetBinLowEdge(j));
	break;
      }
    }
    // find first bin != 0 from left
    for ( unsigned j = h->GetNbinsX()+1; j >= 1 ; --j ) {
      if (  h->GetBinContent(j) != 0.0 ) {
	max->SetBinContent(bin, h->GetBinLowEdge(j));
	break;
      }
    } 

    min->GetXaxis()->SetBinLabel(bin, label);  
    max->GetXaxis()->SetBinLabel(bin, label);  
  
    //cerr << "got min and max" << endl;
  }


  pad->cd();
  
  TH1F *mean_plus_rms = (TH1F*)mean->Clone("means_plus_rms");
  mean_plus_rms->Add(rms);
  
  TH1F* mean_minus_rms = (TH1F*)mean->Clone("means_minus_rms");
  mean_minus_rms->Add(rms, -1.0);
  

  mean->GetXaxis()->LabelsOption("v>");
  mean->GetYaxis()->SetLabelSize(0.02);
  mean->SetStats(0);
  mean->GetXaxis()->SetRangeUser(0, bins);
  //L2
  //mean->GetYaxis()->SetRangeUser(0, 200);
  //EF
  mean->GetYaxis()->SetLabelSize(0.02);
  cout << "igb:" << sel  << endl;
  //mean->GetYaxis()->SetRangeUser(0, 2000);
  mean->Draw("hbar");  

  TH1F* max_to_draw = (TH1F*)mean->Clone("timing");
  max_to_draw->Reset();
  fillAccordingToLabels(max_to_draw, max);

  TH1F*  mean_plus_rms_to_draw = (TH1F*)mean->Clone("timingrms+");
  mean_plus_rms_to_draw->Reset();
  fillAccordingToLabels(mean_plus_rms_to_draw, mean_plus_rms);

  TH1F*  mean_minus_rms_to_draw = (TH1F*)mean->Clone("timingrms-");
  mean_minus_rms_to_draw->Reset();
  fillAccordingToLabels(mean_minus_rms_to_draw, mean_minus_rms);

  max_to_draw->SetFillColor(kRed);
  max_to_draw->SetLineColor(kRed);
  max_to_draw->Draw("hbar1");
  
  mean_plus_rms_to_draw->SetFillColor(kOrange);
  mean_plus_rms_to_draw->SetLineColor(kOrange);  
  mean_plus_rms_to_draw->Draw("hbar1same");

  //mean->SetFillColor(kRed);
  //mean_to_draw->SetLineColor(kBlack);
  //mean_to_draw->SetMarkerStyle(kFullSquare);  
  //mean_to_draw->SetMarkerColor(kBlue);  
  //mean_to_draw->SetFillColor(kBlue);
  //mean_to_draw->Draw("hbarsameP");
  
  //mean->SetFillColor(kRed);
  mean->SetLineColor(kBlack);
  mean->SetMarkerStyle(kFullSquare);
  mean->SetMarkerColor(kBlue);
  mean->SetFillColor(kBlue);
  mean->Draw("hbarsameP");

  //mean_minus_rms_to_draw->SetFillColor(kBlue);
  //mean_minus_rms_to_draw->SetLineColor(kBlue);  
  //mean_minus_rms_to_draw->Draw("hbar1same");
  
  //  min->SetFillColor(kYellow);
  //  min->SetLineColor(kYellow);
  //  min->Draw("same");    

  TLegend *leg = new TLegend(0.8, 0.8, 1., 1.);
  leg->AddEntry(max_to_draw, "Max values", "f");
  leg->AddEntry(mean, "Average Timing", "p");
  leg->AddEntry(mean_plus_rms_to_draw, "RMS", "f");
  leg->Draw();

  pad->Update();

  return mean;
}

TObject* drawSequencesTimingPerEventFromTH2( TVirtualPad* pad,
					     TList* list, const TString& file_prefix, const TString& sel, int bins=30 ) {
  
  TH2F* hprof = (TH2F*) list->FindObject("TrigSteer_"+file_prefix+"_"+sel);
  
  unsigned Nbinsx=hprof->GetNbinsX();
  unsigned Nbinsy=hprof->GetNbinsY();
  

  // sort objects in the list by highest mean value
  TH1 *mean = new TH1F("mean",sel+"Per Event;;time [ms]", Nbinsx, 0, Nbinsx);
  TH1 *rms  = new TH1F("rms","RMS", Nbinsx, 0, Nbinsx);
  //  TH1D mean_plus_rms("mean_plus_rms","RMS+", N, 0, N);
  //  TH1D mean_minus_rms("mean_minus_rms","RMS-", N, 0, N);
  
  TH1 *min  = new TH1F ("min","Min", Nbinsx, 0, Nbinsx);
  TH1 *max  = new TH1F("max","Max", Nbinsx, 0, Nbinsx);
  TH1 *overflows = new TH1I("overflow","Overflows", Nbinsx, 0, Nbinsx);

  TH1 *htemp = new TH1F("htemp",";;time[ms]", Nbinsy, 0, Nbinsy);
  htemp->SetDirectory(0);
  
  for ( unsigned i = 0; i < Nbinsx; ++i ) {

    unsigned binx = i+1;

    TString label = TString(hprof->GetXaxis()->GetBinLabel(binx));
    label.Replace(0, 22, "");

    htemp->Reset();
    htemp=hprof->ProjectionY("",binx,binx);

    mean->SetBinContent(binx, htemp->GetMean());
    mean->GetXaxis()->SetBinLabel(binx, label);

    rms->SetBinContent(binx, htemp->GetRMS());
    rms->GetXaxis()->SetBinLabel(binx, label);

    overflows->SetBinContent(binx, htemp->GetBinContent(htemp->GetNbinsX()+1));
    overflows->GetXaxis()->SetBinLabel(binx, label);


    // find first bin != 0
    for ( unsigned j = 1; j <= htemp->GetNbinsX(); ++j ) {
      if (  htemp->GetBinContent(j) != 0.0 ) {
	min->SetBinContent(binx, htemp->GetBinLowEdge(j));
	break;
      }
    }
    // find first bin != 0 from left
    for ( unsigned j = htemp->GetNbinsX()+1; j >= 1 ; --j ) {
      if (  htemp->GetBinContent(j) != 0.0 ) {
	max->SetBinContent(binx, htemp->GetBinLowEdge(j));
	break;
      }
    } 

    min->GetXaxis()->SetBinLabel(binx, label);  
    max->GetXaxis()->SetBinLabel(binx, label);  
  
    //cerr << "got min and max" << endl;
  }

  delete htemp;

  pad->cd();
  
  TH1F *mean_plus_rms = (TH1F*)mean->Clone("means_plus_rms");
  mean_plus_rms->Add(rms);
  
  TH1F* mean_minus_rms = (TH1F*)mean->Clone("means_minus_rms");
  mean_minus_rms->Add(rms, -1.0);
  

  mean->GetXaxis()->LabelsOption("v>");
  mean->GetYaxis()->SetLabelSize(0.02);
  mean->SetStats(0);
  mean->GetXaxis()->SetRangeUser(0, bins);
  //L2
  //mean->GetYaxis()->SetRangeUser(0, 200);
  //EF
  mean->GetYaxis()->SetLabelSize(0.02);
  cout << "igb:" << sel  << endl;
  //mean->GetYaxis()->SetRangeUser(0, 2000);
  mean->Draw("hbar");  

  TH1F* max_to_draw = (TH1F*)mean->Clone("timing");
  max_to_draw->Reset();
  fillAccordingToLabels(max_to_draw, max);

  TH1F*  mean_plus_rms_to_draw = (TH1F*)mean->Clone("timingrms+");
  mean_plus_rms_to_draw->Reset();
  fillAccordingToLabels(mean_plus_rms_to_draw, mean_plus_rms);

  TH1F*  mean_minus_rms_to_draw = (TH1F*)mean->Clone("timingrms-");
  mean_minus_rms_to_draw->Reset();
  fillAccordingToLabels(mean_minus_rms_to_draw, mean_minus_rms);

  max_to_draw->SetFillColor(kRed);
  max_to_draw->SetLineColor(kRed);
  max_to_draw->Draw("hbar1");
  
  mean_plus_rms_to_draw->SetFillColor(kOrange);
  mean_plus_rms_to_draw->SetLineColor(kOrange);  
  mean_plus_rms_to_draw->Draw("hbar1same");

  //mean->SetFillColor(kRed);
  //mean_to_draw->SetLineColor(kBlack);
  //mean_to_draw->SetMarkerStyle(kFullSquare);  
  //mean_to_draw->SetMarkerColor(kBlue);  
  //mean_to_draw->SetFillColor(kBlue);
  //mean_to_draw->Draw("hbarsameP");
  
  //mean->SetFillColor(kRed);
  mean->SetLineColor(kBlack);
  mean->SetMarkerStyle(kFullSquare);
  mean->SetMarkerColor(kBlue);
  mean->SetFillColor(kBlue);
  mean->Draw("hbarsameP");

  //mean_minus_rms_to_draw->SetFillColor(kBlue);
  //mean_minus_rms_to_draw->SetLineColor(kBlue);  
  //mean_minus_rms_to_draw->Draw("hbar1same");
  
  //  min->SetFillColor(kYellow);
  //  min->SetLineColor(kYellow);
  //  min->Draw("same");    

  TLegend *leg = new TLegend(0.8, 0.8, 1., 1.);
  leg->AddEntry(max_to_draw, "Max values", "f");
  leg->AddEntry(mean, "Average Timing", "p");
  leg->AddEntry(mean_plus_rms_to_draw, "RMS", "f");
  leg->Draw();

  pad->Update();

  return mean;
}

TObject* drawSequencesTimingPerEventNoEvents( TVirtualPad* pad,
				      TList* list, const TString& sel, int bins=30 ) {
  
  // sort objects in the list by highest mean value
  const unsigned N = (list->GetSize());
  TH1 *mean = new TH1F("mean","Events", N, 0, N);
  TH1 *rms  = new TH1F("rms","RMS", N, 0, N);
  //  TH1D mean_plus_rms("mean_plus_rms","RMS+", N, 0, N);
  //  TH1D mean_minus_rms("mean_minus_rms","RMS-", N, 0, N);
  
  TH1 *min  = new TH1F ("min","Min", N, 0, N);
  TH1 *max  = new TH1F("max","Max", N, 0, N);
  TH1 *overflows = new TH1I("overflow","Overflows", N, 0, N);

  TH1 *event = new TH1F("event",sel+" Per Event;;Events", N, 0, N);

  cerr << "size " << N << endl;
  
  for ( unsigned i = 0; i < N; ++i ) {
    // 

    TH1* h = (TH1*) list->At(i);
    TString label = TString(h->GetName());

    normalizeLabel(label, sel);

    unsigned bin = i+1;
    mean->SetBinContent(bin, h->GetMean());
    mean->GetXaxis()->SetBinLabel(bin, label);

    event->SetBinContent(bin, h->GetEntries());
    event->GetXaxis()->SetBinLabel(bin, label);

    rms->SetBinContent(bin, h->GetRMS());
    rms->GetXaxis()->SetBinLabel(bin, label);

    overflows->SetBinContent(bin, h->GetBinContent(h->GetNbinsX()+1));
    overflows->GetXaxis()->SetBinLabel(bin, label);


    // find first bin != 0
    for ( unsigned j = 1; j <= h->GetNbinsX(); ++j ) {
      if (  h->GetBinContent(j) != 0.0 ) {
	min->SetBinContent(bin, h->GetBinLowEdge(j));
	break;
      }
    }
    // find first bin != 0 from left
    for ( unsigned j = h->GetNbinsX()+1; j >= 1 ; --j ) {
      if (  h->GetBinContent(j) != 0.0 ) {
	max->SetBinContent(bin, h->GetBinLowEdge(j));
	break;
      }
    } 

    min->GetXaxis()->SetBinLabel(bin, label);  
    max->GetXaxis()->SetBinLabel(bin, label);  
  
    //cerr << "got min and max" << endl;
  }


  pad->cd();
  
  TH1F *mean_plus_rms = (TH1F*)mean->Clone("means_plus_rms");
  mean_plus_rms->Add(rms);
  
  TH1F* mean_minus_rms = (TH1F*)mean->Clone("means_minus_rms");
  mean_minus_rms->Add(rms, -1.0);

  mean->GetXaxis()->LabelsOption("v>");
  mean->GetYaxis()->SetLabelSize(0.02);
  mean->SetStats(0);
  mean->GetXaxis()->SetRangeUser(0, bins);
  //mean->GetYaxis()->SetRangeUser(0, 200);
  //mean->Draw("hbar");  
  
  TH1F* max_to_draw = (TH1F*)mean->Clone("timing");
  max_to_draw->Reset();
  fillAccordingToLabels(max_to_draw, max);

  TH1F*  mean_plus_rms_to_draw = (TH1F*)mean->Clone("timingrms+");
  mean_plus_rms_to_draw->Reset();
  fillAccordingToLabels(mean_plus_rms_to_draw, mean_plus_rms);

  TH1F*  mean_minus_rms_to_draw = (TH1F*)mean->Clone("timingrms-");
  mean_minus_rms_to_draw->Reset();
  fillAccordingToLabels(mean_minus_rms_to_draw, mean_minus_rms);

  TH1F* event_to_draw = (TH1F*)mean->Clone("timing");
  event_to_draw->Reset();
  fillAccordingToLabels(event_to_draw, event);

  pad->cd();
  //pad->SetLogx();

  //event_to_draw->GetXaxis()->LabelsOption("v>");
  event_to_draw->GetYaxis()->SetLabelSize(0.015);
  event_to_draw->SetStats(0);
  event_to_draw->GetXaxis()->SetRangeUser(0, bins);
  //event_to_draw->GetYaxis()->SetRangeUser(0, 10000);
  event_to_draw->Draw("hbar1");  

  event_to_draw->SetLineColor(kBlack);
  event_to_draw->SetMarkerStyle(kFullSquare);
  event_to_draw->SetMarkerColor(kBlue);
  event_to_draw->SetFillColor(kBlue);
  event_to_draw->Draw("hbar1");

  //TLegend *leg = new TLegend(0.8, 0.8, 1., 1.);
  //leg->AddEntry(event_to_draw, "Events", "p");
  //leg->Draw();

  //pad->Update();

  return event;
}

TObject* drawSequencesTimingPerEventNoEventsFromTH2( TVirtualPad* pad,
						     TList* list, const TString& file_prefix, const TString& sel, int bins=30 ) {
  
  TH2F* hprof = (TH2F*) list->FindObject("TrigSteer_"+file_prefix+"_"+sel);

  unsigned Nbinsx=hprof->GetNbinsX();
  unsigned Nbinsy=hprof->GetNbinsY();

  // sort objects in the list by highest mean value
  TH1 *mean = new TH1F("mean","Events", Nbinsx, 0, Nbinsx);
  TH1 *rms  = new TH1F("rms","RMS", Nbinsx, 0, Nbinsx);
  //  TH1D mean_plus_rms("mean_plus_rms","RMS+", N, 0, N);
  //  TH1D mean_minus_rms("mean_minus_rms","RMS-", N, 0, N);
  
  TH1 *min  = new TH1F ("min","Min", Nbinsx, 0, Nbinsx);
  TH1 *max  = new TH1F("max","Max", Nbinsx, 0, Nbinsx);
  TH1 *overflows = new TH1I("overflow","Overflows", Nbinsx, 0, Nbinsx);

  TH1 *event = new TH1F("event",sel+" Per Event;;Events", Nbinsx, 0, Nbinsx);

  TH1 *htemp = new TH1F("htemp",";;time[ms]", Nbinsy, 0, Nbinsy);
  htemp->SetDirectory(0);

  for ( unsigned i = 0; i < Nbinsx; ++i ) {

    unsigned binx = i+1;
    
    TString label = TString(hprof->GetXaxis()->GetBinLabel(binx));
    label.Replace(0, 22, "");

    htemp->Reset();
    htemp=hprof->ProjectionY("",binx,binx);

    mean->SetBinContent(binx, htemp->GetMean());
    mean->GetXaxis()->SetBinLabel(binx, label);

    event->SetBinContent(binx, htemp->GetEntries());
    event->GetXaxis()->SetBinLabel(binx, label);

    rms->SetBinContent(binx, htemp->GetRMS());
    rms->GetXaxis()->SetBinLabel(binx, label);

    overflows->SetBinContent(binx, htemp->GetBinContent(htemp->GetNbinsX()+1));
    overflows->GetXaxis()->SetBinLabel(binx, label);


    // find first bin != 0
    for ( unsigned j = 1; j <= htemp->GetNbinsX(); ++j ) {
      if (  htemp->GetBinContent(j) != 0.0 ) {
	min->SetBinContent(binx, htemp->GetBinLowEdge(j));
	break;
      }
    }
    // find first bin != 0 from left
    for ( unsigned j = htemp->GetNbinsX()+1; j >= 1 ; --j ) {
      if (  htemp->GetBinContent(j) != 0.0 ) {
	max->SetBinContent(binx, htemp->GetBinLowEdge(j));
	break;
      }
    } 

    min->GetXaxis()->SetBinLabel(binx, label);  
    max->GetXaxis()->SetBinLabel(binx, label);  
  
    //cerr << "got min and max" << endl;
  }

  delete htemp;
  pad->cd();
  
  TH1F *mean_plus_rms = (TH1F*)mean->Clone("means_plus_rms");
  mean_plus_rms->Add(rms);
  
  TH1F* mean_minus_rms = (TH1F*)mean->Clone("means_minus_rms");
  mean_minus_rms->Add(rms, -1.0);

  mean->GetXaxis()->LabelsOption("v>");
  mean->GetYaxis()->SetLabelSize(0.02);
  mean->SetStats(0);
  mean->GetXaxis()->SetRangeUser(0, bins);
  //mean->GetYaxis()->SetRangeUser(0, 200);
  //mean->Draw("hbar");  
  
  TH1F* max_to_draw = (TH1F*)mean->Clone("timing");
  max_to_draw->Reset();
  fillAccordingToLabels(max_to_draw, max);

  TH1F*  mean_plus_rms_to_draw = (TH1F*)mean->Clone("timingrms+");
  mean_plus_rms_to_draw->Reset();
  fillAccordingToLabels(mean_plus_rms_to_draw, mean_plus_rms);

  TH1F*  mean_minus_rms_to_draw = (TH1F*)mean->Clone("timingrms-");
  mean_minus_rms_to_draw->Reset();
  fillAccordingToLabels(mean_minus_rms_to_draw, mean_minus_rms);

  TH1F* event_to_draw = (TH1F*)mean->Clone("timing");
  event_to_draw->Reset();
  fillAccordingToLabels(event_to_draw, event);

  pad->cd();
  //pad->SetLogx();

  //event_to_draw->GetXaxis()->LabelsOption("v>");
  event_to_draw->GetYaxis()->SetLabelSize(0.015);
  event_to_draw->SetStats(0);
  event_to_draw->GetXaxis()->SetRangeUser(0, bins);
  //event_to_draw->GetYaxis()->SetRangeUser(0, 10000);
  event_to_draw->Draw("hbar1");  

  event_to_draw->SetLineColor(kBlack);
  event_to_draw->SetMarkerStyle(kFullSquare);
  event_to_draw->SetMarkerColor(kBlue);
  event_to_draw->SetFillColor(kBlue);
  event_to_draw->Draw("hbar1");

  //TLegend *leg = new TLegend(0.8, 0.8, 1., 1.);
  //leg->AddEntry(event_to_draw, "Events", "p");
  //leg->Draw();

  //pad->Update();

  return event;
}

void draw (const std::string& dir_name, const std::string& file_prefix, const std::string& selector, const std::string& slice_selector = "all") {
  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetPadLeftMargin(0.5);
  gStyle->SetLabelSize(0.025);
  //  gStyle->SetPadLeftMargin(0.6);
  //gStyle->SetFillColor(kWhite);
  gStyle->SetPadColor(kWhite);
  gStyle->SetFrameFillColor(kWhite);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetCanvasBorderMode(0);

  TDirectory *dir = gDirectory->GetDirectory(dir_name.c_str());
  //cout << "dir " << dir << endl;
  TList *l = (TList*)dir->GetListOfKeys();
  TList *seqHistogramsList = new TList();
  std::string level_selector = "_"+file_prefix+"_";
  std::string TotalTime_selector = "TotalTime";

  const unsigned egammaItems=56;
  std::string egamma[egammaItems] =        {
    "em20_passHLT", "em20i_passHLT",
    "em105_passHLT",
    "2e5_medium", "e5_medium",
    "e10_medium",
    "e20_loose",
    "e10_medium_SiTrk", "e20_loose_SiTrk",
    "e20_loose_passL2", "e20_loose_passEF", "e20i_loose",
    "g10_loose",
    "g20_loose", "g20i_loose",
    "2g20_loose",
    "2e5_medium1", "2e6_medium", "2e6_medium1", "e12_medium",
    "g25_loose", "e5_medium1", "e6_medium", "e6_medium1",
    "e15_medium", "e15i_medium", "e22i_tight", "e25_loose", "e25i_loose", "e25i_medium1",
    "e55_loose1", "e105_loose1",
    "e10_medium_SiTrk_robust", "e20_loose_SiTrk_robust", "e5_medium_e10_loose",
    "2e12_tight", "2e15_medium", "3e15_medium", "2e20_loose",
    "g15_loose", "g25i_loose", "g55_tight", "g105", "g150", "2g17i_tight", "Zee", "Jpsiee",
    "2g10_loose", "3g10_loose",
    "g3_nocut",
    "e5_medium_Ringer","e10_medium_Ringer",
    "g20_loose_cnv",
    "e5_medium_L2SW","e10_medium_L2SW","e20_loose_L2SW"
  };
  
  const unsigned muonItems=26;
  std::string muon[muonItems] = {
    "mu4", "mu6",
    "mu10",
    "2mu4", "mu4_mu6","2mu10",
    "mu4_tile", "mu4_trod",
    "mu20_MSonly", "mu10i_loose",
    "mu15", "2mu6",
    "mu15i_loose", "mu20i_loose", "mu20", "mu40", "2mu20", "3mu6", "stau", "mu10_MuTagIMO",
    "2mu4_cl",
    "mu20i_loose_MSonly",
    "mu4_SiTrk", "mu10_SiTrk", "mu20_passHLT", "mu40_passHLT"
  };

  const unsigned tauItems=30;
  std::string tau[tauItems] = {
    "tau16i_loose", "tau20i_loose", "tau50_loose",
    "2tau20i_loose",
    "2tau20i_loose_PT",
    "tau16i_medium",
    "tau16i_tight",
    "trk9_loose",
    "trk16_loose",
    "tau50_medium", "tau50_tight", "tau84_loose", "tau125_loose", "2tau29i_loose", "2tau29i_medium",
    "tau12_loose", "tau12_loose_PT", "tau16_loose", "tau20_loose", "tau29_loose",
    "tau29i_loose", "tau38_loose",
    "tauNoCut_TRT",
    "tau16_loose_SiTrk", "tau50_loose_SiTrk", "2tau29_loose",
    "tauNoCut", "tauNoCut_SiTrk", "tau16_loose_PT", "tau50_loose_PT"
  };

  const unsigned BphysicsItems=60;
  std::string Bphysics[BphysicsItems] = {
    "mu4_DsPhiPi_FS",
    "MU4_DiMu_FS",
    "MU4_Jpsimumu_FS", "MU4_Upsimumu_FS","MU4_Bmumu_FS",
    "2mu4_DiMu",
    "2mu4_Jpsimumu","2mu4_Upsimumu", "2mu4_Bmumu",
    "MU4_Jpsie5e3_FS",
    "MU6_DsPhiPi", "MU4_DsPhiPi", "mu6_DsPhiPi", "mu4_DsPhiPi",
    "MU6_DsPhiPi_FS", "MU4_DsPhiPi_FS", "mu6_DsPhiPi_FS",
    "MU4_Jpsie5e3",
    "MU4_Jpsimumu", "MU6_Jpsimumu","MU4_Upsimumu","MU6_Upsimumu",
    "MU4_Bmumu","MU6_Bmumu","MU4_DiMu","MU6_DiMu",
    "mu4_Jpsimumu","mu4_Upsimumu","mu4_DiMu",
    "mu6_Jpsimumu","mu6_Upsimumu","mu6_DiMu",
    "mu4_Bmumu","mu6_Bmumu","mu4_DiMu_FS","mu6_DiMu_FS",
    "MU6_Jpsimumu_FS","MU6_Upsimumu_FS",
    "MU6_Bmumu_FS","MU6_DiMu_FS",
    "mu4_Jpsimumu_FS", "mu6_Jpsimumu_FS","mu4_Upsimumu_FS","mu6_Upsimumu_FS",
    "mu4_Bmumu_FS","mu6_Bmumu_FS",
    "MU6_BmumuX", "MU4_BmumuX", "mu6_BmumuX", "mu4_BmumuX" ,
    "2mu6_Bmumu","2mu4_Bmumux","2mu6_Bmumux","2mu6_DiMu",
    "2mu6_Jpsimumu",
    "2mu6_Upsimumu",
    "MU4_DsPhiPi_SiTrk", "MU4_Jpsie5e3_SiTrk", "MU4_DiMu_SiTrk",
    "2mu4_DiMu_SiTrk"
  };

  const unsigned BjetItems=16;
  std::string Bjet[BjetItems] = {
    "b18", "b23","b35","b42","b70",
    "2b23_3L1J23",
    "3b18_4L1J18",
    "mu4_j10_matched", "mu4_j18_matched", "mu4_j23_matched",
    "mu4_j35_matched", "mu4_j42_matched",
    "2b23_4L1J23", "4b18_4L1J18",
    "b18_noCut",
    "b18_IDScan"
  };

  const unsigned jetItems=34;
  std::string jet[jetItems] = {
    "j10", "j50", "j80", "j115", "j140", "j180",
    "j265", "j350",
    "3j25", "3j60", "3j180",
    "3j35",
    "4j45", "4j80", "4j95",
    "fj18", "fj35", "fj70",
    "2fj18",
    "4j125",
    "4j35_3j45_2j50_j60", "5j10_4j35_3j45_j60", "4j45_j60", "3j18_j42", "3j35c4",
    "fj120", "2fj35", "2fj70",
    "je120", "je220", "je280", "je340",
    "j35_L1tau40hv", "j35trackless"
  };

  const unsigned METItems=21;
  std::string MET[METItems] = {
    "te150", "te250", "te360",
    "xe15", "xe20", "xe25", "xe30",
    "xe40", "xe50", "xe70",
    "xe15_noMu", "xe20_noMu", "xe25_noMu", "xe30_noMu", "xe40_noMu",
    "xe50_noMu", "xe70_noMu",
    "xe80", "xe80_noMu", "te650",
    "xe30_unseeded"
  };

  const unsigned MinBiasItems=13;
  std::string MinBias[MinBiasItems] = {
    "mbSpTrk", "mbMbts_2", "mbID_calib", "mbID_MS_calib",
    "mbTrtTrk",
    "mbMbts_1_NoCut_Time",
    "mbMbts_1",
    "mbMbts_2_NoCut_Time",
    "mbMbts_2",
    "mbMbts_1_1_NoCut_Time",
    "mbMbts_1_1",
    "mbLucid",
    "mbZdc"
  };

  const unsigned CombinedItems=47;
  std::string Combined[CombinedItems] = {
    "tau16i_loose_e10_loose",
    "tau16i_loose_mu10",
    "tau16i_loose_EFxe30",
    "g25_loose_xe30", "tau16i_loose_4j23",
    "tau16i_loose_j70_WO",
    "tau16i_loose_j120",
    "mu4_j10", "mu10_j18", "j70_xe30",
    "2j42_xe30",
    "tau16i_loose_3j23",
    "tau20i_loose_2b23",
    "g20_loose_xe15", "2g10_loose_mu6",
    "tau12_loose_e10_loose", "tau16i_loose_e15i_medium", "tau16i_loose_EFxe40", "tau16i_medium_EFxe70",
    "tau29i_medium_EFxe40", "tau38_loose_xe40", "tau16i_loose_mu6",
    "e20_loose_xe15", "e20_loose_xe30",
    "tau16i_loose_xe30", "tau16i_loose_xe30_loose",
    "tau16_loose_xe20", "tau16i_loose_xe20", "tau16_loose_xe25", "tau16i_loose_xe25",
    "tau16i_loose_4j23_WO", "tau16i_loose_2j23", "tau16i_loose_j70",
    "tau16i_loose_2j70", "tau20i_loose_j120", "tau16i_loose_mu15i_loose", "tau16i_loose_EFxe30_j50",
    "mu15_xe15", "mu20_xe30","2mu6_e10_loose", "2mu6_g10_loose",
    "4j23_e15i_medium", "4j23_mu15", "j42_xe30_e15i_medium", "j42_xe30_mu15",
    "tau16i_loose_mu10i_loose",
    "e10_loose_mu6"
  };

  const unsigned calibItems=9;
  std:: string calib[calibItems] = {
    "trk9_IDCalib", "trk29_IDCalib",
    "g10_larcalib", "g20_larcalib", "g20i_larcalib",
    "j10_larcalib","j80_larcalib", "trk16i_IDCalib", "g3_larcalib"
  };

  for ( unsigned i = 0;  i < l->GetSize(); ++i ) {
    if ( TString(l->At(i)->GetName()).Contains(selector.c_str()) && (TString(l->At(i)->GetName()).Contains(level_selector.c_str()) || TString(l->At(i)->GetName()).Contains(TotalTime_selector.c_str()))) {

      if(slice_selector=="egamma"){
	for(unsigned j=0; j < egammaItems; j++ ) {
	  if ( TString(l->At(i)->GetName()).Contains(egamma[j].c_str())){
	    seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
	    cout << "added: " << l->At(i)->GetName() << endl;
	    break;
	  }
	} 
      }
      elseif(slice_selector=="muon"){
	for(unsigned j=0; j < muonItems; j++ ) {
	  //for(unsigned k=0; k < CombinedItems; k++ ) {
	    if ( TString(l->At(i)->GetName()).Contains(muon[j].c_str())){
	      seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
	      cout << "added: " << l->At(i)->GetName() << endl;
	      break;
	    }
	    //}
	} 
      }
      elseif(slice_selector=="tau"){
	for(unsigned j=0; j < tauItems; j++ ) {
	    if ( TString(l->At(i)->GetName()).Contains(tau[j].c_str())){
	      seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
	      cout << "added: " << l->At(i)->GetName() << endl;
	      break;
	    }
	} 
      }
      elseif(slice_selector=="Bphysics"){
	for(unsigned j=0; j < BphysicsItems; j++ ) {
	    if ( TString(l->At(i)->GetName()).Contains(Bphysics[j].c_str())){
	      seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
	      cout << "added: " << l->At(i)->GetName() << endl;
	      break;
	    }
	} 
      }
      elseif(slice_selector=="Bjet"){
	for(unsigned j=0; j < BjetItems; j++ ) {
	    if ( TString(l->At(i)->GetName()).Contains(Bjet[j].c_str())){
	      seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
	      cout << "added: " << l->At(i)->GetName() << endl;
	      break;
	    }
	} 
      }
      elseif(slice_selector=="jet"){
	for(unsigned j=0; j < jetItems; j++ ) {
	    if ( TString(l->At(i)->GetName()).Contains(jet[j].c_str())){
	      seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
	      cout << "added: " << l->At(i)->GetName() << endl;
	      break;
	    }
	} 
      }
      elseif(slice_selector=="MinBias"){
	for(unsigned j=0; j < MinBiasItems; j++ ) {
	    if ( TString(l->At(i)->GetName()).Contains(MinBias[j].c_str())){
	      seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
	      cout << "added: " << l->At(i)->GetName() << endl;
	      break;
	    }
	} 
      }
      elseif(slice_selector=="MET"){
	for(unsigned j=0; j < METItems; j++ ) {
	    if ( TString(l->At(i)->GetName()).Contains(MET[j].c_str())){
	      seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
	      cout << "added: " << l->At(i)->GetName() << endl;
	      break;
	    }
	} 
      }
      elseif(slice_selector=="calib"){
	for(unsigned j=0; j < calibItems; j++ ) {
	    if ( TString(l->At(i)->GetName()).Contains(calib[j].c_str())){
	      seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
	      cout << "added: " << l->At(i)->GetName() << endl;
	      break;
	    }
	} 
      }
      elseif(slice_selector=="Combined"){
	for(unsigned j=0; j < CombinedItems; j++ ) {
	    if ( TString(l->At(i)->GetName()).Contains(Combined[j].c_str())){
	      seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
	      cout << "added: " << l->At(i)->GetName() << endl;
	      break;
	    }
	} 
      }

      else{
	    seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
	    //cout << "added: " << l->At(i)->GetName() << endl;
      }
    }
  }

  if (selector == "TotalTime"){
    TCanvas *c= new TCanvas("c", "time", 700, 700);
    c->SetGridx(1);
    c->SetLogx();
    drawSequencesTimingPerEvent(c, seqHistogramsList, selector);
    c->SaveAs((file_prefix+selector+"PerEvent.gif").c_str());
    
    TCanvas *c1= new TCanvas("c1", "time", 700, 700);
    c1->SetGridx(1);
    //c1->SetLogx();
    
    //c1->SetLogx();
    drawSequencesTimingPerEventNoEvents(c1, seqHistogramsList, selector);
    c1->SaveAs((file_prefix+selector+"PerEventNoEvents.gif").c_str());

    TCanvas *d= new TCanvas("d", "time", 700, 700);
    d->SetGridx(1);
    d->SetLogx();
    drawSequencesTimingGlobal(d, seqHistogramsList, selector, 30);
    d->SaveAs((file_prefix+selector+"Overall.gif").c_str());
  }
  else{
    TCanvas *c= new TCanvas("c", "time", 700, 700);
    c->SetGridx(1);
    c->SetLogx();
    drawSequencesTimingPerEventFromTH2(c, seqHistogramsList, file_prefix, selector);
    c->SaveAs((file_prefix+selector+"PerEvent.gif").c_str());

    TCanvas *d= new TCanvas("d", "time", 700, 700);
    d->SetGridx(1);
    d->SetLogx();
    drawSequencesTimingGlobalFromTH2(d, seqHistogramsList, file_prefix, selector, 30);
    d->SaveAs((file_prefix+selector+"Overall.gif").c_str());

    TCanvas *c1= new TCanvas("c1", "time", 700, 700);
    c1->SetGridx(1);
    drawSequencesTimingPerEventNoEventsFromTH2(c1, seqHistogramsList, file_prefix, selector);
    c1->SaveAs((file_prefix+selector+"PerEventNoEvents.gif").c_str());



  }



  //get totalTiming histos
  TCanvas *total= new TCanvas("total", "Time summary",1);
  total->Divide(2,2);
  
  const unsigned N = (seqHistogramsList->GetSize());
  
  for ( unsigned i = 0; i < N; ++i ) {
    // 

    TH1* h = (TH1*) seqHistogramsList->At(i);
    TString label = TString(h->GetName());

    if (label=="TrigSteer_L2_TotalTime" 
	|| label=="TrigSteer_L2_TotalTimeAccepted" 
	|| label=="TrigSteer_L2_TotalTimeRejected"
	|| label=="TrigSteer_L2_ChainsRerunTime"
	|| label=="TrigSteer_L2_ChainsTime"
	|| label=="TrigSteer_EF_TotalTime" 
	|| label=="TrigSteer_EF_TotalTimeAccepted" 
	|| label=="TrigSteer_EF_TotalTimeRejected"
	|| label=="TrigSteer_EF_ChainsRerunTime"
	|| label=="TrigSteer_EF_ChainsTime"
	){
      //cout << "label: " << label << endl;   
      total->cd();
      total->SetLogy();
      //igb
      h->GetXaxis()->SetLabelSize(0.02);
      h->GetXaxis()->LabelsOption("v>");
      h->Draw();
      total->SaveAs(label+"-TimeSummary.gif");
    }
  }
}

void sequencesTiming( const int level=2 ) {

  TFile _file0("expert-monitoring.root");

  if (level==2){
    
    //std::string l2path = "Histogramming/Top-LVL2-L2-Segment-1-Gatherer/EXPERT/TIMERS";  
    
    std::string l2path = "TIMERS";

    draw(l2path, "L2", "Sequences");
    draw(l2path, "L2", "Chains");
    draw(l2path, "L2", "TotalTime");
  }
  elseif (level==3){

    //std::string efpath = "Histogramming/Top-EF-EBEF-Segment/EXPERT/TIMERS";
    std::string efpath = "TIMERS";
    
    draw(efpath, "EF", "Sequences");
    draw(efpath, "EF", "Chains", "");
    draw(efpath, "EF", "TotalTime");
  }
  else{
    cout << "Type either 2=LVL2 or 3=EF. Other values are not allowed! " << endl;
  }
    
    //exit();
  gROOT->ProcessLine(".q");
}
