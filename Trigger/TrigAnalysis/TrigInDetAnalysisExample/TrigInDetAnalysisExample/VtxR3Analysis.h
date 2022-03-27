/* emacs: this is -*- c++ -*- */
/**
 **     @file    VtxR3Analysis.h
 **
 **     @author  mark sutton
 **     @date    Sun  6 Feb 2022 00:02:23 CEST 
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  VTXR3ANALYSIS_H
#define  VTXR3ANALYSIS_H

#include <iostream>
#include <vector>

#include "TrigInDetAnalysis/VertexAnalysis.h"
#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysisExample/TIDAHistogram.h"



class VtxR3Analysis : public VertexAnalysis {

public:

  VtxR3Analysis( const std::string& n );

  virtual ~VtxR3Analysis() { } // if ( mdir ) delete mdir; } 

  void initialise();

  void execute(const std::vector<TIDA::Vertex*>& vtx0,
	       const std::vector<TIDA::Vertex*>& vtx1, 
	       const TIDA::Event* te=0);
  
  void finalise();

  void set_monTool( ToolHandle<GenericMonitoringTool>* m ) { m_monTool=m; }

  ToolHandle<GenericMonitoringTool>* monTool() { return m_monTool; }

private:

  bool m_initialised;

  ToolHandle<GenericMonitoringTool>* m_monTool;

  //  TIDDirectory* mdir;

  TIDA::Histogram<float>*  m_hnvtx;
  TIDA::Histogram<float>*  m_hzed;
  TIDA::Histogram<float>*  m_hntrax;

  TIDA::Histogram<float>*  m_hnvtx_rec;
  TIDA::Histogram<float>*  m_hzed_rec;
  TIDA::Histogram<float>*  m_hntrax_rec;

  TIDA::Histogram<float>*  m_hzed_res;

  TIDA::Histogram<float>*  m_rdz_vs_zed;
  TIDA::Histogram<float>*  m_rdz_vs_ntrax;
  TIDA::Histogram<float>*  m_rdz_vs_nvtx;
  //No currently used 
  //but retained in case
  //TIDA::Histogram<float>* m_rdz_vs_mu;

  TIDA::Histogram<float>* m_eff_zed;
  TIDA::Histogram<float>* m_eff_ntrax;
  TIDA::Histogram<float>* m_eff_nvtx;
  TIDA::Histogram<float>* m_eff_mu;
  TIDA::Histogram<float>* m_eff_lb;
 
};


inline std::ostream& operator<<( std::ostream& s, const VtxR3Analysis&  ) { 
  return s;
}


#endif  // VTXR3ANALYSIS_H 










