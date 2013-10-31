/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_PRIM_VERTEX_FITTER__
#define __TRIG_PRIM_VERTEX_FITTER__

#include "GaudiKernel/AlgTool.h"
#include "TrigVertexFitter/ITrigPrimaryVertexFitter.h"

class TrigTimer;

class TrigPrimaryVertexFitter: public AlgTool, virtual public ITrigPrimaryVertexFitter
{
 public:
  TrigPrimaryVertexFitter( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigPrimaryVertexFitter();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual TrigVertex* fit(const TrigInDetTrackCollection*, double z=0.0);
  virtual TrigVertex* fit(const TrigInDetTrackCollection*, std::vector<double>&, double z=0.0);
private:
  bool m_checkTracks(const TrigInDetTrackCollection*);
  int m_numIter;
  double m_zVariance,m_maxChi2Increase, m_chi2cut, m_chi2cut_loose;
  bool m_createTrackLists;
  double m_xyVariance;

#define TRIGVERTEXFIT_NTIMERS 5
  TrigTimer* m_timer[TRIGVERTEXFIT_NTIMERS];
  bool m_timers;

};

#endif
