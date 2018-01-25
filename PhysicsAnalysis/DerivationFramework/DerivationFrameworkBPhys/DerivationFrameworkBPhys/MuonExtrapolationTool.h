/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

//////////////////////////////////////////////////////////////////////////////
// MuonExtrapolationTool
//////////////////////////////////////////////////////////////////////////////
#ifndef MuonExtrapolationTool_H
#define MuonExtrapolationTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/Muon.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk { 
  class IExtrapolator; 
}

namespace DerivationFramework {
  class MuonExtrapolationTool : public AthAlgTool, public IAugmentationTool {
    
  public:  
    MuonExtrapolationTool(const std::string& t, const std::string& n, const IInterface *p);
  
    virtual StatusCode initialize();
    virtual StatusCode addBranches() const;

    ToolHandle<Trk::IExtrapolator> m_extrapolator;
  private:

    /// run the extrapolation - only available in full athena
    const Trk::TrackParameters* extrapolateToTriggerPivotPlane(const xAOD::TrackParticle& track) const;

  // Utility method to handle extrapolation and decoration for one TrackParticle. 
  // It looks for the decoration, and, if it is missing, runs track extrapolation, decorating the result
  // to the particle to avoid repeating the process unnecessarily. 
  // Returns success (true) or failure (false) of the procedure, fills eta and phi coordinates via reference
  // If the extrapolation fails or the decoration is missing in AthAnalysis, it will *not* change eta and phi
  // So you can set them to defaults before calling this guy, and they will be preserved in case of failure. 
  // Will not run outside athena, because it requires the extrapolator
    bool extrapolateAndDecorateTrackParticle(const xAOD::TrackParticle* particle, float & eta, float & phi) const;

    // utility method: Obtains the track particle which we want to extrapolate into the MS. 
    // Works for all kinds of probes. 
    const xAOD::TrackParticle* getPreferredTrackParticle (const xAOD::IParticle* probe) const;

    // these define the surfaces that we extrapolate to. 
    // We approximate the pivot plane in the form of a cylinder surface and two disks
    double m_endcapPivotPlaneZ;
    double m_endcapPivotPlaneMinimumRadius;
    double m_endcapPivotPlaneMaximumRadius;
    double m_barrelPivotPlaneRadius;
    double m_barrelPivotPlaneHalfLength;
    std::string m_muonContainerName;
  };
}
#endif // MuonExtrapolationTool_H
