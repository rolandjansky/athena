/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIER_H
#define MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIER_H
/********************************************************************
NAME:     MCTruthClassifier.h 
PACKAGE:  atlasoff/PhysicsAnalysis/MCTruthClassifier
AUTHORS:  O. Fedin
CREATED:  Sep 2007
PURPOSE:            

MODIFIED: 02/07/2020 
AUTHOR: Sukanya Sinha (sukanya.sinha@cern.ch)
Documentation: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/MonteCarloTruthClassifier
********************************************************************/

// INCLUDE HEADER FILES:
#include "AsgTools/AsgTool.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
// EDM includes

#include "xAODTruth/TruthParticleContainerFwd.h"
#include "xAODTruth/TruthVertexFwd.h"

#ifndef XAOD_ANALYSIS
#include "GaudiKernel/ToolHandle.h"
#include "ParticlesInConeTools/ITruthParticlesInConeTool.h"
namespace HepMC {
 class GenParticle;
}

namespace Trk {
  class IParticleCaloExtensionTool;
}
#endif


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
    
  //Old EDM  
#ifndef XAOD_ANALYSIS
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const HepMC::GenParticle *) override; 
  bool compareTruthParticles(const HepMC::GenParticle *genPart, const xAOD::TruthParticle *truthPart);
#endif
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::TruthParticle *) override;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::TrackParticle *) override;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Electron* ) override;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Photon* ) override;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Muon* ) override; 
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::CaloCluster* ) override;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Jet*, bool DR ) override;

  virtual unsigned int classify(const xAOD::TruthParticle  *) override; 

  enum MCTC_bits { HadTau=0, Tau, hadron, frombsm, uncat, isbsm, isgeant, stable, totalBits };

  /// \brief These helper functions return the value that the respective bit is set to in \ref MCTruthClassifier

  static unsigned int isGeant(const unsigned int classify) { return std::bitset<MCTC_bits::totalBits> (classify).test(MCTC_bits::isgeant); }
  static unsigned int isBSM(const unsigned int classify) { return std::bitset<MCTC_bits::totalBits> (classify).test(MCTC_bits::isbsm); }
  static unsigned int fromBSM(const unsigned int classify) { return std::bitset<MCTC_bits::totalBits> (classify).test(MCTC_bits::frombsm); }

  /*! \brief This helper function returns the value -1 by checking the bit set in \ref MCTruthClassifier. 
   * It returns the value -1 if uncategorised, 0 if non-prompt, 1 if prompt
   * It also checks for prompt taus
   */

  static int isPrompt(const unsigned int classify, bool allow_prompt_tau_decays = true) {
    std::bitset<MCTC_bits::totalBits> res(classify);
    if (res.test(MCTC_bits::uncat)) return -1;
    bool fromPromptTau = res.test(MCTC_bits::Tau) && !res.test(MCTC_bits::HadTau);
    if (fromPromptTau) return int(allow_prompt_tau_decays);
    return !res.test(MCTC_bits::hadron);
    }


  virtual MCTruthPartClassifier::ParticleOutCome getParticleOutCome() override{return  m_ParticleOutCome;}

  virtual float getProbTrktoTruth() override{return m_probability;}

  virtual const xAOD::TruthParticle* getGenPart(const xAOD::TrackParticle *)  override;
  virtual const xAOD::TruthParticle* getGenPart()  override{return m_thePart;}
  const xAOD::TruthParticle* getMother(const xAOD::TruthParticle*);
  virtual const xAOD::TruthParticle* getMother() override{return m_Mother;}

  virtual int   getMotherPDG() override{return m_MotherPDG; }
  virtual int   getMotherBarcode() override{return m_MotherBarcode; }

  virtual const xAOD::TruthParticle* getPhotonMother() override{return m_PhotonMother; }
  virtual int   getPhotonMotherPDG() override{return m_PhotonMotherPDG;}
  virtual int   getPhotonMotherBarcode() override{return m_PhotonMotherBarcode;}

  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>
                         checkOrigOfBkgElec(const xAOD::TruthParticle* thePart) override;

  virtual const xAOD::TruthParticle* getBkgElecMother() override{return m_BkgElecMother;}

  virtual std::vector<const xAOD::TruthParticle*>* getTauFinalState()  override{return &m_tauFinalStatePart; }
 
  virtual float getdeltaRMatch() override{return m_deltaRMatch; }
  virtual float getdeltaPhiMatch() override{return m_deltaPhi; }
  virtual uint8_t   getNumOfSiHits() override{return m_NumOfSiHits; }

  virtual std::vector<const xAOD::TruthParticle*>  getEGPartPntr()  override{return m_egPartPtr; } 
  virtual std::vector<float> getEGPartdR() override{return m_egPartdR; }
  virtual std::vector<std::pair<MCTruthPartClassifier::ParticleType,MCTruthPartClassifier::ParticleOrigin> >  getEGPartClas() override{return m_egPartClas; }

  virtual std::vector<const xAOD::TrackParticle*>   getCnvPhotTrkPtr() override{return m_cnvPhtTrkPtr; }
  virtual std::vector<const xAOD::TruthParticle*>   getCnvPhotTrkToTruthPart() override{return m_cnvPhtTrPart; }
  virtual std::vector<MCTruthPartClassifier::ParticleType>   getCnvPhotPartType()  override{return  m_cnvPhtPartType; }
  virtual std::vector<MCTruthPartClassifier::ParticleOrigin> getCnvPhotPartOrig()  override{return  m_cnvPhtPartOrig; }

 private:

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
   MCTruthPartClassifier::ParticleOrigin  defOrigOfNeutrino(const xAOD::TruthParticleContainer* m_xTruthParticleContainer ,const xAOD::TruthParticle*) ;
   //MCTruthPartClassifier::ParticleOrigin
   virtual unsigned int defOrigOfParticle(const xAOD::TruthParticle*) override;
   //
   MCTruthPartClassifier::ParticleOrigin  defHadronType(long);
   bool isHadron(const xAOD::TruthParticle*);
   MCTruthPartClassifier::ParticleType    defTypeOfHadron(long);
   MCTruthPartClassifier::ParticleOrigin  convHadronTypeToOrig(MCTruthPartClassifier::ParticleType pType);
   //
   const xAOD::TruthVertex* findEndVert(const xAOD::TruthParticle*);
   bool  isHardScatVrtx(const xAOD::TruthVertex* );
   //
   std::vector<const xAOD::TruthParticle*> findFinalStatePart(const xAOD::TruthVertex*);
   //
   double partCharge(const xAOD::TruthParticle*);
