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

  virtual ~VtxAnalysis() { if ( m_dir ) delete m_dir; } 

  void initialise();

  void execute(const std::vector<TIDA::Vertex*>& vtx0,
	       const std::vector<TIDA::Vertex*>& vtx1, 
	       const TIDA::Event* te=0);
  
  void finalise();

private:

  bool m_initialised;

  TIDDirectory* m_dir;

  TH1F*    m_hnvtx;
  TH1F*    m_hzed;
  TH1F*    m_hntrax;

  TH1F*    m_hnvtx_rec;
  TH1F*    m_hzed_rec;
  TH1F*    m_hntrax_rec;

  TH1F*    m_hzed_res;

  TProfile* m_rdz_vs_zed;
  TProfile* m_rdz_vs_ntrax;
  TProfile* m_rdz_vs_nvtx;
  TProfile* m_rdz_vs_mu;

  TProfile* m_eff_zed;
  TProfile* m_eff_ntrax;
  TProfile* m_eff_nvtx;
  TProfile* m_eff_mu;
  TProfile* m_eff_lb;
 
};


inline std::ostream& operator<<( std::ostream& s, const VtxAnalysis&  ) { 
  return s;
}


#endif  // VTXANALYSIS_H 










