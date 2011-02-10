
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
					   TList* list, const TString& file_prefix, const TString& sel, 
					   int bins=30, const std::string& slice_selector = "all") {
  
  TH2F* hprof = (TH2F*) list->FindObject("TrigSteer_"+file_prefix+"_"+sel);
  
  unsigned Nbinsx=hprof->GetNbinsX();
  unsigned Nbinsy=hprof->GetNbinsY();

  TH1 *total = new TH1F("total",";;time[ms]", Nbinsx, 0, Nbinsx);  
  total->SetFillColor(kRed);

  TH1 *htemp = new TH1F("htemp",";;time[ms]", Nbinsy, 0, Nbinsy);
  htemp->SetDirectory(0);

  float IntegratedTime=0.;
  for ( unsigned i = 0; i < Nbinsx; ++i ) {
    
    unsigned binx = i+1;
    
    TString label = TString(hprof->GetXaxis()->GetBinLabel(binx));
   
    if (select_slice(label,  slice_selector)){

      label.Replace(0, 22, "");      
      
      total->GetXaxis()->SetBinLabel(binx, label);
      
      htemp->Reset();
      htemp=hprof->ProjectionY("",binx,binx);
      double entry=htemp->GetMean()*htemp->GetEntries();
      IntegratedTime=IntegratedTime+entry;
      total->SetBinContent(binx, entry );   
    }
  }

  cout << "Integrated time for " << slice_selector << " is " << IntegratedTime << " [ms]" << endl;

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

  TH1 *mean = new TH1F("mean",";;time [ms]", N, 0, N);
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
  TH1 *mean = new TH1F("mean",";;time [ms]", Nbinsx, 0, Nbinsx);
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
  TH1 *mean = new TH1F("mean","", N, 0, N);
  TH1 *rms  = new TH1F("rms","RMS", N, 0, N);
  //  TH1D mean_plus_rms("mean_plus_rms","RMS+", N, 0, N);
  //  TH1D mean_minus_rms("mean_minus_rms","RMS-", N, 0, N);
  
  TH1 *min  = new TH1F ("min","Min", N, 0, N);
  TH1 *max  = new TH1F("max","Max", N, 0, N);
  TH1 *overflows = new TH1I("overflow","Overflows", N, 0, N);

  TH1 *event = new TH1F("event",";;Events", N, 0, N);

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
  TH1 *mean = new TH1F("mean","", Nbinsx, 0, Nbinsx);
  TH1 *rms  = new TH1F("rms","RMS", Nbinsx, 0, Nbinsx);
  //  TH1D mean_plus_rms("mean_plus_rms","RMS+", N, 0, N);
  //  TH1D mean_minus_rms("mean_minus_rms","RMS-", N, 0, N);
  
  TH1 *min  = new TH1F ("min","Min", Nbinsx, 0, Nbinsx);
  TH1 *max  = new TH1F("max","Max", Nbinsx, 0, Nbinsx);
  TH1 *overflows = new TH1I("overflow","Overflows", Nbinsx, 0, Nbinsx);

  TH1 *event = new TH1F("event",";;Events", Nbinsx, 0, Nbinsx);

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

bool select_slice ( const TString & label, const std::string& slice_selector = "all") {

  //InitialBeam_v1 menu
  const unsigned egammaItems=13;
  std::string egamma[egammaItems] =        {
    "e5_NoCut_cells",
    "e5_NoCut", "e5_NoCut_SiTrk",
    "e5_NoCut_IdScan", "e5_NoCut_TRT",
    "e5_NoCut_FwdBackTrk",
    "e10_loose",
    "e10_loose_passL2", "e10_loose_passEF",
    "e10_medium", "g5_nocut",
    "g10_loose",
    "e20_loose_NoIDTrkCut"
  };
  
  const unsigned muonItems=34;
  std::string muon[muonItems] = {
    "mu4", "mu4_L2MSonly_passL2", "mu4_MSonly", "mu4_passL2MS",
    "mu4_SiTrk", "mu4_tile", "mu4_tile_SiTrk", "mu4_trod",
    "mu4_trod_SiTrk", "mu4_MG",
    "mu6", "mu6_passL2MS", "mu6_MSonly", "mu6_L2MSonly_passL2",
    "mu6_SiTrk","mu6_MG",
    "mu10","mu10_passL2MS", "mu10_MSonly", "mu10_L2MSonly_passL2",
    "mu10_SiTrk", "mu10i_loose", "mu10i_loose_MSonly",
    "mu10i_loose_MSonly_SiTrk", "mu10_MG", "mu13",
    "mu20", "mu20_MSonly",
    "2mu4", "2mu4_MSonly", "2mu6", "2mu10", "mu4_mu6",
    "mu10_NoIDTrkCut"
  };

  const unsigned tauItems=9;
  std::string tau[tauItems] = {
    "tauNoCut_cosmic", "tauNoCut", "tauNoCut_cells", "tauNoCut_SiTrk",
    "trk9_loose", "tau16_loose", "tau12_loose",
    "tauNoCut_NoIDTrkCut", "tau16i_NoIDTrkCut"
  };

  const unsigned BphysicsItems=20;
  std::string Bphysics[BphysicsItems] = {
    "MU6_Jpsie5e3_FS",
    "MU4_DiMu",
    "MU4_DiMu_FS",
    "mu4_DiMu","mu4_DiMu_FS",
    "MU6_DiMu","MU6_DiMu_FS",
    "mu6_DiMu","mu6_DiMu_FS",
    "MU4_DiMu_SiTrk_FS","MU4_DiMu_MG_FS",
    "2mu4_DiMu_SiTrk",
    "2mu6_DiMu",
    "mu4mu6_DiMu",
    "2mu4_DiMu",
    "MU6_Jpsie5e3_SiTrk_FS",
    "MU4_Trk_Jpsi","MU4_Trk_Jpsi_FS",
    "MU6_Jpsie5e3",
    "mu6_Jpsie5e3"
  };

  const unsigned BjetItems=0;
  std::string Bjet[BjetItems] = {
  };

  const unsigned jetItems=15;
  std::string jet[jetItems] = {
    "j10v3", "j20v2", "j40", "j80v2", "j140v2", "j200v2",
    "3j20", "2j10", "2j20", "2j40", 
    "2j10_deta3_5", "2j10_deta5", "2j20_deta3_5", "2j40_deta3_5",
    "fj18"
  };

  const unsigned METItems=10;
  std::string MET[METItems] = {
    "xe20",
    "xe20_allCells",
    "xe20_FEB",
    "te150",
    "xe30_allL1",
    "xe30_allL1_allCells",
    "xe30_allL1_FEB",
    "xe30_unbiased",
    "xe30_unbiased_allCells",
    "xe30_unbiased_FEB"
  };

  const unsigned MinBiasItems=39;
  std::string MinBias[MinBiasItems] = {
    "mbSpTrk", 
    "mbTrtTrk", 
    "mbMbts_1_NoCut_Time",
    "mbMbts_2_NoCut_Time",
    "mbMbts_1_1_NoCut_Time",
    "mbMbts_1",
    "mbMbts_2",
    "mbMbts_1_1",
    "mb_RM_calib",
    "mb_RM_calib_cosmic",
    "mb_BM_calib",
    "mb_LD_calib",
    "mb_MS_calib",
    "mb_ZC_calib",
    "mbBcm", 
    "mbRndm",
    "mbLucid",
    "mbZdc", 
    "mb_MU",
    "mbRmMbts_1",
    "mbRmMbts_2",
    "mb_BX0_calib",
    "mb_BX1_calib",
    "mbMbts_1_BX0",
    "mbMbts_1_BX1",
    "mbMbts_2_BX0",
    "mbMbts_2_BX1",
    "mbTrtTrk_BX0",
    "mbTrtTrk_BX1",
    "mbSpTrk_BX0",
    "mbSpTrk_BX1",
    "mbMbts_1_1_BX0",
    "mbMbts_1_1_BX1",
    "mbMbts_1_NoCut_Time_BX0",
    "mbMbts_1_NoCut_Time_BX1",
    "mbMbts_2_NoCut_Time_BX0",
    "mbMbts_2_NoCut_Time_BX1",
    "mbMbts_1_1_NoCut_Time_BX0",
    "mbMbts_1_1_NoCut_Time_BX1"
  };

  const unsigned CombinedItems=0;
  std::string Combined[CombinedItems] = {
  };

  const unsigned calibItems=10;
  std:: string calib[calibItems] = {
    "trk9_Central_Tau_IDCalib", "trk9_Fwd_Tau_IDCalib",
    "trk9_Central_Tau_SiTrack_IDCalib", "trk9_Fwd_Tau_SiTrack_IDCalib",
    "trk2_IDSCAN_IDCalib", "trk2_SiTrk_IDCalib",
    "allID_IDCalib",
    "g10_larcalib", "j10_larcalibV2", "j80_larcalibV2"
  };

  const unsigned beamSpotItems=6;
  std:: string beamSpot[beamSpotItems] = {
        "beamspot_vertex_FSTracks",
        "beamspot_vertex_activeTE_peb",
        "beamspot_vertex_allTE_peb",
        "beamspot_vertex_FSTracks_allL1",
        "beamspot_vertex_allTE_peb_allL1",
        "beamspot_vertex_activeTE_peb_allL1"
  };

  if(slice_selector=="egamma"){
    for(unsigned j=0; j < egammaItems; j++ ) {
      if ( label.Contains(egamma[j].c_str())){
	cout << "added: " << label << endl;
	return true;
      }
    } 
  }
  elseif(slice_selector=="muon"){
    for(unsigned j=0; j < muonItems; j++ ) {
      //for(unsigned k=0; k < CombinedItems; k++ ) {
      if ( label.Contains(muon[j].c_str())){
	cout << "added: " << label << endl;
	return true;
      }
    } 
  }
  elseif(slice_selector=="tau"){
    for(unsigned j=0; j < tauItems; j++ ) {
      if ( label.Contains(tau[j].c_str())){
	cout << "added: " << label << endl;
	return true;
      }
    } 
  }
  elseif(slice_selector=="Bphysics"){
    for(unsigned j=0; j < BphysicsItems; j++ ) {
      if ( label.Contains(Bphysics[j].c_str())){
	cout << "added: " << label << endl;
	return true;
      }
    } 
  }
  elseif(slice_selector=="Bjet"){
    for(unsigned j=0; j < BjetItems; j++ ) {
      if ( label.Contains(Bjet[j].c_str())){
	cout << "added: " << label << endl;
	return true;
      }
    } 
  }
  elseif(slice_selector=="jet"){
    for(unsigned j=0; j < jetItems; j++ ) {
      if ( label.Contains(jet[j].c_str())){
	cout << "added: " << label << endl;
	return true;
      }
    } 
  }
  elseif(slice_selector=="MinBias"){
    for(unsigned j=0; j < MinBiasItems; j++ ) {
      if ( label.Contains(MinBias[j].c_str())){
	cout << "added: " << label << endl;
	return true;
      }
    } 
  }
  elseif(slice_selector=="MET"){
    for(unsigned j=0; j < METItems; j++ ) {
      if ( label.Contains(MET[j].c_str())){
	cout << "added: " << label << endl;
	return true;
      }
    } 
  }
  elseif(slice_selector=="calib"){
    for(unsigned j=0; j < calibItems; j++ ) {
      if ( label.Contains(calib[j].c_str())){
	cout << "added: " << label << endl;
	return true;
      }
    } 
  }
  elseif(slice_selector=="Combined"){
    for(unsigned j=0; j < CombinedItems; j++ ) {
      if ( label.Contains(Combined[j].c_str())){
	cout << "added: " << label << endl;
	return true;
      }
    } 
  }
  elseif(slice_selector=="BeamSpot"){
    for(unsigned j=0; j < beamSpotItems; j++ ) {
      if ( label.Contains(beamSpot[j].c_str())){
	cout << "added: " << label << endl;
	return true;
      }
    } 
  }
  else{
    return true;
    //cout << "added: " << l->At(i)->GetName() << endl;
  }

return false;
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

  for ( unsigned i = 0;  i < l->GetSize(); ++i ) {
    if ( TString(l->At(i)->GetName()).Contains(selector.c_str()) && (TString(l->At(i)->GetName()).Contains(level_selector.c_str()) || TString(l->At(i)->GetName()).Contains(TotalTime_selector.c_str()))) {
      
      seqHistogramsList->AddLast( dir->Get( l->At(i)->GetName() ) );
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
    drawSequencesTimingGlobalFromTH2(d, seqHistogramsList, file_prefix, selector, 30, slice_selector);
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
    
    std::string l2path = "Histogramming/Top-LVL2-L2-Segment-1-Gatherer/EXPERT/TIMERS";  
    
    //std::string l2path = "TIMERS";

    draw(l2path, "L2", "Sequences");
    draw(l2path, "L2", "Chains", "BeamSpot");
    draw(l2path, "L2", "TotalTime");
  }
  elseif (level==3){

    std::string efpath = "Histogramming/Top-EF-EBEF-Segment/EXPERT/TIMERS";
    //std::string efpath = "TIMERS";
    
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
