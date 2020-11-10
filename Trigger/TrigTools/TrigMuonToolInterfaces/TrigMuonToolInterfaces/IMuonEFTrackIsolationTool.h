/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGMUONEF_IMUONEFTRACKISOLATIONTOOL_H__
#define _TRIGMUONEF_IMUONEFTRACKISOLATIONTOOL_H__

// include base class
#include "GaudiKernel/IAlgTool.h"

// stl
#include <vector>

// xAOD classes
#include "xAODMuon/Muon.h"

// fwd declare classes
class TrigMuonEFInfoTrack;
namespace Rec {
  class TrackParticleContainer;
}

/// Interface ID
static const InterfaceID IID_IMuonEFTrackIsolationTool("IMuonEFTrackIsolationTool", 1, 0);

/**
 * Interface for EF muon track isolation tools.
 */
class IMuonEFTrackIsolationTool : virtual public IAlgTool {

 public:
  
  /// Function where the isolation should be calculated for an L2 muon & FTK tracks (xAOD version)                                                                                       
  virtual StatusCode calcTrackIsolation(const xAOD::Muon* efmuon, const xAOD::TrackParticleContainer* idtrks, const std::vector<double> conesizes, std::vector<double>& results, std::vector<double>* dzvals, std::vector<double>* drvals, std::vector<double>* selfremoval) const =0;

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonEFTrackIsolationTool;}
 
};


#endif //_TRIGMUONEF_IMUONEFTRACKISOLATIONTOOL_H__
