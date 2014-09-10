/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_PERIGEEFILLERTOOL_H
#define TRACKD3PDMAKER_PERIGEEFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrkParameters/TrackParameters.h"

namespace D3PD {

class PerigeeFillerTool
  : public D3PD::BlockFillerTool<Trk::TrackParameters>
{
public:
  PerigeeFillerTool (const std::string& type,
		     const std::string& name,
		     const IInterface* parent);

  virtual StatusCode book();

  virtual StatusCode fill (const Trk::TrackParameters &p);

private:

  bool m_fillMomentum;
  bool m_fillPerigee;
  bool m_fillThetaAndQoverP;

  /// Parameter: Should the phi variable be filled?
  bool m_fillPhi;

  /// Parameter: Value to which to set variables if they don't get filled.
  float m_defaultValue;

  /* NTuple variables: **/
  // track parameters at perigee
  float *m_d0;
  float *m_z0;
  float *m_phi;
  float *m_theta;
  float *m_qoverp;

  // momentum
  float *m_pt;
  float *m_eta;

}; // class PerigeeFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_PERIGEEFILLERTOOL_H
