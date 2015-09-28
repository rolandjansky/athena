/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
//
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
//
// EDM includes
#include "xAODTruth/TruthParticleContainer.h"
//Truth PArticles in cone
#include "ParticlesInConeTools/ITruthParticlesInConeTool.h"

namespace HepMC {
 class GenParticle;
}
// CLHEP
#include "HepPDT/ParticleDataTable.hh"

namespace Trk {
  class IParticleCaloExtensionTool;
}

class MCTruthClassifier : public AthAlgTool, virtual public IMCTruthClassifier {

 public:
  // constructor 
  MCTruthClassifier(const std::string& type, const std::string& name, const IInterface* parent);

  // destructor 
  ~MCTruthClassifier();  

  // Gaudi algorithm hooks
  
  StatusCode initialize();
  virtual StatusCode execute();
  StatusCode finalize();
    
  //temporary interface to old EDM  
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const HepMC::GenParticle *); 

  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::TruthParticle *);     
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::TrackParticle *);
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Electron* );     
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Photon* );     
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Muon* );     
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::CaloCluster* );
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(const xAOD::Jet*, bool DR );

  // It is just to keep backward compatibility  - don't use this  methode. 
  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> trackClassifier(const xAOD::TrackParticle *);


  MCTruthPartClassifier::ParticleOutCome getParticleOutCome(){return  m_ParticleOutCome;}

  float getProbTrktoTruth(){return m_probability;}

  const xAOD::TruthParticle* getGenPart(const xAOD::TrackParticle *);
  const xAOD::TruthParticle* getGenPart(){return m_thePart;}

  const xAOD::TruthParticle* getMother(const xAOD::TruthParticle*);
  const xAOD::TruthParticle* getMother(){return m_Mother;}

  int   getMotherPDG(){return m_MotherPDG;};
  int   getMotherBarcode(){return m_MotherBarcode;};

  const xAOD::TruthParticle* getPhotonMother(){return m_PhotonMother;}
  int   getPhotonMotherPDG(){return m_PhotonMotherPDG;}
  int   getPhotonMotherBarcode(){return m_PhotonMotherBarcode;}

  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>
                         checkOrigOfBkgElec(const xAOD::TruthParticle* thePart);

  const xAOD::TruthParticle* getBkgElecMother(){return m_BkgElecMother;}

  std::vector<const xAOD::TruthParticle*>* getTauFinalState(){return &m_tauFinalStatePart;}
 

  float getdeltaRMatch(){return m_deltaRMatch;}
  void  setdeltaRMatchCut(float m_deltaRMatch){m_deltaRMatchCut=m_deltaRMatch; return;}
  float getdeltaPhiMatch(){return m_deltaPhi;}
  void  setdeltaPhiMatchCut(float m_deltaPhi){m_deltaPhiMatchCut=m_deltaPhi; return;}
      
  void  setNumOfSiHitsCut(uint8_t m_NumOfSiHits){m_NumOfSiHitsCut=m_NumOfSiHits; return;}
  uint8_t   getNumOfSiHits(){return m_NumOfSiHits;}

  void  setPhtdRMatchToTrCut(float m_phtdRtoTr){m_phtdRtoTrCut=m_phtdRtoTr; return;}
  void  setFwrdEledRMatchToTrCut(float m_fwrdRtoTr){m_fwrdEledRtoTrCut=m_fwrdRtoTr; return;}

  std::vector<const xAOD::TruthParticle*>  getEGPartPntr(){return m_egPartPtr;} 
  std::vector<float> getEGPartdR(){return m_egPartdR;}
  std::vector<std::pair<MCTruthPartClassifier::ParticleType,MCTruthPartClassifier::ParticleOrigin> >  getEGPartClas(){return m_egPartClas;}

  std::vector<const xAOD::TrackParticle*>   getCnvPhotTrkPtr()        {return m_cnvPhtTrkPtr;}
  std::vector<const xAOD::TruthParticle*>   getCnvPhotTrkToTruthPart(){return m_cnvPhtTrPart;}
  std::vector<MCTruthPartClassifier::ParticleType>   getCnvPhotPartType() {return  m_cnvPhtPartType;}
  std::vector<MCTruthPartClassifier::ParticleOrigin> getCnvPhotPartOrig() {return  m_cnvPhtPartOrig;}

 private:

   float detEta(float x, float y ) {return fabs(x-y);}
   float detPhi(float , float );
   float rCone (float x, float y ){return sqrt(x*x + y*y);}
   //
   MCTruthPartClassifier::ParticleType    defTypeOfElectron(MCTruthPartClassifier::ParticleOrigin);
   MCTruthPartClassifier::ParticleOrigin  defOrigOfElectron(const xAOD::TruthParticleContainer* m_xTruthParticleContainer ,const xAOD::TruthParticle*);
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
   bool genPartToCalo(const xAOD::CaloCluster* , const xAOD::TruthParticle* , bool, double&, bool& );
   const xAOD::TruthParticle* egammaClusMatch(const xAOD::CaloCluster*, bool );
   //
   void findAllJetMothers(const xAOD::TruthParticle* thePart,std::set<const xAOD::TruthParticle*>&);
   MCTruthPartClassifier::ParticleOrigin defJetOrig(std::set<const xAOD::TruthParticle*>);
   //
   inline double deltaR(const xAOD::TruthParticle& v1, const xAOD::Jet & v2) {
     double dphi = std::fabs(v1.phi()-v2.phi()) ;
     dphi = (dphi<=M_PI)? dphi : 2*M_PI-dphi;
     double deta = std::fabs(v1.eta()-v2.eta()) ;
     return std::sqrt(dphi*dphi+deta*deta) ;
   }
  
   /** Return true if genParticle and truthParticle have the same pdgId, barcode and status **/
   const xAOD::TruthParticle* barcode_to_particle(const xAOD::TruthParticleContainer*,int );
  
   //for old EDM
   bool compareTruthParticles(const HepMC::GenParticle *genPart, const xAOD::TruthParticle *truthPart);


   //const xAOD::TrackParticleContainer  * m_trackTES;
   //const xAOD::TruthParticleContainer  * m_xTruthParticleContainer;
 
   const xAOD::TruthParticle* m_thePart;
   const xAOD::TruthParticle* m_Mother;
  
   int   m_MotherPDG;
   int   m_MotherStatus;
   long  m_MotherBarcode;
   int   m_NumOfParents;
   int   m_NumOfDaug;

   const xAOD::TruthVertex*  m_MothOriVert;
   const xAOD::TruthVertex*  m_partOriVert;

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

   //float m_phtdRtoTr;
   //bool  m_first;
   //bool  m_useBremRefitTrk;

   
   std::vector<const xAOD::TruthParticle*> m_egPartPtr;
   std::vector<float> m_egPartdR;
   std::vector<std::pair<MCTruthPartClassifier::ParticleType,MCTruthPartClassifier::ParticleOrigin> > m_egPartClas;


   MCTruthPartClassifier::ParticleOutCome  m_ParticleOutCome;

   std::vector< const xAOD::TrackParticle*>  m_cnvPhtTrkPtr;
   std::vector<const xAOD::TruthParticle*>   m_cnvPhtTrPart;
   std::vector<MCTruthPartClassifier::ParticleType>    m_cnvPhtPartType;
   std::vector<MCTruthPartClassifier::ParticleOrigin>  m_cnvPhtPartOrig;

   const HepPDT::ParticleDataTable* m_particleTable;
   
