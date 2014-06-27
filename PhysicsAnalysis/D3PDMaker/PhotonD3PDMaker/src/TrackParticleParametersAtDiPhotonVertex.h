/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTOND3PDMAKER_TRACKPARTICLEPARAMETERSATDIPHOTONVERTEX_H
#define PHOTOND3PDMAKER_TRACKPARTICLEPARAMETERSATDIPHOTONVERTEX_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "ITrackToVertex/ITrackToVertex.h"

namespace Rec{
  class TrackParticle;
}

namespace D3PD {
  
  class TrackParticleParametersAtDiPhotonVertex
    : public D3PD::BlockFillerTool<Rec::TrackParticle>
  {
  public:
    TrackParticleParametersAtDiPhotonVertex (const std::string& type,
					     const std::string& name,
					     const IInterface* parent);
    virtual StatusCode initialize();
    
    virtual StatusCode book();
    
    virtual StatusCode fill (const Rec::TrackParticle &p);
    
  private:
    
    /// Parameter: Value to which to set variables if they don't get filled.
    float m_defaultValue;
    
    /* NTuple variables: **/
    // track parameters at perigee
    float *m_d0;
    float *m_z0;
    float *m_phi;

    /* Track to vertex extrapolator **/
    ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
    
  }; // class TrackParticleParametersAtDiPhotonVertex
  
} // namespace D3PD

#endif // not PHOTOND3PDMAKER_TRKTRACKPARAMETERSATDIPHOTONVERTEX_H
