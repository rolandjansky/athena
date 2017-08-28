#ifndef PFTRACKSELECTOR_H
#define PFTRACKSELECTOR_H

/** Algorithm which will select tracks for downstream algorithms to use within eflowRec */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"

#include "eflowRec/eflowRecTrack.h"
#include "Particle/TrackParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

class eflowTrackExtrapolatorBaseAlgTool;
namespace InDet { class IInDetTrackSelectionTool; }

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

  /** ReadHandle for the TrackParticleContainer to be used as input */
  SG::ReadHandle<xAOD::TrackParticleContainer> m_tracksReadHandle;

  /** ReadHandle for the ElectronContainer to be used as input */
  SG::ReadHandle<xAOD::ElectronContainer> m_electronsReadHandle;

  /** ReadHandle for the MuonContainer to be used as input */
  SG::ReadHandle<xAOD::MuonContainer> m_muonsReadHandle;

  /** WriteHandle for the eflowRecTrackContainer to write out: */
  SG::WriteHandle<eflowRecTrackContainer> m_eflowRecTracksWriteHandle;

  /** Handle to interface on TrackToCalo tool. */
  ToolHandle<eflowTrackExtrapolatorBaseAlgTool> m_theTrackExtrapolatorTool;

  /** New track selection tool */
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectorTool;

  /** Upper limit on track Pt for input tracks */
  float m_upperTrackPtCut;

  
};
#endif
