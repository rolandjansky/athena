/* emacs: this is -*- c++ -*- */
/**
 **     @file    ConfVtxAnalysis.h
 **
 **     @author  mark sutton
 **     @date    Sun  9 Aug 2015 00:02:23 CEST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


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
	       const std::vector<TIDA::Vertex*>& vtx1,
	       const TIDA::Event* tevt=0 );
  
  void finalise();

private:

  bool m_initialised;
  bool m_finalised;

  TIDDirectory* mdir;

  TH1F*    hnvtx;
  TH1F*    hzed;
  TH1F*    hntrax;
  TH1F*    hmu;
  TH1F*    hlb;

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
  Efficiency* eff_lb;

  Resplot* rdx_vs_lb;
  Resplot* rdy_vs_lb;
  Resplot* rdz_vs_lb;
 
  //  Contour<Efficiency>* eff_zed_vs_ntrax;

};


inline std::ostream& operator<<( std::ostream& s, const ConfVtxAnalysis&  ) { 
  return s;
}


#endif  // CONFVTXANALYSIS_H 










