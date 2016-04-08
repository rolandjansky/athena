/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef USERANALYSISUTILS_USERANALYSISSELECTIONTOOL_H  
#define USERANALYSISUTILS_USERANALYSISSELECTIONTOOL_H 

/*****************************************************************************
Name    : UserAnalysisSelectionTool.h
Package : offline/PhysicsAnalysis/AnalysisCommon/UserAnalysisUtils
Author  : Ketevi A. Assamagan
Created : November 2007
Purpose : User tools for analyis preparation on ESD/AOD/DPD in Athena - selections
          - Take a list of input containers
          - Call the selections tools to see is containee passed selection
          - if passed selection put containee in a new container  
*****************************************************************************/

//#include "GaudiKernel/AlgTool.h"
//#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "VxVertex/VxContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "TrkSegment/SegmentCollection.h"

#include "muonEvent/MuonContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/PhotonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "JetEvent/JetCollection.h"
#include "MissingETEvent/MissingET.h"

#include "NavFourMom/IParticleContainer.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include <string>
#include <map>
#include <vector>

class MsgStream;

/** Interface ID */  
static const InterfaceID IID_UserAnalysisSelectionTool("UserAnalysisSelectionTool", 1, 0);

class UserAnalysisSelectionTool : public AthAlgTool {

public:

  /** Standard Constructor */
  UserAnalysisSelectionTool(const std::string& type, const std::string& name,
	                    const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_UserAnalysisSelectionTool; };

  /** Overriding initialize, finalize, and execute */
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /** pre-selections */
  bool isSelected( const Analysis::Electron * electron ) const;
  bool isSelected( const Analysis::Photon * photon ) const;
  bool isSelected( const Analysis::Muon * muon ) const;
  bool isSelected( const Analysis::TauJet * tauJet ) const;
  bool isSelected( const Jet* jet ) const;
  bool isSelected( const Rec::TrackParticle * trackParticle ) const;
  bool isSelected( const CaloCluster* caloCluster ) const;

  bool isBJet( const Jet * jet ) const;

protected:

   /** Standard destructor */
   virtual ~UserAnalysisSelectionTool();

private:


  /** this is Atlfast data */
  bool m_isAtlfast;

  /** Electron selection */
  double m_electronPt;
  double m_electronEta;
  std::string m_egDetailContainerName;
  std::string m_electronIsEMFlag;
  int    m_electronIsEM;
  int    m_electronIsolationConeIndex;
  bool   m_doElectronIsolation;
  bool   m_authorEgammaOnly;
  bool   m_electronEtaWindCut;/// apply (or not) eta cut in bad region window  
  double m_electronEtaWind;     /// width of window
  double m_electronEtaWindCent; /// center of window
  double m_electronIsolationEt;
  double m_normElectronIsolEt;  // normalised electron isolation ET: EtCone/Pt

  /** Photon selection */
  double m_photonPt;
  double m_photonEta;
  double m_photonIsEM;

  /** Muon selection */
  double m_muonPt;
  double m_muonEta;
  bool   m_doMuonIsolation;
  int    m_muonIsolationConeIndex;
  double m_muonIsolationEt;
  bool   m_useMatchChi2;
  double m_muonMatchChi2;
  double m_normMuonIsolEt;  // normalised muon isolation ET: EtCone/Pt

  /** TauJet selection */
  double m_tauJetPt;
  double m_tauJetEta;
  double m_tauJetLikelihood;
  double m_tauEleBDTCut;

  /** Jet selection */
  double m_jetPt;
  double m_jetEta;
  double m_bJetLikelihood;

  /** caloCluster selection */
  double m_caloClusterE;

  /** TrackParticle selection */
  double m_trackParticlePt;

};

#endif // USERANALYSISUTILS_USERANALYSISSELECTIONTOOL_H 




