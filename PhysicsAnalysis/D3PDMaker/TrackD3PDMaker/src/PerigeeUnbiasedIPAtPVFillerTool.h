// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_PERIGEEUNBIASEDIPATPVFILLERTOOL_H
#define TRACKD3PDMAKER_PERIGEEUNBIASEDIPATPVFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrackD3PDMaker/PerigeePair.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk{
class VxCandidate;
}

namespace D3PD {

//
// Can take as input either a single Perigee, or a pair of Perigee objects,
// before and after refitting.
//
class PerigeeUnbiasedIPAtPVFillerTool
  : public D3PD::BlockFillerTool<D3PD::Types<Trk::Perigee, D3PD::PerigeePair> >
{
public:
  typedef D3PD::BlockFillerTool<D3PD::Types<Trk::Perigee, D3PD::PerigeePair> >
    Base;

  PerigeeUnbiasedIPAtPVFillerTool (const std::string& type,
		     const std::string& name,
		     const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const Trk::Perigee& perigee);
  virtual StatusCode fill (const D3PD::PerigeePair& ppair);

private:
  /* StoreGate keys **/
  std::string m_vxCandidate;

  /* Track to vertex extrapolation tools **/
  ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;

  std::string m_errPrefix;
  std::string m_errPVPrefix;
  std::string m_biasedName;
  std::string m_unbiasedName;
  float m_defaultValue;
  bool m_doBiased;
  bool m_doUnbiased;

  /* NTuple variables: **/
  struct Vars {
    float *m_d0;
    float *m_z0;
    float *m_err_d0;
    float *m_err_z0;
    float *m_errPV_d0;
    float *m_errPV_z0;
  };
  Vars m_biased;
  Vars m_unbiased;

  StatusCode bookVars (Vars& vars, const std::string nameFrag);
  StatusCode fillVars (Vars& vars,
                       const D3PD::PerigeePair& perigee,
                       const Trk::VxCandidate* vx,
                       bool unbiased);

}; // class PerigeeUnbiasedIPAtPVFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_PERIGEEUNBIASEDIPATPVFILLERTOOL_H
