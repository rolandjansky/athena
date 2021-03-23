/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */
#ifndef MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIER_H
#define MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIER_H
/********************************************************************
NAME:     MCTruthClassifier.h 
PACKAGE:  atlasoff/PhysicsAnalysis/MCTruthClassifier
AUTHORS:  O. Fedin
CREATED:  Sep 2007
PURPOSE:            
Updated:  
 ********************************************************************/

// INCLUDE HEADER FILES:
#include "AsgTools/AsgTool.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
// EDM includes
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"

#ifndef XAOD_ANALYSIS
#include "GaudiKernel/ToolHandle.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "HepMC/GenParticle.h"
#endif

#ifndef GENERATIONBASE
//EDM includes
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODJet/Jet.h"
#endif

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE)
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
 #include "ParticlesInConeTools/ITruthParticlesInConeTool.h"
#include "AthenaKernel/Units.h"
#endif

//std includes
#include <cmath>
#include <utility>

class MCTruthClassifier : virtual public IMCTruthClassifier , public asg::AsgTool {

  ASG_TOOL_CLASS(MCTruthClassifier, IMCTruthClassifier)

public:

  // constructor 
  MCTruthClassifier(const std::string& type);
  // destructor 
  ~MCTruthClassifier();  

  // Gaudi algorithm hooks
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

  /*All Get to see those*/
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::TruthParticle *) override;

  virtual unsigned int classify(const xAOD::TruthParticle  *) override;

  virtual const xAOD::TruthParticle* getParentHadron(const xAOD::TruthParticle*)  override;

  virtual int getParentHadronID(const xAOD::TruthParticle*) override;

  enum MCTC_bits { HadTau=0, Tau, hadron, frombsm, uncat, isbsm, isgeant, stable, totalBits };

  static unsigned int isGeant(const unsigned int classify) { return std::bitset<MCTC_bits::totalBits> (classify).test(MCTC_bits::isgeant); }
  static unsigned int isBSM(const unsigned int classify) { return std::bitset<MCTC_bits::totalBits> (classify).test(MCTC_bits::isbsm); }
  static unsigned int fromBSM(const unsigned int classify) { return std::bitset<MCTC_bits::totalBits> (classify).test(MCTC_bits::frombsm); }


  /*! \brief This helper function returns the value -1 by checking the bit set in \ref MCTruthClassifier. 
   * It returns the value -1 if uncategorised, 0 if non-prompt, 1 if prompt
   * It also checks for prompt taus
   */

  static int isPrompt(const unsigned int classify, bool allow_prompt_tau_decays = true) {
    std::bitset<MCTC_bits::totalBits> res(classify);
    if (res.test(MCTC_bits::uncat))  return -1;
    bool fromPromptTau = res.test(MCTC_bits::Tau) && !res.test(MCTC_bits::HadTau);
    if (fromPromptTau) return int(allow_prompt_tau_decays);
    return !res.test(MCTC_bits::hadron);
  }

  MCTruthPartClassifier::ParticleOutCome getParticleOutCome() override { return  m_ParticleOutCome; }

  const xAOD::TruthParticle* getMother (const xAOD::TruthParticle*);
  const xAOD::TruthParticle* getMother() override {return m_Mother;}

  int   getMotherPDG() override {return m_MotherPDG;};
  int   getMotherBarcode() override {return m_MotherBarcode;};

  const xAOD::TruthParticle* getPhotonMother() override {return m_PhotonMother;}
  int   getPhotonMotherPDG() override {return m_PhotonMotherPDG;}
  int   getPhotonMotherBarcode() override {return m_PhotonMotherBarcode;}

  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>
    checkOrigOfBkgElec (const xAOD::TruthParticle* thePart) override;

  const xAOD::TruthParticle* getBkgElecMother() override {return m_BkgElecMother;}

  std::vector<const xAOD::TruthParticle*>* getTauFinalState() override {return &m_tauFinalStatePart;}


#ifndef XAOD_ANALYSIS /*Not for analysis*/
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const HepMC::GenParticle*) override; 
  bool compareTruthParticles(const HepMC::GenParticle *genPart, const xAOD::TruthParticle *truthPart);
#endif


