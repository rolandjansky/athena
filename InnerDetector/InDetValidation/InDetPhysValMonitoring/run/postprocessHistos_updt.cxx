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
#include "TH3.h"
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
// GetMeanWidth tool
// this code is copy-pasted from the corresponding src/.cxx and .h classes
// requires manual update when those are updated 
//....................................................................................
// GetMeanWidth.h
namespace IDPVM {
  
  class GetMeanWidth {
  public:
    GetMeanWidth();
    ~GetMeanWidth() {
      // nop
    };
    
    // methods acc to which mean&RMS can be evaluated
    enum methods { iterRMS_convergence, Gauss_fit, fusion_iterRMS_Gaussfit };
    
    // wrapper to set mean,rms,and fraction of events in tails
    // nb: some of the methods are allowed to modify input histogram
    void setResults(TH1* p_input_hist, methods p_method);

    // results getters
    double getMean() { return m_mean; };
    double getMeanError() { return m_meanError; };
    double getRMS() { return m_RMS; };
    double getRMSError() { return m_RMSError; };
    double getFracOut() { return m_FracOut; };
    double getFracOutUnc() { return m_FracOutUnc; };
    // return and clear accumulated warnings and errors
    vector<string> getInfos() { return m_infos; };
    vector<string> getWarnings() { return m_warnings; };
    vector<string> getErrors() { return m_errors; };
    
  private:
    // use gaussian fit, return 0 in case of successful fit
    int setGaussFit(TH1* p_input_hist);
    // iteratively change histogram range, until convergence
    // return # remaining iterations before hitting the max. allowed
    int setIterativeConvergence(TH1* p_input_hist);
    // evaluate the fraction of evens out of signal region and its uncertainty
    void setFout(double p_nsig,double p_ntot);
    // helper to fill-in starting values of the results vector
    bool initialize(TH1* p_input_hist);


    // results/outputs:
    double m_mean,m_meanError,m_RMS,m_RMSError,m_FracOut,m_FracOutUnc;  
    vector<string> m_infos;
    vector<string> m_warnings;
    vector<string> m_errors;
    
    //helpers 
    string m_inHistName;   
  };
} // end of namespace
// end of GetMeanWidth.h
//....................................................................................
// GetMeanWidth.cxx 
namespace IDPVM {
  
  GetMeanWidth::GetMeanWidth() :
    m_mean(0.),
    m_meanError(0.),
    m_RMS(0.),
    m_RMSError(0.),
    m_FracOut(0.),
    m_FracOutUnc(0.),
    m_inHistName("") {
    //nop
  }
  
  bool GetMeanWidth::initialize(TH1* p_input_hist) {

    if ( !m_infos.empty())
      m_infos.clear();      
    if ( !m_warnings.empty())
      m_warnings.clear();
    if ( !m_errors.empty())
      m_errors.clear();
       
    if (!p_input_hist) {
      m_errors.push_back("GetMeanWidth::initialize: empty input histogram passed: ");
       m_mean=m_meanError=m_RMS=m_RMSError=m_FracOut=m_FracOutUnc=0.;
      return false;
    }
    
    m_inHistName = p_input_hist->GetName();
    
    if ( 0==p_input_hist->GetEntries() ) {
      m_infos.push_back("GetMeanWidth::initialize: got input histogram with 0 entries: "+ m_inHistName);
      m_mean=m_meanError=m_RMS=m_RMSError=m_FracOut=m_FracOutUnc=0.;
      return false;
    }

    m_RMS = p_input_hist->GetRMS();
    m_RMSError = p_input_hist->GetRMSError();
    m_mean = p_input_hist->GetMean();
    m_meanError = p_input_hist->GetMeanError();   
    m_FracOut = 0.;
    m_FracOutUnc = 0.;
    
    return true;
  }
  
  void GetMeanWidth::setFout(double p_nsig,double p_ntot) {
    m_FracOut=0.;
    m_FracOutUnc=0.;
    if (p_ntot>0.) {
      double nout=p_ntot-p_nsig;
      if (nout / p_ntot > 0.9) {
	nout = 0.;
      }
      if (nout > 0.) {
	m_FracOut=nout/p_nsig;
	m_FracOutUnc=(nout / p_ntot) * TMath::Sqrt(1. / nout + 1. / p_ntot);
      }
    }
    return;
  }
  