#ifndef XAOD_ANALYSIS
   bool genPartToCalo(const xAOD::CaloCluster* , const xAOD::TruthParticle* , bool, double&, bool& );
   const xAOD::TruthParticle* egammaClusMatch(const xAOD::CaloCluster*, bool );
#endif
   //
   void findAllJetMothers(const xAOD::TruthParticle* thePart,std::set<const xAOD::TruthParticle*>&);
   double deltaR(const xAOD::TruthParticle& v1, const xAOD::Jet & v2) ;
   MCTruthPartClassifier::ParticleOrigin defJetOrig(std::set<const xAOD::TruthParticle*>);
   //
  
   /** Return true if genParticle and truthParticle have the same pdgId, barcode and status **/
   const xAOD::TruthParticle* barcode_to_particle(const xAOD::TruthParticleContainer*,int );
  
   const xAOD::TruthParticle* m_thePart;
   const xAOD::TruthParticle* m_Mother;
  
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

   float m_deltaRMatch;
   float m_deltaPhi;
   float  m_NumOfSiHits;
   float m_probability;
   
   std::vector<const xAOD::TruthParticle*> m_egPartPtr;
   std::vector<float> m_egPartdR;
   std::vector<std::pair<MCTruthPartClassifier::ParticleType,MCTruthPartClassifier::ParticleOrigin> > m_egPartClas;

   MCTruthPartClassifier::ParticleOutCome  m_ParticleOutCome;

   std::vector< const xAOD::TrackParticle*>  m_cnvPhtTrkPtr;
   std::vector<const xAOD::TruthParticle*>   m_cnvPhtTrPart;
   std::vector<MCTruthPartClassifier::ParticleType>    m_cnvPhtPartType;
   std::vector<MCTruthPartClassifier::ParticleOrigin>  m_cnvPhtPartOrig;

//------------------------------------------------------------------------
//      configurable data members
//------------------------------------------------------------------------

   
#ifndef XAOD_ANALYSIS
   ToolHandle< Trk::IParticleCaloExtensionTool >  m_caloExtensionTool;
   ToolHandle<xAOD::ITruthParticlesInConeTool> m_truthInConeTool;
   std::string m_truthLinkVecName;             
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
#endif

   std::string m_xaodTruthParticleContainerName ;
   float m_deltaRMatchCut;
   float m_deltaPhiMatchCut;
   int   m_NumOfSiHitsCut;
   float m_pTChargePartCut;
   float m_pTNeutralPartCut;
   long  m_barcodeShift;
   long  m_barcodeG4Shift;
   float m_jetPartDRMatch;
   bool  m_inclG4part;
   bool  m_inclEgammaPhoton;
   bool  m_inclEgammaFwrdEle;
   bool  m_LQpatch;

};
#endif  // MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIER_H 
