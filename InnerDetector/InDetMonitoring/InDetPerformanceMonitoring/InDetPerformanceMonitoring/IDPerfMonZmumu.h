/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDPERFMON_ZMUMU_H
#define IDPERFMON_ZMUMU_H

//==============================================================================
// Include files...
//==============================================================================
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetPerformanceMonitoring/ZmumuEvent.h"
#include "InDetPerformanceMonitoring/FourMuonEvent.h"
#include "InDetPerformanceMonitoring/EventAnalysis.h"
#include "InDetPerformanceMonitoring/MuonSelector.h"

#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "ITrackToVertex/ITrackToVertex.h"

#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaTruthxAODHelpers.h"

#include "TrackVertexAssociationTool/TrackVertexAssociationTool.h"

#include <map>
#include <string>
#include "TH1.h"
#include "TH2.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkExInterfaces/IExtrapolator.h"

class IegammaTrkRefitterTool;
class IBeamCondSvc;

namespace Trig{
  class TrigDecisionTool;
}

namespace Trk{
  class ITrackToVertexIPEstimator;
  class IExtrapolator;
}


namespace InDet {
  class IInDetTrackSelectionTool;
}

namespace CP { class ITrackVertexAssociationTool;}

class IDPerfMonZmumu : public AthAlgorithm
{
 public:
  // Constructors & destructors

  IDPerfMonZmumu(const std::string& name, ISvcLocator* pSvcLocator);

  ~IDPerfMonZmumu();

  // Overriden class functions.
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 protected:

 private:
  // Private class member functions.
  StatusCode          bookTrees ();
  StatusCode          CheckTriggerStatusAndPrescale ();
  void                Clear4MuNtupleVariables (); 
  int                 GetMuonQualityValue(std::string qualityname);
  void                RegisterHistograms ();
  const xAOD::Vertex* GetDiMuonVertex (const xAOD::TrackParticle*,const  xAOD::TrackParticle*);
  StatusCode          FillRecParameters (const Trk::Track* track, const xAOD::TrackParticle* trackp_for_unbias, double charge,const xAOD::Vertex* vertex);
  StatusCode          FillRecParametersTP (const xAOD::TrackParticle* trackp, const xAOD::TrackParticle* trackp_for_unbias,double charge,const xAOD::Vertex* vertex = nullptr);
  StatusCode          FillTruthParameters (const xAOD::TrackParticle* track);
  const xAOD::TruthParticle* getTruthParticle( const xAOD::IParticle& p );
  StatusCode          RunFourLeptonAnalysis ();

  // The Z0 tagger.
  ZmumuEvent     m_xZmm;
  FourMuonEvent  m_4mu;
  bool m_UseTrigger;
  bool m_doIsoSelection;
  bool m_doIPSelection;
  bool m_doMCPSelection;
  double m_MassWindowLow;
  double m_MassWindowHigh;
  double m_LeadingMuonPtCut;
  double m_SecondMuonPtCut;
  double m_OpeningAngleCut;
  double m_Z0GapCut;
  bool m_isMC;
  bool m_doRefit;
  bool m_useTrackSelectionTool;
  bool m_doIP;
  bool m_doFourMuAnalysis;
  bool m_storeZmumuNtuple;
  std::vector<std::string> m_regions;


  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter1;

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter2;

  /** @brief tool to extrapolate tracks to BL*/
  ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;

  /** @brief The trigger decision tool */
  ToolHandle<Trig::TrigDecisionTool> m_triggerDecision;

  /** @brief The track selection Tool */
  ToolHandle< InDet::IInDetTrackSelectionTool > m_selTool;

  /** Needed for IP resolution studies **/
  ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;

  /** used for truth parameters**/
  ServiceHandle<IBeamCondSvc> m_beamSpotSvc;
  ToolHandle<Trk::IExtrapolator> m_extrapolator;

  /* vertex */
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexKey { this, "VertexContainer", "PrimaryVertices", "primary vertex container" };

  //Validation Ntuple variables
  //!< boolean to switch to validation mode
  bool                            m_validationMode;

  //!< validation tree name - to be acessed by this from root
  std::string                     m_defaultTreeName;       //Default Tracks
  std::string                     m_IDTreeName;            //Default ID Tracks
  std::string                     m_refit1TreeName;        //Refit ID Tracks
  std::string                     m_refit2TreeName;        //Refit ID Tracks
  std::string                     m_truthTreeName;         //Truth Tracks
  std::string                     m_combTreeName;          //Combined Staco
  std::string                     m_MSTreeName;            //MS tracks
  std::string                     m_FourMuTreeName;        //Four lepton tree

  //!< validation tree description - second argument in TTree
  std::string                     m_ValidationTreeDescription;
  //!< stream/folder to for the TTree to be written out
  std::string                     m_defaultTreeFolder;
  std::string                     m_IDTreeFolder;
  std::string                     m_refit1TreeFolder;
  std::string                     m_refit2TreeFolder;
  std::string                     m_truthTreeFolder;
  std::string                     m_combTreeFolder;
  std::string                     m_MSTreeFolder;
  std::string                     m_FourMuTreeFolder;

  std::string m_truthName;          /// Track(Particle)TruthCollection input name
  std::string m_trackParticleName;  /// TrackParticle input name
  std::string m_truthLinkVecName;   /// link vector to map HepMC onto xAOD truth

