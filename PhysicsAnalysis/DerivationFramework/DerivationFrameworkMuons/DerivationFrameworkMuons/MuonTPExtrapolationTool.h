//////////////////////////////////////////////////////////////////////////////
// MuonTPExtrapolationTool
//  ASG Tool performing extrapolations
//
//  (c) ATLAS Muon Combined Performance Software
//////////////////////////////////////////////////////////////////////////////
#ifndef MuonTPExtrapolationTool_H
#define MuonTPExtrapolationTool_H

#include "DerivationFrameworkMuons/IMuonTPExtrapolationTool.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

namespace Trk { 
  class IExtrapolator; 
}

class MuonTPExtrapolationTool
: public asg::AsgTool,
  virtual public IMuonTPExtrapolationTool {
  ASG_TOOL_CLASS(MuonTPExtrapolationTool, IMuonTPExtrapolationTool)
    
    public:  
  MuonTPExtrapolationTool(std::string myname);
  virtual ~MuonTPExtrapolationTool();
  
  virtual StatusCode initialize();
  
  /// compute dR on trigger pivot plane
  /// see interface class for full description
  double dROnTriggerPivotPlane(const xAOD::Muon& tag, const xAOD::IParticle* probe);

  // this method is intended for use in the DAOD production. It takes only one particle as argument, and handles the decoration. 
  virtual StatusCode decoratePivotPlaneCoords(const xAOD::IParticle* particle);

#ifndef XAOD_ANALYSIS
  /// run the extrapolation - only available in full athena
  const Trk::TrackParameters* extrapolateToTriggerPivotPlane(const xAOD::TrackParticle& track);
#endif

  // Utility method to handle extrapolation and decoration for one TrackParticle. 
  // In AthAnalysis, it attempts to read the decoration that should be present in the DAODs we use (MUON1/2).
  // In full athena, it looks for the decoration, and, if it is missing, runs track extrapolation, decorating the result
  // to the particle to avoid repeating the process unnecessarily. 
  // Returns success (true) or failure (false) of the procedure, fills eta and phi coordinates via reference
  // If the extrapolation fails or the decoration is missing in AthAnalysis, it will *not* change eta and phi
  // So you can set them to defaults before calling this guy, and they will be preserved in case of failure. 
  bool extrapolateAndDecorateTrackParticle(const xAOD::TrackParticle* particle, float & eta, float & phi);
  
 private:

#ifndef XAOD_ANALYSIS
  ToolHandle<Trk::IExtrapolator> m_extrapolator;
#endif

  // utility method: Obtains the track particle which we want to extrapolate into the MS. 
  // Works for all kinds of probes. 
  const xAOD::TrackParticle* getPreferredTrackParticle (const xAOD::IParticle* probe);

  // these define the surfaces that we extrapolate to. 
  // We approximate the pivot plane in the form of a cylinder surface and two disks
  double m_endcapPivotPlaneZ;
  double m_endcapPivotPlaneMinimumRadius;
  double m_endcapPivotPlaneMaximumRadius;
  double m_barrelPivotPlaneRadius;
  double m_barrelPivotPlaneHalfLength;
  

  };

#endif
