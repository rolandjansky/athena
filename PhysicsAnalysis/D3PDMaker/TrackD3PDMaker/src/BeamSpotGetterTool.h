/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_BEAMSPOTGETTERTOOL_H
#define TRACKD3PDMAKER_BEAMSPOTGETTERTOOL_H

#include "D3PDMakerUtils/ObjGetterTool.h"
#include "VxVertex/RecVertex.h"

#include "GaudiKernel/ToolHandle.h"

class IBeamCondSvc;

namespace D3PD {

class BeamSpotGetterTool : public D3PD::ObjGetterTool<Trk::RecVertex> {
public:
  typedef D3PD::ObjGetterTool<Trk::RecVertex> Base;

  BeamSpotGetterTool(const std::string& type,
		     const std::string& name,
		     const IInterface* parent);
  virtual ~BeamSpotGetterTool();

  StatusCode initialize();

  /**
     get method:
      use the beam spot service to extract beam spot parameters
      wrap the info in a Trk::RecVertex object (position and covariance matrix
  */
  virtual const Trk::RecVertex* get (bool allowMissing = false);

  /**
     delete the pointer created by get
   */
  virtual void releaseObject (const Trk::RecVertex* p);

private:

  /* the beam condition service **/
  IBeamCondSvc* m_iBeamCondSvc;
  std::string   m_beamCondSvcName;

}; // class BeamSpotGetterTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_BEAMSPOTGETTERTOOL_H