#ifndef GENERATIONBASE /*Disable when no recostruction packages are expected*/
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::TrackParticle *);
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Electron* );     
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Photon* );     
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Muon* );     
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::CaloCluster* );
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Jet*, bool DR );

  double deltaR(const xAOD::TruthParticle& v1, const xAOD::Jet & v2) ;
  float getProbTrktoTruth(){return m_probability;}

  const xAOD::TruthParticle* getGenPart(const xAOD::TrackParticle *);
  const xAOD::TruthParticle* getGenPart(){return m_thePart;}

  float getdeltaRMatch(){return m_deltaRMatch;}
  float getdeltaPhiMatch(){return m_deltaPhi;}
  uint8_t getNumOfSiHits(){return m_NumOfSiHits;}

  std::vector<const xAOD::TruthParticle*>  getEGPartPntr(){return m_egPartPtr;} 
  std::vector<float> getEGPartdR(){return m_egPartdR;}
  std::vector<std::pair<MCTruthPartClassifier::ParticleType,MCTruthPartClassifier::ParticleOrigin> >  getEGPartClas(){return m_egPartClas;}

  std::vector<const xAOD::TrackParticle*>   getCnvPhotTrkPtr()        {return m_cnvPhtTrkPtr;}
  std::vector<const xAOD::TruthParticle*>   getCnvPhotTrkToTruthPart(){return m_cnvPhtTrPart;}
  std::vector<MCTruthPartClassifier::ParticleType>   getCnvPhotPartType() {return  m_cnvPhtPartType;}
  std::vector<MCTruthPartClassifier::ParticleOrigin> getCnvPhotPartOrig() {return  m_cnvPhtPartOrig;}
#endif

private:

  //methods for all cases
  float detEta(float x, float y ) {return fabs(x-y);}
  float detPhi(float , float );
  float rCone (float x, float y ){return sqrt(x*x + y*y);}
  //
  MCTruthPartClassifier::ParticleType    defTypeOfElectron(MCTruthPartClassifier::ParticleOrigin);
  MCTruthPartClassifier::ParticleOrigin  defOrigOfElectron(const xAOD::TruthParticleContainer* xTruthParticleContainer ,const xAOD::TruthParticle*);
  MCTruthPartClassifier::ParticleOutCome defOutComeOfElectron(const xAOD::TruthParticle*);
  //
  MCTruthPartClassifier::ParticleType    defTypeOfMuon(MCTruthPartClassifier::ParticleOrigin);
  MCTruthPartClassifier::ParticleOrigin  defOrigOfMuon(const xAOD::TruthParticleContainer* m_xTruthParticleContainer ,const xAOD::TruthParticle*);
  MCTruthPartClassifier::ParticleOutCome defOutComeOfMuon(const xAOD::TruthParticle*);
  //
  MCTruthPartClassifier::ParticleType    defTypeOfTau(MCTruthPartClassifier::ParticleOrigin);
  MCTruthPartClassifier::ParticleOrigin  defOrigOfTau(const xAOD::TruthParticleContainer* m_xTruthParticleContainer ,const xAOD::TruthParticle*);
  MCTruthPartClassifier::ParticleOutCome defOutComeOfTau(const xAOD::TruthParticle*);
  //
  MCTruthPartClassifier::ParticleType    defTypeOfPhoton(MCTruthPartClassifier::ParticleOrigin);
  MCTruthPartClassifier::ParticleOrigin  defOrigOfPhoton(const xAOD::TruthParticleContainer* m_xTruthParticleContainer ,const xAOD::TruthParticle*);
  MCTruthPartClassifier::ParticleOutCome defOutComeOfPhoton(const xAOD::TruthParticle*);
  //
  MCTruthPartClassifier::ParticleOrigin  defOrigOfNeutrino(const xAOD::TruthParticleContainer* m_xTruthParticleContainer ,const xAOD::TruthParticle*);
  //
  virtual std::tuple<unsigned int, const xAOD::TruthParticle*> defOrigOfParticle(const xAOD::TruthParticle*) override; 
  //
  MCTruthPartClassifier::ParticleOrigin  defHadronType(long);
  bool isHadron(const xAOD::TruthParticle*);
  MCTruthPartClassifier::ParticleType    defTypeOfHadron(long);
  MCTruthPartClassifier::ParticleOrigin  convHadronTypeToOrig(MCTruthPartClassifier::ParticleType pType);
  //
  const xAOD::TruthVertex* findEndVert(const xAOD::TruthParticle*);
  bool  isHardScatVrtx(const xAOD::TruthVertex* );
  //
  /** Return true if genParticle and truthParticle have the same pdgId, barcode and status **/
  const xAOD::TruthParticle* barcode_to_particle(const xAOD::TruthParticleContainer*,int );
  // 
  std::vector<const xAOD::TruthParticle*> findFinalStatePart(const xAOD::TruthVertex*);
  //
  void findAllJetMothers(const xAOD::TruthParticle* thePart,std::set<const xAOD::TruthParticle*>&);
  MCTruthPartClassifier::ParticleOrigin defJetOrig(std::set<const xAOD::TruthParticle*>);
  // 

  double partCharge(const xAOD::TruthParticle*);

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE) /*When no Athena Reconstruction packages expected*/
  bool genPartToCalo(const xAOD::CaloCluster* , const xAOD::TruthParticle* , bool, double&, bool& );
  const xAOD::TruthParticle* egammaClusMatch(const xAOD::CaloCluster*, bool );
