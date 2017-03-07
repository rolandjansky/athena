/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  ____________________________________________________________________________
  @file postprocessHistos_updt.cxx
  @author: Liza Mijovic, Soeren Prell

  Goal: merge root files from several (grid) runs
  Why needed: dedicated treatment of some of the profiles 
              to set mean, width and their uncertainties
  Notes: tools to set these correspond to InDetPVM GetMeanWidth class tools
         For the script to work correctly, you need to update it if:
         GetMeanWidth tools or call parameters in InDetPVM code change.
   ____________________________________________________________________________
*/

// 0..4 in increasing verbosity
#define PRINTDBG 1

#include <iostream>
#include <iterator>
#include <utility>

#include <boost/algorithm/string.hpp>

#include "TFile.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TEfficiency.h"
#include "TMath.h"
#include "TROOT.h"
#include "TKey.h"
#include "TClass.h"
#include "TObject.h"
#include "TObjString.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"

using namespace std;

//____________________________________________________________________________________
// root-related  helpers
TObject* get_obj(string p_name) {
  TObject* obj = (TObject*)gDirectory->Get(p_name.c_str());
  return obj;
}

void get_all_of(TDirectory* const p_source, vector<string>& p_all_objects,
		const TClass *p_cl=TObject::Class(),string p_namematch="",string p_dirup="") {
  
  TIter nextkey(p_source->GetListOfKeys());
  TDirectory *savdir = gDirectory;
  TKey *key;
  while ((key=(TKey*)nextkey())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl) continue;
    if (cl->InheritsFrom(TDirectory::Class())) {
      p_source->cd(key->GetName());
      TDirectory *subdir = gDirectory;
      string oneup=subdir->GetName();
      string dirup = (""!=p_dirup) ? p_dirup + "/" + oneup : oneup;
      get_all_of(subdir,p_all_objects,p_cl,p_namematch,dirup);      
    }
    string name=string(key->GetName());
    if (cl->InheritsFrom(p_cl) && boost::contains(name,p_namematch))
      p_all_objects.push_back(p_dirup+"/"+name);
    savdir->cd();
  }
  return;
}

bool file_exists(const string p_name) {
  return (gSystem->AccessPathName(p_name.c_str(), kFileExists))?
    false : true;    
}

// can't use boost::combine with asetup
TObject* get_zip(string p_name,vector<string> p_names, vector<TObject*> p_objs) {
  TObject* obj = NULL;
  if (p_names.size()==p_objs.size()) {
    unsigned int ni=0;
    for (auto name : p_names) {
      if (name == p_name) 
	return p_objs[ni];      
      ++ni;
    }
  }
  return obj;
}

//____________________________________________________________________________________


//____________________________________________________________________________________
// postprocessing  helpers
// base from which tool-specific postprocessing should inherit
class IDPVM_pproc {
public:
  IDPVM_pproc(){};
  ~IDPVM_pproc(){};  
  // fetching and book-keeping of objects needed for postprocessing
  virtual void initializePlots(){};
  // function implementing/calling the actual posprocessing 
  virtual void finalizePlots(){};
  // wraper to get all that postprocessing requires in one go
  virtual int postprocess_dir(TDirectory* p_dir){return 0;};
  // set keys of histograms to postprocess  
  virtual void set_pproc_targnames(){};
  // set list of histograms needed as inputs to postprocessing
  virtual void set_pproc_srcnames(){};
  // conventions to get a source name for a target object to postprocess
  virtual string get_srcname_for(string p_name){return "";};
  // common (not tool-specific) helpers:
  vector<string> get_pproc_targnames(){return m_pproc_targnames;};
  vector<string> get_pproc_srcnames(){return m_pproc_srcnames;};
  TObject* get_target(string p_name);
  TObject* get_source(string p_name);
  int write_targets();
protected:
  vector<string> m_pproc_srcnames;
  vector<TObject*> m_pproc_sources;
  vector<string> m_pproc_targnames;
  vector<TObject*> m_pproc_targets; 
};

TObject* IDPVM_pproc::get_target(string p_name) {
  return get_zip(p_name,m_pproc_targnames,m_pproc_targets);
}

TObject* IDPVM_pproc::get_source(string p_name) {
  return get_zip(p_name,m_pproc_srcnames,m_pproc_sources);
}

int IDPVM_pproc::write_targets() {
  for (auto targ : m_pproc_targets)
    targ->Write(targ->GetName(),TObject::kOverwrite);
  return 0;
}


