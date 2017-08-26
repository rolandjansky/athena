// emacs: this is -*- c++ -*-
//
//   @file    PurityAnalysis.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: PurityAnalysis.h 688225 2015-08-07 20:12:06Z sutt $


#ifndef __PURITYANALYSIS_H
#define __PURITYANALYSIS_H

#include <iostream>
#include <vector>
#include <map>

#include "TrigInDetAnalysis/TrackAnalysis.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysis/Efficiency.h"
// #include "TrigInDetAnalysis/Efficiency2D.h"

#include "Resplot.h"


#include "TH1F.h"
#include "TH2F.h"



class PurityAnalysis : public TrackAnalysis { 
  
  //  private:
  //     typedef std::map<std::string, TH1F*> maptype; 

public:
  
  PurityAnalysis( const std::string& name ) : 
    TrackAnalysis( name ), Nreco(0), Nref(0), Nmatched(0), m_print(false) {
    std::cout << "PurityAnalysis::PurityAnalysis() " << TrackAnalysis::name() << std::endl;
  }  

  ~PurityAnalysis() {
    std::cout << "PurityAnalysis::~PurityAnalysis() " << name() << std::endl;
    std::map<std::string, TH1F*>::iterator hitr=m_histos.begin();
    std::map<std::string, TH1F*>::iterator hend=m_histos.end();
    for ( ; hitr!=hend ; hitr++ ) delete hitr->second;     

    Efficiency* heff[4]    = {    eff_pt,    eff_eta,    eff_phi,    eff_z0 };
    Efficiency* hpurity[4] = { purity_pt, purity_eta, purity_phi, purity_z0 };

    for ( int i=4 ; i-- ; ) { delete heff[i]; delete hpurity[i]; } 

    delete eff_ptp;
    delete eff_ptm;

  }  
  
  

  virtual void initialise();

  virtual void execute(const std::vector<TIDA::Track*>& reftracks,
                       const std::vector<TIDA::Track*>& testtracks,
                       TrackAssociator* matcher );

  virtual void finalise();
  
  void setprint(bool p) { m_print=p; }

private:
  
  void addHistogram( TH1F* h ) { 
      std::string name = h->GetName();
      m_histos.insert( std::map<std::string, TH1F*>::value_type( name, h) );
  }
  
  TH1F* find( const std::string& n ) { 
      std::map<std::string, TH1F*>::iterator hmitr = m_histos.find(n);
      if ( hmitr!=m_histos.end() ) return hmitr->second;
      else                         return 0; 
  }

private:


  TIDDirectory* mdir;

  std::map<std::string, TH1F*> m_histos;
 
  Efficiency* eff_pt;
  Efficiency* eff_ptp;
  Efficiency* eff_ptm;

  Efficiency* eff_eta;
  Efficiency* eff_phi;
  Efficiency* eff_z0;
  Efficiency* eff_d0;
  Efficiency* eff_a0;

  Efficiency* purity_pt;
  Efficiency* purity_eta;
  Efficiency* purity_phi;
  Efficiency* purity_z0;
  Efficiency* purity_d0;
  Efficiency* purity_a0;

  //  Efficiency2D* eff_pteta; 
  //  Efficiency2D* eff_etapt; 

  TH1F* hDeltaR;

  /// number of reconstructed tracks 
  int Nreco;
  int Nref;
  int Nmatched;
  
  TH1F* hchi2;

  /// flag to print out the matched tracks etc
  bool m_print;

};



inline std::ostream& operator<<( std::ostream& s, const PurityAnalysis& _s ) { 
  return s << _s.name();
}





#endif  // __PURITYANALYSIS_H 










