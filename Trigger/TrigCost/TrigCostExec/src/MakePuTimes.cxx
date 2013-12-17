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

// Local
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostExec/MakePuTimes.h"

REGISTER_ANP_OBJECT(AlgEvent,MakePuTimes)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
void Anp::PuTimeHist::Print(const std::string &opt, unsigned name_width) const
{
  string out_name = alg_name;
  
  if(name_width > 0) {
    out_name = Anp::PadStrBack(out_name, name_width);
  }

  if(opt == "tex") {
    cout << out_name
	 << " & "   << Anp::Round2Pair(par1, -0.0, 2).first 
	 << " & "   << Anp::Round2Pair(chi2,  1.0, 5).first
	 << " \\\\"
	 << endl;
  }
  else {
    cout << out_name
	 << ": p0 = "   << std::scientific << std::setprecision(3) << par0 << std::fixed
	 << ": p1 = "   << Anp::Round2Pair(par1, -1, 3).first
	 << ": chi2 = "   << std::scientific << std::setprecision(3) << chi2 << std::fixed
	 << endl;
  }
}

//-----------------------------------------------------------------------------
Anp::MakePuTimes::MakePuTimes()
  :fDir(0),
   fDirName("putime"),
   fFitFunc("effn"),
   fCanvasType("no"),
   fDebug(false),
   fIsConfig(false),
   fMinPass(40),
   fMaxEff(0.30),
   fMaxVal(10.9),
   fMidVal(5.0),
   fPrintMin(0.0),
   fFitKey(216),
   fFitNpt(3)
{
}

//-----------------------------------------------------------------------------
Anp::MakePuTimes::~MakePuTimes()
{
}

//-----------------------------------------------------------------------------
void Anp::MakePuTimes::Config(const Registry &reg)
{
  fReg = reg;

  reg.Get("MakePuTimes", "CanvasType",  fCanvasType);
  reg.Get("MakePuTimes", "Debug",       fDebug);

  reg.Get("MakePuTimes::VarNames", fVarNames);
  reg.Get("MakePuTimes::DirName",  fDirName);
  reg.Get("MakePuTimes::FitFunc",  fFitFunc);
  reg.Get("MakePuTimes::MinPass",  fMinPass);
  reg.Get("MakePuTimes::MaxEff",   fMaxEff);
  reg.Get("MakePuTimes::MaxVal",   fMaxVal);
  reg.Get("MakePuTimes::MidVal",   fMidVal);
  reg.Get("MakePuTimes::PrintMin", fPrintMin);
  reg.Get("MakePuTimes::FitKey",   fFitKey);
  reg.Get("MakePuTimes::FitNpt",   fFitNpt);

  //
  // Register with trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "MakePuTimes::ConfAlg", fConf, this);

  //
  // Initiliaze tree branches
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent> ("data");

  //
  // Register self for html writing callback
  //
  Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));
}

//-----------------------------------------------------------------------------
void Anp::MakePuTimes::Signal(const Registry &, const string &signal)
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
void Anp::MakePuTimes::Save(TDirectory *dir)
{
  fDir = Anp::GetDir(dir, fDirName);

  Registry reg;
  reg.Set("PlotEvent::DirName", "event");
  reg.Set("PlotEvent::DirHist", "pu_time");
  
  fPlot = Handle<PlotEvent>(new PlotEvent());
  fPlot -> Config(reg);
  fPlot -> Init();
  fPlot -> Save(fDir);
}

//-----------------------------------------------------------------------------
bool Anp::MakePuTimes::Init()
{
  return true;
}