#ifndef XAOD_ANALYSIS
   ToolHandle< Trk::IParticleCaloExtensionTool >  m_caloExtensionTool;
#endif
   ToolHandle<xAOD::ITruthParticlesInConeTool> m_truthInConeTool;
//------------------------------------------------------------------------
//      configurable data members
//------------------------------------------------------------------------

   std::string m_trackParticleContainerName;
   std::string m_trackGSFParticleContainerName;
   std::string m_xaodTruthParticleContainerName ;
   std::string m_truthLinkVecName;             
   
   float m_deltaRMatchCut;
   float m_deltaPhiMatchCut;
   int   m_NumOfSiHitsCut;

   float m_phtdRtoTrCut;

   float m_fwrdEledRtoTrCut; 
   float m_pTChargePartCut;
   float m_pTNeutralPartCut;
   float m_partExtrConeEta;
   float m_partExtrConePhi;
   float m_phtClasConePhi;
   float m_phtClasConeEta;
   long  m_barcodeShift;
   long  m_barcodeG4Shift;

   float m_FwdElectronTruthExtrEtaCut;
   float m_FwdElectronTruthExtrEtaWindowCut;
   float m_jetPartDRMatch;
   bool  m_inclG4part;
   bool  m_ROICone;
   bool  m_inclEgammaPhoton;
   bool  m_inclEgammaFwrdEle;
   bool  m_LQpatch;
   bool  m_useCaching;

};
#endif  // MCTRUTHCLASSIFIER_MCTRUTHCLASSIFIER_H 
