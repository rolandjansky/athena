// emacs: this is -*- c++ -*-
//
//   @file    ConfAnalysis.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: ConfAnalysis.h 800361 2017-03-12 14:33:19Z sutt $


#ifndef __CONFANALYSIS_H
#define __CONFANALYSIS_H

#include <iostream>
#include <vector>
#include <map>

#include "TrigInDetAnalysis/TrackAnalysis.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysis/Efficiency.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"

#include "TrigInDetAnalysisExample/ChainString.h"


#include "Resplot.h"


#include "TH1F.h"
#include "TH2F.h"
// #include "Reflex/Reflex.h"

// using namespace ROOT::Reflex;


inline const std::string clean( std::string s) { 
  while (true ){ 
    if ( s.find(":")==std::string::npos && 
	 s.find(";")==std::string::npos ) return s;
    if ( s.find(":")!=std::string::npos ) s.replace( s.find(":"), 1, "_" );
    if ( s.find(";")!=std::string::npos ) s.replace( s.find(";"), 1, "_" );
  }
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
                       TrackAssociator* matcher );

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

  Efficiency* eff_pt;
  Efficiency* eff_ptp;
  Efficiency* eff_ptm;

  Efficiency* eff_eta;
  Efficiency* eff_phi;
  Efficiency* eff_z0;
  Efficiency* eff_d0;
  Efficiency* eff_a0;

  Efficiency* eff_roi_deta;
  Efficiency* eff_roi_dphi;
  Efficiency* eff_roi_dR;

  Efficiency* purity_pt;
  Efficiency* purity_eta;
  Efficiency* purity_phi;
  Efficiency* purity_z0;
  Efficiency* purity_d0;
  Efficiency* purity_a0;


#if 0
  TH2F* h2;
  TH2F* h2m;
  TH2F* h2r;
#endif


  Resplot* h2;
  Resplot* h2m;
  Resplot* h2r;

  Resplot* h2a0;
  Resplot* h2a0r;

  Resplot* rChi2prob;
  Resplot* rChi2;
  Resplot* rChi2dof;


  Resplot* rChi2prob_bad;
  Resplot* rChi2_bad;
  Resplot* rChi2dof_bad;

  //  TH2F* hnpix_v_sct;
  //  TH2F* hnpix_v_sct_rec;
  //  TH2F* hnpix_v_sct_match;

  TH1F* hDeltaR;

  /// number of reconstructed tracks 
  int Nreco;
  int Nref;
  int Nmatched;

  
  Resplot* rnpix_eta;
  Resplot* rnsct_eta;
  Resplot* rntrt_eta;
  Resplot* rnsihit_eta;
  Resplot* rnpix_lb;


  Resplot* rnpix_phi;
  Resplot* rnsct_phi;
  Resplot* rntrt_phi;

  Resplot* rnpix_pt;
  Resplot* rnsct_pt;
  Resplot* rntrt_pt;

  Resplot* rnpix_d0; // new
  Resplot* rnsct_d0; // new
  Resplot* rntrt_d0; // new

  Resplot* rnpixh_pt;
  Resplot* rnscth_pt;

  Resplot* rnpixh_d0; // new
  Resplot* rnscth_d0; // new

  Resplot* rnsi_pt;  // new
  Resplot* rnsih_pt; // new

  Resplot* rnsi_eta;  // new
  Resplot* rnsih_eta; // new

  Resplot* rnsi_d0;  // new
  Resplot* rnsih_d0; // new

  Resplot* rnbl_d0;  // new
  Resplot* rnblh_d0; // new

  Resplot* rnpix_pt_bad;
  Resplot* rnsct_pt_bad;
  Resplot* rntrt_pt_bad;

  Resplot* rnpix_eta_rec;
  Resplot* rnsct_eta_rec;
  Resplot* rntrt_eta_rec;
  Resplot* rnsihit_eta_rec;

  Resplot* rnpix_phi_rec;
  Resplot* rnsct_phi_rec;
  Resplot* rntrt_phi_rec;

  Resplot* rnpix_pt_rec;
  Resplot* rnsct_pt_rec;
  Resplot* rntrt_pt_rec;

  Resplot* rnpixh_pt_rec;
  Resplot* rnscth_pt_rec;

  std::vector<Resplot*> mres;

  std::vector<Resplot*> retares;
  std::vector<Resplot*> rphires;
  std::vector<Resplot*> rzedres;
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
  
  Resplot* mdeltaR_v_eta;
  Resplot* mdeltaR_v_pt;


  TH1F*  hphivsDd0res[3];
  TH1F*  hphivsDa0res[3];


  Efficiency* eff_vs_lb;

  Resplot*      z_vs_lb;

  std::map<int, int> rmap;

  //TH1F* hchi2;

  Efficiency* eff_vs_mult;

  TH1F*       n_vtx_tracks;
  Efficiency* eff_vs_ntracks;
  Efficiency* eff_vs_ntracks2;

  TH1F*       n_vtx;
  Efficiency* eff_vs_nvtx;
  TH1F*       mu;
  Efficiency* eff_vs_mu;

  /// beam spot dependent

  Resplot* rd0_vs_phi;
  Resplot* rd0_vs_phi_rec;

  /// Residuals 
  
  Resplot* rRoi_deta_vs_eta;
  Resplot* rRoi_dphi_vs_eta;
  Resplot* rRoi_dzed_vs_eta;

  /// flag to print out the matched tracks etc
  bool m_print;

  const TIDARoiDescriptor* m_roi;

  bool m_initialised;
  bool m_initialiseFirstEvent;

  //  bool m_lfirst;

};



inline std::ostream& operator<<( std::ostream& s, const ConfAnalysis& _s ) { 
  return s << _s.name();
}





#endif  // __CONFANALYSIS_H 