  int GetMeanWidth::setGaussFit(TH1* p_input_hist){

    int ret=0;

    // evaluate mean and with via the Gauss fit
    TFitResultPtr r = p_input_hist->Fit("gaus", "QS0");
    if ( r.Get() and (0 == (r->Status()%1000)) ) {
      m_RMS=r->Parameter(2);//RMS
      m_RMSError=r->ParError(2);//RMS_error
      m_mean=r->Parameter(1);//mean
      m_meanError=r->ParError(1);//mean_error
    }
    else
      ret=1;

    // get fraction of events outside 3*RMS + its ~ uncertainty
    double nSig = p_input_hist->Integral(p_input_hist->GetXaxis()->FindBin(-3.0 * m_RMS),
					 p_input_hist->GetXaxis()->FindBin(3.0 * m_RMS));
    double nTot = p_input_hist->Integral();
    setFout(nSig,nTot);

    // return fit status
    return ret;
  }
  
  // return # remaining iterations before hitting the max. allowed
  int GetMeanWidth::setIterativeConvergence(TH1* p_input_hist) {
    
    // evaluate  mean and RMS using iterative converfgence:
    double mean=p_input_hist->GetMean();
    double RMS = p_input_hist->GetRMS();
    
    // iteration parameters:
    // max iteration steps
    unsigned int ntries_max = 100;
    // width of cutting range in [RMS]
    double nRMS_width = 3.0;
    
    // iteration counters and helpers: 
    // min and max range of the histogram:
    double min=0.;
    double max=0.;
    // min and max bins of the histogram in previous iteration
    // 0-th iteration: range of the original histogram
    int binmin_was = 1;
    int binmax_was = p_input_hist->GetNbinsX();
    // initial number of iteration steps
    unsigned int ntries = 0;
    
    // iteratively cut tails untill the RMS gets stable about mean
    // RMS stable: when input histogram range after cutting by 
    // +- 3*RMS is same as the range before cutting
    while ( ntries<ntries_max ) {
      ++ntries;
      RMS = p_input_hist->GetRMS();
      mean = p_input_hist->GetMean();
      min = -1.0*nRMS_width*RMS + mean;
      max = nRMS_width*RMS + mean;
      // find bins corresponding to new range
      int binmin=p_input_hist->GetXaxis()->FindFixBin(min);
      int binmax=p_input_hist->GetXaxis()->FindFixBin(max);
      // end iteration if these are same bins as in prev. iteration
      if ( binmin_was==binmin && binmax_was==binmax ) {
	break;
      }
      else {
	// else set the new range and continue iteration 
	p_input_hist->GetXaxis()->SetRange(binmin,binmax);
	binmin_was=binmin;
	binmax_was=binmax;
      }
    } // end of ( ntries<ntries_max ) ; iterative convergence loop
    
    // set the iteration results that are accessible to clients: 
    m_RMS=RMS;
    m_RMSError= p_input_hist->GetRMSError();    
    m_mean=mean;
    m_meanError=p_input_hist->GetMeanError();
    
    // get fraction of excluded events + its ~ uncertainty
    double nSig = p_input_hist->Integral(p_input_hist->GetXaxis()->FindBin(min),
					 p_input_hist->GetXaxis()->FindBin(max));
    double nTot = p_input_hist->Integral();
    setFout(nSig,nTot);
    
    // return number of remaining allowed iteration steps
    return(ntries_max-ntries);
  }

