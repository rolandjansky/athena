// emacs: this is -*- c++ -*-
//
//   @file    ConfVtxAnalysis.h        
//
//                   
//  
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: ConfVtxAnalysis.h, v0.0   Sun  9 Aug 2015 00:02:23 CEST sutt $


#ifndef  CONFVTXANALYSIS_H
#define  CONFVTXANALYSIS_H

#include <iostream>

#include "TrigInDetAnalysis/VertexAnalysis.h"
#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysis/Efficiency.h"

#include "Resplot.h"

#include "Resplot.h"
// #include "Contour.h"


class ConfVtxAnalysis : public VertexAnalysis {

public:

  ConfVtxAnalysis( const std::string& n );

  virtual ~ConfVtxAnalysis() { if ( mdir ) delete mdir; } 

  void initialise();

  void execute(const std::vector<TIDA::Vertex*>& vtx0,
	       const std::vector<TIDA::Vertex*>& vtx1 );
  
  void finalise();

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

  Resplot* rdz_vs_zed;
  Resplot* rdz_vs_ntrax;
  Resplot* rdz_vs_nvtx;
  Resplot* rdz_vs_mu;

  Efficiency* eff_zed;
  Efficiency* eff_ntrax;
  Efficiency* eff_nvtx;
  Efficiency* eff_mu;
 
  //  Contour<Efficiency>* eff_zed_vs_ntrax;

};


inline std::ostream& operator<<( std::ostream& s, const ConfVtxAnalysis&  ) { 
  return s;
}


#endif  // CONFVTXANALYSIS_H 