  // cut flow histogram
  TH1F*                           m_h_cutflow;
  //!< Root Validation Tree
  TTree*                          m_defaultTree;
  TTree*                          m_IDTree;
  TTree*                          m_refit1Tree;
  TTree*                          m_refit2Tree;
  TTree*                          m_truthTree;
  TTree*                          m_combTree;
  TTree*                          m_MSTree;
  TTree*                          m_FourMuTree;

  unsigned int  m_runNumber{};
  unsigned int  m_evtNumber{};
  unsigned int  m_lumi_block{};
  unsigned int  m_event_mu{};
  int           m_triggerPrescale;
  unsigned int  m_nVertex{};

  double m_positive_px{};
  double m_positive_py{};
  double m_positive_pz{};
  double m_positive_z0{};
  double m_positive_d0{};
  double m_positive_z0_err{};
  double m_positive_d0_err{};
  double m_positive_z0_PV{};
  double m_positive_d0_PV{};
  double m_positive_z0_PVerr{};
  double m_positive_d0_PVerr{};
  int m_positive_1_vtx{};

  double m_positive_2_px{};
  double m_positive_2_py{};
  double m_positive_2_pz{};
  double m_positive_2_z0{};
  double m_positive_2_d0{};
  double m_positive_2_z0_err{};
  double m_positive_2_d0_err{};
  double m_positive_2_z0_PV{};
  double m_positive_2_d0_PV{};
  double m_positive_2_z0_PVerr{};
  double m_positive_2_d0_PVerr{};
  int m_positive_2_vtx{};

  double m_negative_px{};
  double m_negative_py{};
  double m_negative_pz{};
  double m_negative_z0{};
  double m_negative_d0{};
  double m_negative_z0_err{};
  double m_negative_d0_err{};
  double m_negative_z0_PV{};
  double m_negative_d0_PV{};
  double m_negative_z0_PVerr{};
  double m_negative_d0_PVerr{};
  int m_negative_1_vtx{};

  double m_negative_2_px{};
  double m_negative_2_py{};
  double m_negative_2_pz{};
  double m_negative_2_z0{};
  double m_negative_2_d0{};
  double m_negative_2_z0_err{};
  double m_negative_2_d0_err{};
  double m_negative_2_z0_PV{};
  double m_negative_2_d0_PV{};
  double m_negative_2_z0_PVerr{};
  double m_negative_2_d0_PVerr{};
  int m_negative_2_vtx{};

  // electrons in four leptons analysis
  double m_el_negative1_px{};
  double m_el_negative1_py{};
  double m_el_negative1_pz{};
  double m_el_negative1_z0{};
  double m_el_negative1_d0{};
  double m_el_negative1_z0_err{};
  double m_el_negative1_d0_err{};
  double m_el_negative1_z0_PV{};
  double m_el_negative1_d0_PV{};
  double m_el_negative1_z0_PVerr{};
  double m_el_negative1_d0_PVerr{};
  int    m_el_negative1_vtx{};

  double m_el_negative2_px{};
  double m_el_negative2_py{};
  double m_el_negative2_pz{};
  double m_el_negative2_z0{};
  double m_el_negative2_d0{};
  double m_el_negative2_z0_err{};
  double m_el_negative2_d0_err{};
  double m_el_negative2_z0_PV{};
  double m_el_negative2_d0_PV{};
  double m_el_negative2_z0_PVerr{};
  double m_el_negative2_d0_PVerr{};
  int    m_el_negative2_vtx{};

  double m_el_positive1_px{};
  double m_el_positive1_py{};
  double m_el_positive1_pz{};
  double m_el_positive1_z0{};
  double m_el_positive1_d0{};
  double m_el_positive1_z0_err{};
  double m_el_positive1_d0_err{};
  double m_el_positive1_z0_PV{};
  double m_el_positive1_d0_PV{};
  double m_el_positive1_z0_PVerr{};
  double m_el_psoitive1_d0_PVerr{};
  int    m_el_positive1_vtx{};

  double m_el_positive2_px{};
  double m_el_positive2_py{};
  double m_el_positive2_pz{};
  double m_el_positive2_z0{};
  double m_el_positive2_d0{};
  double m_el_positive2_z0_err{};
  double m_el_positive2_d0_err{};
  double m_el_positive2_z0_PV{};
  double m_el_positive2_d0_PV{};
  double m_el_positive2_z0_PVerr{};
  double m_el_psoitive2_d0_PVerr{};
  int    m_el_positive2_vtx{};

  //
  double m_4mu_minv{};

  double m_pv_x{};
  double m_pv_y{};
  double m_pv_z{};
  unsigned int m_nTrkInVtx{};

  double m_met{};
  double m_metphi{};
  
  std::string m_sTriggerChainName;
  std::string m_outputTracksName;
  bool m_doRemoval{};
  bool m_doDebug{};
  std::string m_MuonQualityName;

  //std::unique_ptr<CP::TrackVertexAssociationTool> m_Trk2VtxAssociationTool;
  ToolHandle< CP::ITrackVertexAssociationTool > m_Trk2VtxAssociationTool;
};
//==============================================================================

#endif