#endif

private:
  //Data members for all cases
  const xAOD::TruthParticle* m_thePart;
  const xAOD::TruthParticle* m_Mother;
  MCTruthPartClassifier::ParticleOutCome  m_ParticleOutCome;

  int   m_MotherPDG;
  int   m_MotherStatus;
  long  m_MotherBarcode;
  int   m_NumOfParents;
  int   m_NumOfDaug;

  const xAOD::TruthVertex*  m_MothOriVert;

  bool m_isPrompt;

  const xAOD::TruthParticle* m_PhotonMother;
  int   m_PhotonMotherPDG;
  long  m_PhotonMotherBarcode;
  long  m_PhotonMotherStatus;

  const xAOD::TruthParticle* m_BkgElecMother;

  std::vector<const xAOD::TruthParticle*> m_tauFinalStatePart;

  std::string m_xaodTruthParticleContainerName ;
  long  m_barcodeShift;
  long  m_barcodeG4Shift;
  bool  m_LQpatch;

#ifndef GENERATIONBASE /*Disable when no recostruction packages are expected*/ 
  float m_deltaRMatch;
  float m_deltaPhi;
  float  m_NumOfSiHits;
  float m_probability;

  std::vector<const xAOD::TruthParticle*> m_egPartPtr;
  std::vector<float> m_egPartdR;
  std::vector<std::pair<MCTruthPartClassifier::ParticleType,MCTruthPartClassifier::ParticleOrigin> > m_egPartClas;

  std::vector< const xAOD::TrackParticle*>  m_cnvPhtTrkPtr;
  std::vector<const xAOD::TruthParticle*>   m_cnvPhtTrPart;
  std::vector<MCTruthPartClassifier::ParticleType>    m_cnvPhtPartType;
  std::vector<MCTruthPartClassifier::ParticleOrigin>  m_cnvPhtPartOrig;

  float m_deltaRMatchCut;
  float m_deltaPhiMatchCut;
  int   m_NumOfSiHitsCut; 
  float m_jetPartDRMatch;
#endif

#ifndef XAOD_ANALYSIS 
  std::string m_truthLinkVecName;             
#endif

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE) /*When no Athena Reconstruction packages expected*/
  ToolHandle< Trk::IParticleCaloExtensionTool >  m_caloExtensionTool;
  ToolHandle<xAOD::ITruthParticlesInConeTool> m_truthInConeTool;
  float m_FwdElectronTruthExtrEtaCut;
  float m_FwdElectronTruthExtrEtaWindowCut;
  float m_partExtrConeEta;
  float m_partExtrConePhi;
  bool  m_useCaching;
  float m_phtClasConePhi;
  float m_phtClasConeEta;
  float m_phtdRtoTrCut;
  float m_fwrdEledRtoTrCut; 
  bool  m_ROICone;
  bool  m_inclG4part;
  float m_pTChargePartCut;
  float m_pTNeutralPartCut; 
#endif

};
#endif  // MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIER_H 
