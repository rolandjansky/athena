#ifndef PFTRACKSELECTOR_H
#define PFTRACKSELECTOR_H

/** Algorithm which will select tracks for downstream algorithms to use within eflowRec */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"

#include "eflowRec/eflowRecTrack.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "Particle/TrackParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

class eflowTrackExtrapolatorBaseAlgTool;

class PFTrackSelector : public AthAlgorithm {

public:
  /** Default constructor */
  PFTrackSelector(const std::string& name, ISvcLocator* pSvcLocator);
  /** Default destructor */
  ~PFTrackSelector() {};

  /** Gaudi AthAlgorithm hooks */
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  /** This applys a selection criteria to the track using the tracking CP track selection tool */
  bool selectTrack(const xAOD::TrackParticle& track);

  /** check if track belongs to an electron */
  bool isElectron(const xAOD::TrackParticle* track);

  /** check if track belongs to an muon */
  bool isMuon(const xAOD::TrackParticle* track);

  /** ReadHandleKey for the TrackParticleContainer to be used as input */
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksReadHandleKey{this,"tracksName","InDetTrackParticles","ReadHandleKey for the TrackParticleContainer to be used as input"};

  /** ReadHandleKey for the ElectronContainer to be used as input */
  SG::ReadHandleKey<xAOD::ElectronContainer> m_electronsReadHandleKey{this,"electronsName","eflowRec_selectedElectrons","ReadHandleKey for the ElectronContainer to be used as input"};

  /** ReadHandleKey for the MuonContainer to be used as input */
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonsReadHandleKey{this,"muonsName","eflowRec_selectedMuons","ReadHandleKey for the MuonContainer to be used as input"};

  /** WriteHandleKey for the eflowRecTrackContainer to write out */
  SG::WriteHandleKey<eflowRecTrackContainer> m_eflowRecTracksWriteHandleKey{this,"eflowRecTracksOutputName","eflowRecTracks","WriteHandleKey for the eflowRecTrackContainer to write out"};

  /** ToolHandle for track extrapolation to calorimeter tool */
  ToolHandle<eflowTrackExtrapolatorBaseAlgTool> m_theTrackExtrapolatorTool{this,"trackExtrapolatorTool","Trk::ParticleCaloExtensionTool","ToolHandle for track extrapolation to calorimeter tool"};

  /** ToolHandle to track selection tool provided by tracking CP */
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectorTool{this,"trackSelectionTool","","ToolHandle to track selection tool provided by tracking CP"};

  /** Upper limit on track Pt for input tracks */
  Gaudi::Property<float> m_upperTrackPtCut{this,"upperTrackPtCut",100.0,"Upper limit on track Pt for input tracks"};

};
#endif
