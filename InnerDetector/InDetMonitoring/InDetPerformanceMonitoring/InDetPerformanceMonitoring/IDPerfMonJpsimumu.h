#ifndef IDPERFMON_ZMUMU_H
#define IDPERFMON_ZMUMU_H

//==============================================================================
// Include files...
//==============================================================================
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetPerformanceMonitoring/JpsimumuEvent.h"
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

#include <map>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ToolHandle.h"


class IegammaTrkRefitterTool;


class IDPerfMonJpsimumu : public AthAlgorithm
{
 public:
  // Constructors & destructors

  IDPerfMonJpsimumu(const std::string& name, ISvcLocator* pSvcLocator);

  ~IDPerfMonJpsimumu();

  // Overriden class functions.
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 protected:

 private:
  // Private class member functions.
  void RegisterHistograms();
  void FillRecParameters(const Trk::Track* track, double charge);
    //void FillRecParameters(const xAOD::TrackParticle* trackparticle, double charge);
  StatusCode FillTruthParameters(const xAOD::TrackParticle* track);
  const xAOD::TruthParticle* getTruthParticle( const xAOD::IParticle& p );

  // The Z0 tagger.
  JpsimumuEvent     m_xZmm;
  bool m_doIsoSelection;
  bool m_isMC;
  std::vector<std::string> m_regions;

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter1;

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter2;

  /** @brief tool to extrapolate tracks to BL*/
  ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;

  //Validation Ntuple Stuff
  //!< boolean to switch to validation mode
  bool                            m_validationMode;

  //!< validation tree name - to be acessed by this from root
  std::string                     m_defaultTreeName;       //Default ID Tracks
  std::string                     m_refit1TreeName;        //Refit ID Tracks
  std::string                     m_refit2TreeName;        //Refit ID Tracks
  std::string                     m_truthTreeName;         //Truth Tracks
  //  std::string                     m_meStacoTreeName;       //Extrapolated Staco not existent in xAOD anymore
  std::string                     m_combStacoTreeName;     //Combined Staco
  std::string                     m_combMuidTreeName;      //Combined Muid
  //!< validation tree description - second argument in TTree
  std::string                     m_ValidationTreeDescription;
  //!< stream/folder to for the TTree to be written out
  std::string                     m_defaultTreeFolder;
  std::string                     m_refit1TreeFolder;
  std::string                     m_refit2TreeFolder;
  std::string                     m_truthTreeFolder;
  //  std::string                     m_meStacoTreeFolder; // not existent in xAOD anymore
  std::string                     m_combStacoTreeFolder;
  std::string                     m_combMuidTreeFolder;

  std::string m_truthName;          /// Track(Particle)TruthCollection input name
  std::string m_trackParticleName;  /// TrackParticle input name
  std::string m_truthLinkVecName;   /// link vector to map HepMC onto xAOD truth

  //!< Root Validation Tree
  TTree*                          m_defaultTree;
  TTree*                          m_refit1Tree;
  TTree*                          m_refit2Tree;
  TTree*                          m_truthTree;
  TTree*                          m_meStacoTree;
  TTree*                          m_combStacoTree;
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


  double m_negative_px;
  double m_negative_py;
  double m_negative_pz;
  double m_negative_z0;
  double m_negative_d0;
  double m_negative_z0_err;
  double m_negative_d0_err;

  std::string m_sTriggerChainName;
  std::string m_outputTracksName;

};
//==============================================================================

#endif