//------------------------------------------------------------------------------------
// postprocessing for InDetPerfPlot_res tool
class InDetPerfPlot_res_pproc : public IDPVM_pproc {
public:
  InDetPerfPlot_res_pproc();
  ~InDetPerfPlot_res_pproc(){};
  void initializePlots();
  // todo: smarter  imple
  int postprocess_dir(TDirectory* p_dir);
  // requieres manual synch with Athena source
  void finalizePlots();
   // ditto
  void Refinement(TH1D* temp, const std::string& width,
		  int var, int j, const std::vector<TH1*>& tvec,
		  const std::vector<TH1*>& rvec); 
  void set_pproc_targnames();
  void set_pproc_srcnames();
  string get_srcname_for(string p_name);
private:  
  // source and target histograms used in postprocessing
  // resolution
  // * vs eta
  vector<TH2*> m_meanbasePlots; // res vs param vs eta
  vector<TH1*> m_meanPlots; // resmean vs param vs eta
  vector<TH1*> m_resoPlots; // reswidth vs param vs eta
  // * vs pt
  vector<TH2*> m_mean_vs_ptbasePlots; // res vs param vs pt
  vector<TH1*> m_mean_vs_ptPlots; // resmean vs param vs pt
  vector<TH1*> m_resptPlots;// reswidth vs param vs pt
  // pulls <-- we do these only vs pt
  vector<TH2*> m_pullbasePlots; // pull vs param vs eta
  vector<TH1*> m_pullmeanPlots; // pullmean vs param vs eta
  vector<TH1*> m_pullwidthPlots; // pullwidth vs param vs eta
  // helpers and quantities used by the postprocessing code
  vector<string> m_paramNames {"d0", "z0", "phi", "theta", "z0*sin(theta)", "qopt"};
  vector<string> m_quantnames {"width","mean"};
  enum Param {
    D0, Z0, PHI, THETA, Z0SIN_THETA, QOPT, NPARAMS
  };
};

int InDetPerfPlot_res_pproc::postprocess_dir(TDirectory* p_dir){
  if (0<PRINTDBG)
    cout << __FILE__ << "\tchecking tool InDetPerfPlot_res "<< endl;
  for (auto checks : {m_pproc_targnames,m_pproc_srcnames})
    for (auto check : checks) {
      if (!p_dir->GetKey(check.c_str())) {
	if (0<PRINTDBG)
	  cout << __FILE__ << "\tNo key " << check <<" found, no postprocessing" << endl; 
	return 0;
      }
    }
  if (0<PRINTDBG)
    cout << __FILE__<< "\tFound all targets and sources, postprocessing "<<endl;
  initializePlots();
  finalizePlots();
  int ret=write_targets();
  return ret;
}

void InDetPerfPlot_res_pproc::set_pproc_targnames() {
  for (auto var : m_paramNames) {
    for (auto quant : m_quantnames) {
      // res vs eta and vs pt
      m_pproc_targnames.push_back("res"+quant+"_"+var+"_vs_eta");
      m_pproc_targnames.push_back("res"+quant+"_"+var+"_vs_pt");
      // pull vs eta 
      m_pproc_targnames.push_back("pull"+quant+"_"+var+"_vs_eta");
    }
  }
  return;
}

string InDetPerfPlot_res_pproc::get_srcname_for(string p_name) {
  for (auto quant : m_quantnames)
    boost::replace_all(p_name, quant, "");
  return p_name;
}

void InDetPerfPlot_res_pproc::set_pproc_srcnames() {
  for (auto targ : m_pproc_targnames)
    m_pproc_srcnames.push_back(get_srcname_for(targ));
  return;
}

InDetPerfPlot_res_pproc::InDetPerfPlot_res_pproc() {
  set_pproc_targnames();
  set_pproc_srcnames();
}

void InDetPerfPlot_res_pproc::initializePlots(){
  for (auto src : m_pproc_srcnames) 
    m_pproc_sources.push_back(get_obj(src));
  for (auto targ : m_pproc_targnames) 
    m_pproc_targets.push_back(get_obj(targ));
  for (auto var : m_paramNames) {
    // res vs eta
    m_meanbasePlots.push_back(dynamic_cast<TH2*>(get_source("res_"+var+"_vs_eta")));
    m_meanPlots.push_back(dynamic_cast<TH1*>(get_target("resmean_"+var+"_vs_eta")));
    m_resoPlots.push_back(dynamic_cast<TH1*>(get_target("reswidth_"+var+"_vs_eta")));
    // res vs pt
    m_mean_vs_ptbasePlots.push_back(dynamic_cast<TH2*>(get_source("res_"+var+"_vs_pt")));
    m_mean_vs_ptPlots.push_back(dynamic_cast<TH1*>(get_target("resmean_"+var+"_vs_pt")));
    m_resptPlots.push_back(dynamic_cast<TH1*>(get_target("reswidth_"+var+"_vs_pt")));
    // pulls vs eta
    m_pullbasePlots.push_back(dynamic_cast<TH2*>(get_source("pull_"+var+"_vs_eta")));
    m_pullmeanPlots.push_back(dynamic_cast<TH1*>(get_target("pullmean_"+var+"_vs_eta")));
    m_pullwidthPlots.push_back(dynamic_cast<TH1*>(get_target("pullwidth_"+var+"_vs_eta")));
  }
  return;
}