  // evaluate results for input histogram
  // * mean and RMS and fraction of events in tails
  // * call one of alternative ways of evaluation
  void GetMeanWidth::setResults(TH1* p_input_hist, methods p_method){

    // set start values of the results vector
    // mean RMS etc as in the input histo prior to trimming
    if (!initialize(p_input_hist))
      return;
    
    if (iterRMS_convergence == p_method) {
      if ( !setIterativeConvergence(p_input_hist) ) {
	m_warnings.push_back("GetMeanWidth::setIterativeConvergence did not converge for "+ m_inHistName);
      }
      return;
    }
    else if (Gauss_fit == p_method) {
      if ( !setGaussFit(p_input_hist) ) {
	m_warnings.push_back("GetMeanWidth::setGaussFit: fit failed for "+ m_inHistName);
      }
      return;
    }
    else if (fusion_iterRMS_Gaussfit == p_method) {
      if ( !setIterativeConvergence(p_input_hist) &&
	   !setGaussFit(p_input_hist) ) {
	m_warnings.push_back("GetMeanWidth::fusion_iterRMS_Gaussfit both methods failed for "+ m_inHistName);
      }
      return;
    }
    else {
      m_errors.push_back("GetMeanWidth::setResults: method not supported. No evaluation for "+ m_inHistName);
      return;
    }

    // should never come here
    return;
  }

}//end of namespace
// end of GetMeanWidth.cxx 
//....................................................................................

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
  void Refinement(TH1D* temp, IDPVM::GetMeanWidth::methods p_method,
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
  // class and methods for evaluating mean and width of distributions
  IDPVM::GetMeanWidth m_getMeanWidth;
  IDPVM::GetMeanWidth::methods m_meanWidthMethod;
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

InDetPerfPlot_res_pproc::InDetPerfPlot_res_pproc() :
  m_meanWidthMethod(IDPVM::GetMeanWidth::iterRMS_convergence) {
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

//.....................................................................................
void InDetPerfPlot_res_pproc::finalizePlots() {
  unsigned int ptBins(0);
  if (m_mean_vs_ptbasePlots[0]) {
    ptBins = m_mean_vs_ptPlots[0]->GetNbinsX();
  } else {
    std::cout << "WARNING InDetPerfPlot_res::finalizePlots(): null pointer for histogram, likely it wasn't booked, possibly due to missing histogram definition" << std::endl;
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
        Refinement(temp, m_meanWidthMethod, var, j, m_meanPlots, m_resoPlots);
	Refinement(temp_pull, m_meanWidthMethod, var, j, m_pullmeanPlots, m_pullwidthPlots);
      }
      auto& mean_vs_ptbasePlot = m_mean_vs_ptbasePlots[var];
      for (unsigned int i = 1; i <= ptBins; i++) {
        TH1D* temp = mean_vs_ptbasePlot->ProjectionY(Form("%s_projy_bin%d", "Big_Histo", i), i, i);
        Refinement(temp, m_meanWidthMethod, var, i, m_mean_vs_ptPlots, m_resptPlots);
      }
    }
  }

  return;
}// end of InDetPerfPlot_res_pproc::finalizePlots()
//.....................................................................................

//.....................................................................................
void
InDetPerfPlot_res_pproc::Refinement(TH1D* temp, IDPVM::GetMeanWidth::methods p_method,
				    int var, int j, const std::vector<TH1*>& tvec,
				    const std::vector<TH1*>& rvec) {
  if (temp->GetXaxis()->TestBit(TAxis::kAxisRange)) {
    // remove range if set previously
    temp->GetXaxis()->SetRange();
    temp->ResetStats();
  }
  
  m_getMeanWidth.setResults(temp, p_method);
  // print out any warnings and errors
  if (PRINTDBG>2) {
    for (auto _it : m_getMeanWidth.getInfos())
      std::cout << _it << std::endl;
  }
  if (PRINTDBG>1) {
    for (auto _it : m_getMeanWidth.getWarnings())
      std::cout <<_it << std::endl;
  }
  if (PRINTDBG>0) {
    for (auto _it : m_getMeanWidth.getErrors())
      std::cout << _it << std::endl;
  }
  // RMS and RMSerror
  (rvec[var])->SetBinContent(j, m_getMeanWidth.getRMS());
  (rvec[var])->SetBinError(j, m_getMeanWidth.getRMSError());
  // mean and meanerror
  (tvec[var])->SetBinContent(j, m_getMeanWidth.getMean());
  (tvec[var])->SetBinError(j, m_getMeanWidth.getMeanError());

  return;
} // InDetPerfPlot_res_pproc::Refinement
//....................................................................................
// end of InDetPerfPlot_res_pproc 
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// postprocessing for InDetPerfPlot_resITk tool
class InDetPerfPlot_resITk_pproc : public IDPVM_pproc {
public:
  InDetPerfPlot_resITk_pproc();
  ~InDetPerfPlot_resITk_pproc(){};
  void initializePlots();
  // todo: smarter  imple
  int postprocess_dir(TDirectory* p_dir);
  // requieres manual synch with Athena source
  void finalizePlots();
  void set_pproc_targnames();
  void set_pproc_srcnames();
  string get_srcname_for(string p_name);
private:  
  enum Param {
    D0, Z0, QOVERP, QOVERPT, THETA, PHI, PT, Z0SIN, NPARAMS
  };

