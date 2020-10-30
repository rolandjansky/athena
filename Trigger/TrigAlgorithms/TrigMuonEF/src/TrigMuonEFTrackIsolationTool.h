/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGMUONEF_TRIGMUONEFTRACKISOLATIONTOOL_H__
#define _TRIGMUONEF_TRIGMUONEFTRACKISOLATIONTOOL_H__

#include "TrigMuonToolInterfaces/IMuonEFTrackIsolationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

/**
 *
 * EF Track Isolation tool.
 *
 * Calculates track isolation around EF muon in varying cone sizes.
 * Has possibility to cut on dz(muon id trk, id trk) to suppress pileup.
 *
 */
class TrigMuonEFTrackIsolationTool : public AthAlgTool, virtual public IMuonEFTrackIsolationTool {

 public:

  /// Constructor
  TrigMuonEFTrackIsolationTool(const std::string& type, const std::string& name, const IInterface* parent);

  /// initialize the tool
  virtual StatusCode initialize();

  /// finalize the tool
  virtual StatusCode finalize();

  // Do the isolation calculation for an EF muon  
  StatusCode calcTrackIsolation(const xAOD::Muon* efmuon,           const xAOD::TrackParticleContainer* idtrks, std::vector<double> conesizes, std::vector<double>& results, std::vector<double>* dzvals, std::vector<double>* drvals, std::vector<double>* selfremoval) const;

 private:

  bool m_debug;
  
  /// cut between z of muon id trk and z of id trks - < 0 means no cut
  double m_deltaz_cut;

  /// flag to determine whether to remove pT of muon track from the calculation
  bool m_removeSelf;

  /// flag to determine if we should use an inner annulus
  bool m_useAnnulus;

  /// size of the annulus within which we ignore tracks
  double m_annulusSize;

  /// flag to determine if we want to use offline isolation variables
  bool m_useVarIso;

  /// Track selection tool
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelTool;

  StatusCode checkIsolation(const xAOD::IParticle* muon, double selfpt, const xAOD::TrackParticle* muon_idtrk, const Trk::Perigee* muidtrk_perigee, const xAOD::TrackParticleContainer* trks, std::vector<double> conesizes, std::vector<double>& results, std::vector<double>* dzvals, std::vector<double>* drvals, std::vector<double>* selfremoval) const;
  

};//class TrigMuonEFTrackIsolationTool


#endif //_TRIGMUONEF_TRIGMUONEFTRACKISOLATIONTOOL_H__