void InDetPerfPlot_res_pproc::finalizePlots() {
  
  std::string width_method = "iterative_convergence";
  std::string pull_width_method = "gaussian";
  unsigned int ptBins(0);
  if (m_mean_vs_ptbasePlots[0]) {
    ptBins = m_mean_vs_ptPlots[0]->GetNbinsX();
  } else {
    cout << __FILE__ << "\tInDetPerfPlot_res_pproc::finalizePlots " << endl;
    cout << "null pointer for histogram "<< endl;;
    return;
  }
  for (unsigned int var(0); var != NPARAMS; ++var) {
    if (m_meanPlots[var]) {
      unsigned int etaBins = m_meanPlots[var]->GetNbinsX();
      auto& meanbasePlot = m_meanbasePlots[var];
      auto& pullbasePlot = m_pullbasePlots[var];
      for (unsigned int j = 1; j <= etaBins; j++) {
        // Create dummy histo w/ content from TH2 in relevant eta bin
        TH1D* temp = meanbasePlot->ProjectionY(Form("%s_projy_bin%d", "Big_Histo", j), j, j);
        TH1D* temp_pull = pullbasePlot->ProjectionY(Form("%s_projy_bin%d", "Pull_Histo", j), j, j);
        Refinement(temp, width_method, var, j, m_meanPlots, m_resoPlots);
        Refinement(temp_pull, pull_width_method, var, j, m_pullmeanPlots, m_pullwidthPlots);
      }
      auto& mean_vs_ptbasePlot = m_mean_vs_ptbasePlots[var];
      for (unsigned int i = 1; i <= ptBins; i++) {
        TH1D* temp = mean_vs_ptbasePlot->ProjectionY(Form("%s_projy_bin%d", "Big_Histo", i), i, i);
        Refinement(temp, width_method, var, i, m_mean_vs_ptPlots, m_resptPlots);
      }
    }
  }
  return;
}

