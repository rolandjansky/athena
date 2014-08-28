//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// IPAUtruthTool.h, (c) ATLAS Detector software 2007
// author: Marcello Fanti, marcello.fanti@mi.infn.it
///////////////////////////////////////////////////////////////////
//
// USAGE:
// -----
// First, load the tool:
//    #include "PhotonAnalysisUtils/IPAUtruthTool.h"
//    IPAUtruthTool* m_PAUtruthTool ;
//    StatusCode sc = toolSvc->retrieveTool( "PAUtruthTool", m_PAUtruthTool );
///////////////////////////////////////////////////////////////////

#ifndef HIGGSANALYSYSUTILS_IHGGTRUTHTOOL_H
#define HIGGSANALYSYSUTILS_IHGGTRUTHTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "McParticleEvent/TruthParticle.h"
#include "JetEvent/JetCollection.h"
#include "PhotonAnalysisUtils/PAUegamma.h" // to define PAU::gamma

class I4Momentum;
class IAnalysisTools;
class TruthParticle;
class CaloCluster ;
namespace Analysis { class Photon ; }

//notneeded:
// struct lparton { 
//       float pt_max;
//       float eta;
//       float phi;
//       int barcode;
//       int pdg;
// };

static const InterfaceID IID_IPAUtruthTool("IPAUtruthTool", 1, 0);

class IPAUtruthTool : virtual public IAlgTool {
  
public:
  /** Virtual destructor */
  virtual ~IPAUtruthTool(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { 
    return IID_IPAUtruthTool; };
 
  // public methods:
 
  virtual StatusCode load() =0;

  virtual int RunNumber() const =0;
  virtual int EventNumber() const =0;
  virtual bool okMCloaded() const =0;

  virtual const std::vector<const TruthParticle*>& getFinalStateMC() const =0;
  virtual const std::vector<const Jet*>& getTruthJets() const =0;

  virtual const TruthParticle* getMother(const TruthParticle* truePart) const =0;
  virtual const std::vector<const TruthParticle*> getMothers(const TruthParticle* truePart) const =0;
  virtual const std::vector<const TruthParticle*> getChildren(const TruthParticle* truePart) const =0;
  virtual bool isConvMC(const TruthParticle* truePart)  const =0;
  //  virtual bool isConvMC(const TruthParticle* truePart,float& RconvMC,float& ZconvMC) const =0;
  virtual bool isConvMC(const TruthParticle* truePart,float& XconvMC,float& YconvMC,float& RconvMC,float& ZconvMC) const =0;
  virtual bool isFinalStatePhotonMC(const TruthParticle* truePart)  const =0;
  virtual bool isDirectPhotonMC(const TruthParticle* truePart)  const =0;
  virtual bool isPromptPhotonMC(const TruthParticle* truePart)  const =0;
  virtual bool isQuarkBremMC(const TruthParticle* truePart)  const =0;
  virtual bool isPromptParticleMC(const TruthParticle* truePart)  const =0;
  virtual bool hasValidStatus(const TruthParticle* truePart)  const =0;
  virtual bool isFinalState(const TruthParticle* truePart)  const =0;
  virtual bool isDecayed(const TruthParticle* truePart)  const =0;

  virtual const TruthParticle* getTruthParticle(const HepMC::GenParticle* genPart) =0;

  virtual bool extrapolToCalo(const TruthParticle* tp, double& etaCalo, double& phiCalo) =0;

  virtual float getHiggsZMC() const =0;
  virtual bool isHggMC()   const =0;
  virtual bool isHiggsMC() const =0;
  virtual const std::vector<const TruthParticle*>& getHiggsParents() const =0;
  virtual const std::vector<const TruthParticle*>& getHiggsDaughters() const =0;

  virtual const std::vector<const TruthParticle*>& getGravitonParents() const =0;
  virtual const std::vector<const TruthParticle*>& getGravitonDaughters() const =0;

  virtual const std::vector<const TruthParticle*>& getHardProcessParents()  const =0;
  virtual const std::vector<const TruthParticle*>& getHardProcessProducts() const =0;
  virtual int getHardProcessType() const =0;

  virtual int getIndexTruthMatch(const I4Momentum* reco4p) =0;
  virtual const TruthParticle* getTruthMatch(const I4Momentum* reco4p, bool checkType=false) =0;
  virtual const TruthParticle* getTruthMatch(const CaloCluster* clus, const std::string partName="") =0; // partName="photon","electron",""
  virtual const TruthParticle* getTruthMatch(const Analysis::Photon*   ph) =0;
  virtual std::vector<const TruthParticle*> getTruthConvMatch(const Analysis::Photon* ph) =0;
  virtual const TruthParticle* getTruthMatch(const Analysis::Electron* el) =0;
  virtual const TruthParticle* getTruthMatch(const Analysis::Muon*     mu) =0;
  virtual const TruthParticle* getTruthMatch(const Rec::TrackParticle* tp) =0;
  virtual const TruthParticle* getEvgenAncestor(const TruthParticle* tp) =0;
  virtual const TruthParticle* getTruthMatchHardProcess(const I4Momentum* reco4p) =0;

  virtual void TruthParticleClassifier(const TruthParticle* tp,
				       bool& foundHiggs, bool& foundGraviton,
				       bool& foundHgg, bool& foundGgg,
				       const TruthParticle* lastInHardProcess) =0;

  virtual unsigned getNumberEventsMC() const =0;
  virtual unsigned getNumberHiggsMC()  const =0;
  virtual unsigned getNumberHggMC()    const =0;

  virtual StatusCode DumpTruth(bool onlyRunEvt=false) =0;

  virtual float GetMCweight()  const =0 ;

  enum SignalType { Higgs=1 , Graviton } ;
  enum HProdChannels { ggH=1 , vbfH , WH , ZH , ttH } ;
  enum GProdchannels { ggG=1 , qqG=2 } ;
  enum UnknownType { unknownSignalType=9, unknownBackgroundType=9 } ;
  enum Background { QCDjetjet=10, QCDgamjet=11, QCDgamgam=12} ;

  virtual const std::vector<double> giveTrueVertex() const =0;     

  //notneeded virtual void leadingPartonCone4(PAU::egamma* g, std::vector<lparton> *)=0 ;
  //notneeded virtual std::vector<lparton> & partonlist() = 0;

  virtual std::vector<const TruthParticle*> partonsInCone(const I4Momentum* pmom, float dRmax) =0;

  virtual float getPartonIsolation(const TruthParticle* pmom, float dRmax, bool OnlyBrothers) =0;

  virtual float getParticleIsolation(const TruthParticle* pmom, float dRmax, bool munu) =0;

  virtual float inROICone(float deta, float dphi) =0;
  virtual float inMatchCone(float deta, float dphi) =0;
  virtual float inNarrowCone(float deta, float dphi) =0;

  // enum SignalType     { ggH=1 , vbfH , WH , ZH , ttH , unknownSignalType } ;
  // enum BackgroundType { QCDgamgam=1 , QCDgamjet , QCDjetjet , unknownBackgroundType } ;

};

#endif // HIGGSANALYSYSUTILS_IHGGTRUTHTOOL_H
