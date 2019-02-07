/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IDPERFMON_ZMUMU_H
#define IDPERFMON_ZMUMU_H

//==============================================================================
// Include files...
//==============================================================================
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetPerformanceMonitoring/ZmumuEvent.h"
#include "InDetPerformanceMonitoring/EventAnalysis.h"

//#include "TrkFitterInterfaces/ITrackFitter.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthParticle.h"
//#include "xAODTruth/xAODTruthHelpers.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "ITrackToVertex/ITrackToVertex.h"


#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaTruthxAODHelpers.h"

#include "TrackVertexAssociationTool/LooseTrackVertexAssociationTool.h"

#include <map>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

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
  void RegisterHistograms();
  const xAOD::Vertex* GetDiMuonVertex(const xAOD::TrackParticle*,const  xAOD::TrackParticle*);
  bool FillRecParameters(const Trk::Track* track, const xAOD::TrackParticle* trackp_for_unbias, double charge,const xAOD::Vertex* vertex);
  bool FillRecParametersTP(const xAOD::TrackParticle* trackp, const xAOD::TrackParticle* trackp_for_unbias,double charge,const xAOD::Vertex* vertex = NULL);
  //void FillRecParameters(const xAOD::TrackParticle* trackparticle, double charge);
StatusCode FillTruthParameters(const xAOD::TrackParticle* track);

  const xAOD::TruthParticle* getTruthParticle( const xAOD::IParticle& p );
  StatusCode CheckTriggerStatusAndPrescale ();

  // The Z0 tagger.
  ZmumuEvent     m_xZmm;
  bool m_UseTrigger;
  bool m_doIsoSelection;
  bool m_doIPSelection;
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

  //Validation Ntuple Stuff
  //!< boolean to switch to validation mode
  bool                            m_validationMode;

  //!< validation tree name - to be acessed by this from root
  std::string                     m_defaultTreeName;       //Default Tracks
  std::string                     m_IDTreeName;            //Default ID Tracks
  std::string                     m_refit1TreeName;        //Refit ID Tracks
  std::string                     m_refit2TreeName;        //Refit ID Tracks
  std::string                     m_truthTreeName;         //Truth Tracks
  //  std::string                     m_meStacoTreeName;       //Extrapolated Staco not existent in xAOD anymore
  std::string                     m_combTreeName;     //Combined Staco
  std::string                     m_combMuidTreeName;      //Combined Muid
  //!< validation tree description - second argument in TTree
  std::string                     m_ValidationTreeDescription;
  //!< stream/folder to for the TTree to be written out
  std::string                     m_defaultTreeFolder;
  std::string                     m_IDTreeFolder;
  std::string                     m_refit1TreeFolder;
  std::string                     m_refit2TreeFolder;
  std::string                     m_truthTreeFolder;
  //  std::string                     m_meStacoTreeFolder; // not existent in xAOD anymore
  std::string                     m_combTreeFolder;
  std::string                     m_combMuidTreeFolder;

  std::string m_truthName;          /// Track(Particle)TruthCollection input name
  std::string m_trackParticleName;  /// TrackParticle input name
  std::string m_truthLinkVecName;   /// link vector to map HepMC onto xAOD truth

  //!< Root Validation Tree
  TTree*                          m_defaultTree;
  TTree*                          m_IDTree;
  TTree*                          m_refit1Tree;
  TTree*                          m_refit2Tree;
  TTree*                          m_truthTree;
  TTree*                          m_meStacoTree;
  TTree*                          m_combTree;
  TTree*                          m_combMuidTree;

  mutable unsigned int            m_runNumber;
  mutable unsigned int            m_evtNumber;
  mutable unsigned int            m_lumi_block;

  double m_positive_px;
  double m_positive_py;
  double m_positive_pz;
  double m_positive_z0;
  double m_positive_d0;
  double m_positive_z0_err;
  double m_positive_d0_err;
  double m_positive_z0_PV;
  double m_positive_d0_PV;
  double m_positive_z0_PVerr;
  double m_positive_d0_PVerr;



  double m_negative_px;
  double m_negative_py;
  double m_negative_pz;
  double m_negative_z0;
  double m_negative_d0;
  double m_negative_z0_err;
  double m_negative_d0_err;
  double m_negative_z0_PV;
  double m_negative_d0_PV;
  double m_negative_z0_PVerr;
  double m_negative_d0_PVerr;

  
  int m_triggerPrescale;

  std::string m_sTriggerChainName;
  std::string m_outputTracksName;
  bool m_doRemoval;
  bool m_doDebug;

  std::unique_ptr<CP::LooseTrackVertexAssociationTool> m_LooseT2VAssociationTool;


};
//==============================================================================

#endif
