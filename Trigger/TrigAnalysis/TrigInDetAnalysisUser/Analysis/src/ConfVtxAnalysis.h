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
#include "TrigInDetAnalysis/TIDAVertexNew.h"
#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysis/Efficiency.h"

#include "Resplot.h"

class ConfVtxAnalysis : public VertexAnalysis {

public:

  ConfVtxAnalysis( const std::string& n );

  virtual ~ConfVtxAnalysis() { if ( mdir ) delete mdir; } 

  void initialise();

  void execute(const std::vector<TIDA::Vertex*>& vtx0,
	      const std::vector<TIDA::Vertex*>& vtx1,
	      const TIDA::Event* tevt=0 );

  void execute(const std::vector<TIDA::VertexNew*>& vtx0,
	      const std::vector<TIDA::VertexNew*>& vtx1,
	      const TIDA::Event* tevt=0 );

  void finalise();

private:

  template<typename Vertex, typename Matcher>
  void execute_internal(const std::vector<Vertex*>& vtx0,
	      const std::vector<Vertex*>& vtx1,
              Matcher& m, 
	      const TIDA::Event* tevt=0);

  bool m_initialised;
  bool m_finalised;

  TIDDirectory* mdir;

  TH1F*    hnvtx = 0;
  TH1F*    hzed = 0;
  TH1F*    hntrax = 0;
  TH1F*    hmu = 0;
  TH1F*    hlb = 0;

  TH1F*    hnvtx_rec = 0;
  TH1F*    hzed_rec = 0;
  TH1F*    hntrax_rec = 0;

  TH1F*    hzed_res = 0;

  Resplot* rdz_vs_zed = 0;
  Resplot* rdz_vs_ntrax = 0;
  Resplot* rdz_vs_nvtx = 0;
  Resplot* rdz_vs_mu = 0;

  Resplot* rnvtxrec_nvtx = 0;

  Efficiency* eff_zed = 0;
  Efficiency* eff_ntrax = 0;
  Efficiency* eff_nvtx = 0;
  Efficiency* eff_mu = 0;
  Efficiency* eff_lb = 0;

  Resplot* rdx_vs_lb = 0;
  Resplot* rdy_vs_lb = 0;
  Resplot* rdz_vs_lb = 0;
 
  //  Contour<Efficiency>* eff_zed_vs_ntrax;

};


inline std::ostream& operator<<( std::ostream& s, const ConfVtxAnalysis&  ) { 
  return s;
}


#endif  // CONFVTXANALYSIS_H 










