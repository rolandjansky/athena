/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDPerfMonEoverP_H
#define IDPerfMonEoverP_H
/**
  @class IDPerfMonEoverP
   Create  a simple ntuple to perform EoverP studies with


  @author A. Morley

*/

/********************************************************************

NAME:     egammaTrackRefitter.h
PACKAGE:

CREATED:  July 2011

********************************************************************/

// INCLUDE HEADER FILES:

#include "AthenaBaseComps/AthAlgorithm.h"
#include "egammaEvent/egammaParamDefs.h"
#include "egammaEvent/egammaPIDdefsObs.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "TrkParameters/TrackParameters.h"
#include "JetInterface/IJetSelector.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/ReadHandle.h"
#include "xAODEventInfo/EventInfo.h"

#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

#include "CxxUtils/checker_macros.h"

#include <map>
#include <vector>



class TTree;
class TH1F;
class TH2F;

class IegammaTrkRefitterTool;

namespace Trig{
  class TrigDecisionTool;
}

namespace Trk{
  class VxTrackAtVertex;
  class Track;
}

#define NOS_ELECTRONS 100
#define NO_PV 50

typedef std::pair<const xAOD::Vertex* , int> VxPos;


class IDPerfMonEoverP : public AthAlgorithm
{
 public:

  /** @brief Default constructor*/
  IDPerfMonEoverP(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief Destructor*/
  ~IDPerfMonEoverP();

  /** @brief Gaudi algorithm hooks*/
  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:
  /** @brief addToValidationNutple*/
  void addToValidationNtuple(const Trk::Perigee*, const xAOD::CaloCluster*,  int isOrginal);

  void clearValidationNtuple();

  void validationAction();
  void deleteAction() const;

  void fillIsEM(const xAOD::Electron *eg);

  void fillGeneral(const xAOD::Electron *eg);

  bool passMETCleaningCuts() const;

  bool fillVertexInformation(std::map<const xAOD::TrackParticle*, VxPos >& trackParticleVertexMap,
                             xAOD::Vertex const* & primaryVertexFirstCandidate);

  bool storeMETinformation();

  //  const Rec::TrackParticle* getTrackParticle(Trk::VxTrackAtVertex *trkAtVx) const;
  //  const xAOD::TrackParticle* getTrackParticle(const Trk::VxTrackAtVertex *trkAtVx) const;
  VxPos findAssociatedVertex(std::map<const xAOD::TrackParticle*, VxPos >& trackParticleVertexMap,
                             const xAOD::Vertex* primaryVertexFirstCandidate,
                             const xAOD::Electron* ) const;

  void extrapolateToTheCalo(const Trk::TrackParameters* trkPar, const xAOD::CaloCluster* cluster,
                            double* eta, double* phi, double* deltaEta, double* delatPhi );

  void fillElectronInfo (const xAOD::Electron* p);
  void fillTriggerInformation();

  bool fillLastMeasurement(const Trk::Track* track,const int fitter);

  double correctIP_PV(int electron_i, bool do_d0);

 private:

  /** @brief Electron collection input name*/
  std::string                 m_InputElectronContainerName;

  std::string m_metRefFinalName;

  /** @brief JEt collection input name*/
  std::string m_jetContainerName;
  /** @brief MET input name*/
  std::string m_missingEtObjectName;
  /** @brief Primary vertex input name*/
  std::string m_primaryVertexCollection;

  /** @brief Name of output of Refitted Inner Detector Tracks*/
  std::string                 m_OutputTrackCollectionName_no1;
  std::string                 m_OutputTrackCollectionName_no2;

  /** @brief Refitted track collection*/
  TrackCollection            *m_refittedTracks_no1;
  TrackCollection            *m_refittedTracks_no2;

  /** @brief ReadHandle to the Event Info */
  SG::ReadHandleKey<xAOD::EventInfo>    m_evt  {this, "EvtInfo", "EventInfo", "EventInfo name"};

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter;

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter_no2;

  /** @brief The trigger decision tool */
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  /** @brief jet selector tool */
  ToolHandle< IJetSelector > m_jetCleaningTool
     {this,"JetCleaningTool","JetCleaningTool/JetCleaningTool",""};

  /* Flag for refitting*/
  bool                            m_refitEverything;

  /* Flag for data*/
  bool                            m_isDATA;

  //Validation Ntuple Stuff
  //!< boolean to switch to validation mode
  bool                            m_validationMode;

  //!< boolean to switch to on Detailed Tree Filling -- large increase in the ntuple size
  bool                            m_fillDetailedTree;


  //!< validation tree name - to be acessed by this from root
  std::string                     m_validationTreeName;
  //!< validation tree description - second argument in TTree
  std::string                     m_validationTreeDescription;
 //!< stream/folder to for the TTree to be written out
  std::string                     m_validationTreeFolder;
  //!< Root Validation Tree
  TTree*                          m_validationTree;


  unsigned int            m_runNumber;
  unsigned int            m_evtNumber;
  unsigned int            m_lumi_block;


  int                     m_nelectrons;
  int                     m_electronCounter;           //!< counter for electrons

  float                   m_electronTheta[3][NOS_ELECTRONS];     //!< Track theta on Surface
  float                   m_electronPhi[3][NOS_ELECTRONS];       //!< Track Phi on electron
  float                   m_electronQoverP[3][NOS_ELECTRONS];    //!< Track q over p on electron
  float                   m_electrond0[3][NOS_ELECTRONS];       //!< Track Phi on electron
  float                   m_electronz0[3][NOS_ELECTRONS];    //!< Track q over p on electron

  float                   m_electronLMQoverP[3][NOS_ELECTRONS];    //!< Track q over p on electron

  float                   m_electronErrTheta[3][NOS_ELECTRONS];  //!< Track theta error on electron
  float                   m_electronErrPhi[3][NOS_ELECTRONS];    //!< Track phi error on electron
  float                   m_electronErrQoverP[3][NOS_ELECTRONS];
  float                   m_electronErrd0[3][NOS_ELECTRONS];
  float                   m_electronErrz0[3][NOS_ELECTRONS];


  int                     m_nTRT[NOS_ELECTRONS];
  int                     m_nSCT[NOS_ELECTRONS];
  int                     m_nBLayer[NOS_ELECTRONS];
  int                     m_nPIX[NOS_ELECTRONS];
  int                     m_nTRTout[NOS_ELECTRONS];
  int                     m_nSCTout[NOS_ELECTRONS];
  int                     m_nPIXout[NOS_ELECTRONS];
  int                     m_nTRTHT[NOS_ELECTRONS];
  int                     m_nTRTHTout[NOS_ELECTRONS];


  int                     m_author[NOS_ELECTRONS];

  float                   m_ClusterEnergy[NOS_ELECTRONS];
  float                   m_ClusterEta[NOS_ELECTRONS];
  float                   m_ClusterPhi[NOS_ELECTRONS];

  bool                    m_IsEMLoose[NOS_ELECTRONS];
  bool                    m_IsEMMedium[NOS_ELECTRONS];
  bool                    m_IsEMTight[NOS_ELECTRONS];
  bool                    m_IsEMTightTRT[NOS_ELECTRONS];
  bool                    m_isGoodOQ[NOS_ELECTRONS];



  //Vertex Resolution Information
  int   m_associatedToVtx[NOS_ELECTRONS];
  VxPos m_vxpos[NOS_ELECTRONS];

  //Vertex information
  int   m_nbpv;
  int   m_pvtype[NO_PV];
  int   m_pvnbtk[NO_PV];
  float m_pvsumpt[NO_PV];
  float m_pvx[NO_PV];
  float m_pvy[NO_PV];
  float m_pvz[NO_PV];
  float m_errpvx[NO_PV];
  float m_errpvy[NO_PV];
  float m_errpvz[NO_PV];
  float m_covpvxpvy[NO_PV];
  float m_covpvypvz[NO_PV];
  float m_covpvzpvx[NO_PV];

  //MET
  bool  m_METgoodness;
  float m_sumet;
  float m_missingEt;
  float m_missingEtx;
  float m_missingEty;


  float m_ePID_ShowerType[50][NOS_ELECTRONS];
  float m_ePID_IsolationType[50][NOS_ELECTRONS];
  float m_ePID_TrackCaloMatchType[50][NOS_ELECTRONS];
  float m_ePID_SummaryType[50][NOS_ELECTRONS];
  //  std::vector< std::pair <xAOD::EgammaParameters::ShowerShapeType, std::string >  > m_PIDNames;
  std::vector< std::pair <xAOD::EgammaParameters::ShowerShapeType, std::string >  > m_PID_ShowerType_Names;
  std::vector< std::pair <xAOD::Iso::IsolationType, std::string >  > m_PID_IsolationType_Names;
  std::vector< std::pair <xAOD::EgammaParameters::TrackCaloMatchType, std::string >  > m_PID_TrackCaloMatchType_Names;
  std::vector< std::pair <xAOD::SummaryType, std::string >  > m_PID_SummaryType_Names;



  bool m_trigger[50];
  std::vector<std::string> m_triggerNames;


  TH1F *  m_ZeeLooseMassOS_Cluster;
  TH1F *  m_ZeeLooseMassSS_Cluster;
  TH1F *  m_ZeeMediumMassOS_Cluster;
  TH1F *  m_ZeeMediumMassSS_Cluster;
  TH2F *  m_ZeeMediumOS_ClusterPtEta;

  TH1F *  m_WenuLooseElectronET;
  TH1F *  m_WenuTight_Met;
  TH1F *  m_WenuTight_MT;
  TH1F *  m_WenuTightMet_MT;
  TH1F *  m_WenuTightElectronET;
  TH1F *  m_WenuTightW_PT;
  TH2F *  m_WenuTightElectron_PTEtaPos;
  TH2F *  m_WenuTightElectron_PTEtaNeg;


  //!< validation tree name - to be acessed by this from root
  std::string                     m_smallValidationTreeName;
  //!< validation tree description - second argument in TTree
  std::string                     m_smallValidationTreeDescription;
  //!< stream/folder to for the TTree to be written out
  std::string                     m_smallValidationTreeFolder;
  //!< Root Validation Tree
  TTree*                          m_smallValidationTree;


  double m_small_QoverP;
  double m_small1_QoverP;
  double m_small2_QoverP;
  double m_smallClusterEnergy;
  double m_smallClusterPhi;
  double m_smallClusterEta;
  double m_smallTrackTheta;
  double m_smallCharge;
  double m_smalld0;
  double m_smallz0;

  std::vector<int> FillSimpleTree();
  bool passWenuSelection(std::vector<int>& electrons);
  bool passZeeSelection(std::vector<int>& electrons);
  double getMassCluster(int el1, int el2);

  //Likelihood tool:
  std::string m_lhTune;
  AsgElectronLikelihoodTool* m_LHToolLoose2015; //!
  AsgElectronLikelihoodTool* m_LHToolMedium2015; //!
  AsgElectronLikelihoodTool* m_LHToolTight2015; //!
};

#endif
