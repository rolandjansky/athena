/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGVERTEXFITTER_TRIGPRIMARYVERTEXFITTER_H
#define TRIGVERTEXFITTER_TRIGPRIMARYVERTEXFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigInDetToolInterfaces/ITrigPrimaryVertexFitter.h"

class TrigTimer;

class TrigPrimaryVertexFitter: public AthAlgTool, virtual public ITrigPrimaryVertexFitter
{
 public:
  TrigPrimaryVertexFitter( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigPrimaryVertexFitter();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual TrigVertex* fit(const TrackCollection*, TrackCollection&, double z=0.0) const;
  virtual TrigVertex* fit(const TrackCollection*, std::vector<double>&, double z=0.0) const;
private:
  int m_numIter;
  double m_zVariance,m_maxChi2Increase, m_chi2cut, m_chi2cut_loose;
  bool m_createTrackLists;
  double m_xyVariance;

#define TRIGVERTEXFIT_NTIMERS 5
  TrigTimer* m_timer[TRIGVERTEXFIT_NTIMERS];
  bool m_timers;

};

#endif
