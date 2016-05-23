// emacs: this is -*- c++ -*-
//
//   @file    VtxAnalysis.h        
//
//                   
//  
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: VtxAnalysis.h, v0.0   Sun  9 Aug 2015 00:02:23 CEST sutt $


#ifndef  VTXANALYSIS_H
#define  VTXANALYSIS_H

#include <iostream>
// #include <map>
#include <vector>

#include "TrigInDetAnalysis/VertexAnalysis.h"
#include "TrigInDetAnalysis/TIDDirectory.h"

#include "TH1F.h"
#include "TProfile.h"

class VtxAnalysis : public VertexAnalysis {

public:

  VtxAnalysis( const std::string& n );

  virtual ~VtxAnalysis() { if ( mdir ) delete mdir; } 

  void initialise();

  void execute(const std::vector<TIDA::Vertex*>& vtx0,
	       const std::vector<TIDA::Vertex*>& vtx1 );
  
  void finalise();

  void addHistogram( TH1* h ) {
    //    std::string name = h->GetName();
    //   m_histos.insert( std::map<std::string, TObject*>::value_type( name, h) );
    m_histos.push_back( h );
  }


  void addHistogram( TProfile* h ) {
    //    std::string name = h->GetName();
    //   m_histos.insert( std::map<std::string, TObject*>::value_type( name, h) );
    m_profs.push_back( h );
  }

  //  void addHistogram( TProfile* h ) {
  //    std::string name = h->GetName();
  //    m_effhistos.insert( std::map<std::string, TProfile*>::value_type( name, h) );
  //  }

  std::vector<TH1*>       objects() { return m_histos; }
  std::vector<TProfile*>  profs()   { return m_profs; }

private:

  //  std::map<std::string, TObject*>  m_histos;
  std::vector<TH1*>       m_histos;
  std::vector<TProfile*>  m_profs;
  //  std::map<std::string, TProfile*> m_effhistos;

private:

  bool m_initialised;

  TIDDirectory* mdir;

  TH1F*    hnvtx;
  TH1F*    hzed;
  TH1F*    hntrax;

  TH1F*    hnvtx_rec;
  TH1F*    hzed_rec;
  TH1F*    hntrax_rec;

  TH1F*    hzed_res;

  TProfile* rdz_vs_zed;
  TProfile* rdz_vs_ntrax;
  TProfile* rdz_vs_nvtx;
  TProfile* rdz_vs_mu;

  TProfile* eff_zed;
  TProfile* eff_ntrax;
  TProfile* eff_nvtx;
  TProfile* eff_mu;
 
};


inline std::ostream& operator<<( std::ostream& s, const VtxAnalysis&  ) { 
  return s;
}


#endif  // VTXANALYSIS_H 










