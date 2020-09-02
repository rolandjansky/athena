/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef PFTRACKSELECTOR_H
#define PFTRACKSELECTOR_H

/** Algorithm which will select tracks for downstream algorithms to use within eflowRec */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "eflowRec/eflowRecTrack.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "Particle/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
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

  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexKey { this, "VertexContainer", "PrimaryVertices", "primary vertex container" };

  /** ReadCondHandleKey for Pixel detector elements. This is needed to read ESD and AOD in AthenaMT for P->T conversion of ID tracks. */  
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  /** ReadCondHandleKey for SCT detector elements. This is needed to read ESD and AOD in AthenaMT for P->T conversion of ID tracks. */  
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey{this, "TRTDetEleContKey", "TRT_DetElementContainer", "Key of TRT_DetElementContainer"};

  /** ToolHandle for track extrapolation to calorimeter tool */
  ToolHandle<eflowTrackExtrapolatorBaseAlgTool> m_theTrackExtrapolatorTool{this,"trackExtrapolatorTool","Trk::ParticleCaloExtensionTool","ToolHandle for track extrapolation to calorimeter tool"};

  /** ToolHandle to track selection tool provided by tracking CP */
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectorTool{this,"trackSelectionTool","","ToolHandle to track selection tool provided by tracking CP"};

  /** Upper limit on track Pt for input tracks */
  Gaudi::Property<float> m_upperTrackPtCut{this,"upperTrackPtCut",100.0,"Upper limit on track Pt for input tracks"};

  /** Online monitoring tool for recording histograms of the alg in action */
  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
};
#endif
