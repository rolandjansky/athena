//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// PAUtruthTool.h, (c) ATLAS Detector software 2007
// author: Marcello Fanti, marcello.fanti@mi.infn.it
///////////////////////////////////////////////////////////////////


#ifndef HIGGSANALYSYSUTILS_HGGTRUTHTOOL_H
#define HIGGSANALYSYSUTILS_HGGTRUTHTOOL_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "PhotonAnalysisUtils/IPAUtruthTool.h"

class ITruthParticleCnvTool;
class CaloDepthTool;
class IExtrapolateToCaloTool;
class IMCTruthClassifier;
class TH1;

#include <iostream>
#include <fstream>
#include <string>
#include <map>


class PAUtruthTool : public AlgTool, virtual public IPAUtruthTool, virtual public IIncidentListener {
  
public:
  
  /** AlgTool like constructor */
  PAUtruthTool(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~PAUtruthTool();
  
  /** AlgTool initialize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();
  /** handle for incident service */
  virtual void handle(const Incident& inc) ;

  // public methods:
 
  StatusCode load() ;

  virtual int RunNumber() const   { return (m_MCloaded) ? m_run0 : -99999 ; } ;
  virtual int EventNumber() const { return (m_MCloaded) ? m_evt0 : -99999 ; } ;
  virtual bool okMCloaded() const { return m_MCloaded ; } ;

  virtual const std::vector<const TruthParticle*>& getFinalStateMC() const { return m_mytruthFinalState ; } ;
  virtual const std::vector<const Jet*>& getTruthJets() const { return m_truthJets ; } ;

  virtual const TruthParticle* getMother(const TruthParticle* truePart) const ;
  virtual const std::vector<const TruthParticle*> getMothers(const TruthParticle* truePart) const ;
  virtual const std::vector<const TruthParticle*> getChildren(const TruthParticle* truePart) const ;
  virtual bool isConvMC(const TruthParticle* truePart)  const ;
  //virtual bool isConvMC(const TruthParticle* truePart,float& RconvMC,float& ZconvMC) const ;
  virtual bool isConvMC(const TruthParticle* truePart,float& XconvMC,float& YconvMC,float& RconvMC,float& ZconvMC) const ; //will be deleted later (Marc)

  virtual bool isFinalStatePhotonMC(const TruthParticle* truePart)  const ;
  virtual bool isPromptPhotonMC(const TruthParticle* truePart)  const ;
  virtual bool isDirectPhotonMC(const TruthParticle* truePart)  const ;
  virtual bool isQuarkBremMC(const TruthParticle* truePart)  const ;
  virtual bool isPromptParticleMC(const TruthParticle* truePart)  const ;
  virtual bool hasValidStatus(const TruthParticle* truePart)  const ;
  virtual bool isFinalState(const TruthParticle* truePart)  const ;
  virtual bool isDecayed(const TruthParticle* truePart)  const ;

  virtual const TruthParticle* getTruthParticle(const HepMC::GenParticle* genPart) ;

  virtual bool extrapolToCalo(const TruthParticle* tp, double& etaCalo, double& phiCalo) ;

  virtual float getHiggsZMC()              const { return (m_isHiggsMC) ? m_HiggsZVMC : -99999 ; } ; 
  virtual bool isHggMC()                   const { return m_isHggMC ; } ;
  virtual bool isHiggsMC()                 const { return m_isHiggsMC ; } ;
  virtual const std::vector<const TruthParticle*>& getHiggsParents()   const { return m_HiggsParents ; } ;
  virtual const std::vector<const TruthParticle*>& getHiggsDaughters() const { return m_HiggsDaughters ; } ;

  virtual const std::vector<const TruthParticle*>& getGravitonParents()   const { return m_GravitonParents ; } ;
  virtual const std::vector<const TruthParticle*>& getGravitonDaughters() const { return m_GravitonDaughters ; } ;

  virtual const std::vector<const TruthParticle*>& getHardProcessParents()  const { return m_HardProcPar ; } ;
  virtual const std::vector<const TruthParticle*>& getHardProcessProducts() const { return m_HardProcProd ; } ;
  virtual int getHardProcessType() const ;

  virtual int getIndexTruthMatch(const I4Momentum* reco4p) ;
  virtual const TruthParticle* getTruthMatch(const I4Momentum* reco4p, bool checkType=false) ;
  virtual const TruthParticle* getTruthMatch(const CaloCluster* clus, const std::string partName="") ;
  virtual const TruthParticle* getTruthMatch(const Rec::TrackParticle* tp) ;
  virtual const TruthParticle* getTruthMatch(const Analysis::Muon*     mu) ;
  virtual const TruthParticle* getTruthMatch(const Analysis::Electron* el) ;
  virtual const TruthParticle* getTruthMatch(const Analysis::Photon*   ph) ;
  virtual std::vector<const TruthParticle*> getTruthConvMatch(const Analysis::Photon* ph) ;
  virtual const TruthParticle* getEvgenAncestor(const TruthParticle* tp) ;
  virtual const TruthParticle* getTruthMatchHardProcess(const I4Momentum* reco4p) ;

  virtual unsigned getNumberEventsMC() const { return m_numberEventsMC ; } ;
  virtual unsigned getNumberHiggsMC()  const { return m_numberHiggsMC ; } ;
  virtual unsigned getNumberHggMC()    const { return m_numberHggMC ; } ;

  virtual void TruthParticleClassifier(const TruthParticle* tp,
				       bool& foundHiggs, bool& foundGraviton,
				       bool& foundHgg, bool& foundGgg,
				       const TruthParticle* lastInHardProcess);

  virtual StatusCode DumpTruth(bool onlyRunEvt=false) ;

  virtual float GetMCweight()  const ;

  const std::vector<double> giveTrueVertex() const;

  virtual std::vector<const TruthParticle*> partonsInCone(const I4Momentum* pmom, float dRmax) ;

  virtual float getPartonIsolation(const TruthParticle* gmom, float dRmax, bool OnlyBrothers);

  virtual float getParticleIsolation(const TruthParticle* gmom, float dRmax, bool munu);

  virtual inline float inROICone(float deta, float dphi) { return inCone(deta,dphi,m_deltaEtaROI, m_deltaPhiROI) ; }
  virtual inline float inMatchCone(float deta, float dphi) { return inCone(deta,dphi,m_deltaEtaMatch, m_deltaPhiMatch) ; }
  virtual inline float inNarrowCone(float deta, float dphi) { return inCone(deta,dphi,m_deltaEtaNarrow, m_deltaPhiNarrow) ; }

private:

  bool isParton(const TruthParticle* tp);
  bool okIndexMC(int i) const { return (m_MCloaded) && (i>=0) && (i<(int)m_mytruthFinalState.size()) ; } ;
  bool MCloaded() { return (load().isSuccess()) && (m_MCloaded) ; } ; // try to force event load, if failure return false
  void clear() ;
  StatusCode TruthExplorer() ;
  StatusCode TruthJetsLoader() ;
  bool particleExtrapolator(const TruthParticle* tp, double& etaCalo, double& phiCalo) ;
  bool simpleExtrapolator(const TruthParticle* tp, double& etaCalo, double& phiCalo) ;
  void flagMatch(const TruthParticle* tp);
  void debugMessage(const std::string& msg,const I4Momentum* p,const Rec::TrackParticle* trk=0,const HepMC::GenParticle* genpart=0) const ;

  inline float inCone(float deta, float dphi, float reta, float rphi) {
    const double PI = 3.141592654 ;
    while ( dphi > PI )  { dphi -= 2*PI ; }
    while ( dphi < -PI ) { dphi += 2*PI ; }
    float detanorm = deta/reta ;
    float dphinorm = dphi/rphi ;
    return ( detanorm*detanorm + dphinorm*dphinorm ) ; 
  }

private:

  /// a handle on Store Gate and other tools
  StoreGateSvc* m_storeGate;

  typedef ToolHandle<ITruthParticleCnvTool> CnvTool_t;  /** converter tool McEventCollection -> TruthParticle */
  CnvTool_t m_cnvTool;

  IExtrapolateToCaloTool*  m_eToCalo;
  IExtrapolateToCaloTool*  m_gToCalo;
  ToolHandle<IMCTruthClassifier> m_TruthClassifierTool;

  std::string m_extrapolateElectronToCaloToolName;
  std::string m_extrapolateGammaToCaloToolName;

  // algtool properties:
  bool m_egammasFromTruth, m_muonsFromTruth, m_partonsFromTruth, m_hadronsFromTruth ;
  bool m_useG4Particles, m_useExtrapolation, m_useAtlasExtrapolator ;
  float m_r0TruthCut, m_z0TruthCut, m_ptTruthCut, m_rTruthConv, m_zTruthConv, m_etaTruthConv, m_ptTruthConv, 
    m_deltaRmatch, m_deltaRmatch_squared ;
  float m_deltaEtaROI, m_deltaPhiROI, m_deltaEtaMatch, m_deltaPhiMatch, m_deltaEtaNarrow, m_deltaPhiNarrow ;
  float m_ptCutCharged, m_ptCutNeutral ;
  std::string m_truthParticleContainerName;
  std::string m_truthEventCollectionName;
  std::string m_truthJetsContainerName ;
  std::ofstream m_dumpTruthFile; 

  bool m_useTruthClassifierTool;

  bool m_fromESD;
  bool m_MCloaded, m_isHggMC, m_isHiggsMC ; 
  bool m_isGggMC, m_isGravitonMC ;
  float m_HiggsZVMC ;
  std::vector<const TruthParticle*> m_HiggsParents , m_HiggsDaughters ;
  std::vector<const TruthParticle*> m_GravitonParents , m_GravitonDaughters ;
  std::vector<const TruthParticle*> m_HardProcPar, m_HardProcProd ;

  int m_run0, m_evt0 ;
  unsigned m_ncalls_gToCalo, m_ncalls_eToCalo ;

  std::vector<const TruthParticle*> m_mytruthFinalState ;
  std::vector<const Jet*> m_truthJets ;
  std::vector<bool> m_matchedWithReco ;
  std::vector<bool> m_isConvMC ;
  std::vector<float> m_RconvMC ;
  std::vector<float> m_ZconvMC ;
  std::vector<float> m_XconvMC ;
  std::vector<float> m_YconvMC ;

  typedef std::pair<double,double>               EtaPhi ;
  typedef std::map<const TruthParticle*,EtaPhi>  ParticleExtrapolationMap ;
  ParticleExtrapolationMap m_truthPartToCalo ;

  typedef std::map<const HepMC::GenParticle*,const TruthParticle*> GenToTruthMap ;
  GenToTruthMap m_gen2truth ;

  // some statistics:
  unsigned m_numberEventsMC, m_numberHiggsMC, m_numberHggMC ;

  //notneeded  std::vector<lparton> m_partonlist;

};


#endif // HIGGSANALYSYSUTILS_HGGTRUTHTOOL_H