  static const int m_nEtaBins = 16;
  static const int m_nPtBins = 9;
  static const int m_nResHist = 4;

  float m_EtaBins[m_nEtaBins + 1];
  // source and target histograms used in postprocessing
  // ---------------------------
  TH2* m_resITk_resHelpereta[NPARAMS];
  TH1* m_resITk_Resolution_vs_eta[NPARAMS][m_nResHist];
  TH1* m_resITk_ResProjections_vs_eta[NPARAMS][m_nEtaBins];

  TH2* m_resITk_resHelpereta_pos[NPARAMS];
  TH1* m_resITk_Resolution_vs_eta_pos[NPARAMS][m_nResHist];
  TH2* m_resITk_resHelpereta_neg[NPARAMS];
  TH1* m_resITk_Resolution_vs_eta_neg[NPARAMS][m_nResHist];

  TH2* m_resITk_resHelperpt[NPARAMS];
  TH1* m_resITk_Resolution_vs_pt[NPARAMS][m_nResHist];
  TH1* m_resITk_ResProjections_vs_pt[NPARAMS][m_nPtBins];

  TH2* m_resITk_resHelperpt_pos[NPARAMS];
  TH1* m_resITk_Resolution_vs_pt_pos[NPARAMS][m_nResHist];
  TH2* m_resITk_resHelperpt_neg[NPARAMS];
  TH1* m_resITk_Resolution_vs_pt_neg[NPARAMS][m_nResHist];

  TH2* m_resITk_pullHelperpt[NPARAMS]; // pull width as a function of pT
  TH2* m_resITk_pullHelpereta[NPARAMS]; // pull width as a function of pT

  TH1* m_resITk_pullResolution_vs_pt[NPARAMS][m_nResHist];
  TH1* m_resITk_pullResolution_vs_eta[NPARAMS][m_nResHist];

  TH1* m_resITk_pullProjections_vs_pt[NPARAMS][m_nPtBins];
  TH1* m_resITk_pullProjections_vs_eta[NPARAMS][m_nEtaBins];

  TH1* m_resITk_Resolution_vs_pt_EtaBin[NPARAMS][4][m_nResHist];
  TH1* m_resITk_Resolution_vs_eta_PtBin[NPARAMS][4][m_nResHist];
  // ---------------------------
  // significance histo sources
  TH2* m_significance_d0;
  TH2* m_significance_z0;
  // significance histo targets
  TH1* m_significance_d0_vs_eta;
  TH1* m_significance_z0_vs_eta;
  // --------------------------- 
  // fix histogram sources 
  TH1* m_fix_qoverpt_res[m_nEtaBins];
  TH1* m_fix_d0_res[m_nEtaBins];
  TH1* m_fix_z0_res[m_nEtaBins];
  // fix histogram targets 
  TH1* m_fix_qoverptresolutionRMS_vs_eta;
  TH1* m_fix_d0resolutionRMS_vs_eta;
  TH1* m_fix_z0resolutionRMS_vs_eta;
  // --------------------------- 

  float m_PtBins[m_nPtBins + 1] = {
    0.0, 0.5, 0.7, 1.0, 2.0, 3.0, 5.0, 10.0, 20.0, 50.0
  };
  std::string m_resHisto[m_nResHist] = {
    "resolutionRMS", "meanRMS", "resolutionGAUS", "meanGAUS"
  };
  
  IDPVM::GetMeanWidth m_getMeanWidth;
  IDPVM::GetMeanWidth::methods m_meanWidthMethod;

  void makeResolutions(TH2* h, TH1* h2[m_nResHist], TH1* h3[], bool s);
  void makeResolutions(TH2* h, TH1* h2[m_nResHist]);
  
  void makeResolutions(TH3* h, TH1* h2[][m_nResHist], TH1* h3[][m_nResHist]);
  void getMeanWidthResultsModUnits(TH1* p_input_hist, vector<float>& p_result,
				   IDPVM::GetMeanWidth::methods p_method);
  void cloneHistogram(TH1D* h, TH1* hcopy);
};

