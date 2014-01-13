/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// ROOT
#include "TCanvas.h"
#include "TF1.h"
#include "TH1.h"
#include "TList.h"
#include "TPaveText.h"

// Local
#include "TrigCostRate/RateChain.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostRate/MakePuRates.h"

REGISTER_ANP_OBJECT(AlgEvent,MakePuRates)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
void Anp::PuDataFit::Print(const std::string &opt, unsigned name_width) const
{
  string name_out = name;
  
  if(name_width > 0) {
    name_out = Anp::PadStrBack(name, name_width);
  }

  if(opt == "tex") {
    cout << name_out
	 << " & "   << Anp::Round2Pair(par1, -0.0, 2).first 
	 << " & "   << Anp::Round2Pair(chi2,  1.0, 5).first
	 << " \\\\"
	 << endl;
  }
  else {
    cout << name_out
	 << ": p0 = "   << std::scientific << std::setprecision(3) << par0 << std::fixed
	 << ": p1 = "   << Anp::Round2Pair(par1, -1, 3).first
	 << ": chi2 = "   << std::scientific << std::setprecision(3) << chi2 << std::fixed
	 << endl;
  }
}

//-----------------------------------------------------------------------------
Anp::MakePuRates::MakePuRates()
  :fDir(0),
   fDirName("purate"),
   fFitFunc("effn"),
   fCanvasType("no"),
   fDebug(false),
   fIsConfig(false),
   fMinPass(40),
   fMinRate(0.001),
   fMaxEff(0.30),
   fMaxVal(10.9),
   fMidVal(5.0),
   fPrintMin(0.0),
   fFitKey(216),
   fFitNpt(3)
{
}

//-----------------------------------------------------------------------------
Anp::MakePuRates::~MakePuRates()
{
}

//-----------------------------------------------------------------------------
void Anp::MakePuRates::Config(const Registry &reg)
{
  fReg = reg;

  reg.Get("MakePuRates", "CanvasType", fCanvasType);
  reg.Get("MakePuRates", "Debug",      fDebug);

  reg.Get("MakePuRates::DirName",  fDirName);
  reg.Get("MakePuRates::FitFunc",  fFitFunc);
  reg.Get("MakePuRates::MinPass",  fMinPass);
  reg.Get("MakePuRates::MinRate",  fMinRate);
  reg.Get("MakePuRates::MaxEff",   fMaxEff);
  reg.Get("MakePuRates::MaxVal",   fMaxVal);
  reg.Get("MakePuRates::MidVal",   fMidVal);
  reg.Get("MakePuRates::PrintMin", fPrintMin);
  reg.Get("MakePuRates::FitKey",   fFitKey);
  reg.Get("MakePuRates::FitNpt",   fFitNpt);

  //
  // Initiliaze tree branches
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");
  
  //
  // Register with trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "MakePuRates::ConfAlg", fConf, this);

  //
  // Register self for html writing callback
  //
  Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));
}

