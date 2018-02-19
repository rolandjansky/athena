// IMuonTPExtrapolationTool.h
#ifndef IMuonTPExtrapolationTool_H
#define IMuonTPExtrapolationTool_H

#include "AsgTools/IAsgTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/Muon.h"

#ifndef XAOD_ANALYSIS
#include "TrkParameters/TrackParameters.h"
#endif


class IMuonTPExtrapolationTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMuonTPExtrapolationTool)

    public:
  
  /// compute dR on trigger pivot plane
  /// This also applies decorations for the eta and phi coordinates in said plane to the tag and the probe track particles 
  /// If we are processing a DAOD in AthAnalysis, it attempts to read the decorations that should be provided by the derivation
  /// In full athena, it will check for the decoration, and run extrapolation if it is not (yet) in place
  virtual double dROnTriggerPivotPlane(const xAOD::Muon& tag, const xAOD::IParticle* probe)=0;

  // this method is intended for use in the DAOD production. It takes only one particle as argument, and handles the decoration. 
  virtual StatusCode decoratePivotPlaneCoords(const xAOD::IParticle* particle)=0;
  };

#endif
