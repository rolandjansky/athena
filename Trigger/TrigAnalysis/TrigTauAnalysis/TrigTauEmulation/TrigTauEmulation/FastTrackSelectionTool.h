/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTTRACKSELECTIONTOOL_FASTTRACKSELECTIONTOOL_H
#define FASTTRACKSELECTIONTOOL_FASTTRACKSELECTIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

#include "TrigTauEmulation/IFastTrackSelectionTool.h"

class FastTrackSelectionTool : public virtual IFastTrackSelectionTool, public asg::AsgTool

{


  /// Proper constructor for Athena
  ASG_TOOL_CLASS(FastTrackSelectionTool, IFastTrackSelectionTool)

 public:
  
  FastTrackSelectionTool(const std::string & name);
  FastTrackSelectionTool(const FastTrackSelectionTool & other);
  FastTrackSelectionTool& operator=(const FastTrackSelectionTool & other) = delete;

  virtual ~FastTrackSelectionTool() {};

  virtual StatusCode initialize();

  virtual const Root::TAccept& accept(const xAOD::TauJet * hlttau, const DataVector<xAOD::TrackParticle> *preselTracksIso, const DataVector<xAOD::TrackParticle> *preselTracksCore) const;
  virtual const Root::TAccept& accept(const xAOD::TauJet * presel_tau) const;

 private:
 
  mutable Root::TAccept m_accept;

  // ftk
  double m_d0;
  double m_z0;
  double m_delta_z0;
  double m_core;
  double m_iso;
  int m_ncore_bound;
  int m_niso_bound;

 protected:
  bool trackSelection(const xAOD::TauJet *hlt_tau, const xAOD::TrackParticle *leadingTrack, const DataVector<xAOD::TrackParticle> *tracks) const;
  const xAOD::TrackParticle* findLeadingTrack(const xAOD::TauJet*, const DataVector<xAOD::TrackParticle>*) const;
  const xAOD::TrackParticle* findCoreLeadingTrack(const DataVector<xAOD::TrackParticle>* tracks) const;
  const xAOD::TrackParticle* findIsoLeadingTrack(const xAOD::TauJet* tau, const DataVector<xAOD::TrackParticle>* tracks) const;




};
#endif
