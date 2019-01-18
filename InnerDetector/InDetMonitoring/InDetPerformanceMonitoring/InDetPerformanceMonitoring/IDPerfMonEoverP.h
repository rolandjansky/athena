/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
class StoreGateSvc;

#include "AthenaBaseComps/AthAlgorithm.h"
#include "egammaEvent/egammaParamDefs.h"
#include "egammaEvent/egammaPIDdefs.h"

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

#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

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
  void addToValidationNtuplexAOD(const xAOD::Electron *eg) const;
  void clearValidationNtuple();

  void validationAction() const;
  void deleteAction() const;

  void fillIsEM(const xAOD::Electron *eg) const;

  void fillGeneral(const xAOD::Electron *eg) const;

  bool passMETCleaningCuts() const;

  bool fillVertexInformation() const;

  bool storeMETinformation() const;

  //  const Rec::TrackParticle* getTrackParticle(Trk::VxTrackAtVertex *trkAtVx) const;
  //  const xAOD::TrackParticle* getTrackParticle(const Trk::VxTrackAtVertex *trkAtVx) const;
  VxPos findAssociatedVertex(const xAOD::Electron* ) const;

  void extrapolateToTheCalo(const Trk::TrackParameters* trkPar, const xAOD::CaloCluster* cluster,
                            double* eta, double* phi, double* deltaEta, double* delatPhi );

  void fillElectronInfo (const xAOD::Electron* p) const;
  void fillTriggerInformation() const;

  bool fillLastMeasurement(const Trk::Track* track,const int fitter)const;

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

  /** @brief Name of outpu of Refitted Inner Detector Tracks*/
  std::string                 m_OutputTrackCollectionName_no1;
  std::string                 m_OutputTrackCollectionName_no2;

  /** @brief Refitted track collection*/
  TrackCollection            *m_refittedTracks_no1;
  TrackCollection            *m_refittedTracks_no2;


  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter;

  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_TrackRefitter_no2;

  /** @brief The trigger decision tool */
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  /** @brief jet selector tool */
  ToolHandle< IJetSelector > m_jetCleaningTool;

  /* Flag for refitting*/
  bool                            m_refitEverything;

  /* Flag for data*/
  bool                            m_isDATA;

  //Validation Ntuple Stuff
  //!< boolean to switch to validation mode
  bool                            m_validationMode;

  //!< boolean to switch to on Detailed Tree Filling -- large increase in the ntuple size
  bool                            m_fillDetailedTree;


  mutable std::map<const xAOD::TrackParticle*, VxPos >   m_trackParticleVertexMap;
  //  mutable const Trk::VxCandidate*                        m_primaryVertexFirstCandidate;
  mutable const xAOD::Vertex*                        m_primaryVertexFirstCandidate;
  //!< validation tree name - to be acessed by this from root
  std::string                     m_validationTreeName;
  //!< validation tree description - second argument in TTree
  std::string                     m_validationTreeDescription;
 //!< stream/folder to for the TTree to be written out
  std::string                     m_validationTreeFolder;
  //!< Root Validation Tree
  TTree*                          m_validationTree;


  mutable unsigned int            m_runNumber;
  mutable unsigned int            m_evtNumber;
  mutable unsigned int            m_lumi_block;


  mutable int                     m_nelectrons;
  mutable int                     m_electronCounter;           //!< counter for electrons

  mutable float                   m_electronTheta[3][NOS_ELECTRONS];     //!< Track theta on Surface
  mutable float                   m_electronPhi[3][NOS_ELECTRONS];       //!< Track Phi on electron
  mutable float                   m_electronQoverP[3][NOS_ELECTRONS];    //!< Track q over p on electron
  mutable float                   m_electrond0[3][NOS_ELECTRONS];       //!< Track Phi on electron
  mutable float                   m_electronz0[3][NOS_ELECTRONS];    //!< Track q over p on electron

  mutable float                   m_electronLMQoverP[3][NOS_ELECTRONS];    //!< Track q over p on electron

  mutable float                   m_electronErrTheta[3][NOS_ELECTRONS];  //!< Track theta error on electron
  mutable float                   m_electronErrPhi[3][NOS_ELECTRONS];    //!< Track phi error on electron
  mutable float                   m_electronErrQoverP[3][NOS_ELECTRONS];
  mutable float                   m_electronErrd0[3][NOS_ELECTRONS];
  mutable float                   m_electronErrz0[3][NOS_ELECTRONS];


  mutable int                     m_nTRT[NOS_ELECTRONS];
  mutable int                     m_nSCT[NOS_ELECTRONS];
  mutable int                     m_nBLayer[NOS_ELECTRONS];
  mutable int                     m_nBLayerOutlier[NOS_ELECTRONS];
  mutable int                     m_nexpectBLayer[NOS_ELECTRONS];
  mutable int                     m_nBLayerHitPlusOutlier[NOS_ELECTRONS];
  mutable int                     m_nPIX[NOS_ELECTRONS];
  mutable int                     m_nTRTout[NOS_ELECTRONS];
  mutable int                     m_nSCTout[NOS_ELECTRONS];
  mutable int                     m_nPIXout[NOS_ELECTRONS];
  mutable int                     m_nTRTHT[NOS_ELECTRONS];
  mutable int                     m_nTRTHTout[NOS_ELECTRONS];


  mutable int                     m_author[NOS_ELECTRONS];

  mutable float                   m_ClusterEnergy[NOS_ELECTRONS];
  mutable float                   m_ClusterEta[NOS_ELECTRONS];
  mutable float                   m_ClusterPhi[NOS_ELECTRONS];

  mutable bool                    m_IsEMLoose[NOS_ELECTRONS];
  mutable bool                    m_IsEMMedium[NOS_ELECTRONS];
  mutable bool                    m_IsEMTight[NOS_ELECTRONS];
  mutable bool                    m_IsEMTightTRT[NOS_ELECTRONS];
  mutable bool                    m_isGoodOQ[NOS_ELECTRONS];



  //Vertex Resolution Information
  mutable int   m_associatedToVtx[NOS_ELECTRONS];
  mutable VxPos m_vxpos[NOS_ELECTRONS];

  //Vertex information
  mutable int   m_nbpv;
  mutable int   m_pvtype[NO_PV];
  mutable int   m_pvnbtk[NO_PV];
  mutable float m_pvsumpt[NO_PV];
  mutable float m_pvx[NO_PV];
  mutable float m_pvy[NO_PV];
  mutable float m_pvz[NO_PV];
  mutable float m_errpvx[NO_PV];
  mutable float m_errpvy[NO_PV];
  mutable float m_errpvz[NO_PV];
  mutable float m_covpvxpvy[NO_PV];
  mutable float m_covpvypvz[NO_PV];
  mutable float m_covpvzpvx[NO_PV];

  //MET
  mutable bool  m_METgoodness;
  mutable float m_sumet;
  mutable float m_missingEt;
  mutable float m_missingEtx;
  mutable float m_missingEty;


  mutable float m_ePID_ShowerType[50][NOS_ELECTRONS];
  mutable float m_ePID_IsolationType[50][NOS_ELECTRONS];
  mutable float m_ePID_TrackCaloMatchType[50][NOS_ELECTRONS];
  mutable float m_ePID_SummaryType[50][NOS_ELECTRONS];
  //  mutable std::vector< std::pair <xAOD::EgammaParameters::ShowerShapeType, std::string >  > m_PIDNames;
  mutable std::vector< std::pair <xAOD::EgammaParameters::ShowerShapeType, std::string >  > m_PID_ShowerType_Names;
  mutable std::vector< std::pair <xAOD::Iso::IsolationType, std::string >  > m_PID_IsolationType_Names;
  mutable std::vector< std::pair <xAOD::EgammaParameters::TrackCaloMatchType, std::string >  > m_PID_TrackCaloMatchType_Names;
  mutable std::vector< std::pair <xAOD::SummaryType, std::string >  > m_PID_SummaryType_Names;



  mutable bool m_trigger[50];
  mutable std::vector<std::string> m_triggerNames;


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

  TH1F *  m_LHResult; // result of electron likelihood for each electron (debug purposes)

  //!< validation tree name - to be acessed by this from root
  std::string                     m_smallValidationTreeName;
  //!< validation tree description - second argument in TTree
  std::string                     m_smallValidationTreeDescription;
  //!< stream/folder to for the TTree to be written out
  std::string                     m_smallValidationTreeFolder;
  //!< Root Validation Tree
  TTree*                          m_smallValidationTree;

  // Flag for xAOD
  bool                            m_isxAOD;

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