void InDetPerfPlot_res_pproc::Refinement(TH1D* temp, const std::string& width,
					 int var, int j, const std::vector<TH1*>& tvec,
					 const std::vector<TH1*>& rvec) {

  if (temp->GetXaxis()->TestBit(TAxis::kAxisRange)) {
    // remove range if set previously
    temp->GetXaxis()->SetRange();
    temp->ResetStats();
  }
  double mean(0.0), mean_error(0.0), prim_RMS(0.0), sec_RMS(0.0), RMS_error(0.0);
  if (temp->GetEntries() != 0.0) {
    mean = temp->GetMean();
    prim_RMS = temp->GetRMS();
    mean_error = temp->GetMeanError();
    RMS_error = temp->GetRMSError();
    if (width == "iterative_convergence") {
      sec_RMS = prim_RMS + 1.0;
      unsigned int withinLoopLimit(10);
      while ((std::fabs(sec_RMS - prim_RMS) > 0.001) and (--withinLoopLimit)) {
        prim_RMS = temp->GetRMS();
        double aymin = -3.0 * prim_RMS;
        double aymax = 3.0 * prim_RMS;
        if (aymin < temp->GetBinLowEdge(1)) {
          aymin = temp->GetBinLowEdge(1);
        }
        if (aymax > temp->GetBinCenter(temp->GetNbinsX())) {
          aymax = temp->GetBinCenter(temp->GetNbinsX());
        }
        temp->SetAxisRange(aymin, aymax);
        mean = temp->GetMean();
        sec_RMS = temp->GetRMS();
      }
      if (not withinLoopLimit) {
	if (0<PRINTDBG) {
	  cout << __FILE__ << "\tWARNING InDetPerfPlot_res_pproc::Refinement: "<< endl;
	  cout << __FILE__ << "\tLoop limit reached in <<iterative>> refinement of resolution" << endl;
	}
      }
      mean_error = temp->GetMeanError();
      RMS_error = temp->GetRMSError();
    } else if (width == "gaussian") {
      TFitResultPtr r = temp->Fit("gaus", "QS0");
      if (r.Get() and r->Status() % 1000 == 0) {
        mean = r->Parameter(1);
        mean_error = r->ParError(1);
        sec_RMS = r->Parameter(2);
        RMS_error = r->ParError(2);
      }
    } else if (width == "fusion") {
      sec_RMS = prim_RMS + 1.0;
      double aymin = temp->GetBinLowEdge(1);
      double aymax = temp->GetBinCenter(temp->GetNbinsX());
      unsigned int withinLoopLimit(10);
      while ((std::fabs(sec_RMS - prim_RMS) > 0.001) and (--withinLoopLimit)) {
        prim_RMS = temp->GetRMS();
        aymin = -3.0 * prim_RMS;
        aymax = 3.0 * prim_RMS;
        if (aymin < temp->GetBinLowEdge(1)) {
          aymin = temp->GetBinLowEdge(1);
        }
        if (aymax > temp->GetBinCenter(temp->GetNbinsX())) {
          aymax = temp->GetBinCenter(temp->GetNbinsX());
        }
        temp->SetAxisRange(aymin, aymax);
        mean = temp->GetMean();
        sec_RMS = temp->GetRMS();
      }
      if (not withinLoopLimit) {
	if (0<PRINTDBG) {
	  cout << __FILE__ << "\tWARNING InDetPerfPlot_res_pproc::Refinement: "<< endl;
	  cout << __FILE__ << "\tLoop limit reached in <<fusion>> refinement of resolution"<< endl;
	}
      }
      TFitResultPtr r = temp->Fit("gaus", "QS0", "", aymin, aymax);
      if (r.Get() and r->Status() % 1000 == 0) {
        mean = r->Parameter(1);
        mean_error = r->ParError(1);
        sec_RMS = r->Parameter(2);
        RMS_error = r->ParError(2);
      }
    }
  }
  (tvec[var])->SetBinContent(j, mean);
  (tvec[var])->SetBinError(j, mean_error);
  (rvec[var])->SetBinContent(j, sec_RMS);
  (rvec[var])->SetBinError(j, RMS_error);
  return;
}
//------------------------------------------------------------------------------------
//____________________________________________________________________________________


//____________________________________________________________________________________
// function driving the postprocessing for this file
int pproc_file(string p_infile) {

  TFile* const infile = new TFile(p_infile.c_str(),"UPDATE"); 
  if (! infile ) {
    cout << "could not open input file for updating "<< endl;
    cout << p_infile << endl;
    return 1;
  }
  
  TDirectory* topdir=gDirectory;
  vector<string> all_dirs;
  get_all_of(topdir,all_dirs,TDirectory::Class());
  for (auto dir : all_dirs) {
    if (0<PRINTDBG)
      cout << __FILE__ << " Post-processign directory " << dir << endl;
    topdir->cd(dir.c_str());
    TDirectory *rdir = gDirectory;
    InDetPerfPlot_res_pproc handle_IDPP_res;
    handle_IDPP_res.postprocess_dir(rdir);
    topdir->cd();
  }
    
  if (1<PRINTDBG)
    cout << __FILE__ << " loop over directories done " << endl;
  
  infile->Close();
  return 0;
}
//____________________________________________________________________________________


//____________________________________________________________________________________
int main(int argc, char* argv[]) {

  string infile= (1==argc) ? "" : argv[1];

  if (""==infile || !file_exists(infile)) {
    cout << "\n-------------------------------------------------------------"<<endl;
    cout << __FILE__ << " Error: invalid input file: " << infile << endl;
    cout << "\nrun like: "<< endl;
    cout << argv[0] << " infile.root\n" << endl;
    cout << "... where infile.root is typically obtained by hadding\n";
    cout << "    outputs of several InDetPVM independent runs." << endl;
    cout << "-------------------------------------------------------------\n"<<endl;
    return 1;
  }
  if (0<PRINTDBG)
    cout << "\n" << __FILE__<< " Post-processing file " << infile << "\n" << endl;
  int ret = pproc_file(infile);
  if (ret)
    cout << "\n" << __FILE__ << "Error when postprocessing file: " << infile << "\n" << endl;
  else if (0<PRINTDBG)
    cout << "\n" << __FILE__ << " Postprocessed finished successfully for file: " << infile << "\n" << endl;
  return ret;
}
//____________________________________________________________________________________      