int InDetPerfPlot_resITk_pproc::postprocess_dir(TDirectory* p_dir){
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

void InDetPerfPlot_resITk_pproc::set_pproc_targnames() {
  // m_pproc_targnames.push_back("res"+quant+"_"+var+"_vs_eta");
  return;
}

string InDetPerfPlot_resITk_pproc::get_srcname_for(string p_name) {
  return p_name;
}

void InDetPerfPlot_resITk_pproc::set_pproc_srcnames() {
  //for (auto targ : m_pproc_targnames)
  //  m_pproc_srcnames.push_back(get_srcname_for(targ));
  return;
}

InDetPerfPlot_resITk_pproc::InDetPerfPlot_resITk_pproc() :
  m_meanWidthMethod(IDPVM::GetMeanWidth::iterRMS_convergence) {
  set_pproc_targnames();
  set_pproc_srcnames();
}

void InDetPerfPlot_resITk_pproc::initializePlots(){
  /*
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
  */
  return;
}
//.....................................................................................
void
InDetPerfPlot_resITk_pproc::getMeanWidthResultsModUnits(TH1* p_input_hist, vector<float>& p_result,
						  IDPVM::GetMeanWidth::methods p_method) {
  // LM: reason for using this function:
  //   orignial code was set up to only have resolution resuls in um,
  //   while inputs are in mm. Do not want to do a substantial rewrite.
  double mm2um = 1000.;

  if (!p_result.empty()) {
    p_result.clear();
  }
  m_getMeanWidth.setResults(p_input_hist, p_method);
  p_result.push_back(m_getMeanWidth.getRMS());
  p_result.push_back(m_getMeanWidth.getRMSError());
  p_result.push_back(m_getMeanWidth.getMean());
  p_result.push_back(m_getMeanWidth.getMeanError());
  p_result.push_back(m_getMeanWidth.getFracOut());
  p_result.push_back(m_getMeanWidth.getFracOutUnc());

  TString vari = p_input_hist->GetName();
  if ( !vari.Contains("pull") &&
       (vari.Contains("d0") || vari.Contains("z0")) ) {
    // the  mean, meanerror, RMS and RMSerror
    // are conventionally first four elements of results vector
    p_result[0]*= mm2um;
    p_result[1]*= mm2um;
    p_result[2]*= mm2um;
    p_result[3]*= mm2um;
  }

  /*
  // print out any warnings and errors
  // <-- LM need to figure out what to do to get
  //     ATH_MSG not working in this class
  for (auto _it : m_getMeanWidth.getWarnings())
    ATH_MSG_WARNING(_it);
  for (auto _it : m_getMeanWidth.getErrors())
    ATH_MSG_ERROR(_it); 
  */
}
// end of InDetPerfPlot_resITk_pproc::getMeanWidthResultsModUnits
//.....................................................................................

void
InDetPerfPlot_resITk_pproc::makeResolutions(TH2* h, TH1* hres[4]) {
  // Should fix this in a better way
  TString hname = h->GetName();

  if (hname.Contains("Helpereta")) {
    for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ieta + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_EtaBins[ieta]),
                                         h->GetXaxis()->FindBin(m_EtaBins[ieta + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hres[0]->SetBinContent(ieta + 1, result.at(0));
      hres[0]->SetBinError(ieta + 1, result.at(1));
      hres[1]->SetBinContent(ieta + 1, result.at(2));
      hres[1]->SetBinError(ieta + 1, result.at(3));
      result.clear();
      /*	  
	      std::vector<float> result;
	      IDPVM::GetMeanWidth::methods altMeanWidthMethod = Gauss_fit;
              getMeanWidthResultsModUnits(tmp, result, altMeanWidthMethod);
              hres[2]->SetBinContent(ieta+1,result.at(0));
              hres[2]->SetBinError(ieta+1,result.at(1));
              hres[3]->SetBinContent(ieta+1,result.at(2));
              hres[3]->SetBinError(ieta+1,result.at(3));*/
      delete tmp;
    }
  } else if (hname.Contains("Helperpt")) {
    for (unsigned int ipt = 0; ipt < m_nPtBins; ipt++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ipt + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                         h->GetXaxis()->FindBin(m_PtBins[ipt + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hres[0]->SetBinContent(ipt + 1, result.at(0));
      hres[0]->SetBinError(ipt + 1, result.at(1));
      hres[1]->SetBinContent(ipt + 1, result.at(2));
      hres[1]->SetBinError(ipt + 1, result.at(3));
      result.clear();
      
/*		  
	    std::vector<float> result;
	    IDPVM::GetMeanWidth::methods altMeanWidthMethod = Gauss_fit;
            getMeanWidthResultsModUnits(tmp, result, altMeanWidthMethod);
            hres[2]->SetBinContent(ipt+1,result.at(0));
            hres[2]->SetBinError(ipt+1,result.at(1));
            hres[3]->SetBinContent(ipt+1,result.at(2));
            hres[3]->SetBinError(ipt+1,result.at(3));*/
      delete tmp;
    }
  }
}

void
InDetPerfPlot_resITk_pproc::makeResolutions(TH2* h, TH1* hres[4], TH1* hproj[m_nEtaBins], bool save) {
  TString hname = h->GetName();

  if (hname.Contains("Helpereta")) {
    for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ieta + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_EtaBins[ieta]),
                                         h->GetXaxis()->FindBin(m_EtaBins[ieta + 1]-0.05));
      if (tmp->Integral() < 1) {
        continue;
      }
      if (save) {
        cloneHistogram(tmp, hproj[ieta]);
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hres[0]->SetBinContent(ieta + 1, result.at(0));
      hres[0]->SetBinError(ieta + 1, result.at(1));
      hres[1]->SetBinContent(ieta + 1, result.at(2));
      hres[1]->SetBinError(ieta + 1, result.at(3));
      result.clear();
      /*	  
	      std::vector<float> result;
	      IDPVM::GetMeanWidth::methods altMeanWidthMethod = Gauss_fit;
              getMeanWidthResultsModUnits(hproj[ieta], result, altMeanWidthMethod);	  
              hres[2]->SetBinContent(ieta+1,result.at(0));
              hres[2]->SetBinError(ieta+1,result.at(1));
              hres[3]->SetBinContent(ieta+1,result.at(2));
              hres[3]->SetBinError(ieta+1,result.at(3));*/
      delete tmp;
    }
  } else if (hname.Contains("Helperpt")) {
    for (unsigned int ipt = 0; ipt < m_nPtBins; ipt++) {
      std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ipt + 1);
      TH1D* tmp = (TH1D*) h->ProjectionY(tmpName.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                         h->GetXaxis()->FindBin(m_PtBins[ipt + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      if (save) {
        cloneHistogram(tmp, hproj[ipt]);
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result, m_meanWidthMethod);
      hres[0]->SetBinContent(ipt + 1, result.at(0));
      hres[0]->SetBinError(ipt + 1, result.at(1));
      hres[1]->SetBinContent(ipt + 1, result.at(2));
      hres[1]->SetBinError(ipt + 1, result.at(3));
      result.clear();
      /*  std::vector<float> result;
          IDPVM::GetMeanWidth::methods altMeanWidthMethod = Gauss_fit;
          getMeanWidthResultsModUnits((hproj[ipt], result, altMeanWidthMethod);
          hres[2]->SetBinContent(ipt+1,result.at(0));
          hres[2]->SetBinError(ipt+1,result.at(1));
          hres[3]->SetBinContent(ipt+1,result.at(2));
          hres[3]->SetBinError(ipt+1,result.at(3));*/
      delete tmp;
    }
  }
}

void
InDetPerfPlot_resITk_pproc::cloneHistogram(TH1D* h, TH1* hcopy) {
  unsigned int nbin = h->GetNbinsX();

  for (unsigned int ibin = 0; ibin < nbin; ibin++) {
    float binContent = h->GetBinContent(ibin + 1);
    float binError = h->GetBinError(ibin + 1);
    hcopy->SetBinContent(ibin + 1, binContent);
    hcopy->SetBinError(ibin + 1, binError);
  }
}

void
InDetPerfPlot_resITk_pproc::makeResolutions(TH3* h, TH1* hres_eta[4][4], TH1* hres_pt[4][4]) {
  float BinEta[5] = {
    0.0, 1.0, 1.5, 2.7, 5.0
  };
  float BinPt[5] = {
    0.0, 1.0, 2.0, 5.0, 500.0
  };

  for (unsigned int ieta = 0; ieta < 4; ieta++) {
    for (unsigned int ipt = 0; ipt < m_nPtBins; ipt++) {
      std::string tmpName = h->GetName() + std::string("pz_binPt") + std::to_string(ipt + 1) +
                            std::string("pz_binEta") + std::to_string(ieta + 1);
      std::string tmpName1 = tmpName + std::string("_neg");
      std::string tmpName2 = tmpName + std::string("_pos");
      TH1D* tmp1 = (TH1D*) h->ProjectionZ(tmpName1.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                          h->GetXaxis()->FindBin(m_PtBins[ipt + 1]),
                                          h->GetYaxis()->FindBin(-BinEta[ieta + 1]),
                                          h->GetYaxis()->FindBin(-BinEta[ieta]));
      TH1D* tmp2 = (TH1D*) h->ProjectionZ(tmpName2.c_str(), h->GetXaxis()->FindBin(m_PtBins[ipt]),
                                          h->GetXaxis()->FindBin(m_PtBins[ipt + 1]),
                                          h->GetYaxis()->FindBin(BinEta[ieta]),
                                          h->GetYaxis()->FindBin(BinEta[ieta + 1]));
      tmp1->Add(tmp2);
      if (tmp1->Integral() < 1) {
        continue;
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp1, result, m_meanWidthMethod);
      hres_eta[ieta][0]->SetBinContent(ipt + 1, result.at(0));
      hres_eta[ieta][0]->SetBinError(ipt + 1, result.at(1));
      hres_eta[ieta][1]->SetBinContent(ipt + 1, result.at(2));
      hres_eta[ieta][1]->SetBinError(ipt + 1, result.at(3));
      delete tmp1;
      delete tmp2;
    }
  }

  for (unsigned int ipt = 0; ipt < 4; ipt++) {
    for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
      std::string tmpName = h->GetName() + std::string("pz_binPt") + std::to_string(ipt + 1) +
                            std::string("pz_binEta") + std::to_string(ieta + 1);
      TH1D* tmp =
        (TH1D*) h->ProjectionZ(tmpName.c_str(), h->GetXaxis()->FindBin(BinPt[ipt]), h->GetXaxis()->FindBin(
                                 BinPt[ipt + 1]), h->GetYaxis()->FindBin(m_EtaBins[ieta]),
                               h->GetYaxis()->FindBin(m_EtaBins[ieta + 1]));
      if (tmp->Integral() < 1) {
        continue;
      }
      std::vector<float> result;
      getMeanWidthResultsModUnits(tmp, result,m_meanWidthMethod);
      hres_pt[ipt][0]->SetBinContent(ieta + 1, result.at(0));
      hres_pt[ipt][0]->SetBinError(ieta + 1, result.at(1));
      hres_pt[ipt][1]->SetBinContent(ieta + 1, result.at(2));
      hres_pt[ipt][1]->SetBinError(ieta + 1, result.at(3));
      delete tmp;
    }
  }
}
// end of InDetPerfPlot_resITk_pproc::makeResolutions
///.....................................................................................
void InDetPerfPlot_resITk_pproc::finalizePlots() {

  for (unsigned int iparam = 0; iparam < NPARAMS; iparam++) {    
    makeResolutions(m_resITk_resHelpereta[iparam], m_resITk_Resolution_vs_eta[iparam],
                    m_resITk_ResProjections_vs_eta[iparam], true);
    makeResolutions(m_resITk_resHelperpt[iparam], m_resITk_Resolution_vs_pt[iparam],
                    m_resITk_ResProjections_vs_pt[iparam], true);
    makeResolutions(m_resITk_resHelperpt_pos[iparam], m_resITk_Resolution_vs_pt_pos[iparam]);
    makeResolutions(m_resITk_resHelperpt_neg[iparam], m_resITk_Resolution_vs_pt_neg[iparam]);
    makeResolutions(m_resITk_resHelpereta_pos[iparam], m_resITk_Resolution_vs_eta_pos[iparam]);
    makeResolutions(m_resITk_resHelpereta_neg[iparam], m_resITk_Resolution_vs_eta_neg[iparam]);
/*    makeResolutions(m_resITk_resHelperetapt[iparam], m_resITk_Resolution_vs_pt_EtaBin[iparam],
                    m_resITk_Resolution_vs_eta_PtBin[iparam]);
*/
    // add for the pull vs pT
    makeResolutions(m_resITk_pullHelperpt[iparam], m_resITk_pullResolution_vs_pt[iparam],
                    m_resITk_pullProjections_vs_pt[iparam], true);
    makeResolutions(m_resITk_pullHelpereta[iparam], m_resITk_pullResolution_vs_eta[iparam],
                    m_resITk_pullProjections_vs_eta[iparam], true);
  }

  // Get total fraction of miss ID and Momentum tails 
  /***********************************************************************
  // not sure how to handle this 
  // omit from hadding, descale, hadd and rescale ...  
  m_resITk_chargeID_vs_pt->Scale(1 / m_resITk_chargeID->GetEntries());
  m_resITk_chargeID_vs_eta->Scale(1 / m_resITk_chargeID->GetEntries());

  m_resITk_momTail_vs_pt->Scale(1 / m_resITk_momTail->GetEntries());
  m_resITk_momTail_vs_phi->Scale(1 / m_resITk_momTail->GetEntries());
  m_resITk_momTail_vs_eta->Scale(1 / m_resITk_momTail->GetEntries());
  ***********************************************************************/

  for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
    TH1D* tmp = (TH1D*) m_significance_d0->ProjectionY("tmp_py", m_significance_d0->GetXaxis()->FindBin(
                                                         m_EtaBins[ieta]),
                                                       m_significance_d0->GetXaxis()->FindBin(m_EtaBins[ieta + 1]));
    m_significance_d0_vs_eta->SetBinContent(ieta + 1, tmp->GetRMS());
    m_significance_d0_vs_eta->SetBinError(ieta + 1, tmp->GetRMSError());
  }
  for (unsigned int ieta = 0; ieta < m_nEtaBins; ieta++) {
    TH1D* tmp = (TH1D*) m_significance_z0->ProjectionY("tmp_py", m_significance_z0->GetXaxis()->FindBin(
                                                         m_EtaBins[ieta]),
                                                       m_significance_z0->GetXaxis()->FindBin(m_EtaBins[ieta + 1]));
    m_significance_z0_vs_eta->SetBinContent(ieta + 1, tmp->GetRMS());
    m_significance_z0_vs_eta->SetBinError(ieta + 1, tmp->GetRMSError());
  }
  // m_fix_qoverpt_res[8] = Book1D(fixName,fixName,600,-0.2,0.2,false);

  for (int ieta = 0; ieta < m_nEtaBins; ieta++) {
    std::vector<float> result;
    getMeanWidthResultsModUnits(m_fix_qoverpt_res[ieta], result, m_meanWidthMethod);
    m_fix_qoverptresolutionRMS_vs_eta->SetBinContent(ieta + 1, result.at(0));
    m_fix_qoverptresolutionRMS_vs_eta->SetBinError(ieta + 1, result.at(1));
  }
  for (int ieta = 0; ieta < m_nEtaBins; ieta++) {
    std::vector<float> result;
    getMeanWidthResultsModUnits(m_fix_d0_res[ieta], result, m_meanWidthMethod);
    m_fix_d0resolutionRMS_vs_eta->SetBinContent(ieta + 1, result.at(0));
    m_fix_d0resolutionRMS_vs_eta->SetBinError(ieta + 1, result.at(1));
  }
  for (int ieta = 0; ieta < m_nEtaBins; ieta++) {
    std::vector<float> result;
    getMeanWidthResultsModUnits(m_fix_z0_res[ieta], result, m_meanWidthMethod);
    m_fix_z0resolutionRMS_vs_eta->SetBinContent(ieta + 1, result.at(0));
    m_fix_z0resolutionRMS_vs_eta->SetBinError(ieta + 1, result.at(1));
  }
  return;
} // end of InDetPerfPlot_resITk_pproc::finalizePlots()
//....................................................................................
// end of of InDetPerfPlot_resITk_pproc tools 
//------------------------------------------------------------------------------------
// end of postprocessing tools code 
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
