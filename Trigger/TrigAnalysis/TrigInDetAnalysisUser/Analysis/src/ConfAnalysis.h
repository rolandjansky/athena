/* emacs: this is -*- c++ -*- */
/**
 **     @file    ConfAnalysis.h
 **
 **     @author  mark sutton
 **     @date    $Id: ConfAnalysis.h 800361 2017-03-12 14:33:19Z 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef ANALYSIS_CONFANALYSIS_H
#define ANALYSIS_CONFANALYSIS_H

#include <iostream>
#include <vector>
#include <map>

#include "TrigInDetAnalysis/TrackAnalysis.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysis/Efficiency.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysis/TrigObjectMatcher.h"

#include "TrigInDetAnalysisExample/ChainString.h"


#include "Resplot.h"


#include "TH1F.h"
#include "TH2F.h"
// #include "Reflex/Reflex.h"

// using namespace ROOT::Reflex;


inline const std::string clean( std::string s) { 
  std::replace(s.begin(), s.end(), ':', '_');
  std::replace(s.begin(), s.end(), ';', '_');
  return s; /// return to keep the compiler quiet
}


class ConfAnalysis : public TrackAnalysis { 
  
  //  private:
  //     typedef std::map<std::string, TH1F*> maptype; 

public:
  
  ConfAnalysis( const std::string& name, const ChainString& config ) : 
    TrackAnalysis( clean(name) ), 
    mconfig(config),
    Nreco(0), Nref(0), Nmatched(0), m_print(false), m_roi(0), 
    m_initialised(false), m_initialiseFirstEvent(false) { // , m_lfirst(true)  {
    std::cout << "ConfAnalysis::ConfAnalysis() " << TrackAnalysis::name() << " ..." << std::endl;
  }  

  ~ConfAnalysis() {
    // std::cout << "ConfAnalysis::~ConfAnalysis() " << name() << std::endl;
    std::map<std::string, TH1F*>::iterator hitr=m_histos.begin();
    std::map<std::string, TH1F*>::iterator hend=m_histos.end();
    for ( ; hitr!=hend ; hitr++ ) delete hitr->second;     
    //2D histograms                                                                                                                                             
    std::map<std::string, TH2F*>::iterator hitr2D=m_histos2D.begin();                                                                                           
    std::map<std::string, TH2F*>::iterator hend2D=m_histos2D.end();                                                                                             
    for ( ; hitr2D!=hend2D ; hitr2D++ ) delete hitr2D->second;     
  }  
  
  virtual void initialise();
  virtual void initialiseInternal();

  virtual void execute(const std::vector<TIDA::Track*>& reftracks,
                       const std::vector<TIDA::Track*>& testtracks,
                       TrackAssociator* matcher, 
		       TrigObjectMatcher* objects );

  virtual void execute(const std::vector<TIDA::Track*>& reftracks,
                       const std::vector<TIDA::Track*>& testtracks,
                       TrackAssociator* matcher ) { 
    execute( reftracks, testtracks, matcher, 0 );
  }

#if 0
  virtual void execute(const std::vector<TIDA::Track*>& reftracks,
                       const std::vector<TIDA::Track*>& testtracks,
                       TrackAssociator* matcher, 
		       TIDARoiDescriptor* roi );
#endif

  virtual void finalise();
  
  void setprint(bool p) { m_print=p; }

  void setRoi(const TIDARoiDescriptor* roi) { m_roi = roi; }

  void initialiseFirstEvent(bool b=true) { m_initialiseFirstEvent=b; }

  const ChainString& config() const { return mconfig; }

private:
  
  void addHistogram( TH1F* h ) { 
      std::string name = h->GetName();
      m_histos.insert( std::map<std::string, TH1F*>::value_type( name, h) );
  }
  void addHistogram2D( TH2F* h ) {                                                                                                                              
      std::string name = h->GetName();                                                                                                                          
      m_histos2D.insert( std::map<std::string, TH2F*>::value_type( name, h) );                                                                                  
  }                              
  
  TH1F* find( const std::string& n ) { 
      std::map<std::string, TH1F*>::iterator hmitr = m_histos.find(n);
      if ( hmitr!=m_histos.end() ) return hmitr->second;
      else                         return 0; 
  }

  TH2F* find2D( const std::string& n ) {                                                                                                                        
    std::map<std::string, TH2F*>::iterator hmitr = m_histos2D.find(n);                                                                                        
    if ( hmitr!=m_histos2D.end() ) return hmitr->second;                                                                                                      
    else                         return 0;                                                                                                                    
  }              

private:

  ChainString  mconfig;

  TIDDirectory* mdir;

  std::map<std::string, TH1F*> m_histos;
  std::map<std::string, TH2F*> m_histos2D;

  Efficiency* eff_pt = 0;
  Efficiency* eff_ptp = 0;
  Efficiency* eff_ptm = 0;

  Efficiency* eff_eta = 0;
  Efficiency* eff_phi = 0;
  Efficiency* eff_z0 = 0;
  Efficiency* eff_d0 = 0;
  Efficiency* eff_a0 = 0;

  Efficiency* eff_roi_deta = 0;
  Efficiency* eff_roi_dphi = 0;
  Efficiency* eff_roi_dR = 0;

  Efficiency* purity_pt = 0;
  Efficiency* purity_eta = 0;
  Efficiency* purity_phi = 0;
  Efficiency* purity_z0 = 0;
  Efficiency* purity_d0 = 0;
  Efficiency* purity_a0 = 0;

#if 0
  TH2F* h2;
  TH2F* h2m;
  TH2F* h2r;
#endif

  Resplot* h2 = 0;
  Resplot* h2m = 0;
  Resplot* h2r = 0;

  Resplot* h2a0 = 0;
  Resplot* h2a0r = 0;

  Resplot* rChi2prob = 0;
  Resplot* rChi2 = 0;
  Resplot* rChi2dof = 0;

  Resplot* rChi2prob_bad = 0;
  Resplot* rChi2_bad = 0;
  Resplot* rChi2dof_bad = 0;

  //  TH2F* hnpix_v_sct;
  //  TH2F* hnpix_v_sct_rec;
  //  TH2F* hnpix_v_sct_match;

  TH1F* hDeltaR;

  /// number of reconstructed tracks 
  int Nreco;
  int Nref;
  int Nmatched;

  Resplot* rnpix_eta = 0;
  Resplot* rnsct_eta = 0;
  Resplot* rntrt_eta = 0;
  Resplot* rnsihit_eta = 0;
  Resplot* rnpix_lb = 0;

  Resplot* rnpix_phi = 0;
  Resplot* rnsct_phi = 0;
  Resplot* rntrt_phi = 0;

  Resplot* rnpix_pt = 0;
  Resplot* rnsct_pt = 0;
  Resplot* rntrt_pt = 0;

  Resplot* rnpix_d0 = 0; // new
  Resplot* rnsct_d0 = 0; // new
  Resplot* rntrt_d0 = 0; // new

  Resplot* rnpixh_pt = 0;
  Resplot* rnscth_pt = 0;

  Resplot* rnpixh_d0 = 0; // new
  Resplot* rnscth_d0 = 0; // new

  Resplot* rnsi_pt = 0;  // new
  Resplot* rnsih_pt = 0; // new

  Resplot* rnsi_eta = 0;  // new
  Resplot* rnsih_eta = 0; // new

  Resplot* rnsi_d0 = 0;  // new
  Resplot* rnsih_d0 = 0; // new

  Resplot* rnbl_d0 = 0;  // new
  Resplot* rnblh_d0 = 0; // new

  Resplot* rnpix_pt_bad = 0;
  Resplot* rnsct_pt_bad = 0;
  Resplot* rntrt_pt_bad = 0;

  Resplot* rnpix_eta_rec = 0;
  Resplot* rnsct_eta_rec = 0;
  Resplot* rntrt_eta_rec = 0;
  Resplot* rnsihit_eta_rec = 0;

  Resplot* rnpix_phi_rec = 0;
  Resplot* rnsct_phi_rec = 0;
  Resplot* rntrt_phi_rec = 0;

  Resplot* rnpix_pt_rec = 0;
  Resplot* rnsct_pt_rec = 0;
  Resplot* rntrt_pt_rec = 0;

  Resplot* rnpixh_pt_rec = 0;
  Resplot* rnscth_pt_rec = 0;

  std::vector<Resplot*> mres;

  std::vector<Resplot*> retares;
  std::vector<Resplot*> rphires;
  std::vector<Resplot*> rzedres;
  std::vector<Resplot*> rzedthetares;
  std::vector<Resplot*> riptres;
  std::vector<Resplot*> rptres;
  std::vector<Resplot*> rd0res;
  std::vector<Resplot*> rDd0res;
  std::vector<Resplot*> rDa0res;
  std::vector<Resplot*> rDz0res;

  Resplot* rzedreslb;

  //  std::vector<Resplot*> rd0res_95;
  //  std::vector<Resplot*> rd0res_rms;

  std::vector<Resplot*> retaresPull;
  std::vector<Resplot*> rphiresPull;
  std::vector<Resplot*> rzedresPull;
  std::vector<Resplot*> riptresPull;
  std::vector<Resplot*> rptresPull;
  std::vector<Resplot*> rd0resPull;

  Resplot* mdeltaR_v_eta = 0;
  Resplot* mdeltaR_v_pt = 0;

  TH1F*  hphivsDd0res[3];
  TH1F*  hphivsDa0res[3];

  Efficiency* eff_vs_lb = 0;

  Resplot* z_vs_lb = 0;

  std::map<int, int> rmap;

  //TH1F* hchi2;

  Efficiency* eff_vs_mult = 0;

  TH1F* n_vtx_tracks = 0;
  Efficiency* eff_vs_ntracks = 0;
  Efficiency* eff_vs_ntracks2 = 0;

  TH1F* n_vtx = 0;
  Efficiency* eff_vs_nvtx = 0;
  TH1F* mu = 0;
  Efficiency* eff_vs_mu = 0;

  /// beam spot dependent

  Resplot* rd0_vs_phi = 0;
  Resplot* rd0_vs_phi_rec = 0;

  /// Residuals

  Resplot* rRoi_deta_vs_eta = 0;
  Resplot* rRoi_dphi_vs_eta = 0;
  Resplot* rRoi_dzed_vs_eta = 0;

  /// electron specific ET/PT related stuff
  TH1F* m_etovpt_raw = 0;
  TH1F* m_etovpt = 0;
  Efficiency* m_eff_vs_etovpt = 0;

  TH1F* m_et = 0;
  Efficiency* m_eff_vs_et = 0;

  /// flag to print out the matched tracks etc
  bool m_print;

  const TIDARoiDescriptor* m_roi = 0;

  bool m_initialised;
  bool m_initialiseFirstEvent;

  Resplot* m_rnsct_vs_npix = 0;
  Resplot* m_rnsct_vs_npix_rec = 0;
};



inline std::ostream& operator<<( std::ostream& s, const ConfAnalysis& _s ) { 
  return s << _s.name();
}





#endif  // ANALYSIS_CONFANALYSIS_H 