//-----------------------------------------------------------------------------
void Anp::MakePuTimes::Exec()
{
  //
  // Read data from either TrigData (full) or TrigDecis (slim) files
  //
  if(fEntry.valid() && 
     fIsConfig &&
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {
    Run(fEntry.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::MakePuTimes::Done()
{
  //
  // Finalize: make histograms 
  //  
  if(!fIsConfig) return;

  if(fDebug) {
    log() << "Done - begin..." << endl;
  }

  //
  // Iterate over algorithm objects make map of number of collisions and total time
  //
  map<string, string> var_names;
  map<string, map<unsigned, PuTime> > chain_map;

  for(TimeMap::iterator it = fTimes.begin(); it != fTimes.end(); ++it) {
    
    const unsigned npileup = it->first;
    Handle<StudyTime> study = it->second;
    
    study -> Done();

    const std::map<uint32_t, Anp::Handle<Anp::TrigTimeAlg> > algs = study -> GetAlgName();

    for(map<uint32_t, Anp::Handle<Anp::TrigTimeAlg> >::const_iterator ialg = algs.begin(); ialg != algs.end(); ++ialg) {

      Handle<TrigTimeAlg> alg = ialg->second;
      if(!alg.valid()) continue;
      
      if(alg->GetVar("ncalls") > 1) {

	PuTime &data = chain_map[alg->GetName()][npileup];
	
	data.npileup  = npileup;
	data.alg_id   = alg -> GetNameId();	  
	data.vars     = alg -> GetVarMap();

	for(std::map<std::string, double>::const_iterator vit = data.vars.begin(); vit != data.vars.end(); ++vit) {
	  var_names[vit->first] = alg->GetVarTitle(vit->first);
	}
      }
    }
  }

  TDirectory *time_dir = Anp::GetDir(fDir, "putime");

  unsigned name_width = 0;
  vector<Anp::PuTimeHist> fit_vec;

  for(map<string, map<unsigned, PuTime> >::iterator it = chain_map.begin(); it != chain_map.end(); ++it) {
    
    const std::string &alg_name = it->first;
    const map<unsigned, PuTime> &pu_map = it->second;

    TDirectory *alg_dir = Anp::GetDir(time_dir, alg_name);

    for(map<string, string>::const_iterator vit = var_names.begin(); vit != var_names.end(); ++vit) {
      const string &var_name = vit->first;

      if(!fVarNames.empty() && std::count(fVarNames.begin(), fVarNames.end(), var_name) == 0) { 
	if(fDebug) {
	  log() << "Done - ignoring variable: " << var_name << endl;
	}
	continue;
      }

      string var_title = vit->second;
      if(var_title.size() < 2) var_title = var_name;
      
      TH1 *h = new TH1D(var_name.c_str(), var_title.c_str(), fTimes.size(), 1, fTimes.size()+1);
      h -> SetDirectory(alg_dir);
      h -> SetStats(false);
      h -> GetXaxis() -> SetTitle("Number of minbias interactions");
      h -> GetXaxis() -> CenterTitle();
      h -> GetYaxis() -> SetTitle(var_title.c_str());
      h -> GetYaxis() -> CenterTitle();

      unsigned alg_id = 0;
      for(map<unsigned, PuTime>::const_iterator pit = pu_map.begin(); pit != pu_map.end(); ++pit) {
	const PuTime &data = pit->second;
	alg_id = data.alg_id;

	map<std::string, double>::const_iterator vit = data.vars.find(var_name);
	if(vit != data.vars.end()) {
	  h -> SetBinContent(pit->first, vit->second);
	}
      }
      
      PuTimeHist fit = FitEff(h, fFitFunc);
      
      fit.hist = h;  
      fit.alg_id   = alg_id;
      fit.alg_name = alg_name;
      fit.var_name = var_name;

      fHists[alg_name].push_back(fit);

      //
      // Save fit results for tables/summary of all algorithms
      //
      name_width = std::max<unsigned>(name_width, alg_name.size());
    }
  }

  if(fPlot.valid()) fPlot->Done();

  if(!fit_vec.empty()) {
    std::sort(fit_vec.begin(), fit_vec.end());
    log() << "End - printing " << fit_vec.size() << " fit results(s) " << endl;
    for(unsigned int i = 0; i < fit_vec.size(); ++i) {
      fit_vec[i].Print("tex", name_width);
    }
  }

  if(fDebug) {
    log() << "Done - end." << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::MakePuTimes::Write(HtmlNode &inode, HtmlMenu &hmenu)
{
  //
  // Make plots of efficiency versus number of pileup events
  //
  if(!fIsConfig || (fCanvasType != "png" && fCanvasType != "eps")) {
    return;
  }  
  if(fHists.empty()) {
    return;
  }

  log() << "Write - processing " << fHists.size() << " object(s)" << endl;

  TCanvas *canvas = new TCanvas("PuTimeCanvas", "", 650, 400);
  canvas -> SetRightMargin(0.05);
  canvas -> SetLeftMargin(0.15);
  
  for(HistMap::iterator hit = fHists.begin(); hit != fHists.end(); ++hit) {
    const string alg_name = hit->first;
    const vector<PuTimeHist> &hist_vec = hit->second;

    Handle<HtmlNode> nodeAlg = hmenu.FindAlgName(alg_name);
    if(!nodeAlg.valid()) { 
      log() << "Write - failed to find HTML node: " << alg_name << endl;
      continue;
    }
    else {
      if(fDebug) {
	log() << "Write - found HTML node: " << alg_name << endl;
      }
    }
    
    Handle<HtmlNode> nodeDir;

    for(unsigned i = 0; i < hist_vec.size(); ++i) {
      const PuTimeHist &h = hist_vec.at(i);
      if(!h.hist) continue;
      
      stringstream alg_base;
      alg_base << "ALG_NAME_" << setfill('0') << setw(10) << h.alg_id;

      if(!nodeDir.valid()) {
	nodeDir = Handle<HtmlNode>(new HtmlNode(alg_base.str()+"_MakePuTimes.html", HtmlNode::kHTML));
	nodeAlg -> GetCont() << "<pre>" << nodeDir->GetLink("Pileup plots") << "</pre>" << endl;
	nodeDir -> GetCont() << "<pre>" << endl;
	inode.AddChild(nodeDir);
      }
      
      const string canvas_name = alg_base.str() + "_" + h.var_name + "." + fCanvasType;
      Anp::PrintHist(nodeDir.ref(), h.hist, canvas, canvas_name, h.hist->GetTitle());
    }

    if(nodeDir.valid()) {
      nodeDir -> GetCont() << "</pre>" << endl;
    }
  }
  
  delete canvas;
}

//-----------------------------------------------------------------------------
void Anp::MakePuTimes::Run(const TrigMonEvent &event)
{
  //
  // Loop over timing sequence and algorithm timing measurements
  //

  //
  // Read pileup event counts
  //
  float nminbias = 0.0, nsignal = 0.0;

  if(!event.getVar(216, nsignal) || !event.getVar(217, nminbias)) {
    return;
  }
  
  const unsigned npileup = static_cast<unsigned>(nsignal+nminbias);

  if(npileup < 1) return;

  //
  // Make histograms for pileup variables
  //
  if(fPlot.valid()) fPlot->Run(event);

  TimeMap::iterator it = fTimes.find(npileup);
  if(it == fTimes.end()) {
    
    log() << "Run - creating StudyTime for npileup=" << npileup << endl;

    Registry reg(fReg);
    reg.Set("StudyTime::IsSubAlg", "yes");
    reg.Set("StudyTime::MakeHists", "no");

    Handle<StudyTime> study(new StudyTime());
    study -> Config(reg);
    study -> Init();
    study -> Save(fDir);
    study -> Set(fConfig);

    it = fTimes.insert(TimeMap::value_type(npileup, study)).first;
  }
  
  //
  // Compute timing for events with fixed number of pileup events
  //
  it->second->Run(event);
}

//-----------------------------------------------------------------------------
Anp::PuTimeHist Anp::MakePuTimes::FitEff(TH1 *h, const std::string &fname)
{  
  //
  // Fit efficiency histogram with polynomial
  //
  Anp::PuTimeHist fit;
  
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
    f1 = new TF1(fname.c_str(), TrigPuTimeFunc, ledge, redge, 2);
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
 
  fit.func = f1;
  fit.key  = f1 -> GetParameter(1);
  fit.par0 = f1 -> GetParameter(0);
  fit.par1 = f1 -> GetParameter(1);
  fit.chi2 = f1 -> GetChisquare();
  fit.ndof = f1 -> GetNDF();

  return fit;
}

//-----------------------------------------------------------------------------
double TrigPuTimeFunc(double *x, double *par)
{
  // Fit function f = e * n^{k}
  // e = par[0] = pass fraction
  // k = par[1] = power law
  //
  assert(x && par && "null array(s)");
   
  return par[0]*std::pow(x[0], par[1]);
}