//-----------------------------------------------------------------------------
void Anp::MakePuRates::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback or parent signal
  //
  if(signal != "TRIG_CONFIG") return;

  if(fConf.valid()) { 
    fConfig = fConf->GetConfig(); 
    fIsConfig = true;
  }
  else {
    log() << "Signal - invalid configuration algorithm" << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::MakePuRates::Save(TDirectory *dir)
{
  fDir = Anp::GetDir(dir, fDirName);

  Registry reg;
  reg.Set("PlotEvent::DirName", "event");
  reg.Set("PlotEvent::DirHist", "pu_rate");
  
  fPlot = Handle<PlotEvent>(new PlotEvent());
  fPlot -> Config(reg);
  fPlot -> Init();
  fPlot -> Save(fDir);
}

//-----------------------------------------------------------------------------
bool Anp::MakePuRates::Init()
{
  return true;
}

//-----------------------------------------------------------------------------
void Anp::MakePuRates::Exec()
{
  //
  // Read event data
  //
  if(fEntry.valid() && 
     fIsConfig &&
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {
    Run(fEntry.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::MakePuRates::Done()
{
  //
  // Finalize rate numbers
  //
  if(!fIsConfig) return;

  map<string, map<unsigned, PuData> > chain_map;

  //
  // Iterate over rates objects and store rates by chain name and number of pileup events
  //
  for(RateMap::iterator it = fRates.begin(); it != fRates.end(); ++it) {
    
    const unsigned npileup = it->first;
    Handle<MakeRates> rates = it->second;
    
    rates -> Done();

    std::vector<Handle<RateChain> > &chains = rates->GetRateChains();

    for(unsigned i = 0; i < chains.size(); ++i) {
      Handle<RateChain> chain = chains.at(i);

      const std::pair<double, double> pfrac = chain->GetPassFrac();
      const std::pair<double, double> prate = chain->GetPassRate();
      
      PuData &data = chain_map[chain->GetName()][npileup];

      data.npileup  = npileup;
      data.rate     = prate.first;
      data.rate_err = prate.second;
      data.eff      = pfrac.first;
      data.eff_err  = pfrac.second;
    }
  }

  TDirectory *deff  = Anp::GetDir(fDir, "eff");
  TDirectory *drate = Anp::GetDir(fDir, "rate");

  unsigned name_width = 0;
  vector<Anp::PuDataFit> fit_vec;

  for(map<string, map<unsigned, PuData> >::iterator it = chain_map.begin(); it != chain_map.end(); ++it) {
    
    const std::string &name = it->first;
    const map<unsigned, PuData> &pu_map = it->second;
    
    const string name_eff  = name + "_eff";
    const string name_rate = name + "_rate";

    TH1 *heff = new TH1D(name_eff.c_str(), name_eff.c_str(), fRates.size(), 1, fRates.size()+1);    
    heff -> SetDirectory(deff);
    heff -> SetStats(false);
    heff -> SetXTitle("Number of MinBias Interactions (I)");
    heff -> SetYTitle("Acceptance Rate (a(I))");

    TH1 *hrate = new TH1D(name_rate.c_str(), name_rate.c_str(), fRates.size(), 1, fRates.size()+1);
    hrate -> SetDirectory(drate);
    hrate -> SetStats(false);
    
    fHistEff[name] = heff;

    for(map<unsigned, PuData>::const_iterator pit = pu_map.begin(); pit != pu_map.end(); ++pit) {
      const PuData &data = pit->second;
      
      if(data.eff > 0.0) {
	heff -> SetBinContent(pit->first, data.eff);
	heff -> SetBinError(pit->first, data.eff_err);
	
      }

      if(data.rate > 0.0) {
	hrate -> SetBinContent(pit->first, data.rate);
	hrate -> SetBinError(pit->first, data.rate_err);
      }
    }

    const PuDataFit fit = FitEff(heff, name, fFitFunc);
    
    if(fit.ndof > 0) {
      name_width = std::max<unsigned>(name_width, name.size());
      fit_vec.push_back(fit);
    }
  }

  if(fPlot.valid()) fPlot->Done();

  if(!fit_vec.empty()) {
    log() << "Done - printing " << fit_vec.size() << " fit results(s) " << endl;

    std::sort(fit_vec.begin(), fit_vec.end());
    for(unsigned int i = 0; i < fit_vec.size(); ++i) {
      fit_vec[i].Print("tex", name_width);
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::MakePuRates::Write(HtmlNode &, HtmlMenu &hmenu)
{
  //
  // Make plots of efficiency versus number of pileup events
  //
  if(!fIsConfig || (fCanvasType != "png" && fCanvasType != "eps")) {
    log() << "Write - invalid input parameter(s)" << endl;
    return;
  }

  //
  // Set nicer ROOT canvas defaults
  //
  Anp::SetROOT_defaults();

  TCanvas *canvas = new TCanvas("PuRateCanvas", "", 600, 800);
  canvas -> SetRightMargin(0.05);
  canvas -> SetLeftMargin(0.15);
  canvas -> Divide(1,2);
  canvas -> SetBatch(true);


  for(HistMap::iterator hit = fHistEff.begin(); hit != fHistEff.end(); ++hit) {
    const string &name = hit->first;
    TH1 *h = hit->second;

    // draw information on the first pad of the canvas
    canvas->cd(2);
    TPaveText infobox(0.1,0.1,0.8,0.8,"br");


    string ltext      = "Efficiency versus number of minbias collisions";
    string ltext_log  = "Log(efficiency) versus number of minbias collisions";


    // Iterate over the histogram's associated list of functions and
    // see if there is a fit associated with this histogram. If so,
    // retrieve it
    TList* h_functions = dynamic_cast<TList *>(h->GetListOfFunctions());
    TListIter iter_functions(h_functions);
    TObject* aFunction = NULL;
    TF1* h_fit = NULL;
    while( (aFunction = iter_functions.Next()) ) {
      if (dynamic_cast<TF1 *>(aFunction))
	h_fit = dynamic_cast<TF1 *> (aFunction);
    }

    std::stringstream fitstr;
    std::stringstream funcstr;

    if (h_fit) {
      float chi2 = h_fit -> GetChisquare();
      float ndof = h_fit -> GetNDF();
      if (ndof == 0) {
	chi2 = -999.0;
	ndof = -1;
      }

      fitstr << "#chi^{2}/ndof = " 
	     << setw(5) << setfill('0') 
	     << chi2 << "/" << ndof
	     << " = " << chi2/ndof;

      if (std::string(h_fit->GetName()) == "effn") {
	funcstr << "a(I) = " 
		<< setw(5) << setfill('0')
		<< h_fit->GetParameter(0) << "*I^{"
		<< h_fit->GetParameter(1) << "}";

	if (h_fit->GetParameter(1) > 0) {
	  ltext     += " (Trending Upward)";
	  ltext_log += " (Trending Upward)";
	} else {
	  ltext     += " (Trending Downward)";
	  ltext_log += " (Trending Downward)";
	}	  
      } else {
	funcstr << "a(I) = " << h_fit->GetTitle();
       }
    } else {
      fitstr << "No fit information available." << endl;
    }
    
    infobox.AddText( fitstr.str().c_str() );
    infobox.AddText( funcstr.str().c_str() );

    infobox.Draw();

    Handle<HtmlNode> chainNode = hmenu.FindChnNode(name);
    if(!chainNode.valid()) continue;
    
    chainNode -> GetCont() << "<pre>";

    canvas->cd(1);

    const string canvas_name = "pu_rate_eff_" + string(h->GetName()) + "." + fCanvasType;
    PrintHist(chainNode.ref(), h, canvas, canvas_name, ltext);
    const string canvas_log_name = "pu_rate_eff_LOG_" + string(h->GetName()) + "." + fCanvasType;
    PrintHist(chainNode.ref(), h, canvas, canvas_log_name, ltext_log, "logy");

    chainNode -> GetCont() << "</pre>" << endl;

    if(fDebug) log() << "Write - chain: " << name << endl;
  }

  delete canvas;
}

//-----------------------------------------------------------------------------
void Anp::MakePuRates::Run(const TrigMonEvent &event)
{
  //
  // Loop over timing sequence and algorithm timing measurements
  //

  //
  // Read pileup event counts
  //
  float nminbias = 0.0, nsignal = 0.0;

  if(!event.getVar(216, nsignal) || !event.getVar(217, nminbias)) {
    if(fDebug) log() << "Run - missing input variable(s)" << endl;
    return;
  }
  
  const unsigned npileup = static_cast<unsigned>(nsignal+nminbias);
  
  if(npileup < 1) { 
    if(fDebug) log() << "Run - no pileup information" << endl;
    return;    
  }
  else {
    if(fDebug) log() << "Run - # of pileup: " << npileup << endl;
  }

  //
  // Make histograms for pileup variables
  //
  if(fPlot.valid()) fPlot->Run(event);

  RateMap::iterator irate = fRates.find(npileup);
  if(irate == fRates.end()) {
    stringstream rstr;
    rstr << "makeRates_pu_" << setw(2) << setfill('0') << npileup;

    Handle<MakeRates> rate(new MakeRates());
    rate -> SetName(rstr.str());
    rate -> Config(fReg);
    rate -> Init();
    rate -> Set(fConfig);
    
    irate = fRates.insert(RateMap::value_type(npileup, rate)).first;
  }
  
  if(fDebug) log() << "Run - found alg for pileup: " << npileup << endl;

  //
  // Compute rates for events with fixed number of pileup events
  //
  irate->second->Run(event);

  if(fDebug) log() << "Run - finished with this event" << endl;
}

//-----------------------------------------------------------------------------
Anp::PuDataFit Anp::MakePuRates::FitEff(TH1 *h,
					const std::string &name,
					const std::string &fname)
{  
  //
  // Fit efficiency histogram with polynomial
  //
  Anp::PuDataFit fit;
  
  if(!h) return fit;

  std::vector<double> xmin, xmax, xeff, evec;

  for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin) {
    const double eff = h -> GetBinContent(ibin);
    const double val = h -> GetBinCenter(ibin);
    const double xle = h -> GetXaxis() -> GetBinLowEdge(ibin);
    const double xue = h -> GetXaxis() -> GetBinUpEdge(ibin);

    if(!(eff > 0.0) || !(val < fMaxVal)) continue;

    xmin.push_back(xle);
    xmax.push_back(xue);
    
    if(eff < fMaxEff) {
      xeff.push_back(xue);
    }
  }

  // require a minimum number of bins
  if(xmin.size() < fFitNpt || xeff.size() < fFitNpt) return fit;
  
  double ledge = *std::min_element(xmin.begin(), xmin.end());
  double redge = *std::max_element(xmax.begin(), xmax.end());

  if(!xeff.empty()) {
    redge = std::min<double>(redge, *std::max_element(xeff.begin(), xeff.end()));
  }

  //
  // Fit efficiency function
  //
  TF1 *f1 = 0;

  if(fname == "pol1") {
    f1 = new TF1(fname.c_str(), fname.c_str(), ledge, redge);
    f1 -> SetParameter(0, 1.0);
    f1 -> SetParameter(1, 0.0);
    f1 -> SetLineColor(2);    
  }
  else if(fname == "pol2") {
    f1 = new TF1(fname.c_str(), fname.c_str(), ledge, redge);
    f1 -> SetParameter(0, 1.0);
    f1 -> SetParameter(1, 0.0);
    f1 -> SetParameter(2, 0.0);
    f1 -> SetLineColor(3);
  }
  else if(fname == "pol3") {
    f1 = new TF1(fname.c_str(), fname.c_str(), ledge, redge);
    f1 -> SetParameter(0, 1.0);
    f1 -> SetParameter(1, 0.0);
    f1 -> SetParameter(2, 0.0);
    f1 -> SetParameter(3, 0.0);
    f1 -> SetLineColor(5);
  }
  else if(fname == "effn") {
    f1 = new TF1(fname.c_str(), TrigPuRateEffFunc, ledge, redge, 2);
    f1 -> SetParameter(0, 0.1);
    f1 -> SetParameter(1, 1.0);
    f1 -> SetLineColor(4);    
  }
  else {
    log() << "FitEff - bad function name: " << fname << endl;
    return fit;
  }

  h -> Fit(f1, "RQ+", "", ledge, redge);
  
  if(fPlot.valid()) {

    fPlot -> Plot("par0", f1->GetParameter(0));
    fPlot -> Plot("par1", f1->GetParameter(1));
    fPlot -> Plot("chi2", f1->GetChisquare());
    fPlot -> Plot("ndof", f1->GetNDF());
    
    if(f1->GetNDF() > 0.0) {
      fPlot -> Plot("chi2_over_ndof", f1->GetChisquare()/f1->GetNDF());
    }
    
    if(fname == "pol2") {
      fPlot -> Plot("par2", f1->GetParameter(2));
    }
    else if(fname == "pol3") {
      fPlot -> Plot("par3", f1->GetParameter(3));
    }
  }
 
  fit.name = name;
  fit.key  = f1 -> GetParameter(1);
  fit.par0 = f1 -> GetParameter(0);
  fit.par1 = f1 -> GetParameter(1);
  fit.chi2 = f1 -> GetChisquare();
  fit.ndof = f1 -> GetNDF();

  return fit;
}

//-----------------------------------------------------------------------------
double TrigPuRateEffFunc(double *x, double *par)
{
  // Fit function f = e * n^{k}
  // e = par[0] = pass fraction
  // k = par[1] = power law
  //
  assert(x && par && "null array(s)");
   
  return par[0]*std::pow(x[0], par[1]);
}
