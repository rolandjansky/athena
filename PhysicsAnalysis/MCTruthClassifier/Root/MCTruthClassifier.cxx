/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************************

NAME:     MCTruthClassifier.cxx
PACKAGE:  atlasoff/PhysicsAnalysis/MCTruthClassifier

AUTHORS:  O. Fedin
CREATED:  Sep 2007

PURPOSE:  to classify  truth particles according to their origin. Based on
          the truth particle classification the tool provide classification of
          ID and combined muon tracks, egamma electrons (including forward
          electrons) and egamma photons. Both AOD and ESD files can be used.
          See  for details:
    https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EgammaMCTruthClassifier
Updated:
********************************************************************************/

//
#include "MCTruthClassifier/MCTruthClassifier.h"
//

// xAOD EDM includes
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODJet/Jet.h"

// For making PID selections easier
#include "TruthUtils/PIDHelpers.h"

#ifndef XAOD_ANALYSIS
//Athena only includes
#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "HepMC/GenParticle.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
//
#endif

//
//std includes
#include <cmath>

using namespace MCTruthPartClassifier;
using std::abs;

// -----------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------
MCTruthClassifier::MCTruthClassifier(const std::string& type)
  :  asg::AsgTool(type)
#ifndef XAOD_ANALYSIS //Add Athena Specific parts in the init list of the constructor
  ,
     m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
     m_truthInConeTool ("xAOD::TruthParticlesInConeTool/TruthParticlesInConeTool")
#endif
{

#ifndef XAOD_ANALYSIS // Add properties used/available only in Athena
  declareProperty("ParticleCaloExtensionTool",   m_caloExtensionTool );
  declareProperty("TruthInConeTool",               m_truthInConeTool );
  declareProperty("xAODTruthLinkVector"            , m_truthLinkVecName="xAODTruthLinks");
  declareProperty("FwdElectronTruthExtrEtaCut" , m_FwdElectronTruthExtrEtaCut = 2.4,
                  "Cut on the eta of the truth Particles to be extrapolated for Fwd electrons");
  declareProperty("FwdElectronTruthExtrEtaWindowCut" , m_FwdElectronTruthExtrEtaWindowCut = 0.15,
                  "Cut on the delta eta of the truth Particles to be extrapolated for Fwd electrons and the current FwdElectron");
  declareProperty("partExtrConePhi"   , m_partExtrConePhi  = 0.4);
  declareProperty("partExtrConeEta"   , m_partExtrConeEta  = 0.2);
  declareProperty("phtClasConePhi"  , m_phtClasConePhi  = 0.05);
  declareProperty("phtClasConeEta"  , m_phtClasConeEta  = 0.025);
  declareProperty( "useCaching", m_useCaching=true);
  declareProperty("phtdRtoTrCut"     , m_phtdRtoTrCut     = 0.1);
  declareProperty("fwrdEledRtoTrCut" , m_fwrdEledRtoTrCut  = 0.15);
  declareProperty("ROICone"           , m_ROICone  = false);
#endif
  //
  //Properties Available in Athena, AtheAnalysisBase and AnalysisBase
  declareProperty("xAODTruthParticleContainerName" , m_xaodTruthParticleContainerName  = "TruthParticles");
  declareProperty("deltaRMatchCut"   , m_deltaRMatchCut   = 0.2);
  declareProperty("deltaPhiMatchCut" , m_deltaPhiMatchCut = 0.2);
  declareProperty("NumOfSiHitsCut"   , m_NumOfSiHitsCut   = 3);
  declareProperty("inclEgammaFwrdEle" , m_inclEgammaFwrdEle  = true);
  declareProperty("inclEgammaPhoton"  , m_inclEgammaPhoton   = true);
  declareProperty("pTChargePartCut"  , m_pTChargePartCut  = 1.0);
  declareProperty("pTNeutralPartCut" , m_pTNeutralPartCut = 0.);
  declareProperty("inclG4part"       , m_inclG4part       = false);
  declareProperty( "LQpatch", m_LQpatch=false);
  declareProperty("jetPartDRMatch" , m_jetPartDRMatch = 0.4);
  declareProperty( "barcodeG4Shift", m_barcodeG4Shift=200001);

  //Other non-static variables init
  m_thePart=0;
  m_Mother=0;
  m_MotherPDG=0;
  m_MotherStatus=0;
  m_MotherBarcode=0;
  m_NumOfParents=0;
  m_NumOfDaug=0;
  m_MothOriVert=0;
  m_isPrompt=false;
  m_PhotonMother=0;
  m_PhotonMotherPDG=0;
  m_PhotonMotherBarcode=0;
  m_PhotonMotherStatus=0;
  m_BkgElecMother=0;
  m_deltaRMatch=-999.0;
  m_deltaPhi=-999.0;
  m_NumOfSiHits=0;
  m_probability=0;
  m_ParticleOutCome=NonDefinedOutCome;
  m_barcodeShift=1000000;
}

// DESTRUCTOR:
MCTruthClassifier::~MCTruthClassifier(){
}

// INITIALIZE METHOD:
StatusCode MCTruthClassifier::initialize(){

  ATH_MSG_INFO( " Initializing MCTruthClassifier" );

  //define barcode scheme
  m_barcodeShift=1000000;
  if(m_barcodeG4Shift==1000001) {m_barcodeShift=10000000;}

#ifndef XAOD_ANALYSIS //Tools making sense only for the Athena only environment
  if( !m_caloExtensionTool.empty() && m_caloExtensionTool.retrieve().isFailure() ) {
    ATH_MSG_WARNING( "Cannot retrieve extrapolateToCaloTool " << m_caloExtensionTool
                     << " - will not perform extrapolation." );
  }
  if(m_truthInConeTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Cannot retrieve Truth in cone Tool " << m_truthInConeTool);
  }
#endif

  return StatusCode::SUCCESS;
}


// FINALIZE METHOD:
StatusCode  MCTruthClassifier::finalize(){

  return StatusCode::SUCCESS;
}

// ATHENA EXECUTE METHOD:
StatusCode MCTruthClassifier::execute(){
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::TruthParticle  *thePart){
  //---------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing particleTruthClassifier" );

  ParticleType   partType = Unknown;
  ParticleOrigin partOrig = NonDefined;
  m_ParticleOutCome       = NonDefinedOutCome;
  m_tauFinalStatePart.clear();

  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0;
  m_PhotonMotherPDG     = 0;
  m_PhotonMotherStatus  = 0;
  m_PhotonMotherBarcode = 0;
  m_ParticleOutCome     = UnknownOutCome;
  m_BkgElecMother       = 0;

  m_isPrompt            = false;

  m_thePart             = thePart;
  //retrieve collection and get a pointer

  if(!thePart) return std::make_pair(partType,partOrig);

  const xAOD::TruthParticleContainer  * xTruthParticleContainer;
  StatusCode sc = evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName);
  if (sc.isFailure()||!xTruthParticleContainer){
    ATH_MSG_WARNING( "No  xAODTruthParticleContainer "<<m_xaodTruthParticleContainerName<<" found" );
    return std::make_pair(partType,partOrig);
  }

  ATH_MSG_DEBUG( "xAODTruthParticleContainer  " << m_xaodTruthParticleContainerName<<" successfully retrieved " );

  int iParticlePDG = thePart->pdgId();
  //status=10902 in Pythia?
  if(thePart->status()!=1&&thePart->status()!=2&&thePart->status()!=10902) {
    return std::make_pair(GenParticle,partOrig);
  }
  bool isPartHadr  = isHadron(thePart);
  if(thePart->status()==2&&(abs(iParticlePDG)!=15&&!isPartHadr)) {
    return std::make_pair(GenParticle,partOrig);
  }

  //SUSY datasets: tau(satus==2)->tau(satus==2)
  if(thePart->status()==2&&abs(iParticlePDG)==15){
    const xAOD::TruthVertex*   endVert = thePart->decayVtx();
    if(endVert!=0){
      int numOfDaught=endVert->nOutgoingParticles();
      if(numOfDaught==1&&abs(endVert->outgoingParticle(0)->pdgId())==15) {
        return std::make_pair(GenParticle,partOrig);
      }
    }
  }

  if(thePart->status()==1&&MC::PID::isSUSY(iParticlePDG)) {
    return std::make_pair(SUSYParticle,partOrig);
  }

  if(thePart->status()==1&&MC::PID::isBSM(iParticlePDG)) {
    return std::make_pair(OtherBSMParticle,partOrig);
  }

  if(thePart->status()==10902&&(abs(iParticlePDG)!=11&&abs(iParticlePDG)!=13&&abs(iParticlePDG)!=15&&abs(iParticlePDG)!=22)&&!isPartHadr) {
    return std::make_pair(GenParticle,partOrig);
  }

  if(abs(iParticlePDG)>1000000000)  {
    return std::make_pair(NuclFrag,partOrig);
  }
  if(abs(iParticlePDG)!=11&&abs(iParticlePDG)!=13&&abs(iParticlePDG)!=15&&
     abs(iParticlePDG)!=12&&abs(iParticlePDG)!=14&&abs(iParticlePDG)!=16&&
     abs(iParticlePDG)!=22&&!isPartHadr) {
    return std::make_pair(partType,partOrig);
  }
  //don't consider  generator particles

  const xAOD::TruthVertex* partOriVert=thePart->hasProdVtx() ? thePart->prodVtx():0;

  if( partOriVert!=0 ) {
    for (unsigned int ipIn=0; ipIn<partOriVert->nIncomingParticles(); ++ipIn) {
      const xAOD::TruthParticle* theMoth=partOriVert->incomingParticle(ipIn);
      if(!theMoth) continue;
      m_MotherPDG      = theMoth->pdgId();
      m_MotherBarcode  = theMoth->barcode();
      m_MotherStatus   = theMoth->status();
    }
  }

  if(partOriVert==0&&thePart->barcode()>m_barcodeShift) {
    return std::make_pair(NonPrimary,partOrig);
  }
  if(partOriVert==0 && abs(iParticlePDG)==11)  {
    // to define electron out come  status
    partOrig =  defOrigOfElectron(xTruthParticleContainer,thePart);
    return std::make_pair(UnknownElectron,partOrig);
  } else  if(partOriVert==0 && abs(iParticlePDG)==13) {
    // to define electron out come  status
    partOrig =  defOrigOfMuon(xTruthParticleContainer,thePart);
    return std::make_pair(UnknownMuon,partOrig);
  } else  if(partOriVert==0 && abs(iParticlePDG)==15) {
    // to define electron out come  status
    partOrig =  defOrigOfTau(xTruthParticleContainer,thePart);
    return std::make_pair(UnknownTau,partOrig);
  } else  if(partOriVert==0 && abs(iParticlePDG)==22) {
    // to define photon out come
    partOrig =  defOrigOfPhoton(xTruthParticleContainer,thePart);
    return std::make_pair(UnknownPhoton,partOrig);
  }
  else if(partOriVert==0 && (abs(iParticlePDG)==12||abs(iParticlePDG)==14||abs(iParticlePDG)==16))  {
    // to define neutrino outcome
    m_ParticleOutCome=NonInteract;
    return std::make_pair(Neutrino,partOrig);
  }

  if(thePart->barcode()%m_barcodeShift == m_MotherBarcode%m_barcodeShift) return std::make_pair(NonPrimary,partOrig);

  if(isPartHadr)  return std::make_pair(Hadron,partOrig);

  if(partOriVert!=0&&m_MotherPDG==0&&partOriVert->nOutgoingParticles()==1&&partOriVert->nIncomingParticles()==0) {
    if(abs(iParticlePDG)==11){m_ParticleOutCome=defOutComeOfElectron(thePart); return std::make_pair(IsoElectron,SingleElec);}
    if(abs(iParticlePDG)==13){m_ParticleOutCome=defOutComeOfMuon(thePart);     return std::make_pair(IsoMuon,SingleMuon);    }
    if(abs(iParticlePDG)==15){m_ParticleOutCome=defOutComeOfTau(thePart);      return std::make_pair(IsoTau,SingleTau);      }
    if(    iParticlePDG ==22){m_ParticleOutCome=defOutComeOfPhoton(thePart);   return std::make_pair(IsoPhoton,SinglePhot);  }
  }

  if( m_MotherPDG==iParticlePDG && m_MotherStatus==3 && thePart->status()==10902)  return std::make_pair(GenParticle,partOrig);

  if(abs(iParticlePDG)==11){
    partOrig =  defOrigOfElectron(xTruthParticleContainer,thePart);
    partType =  defTypeOfElectron(partOrig);
  } else  if(abs(iParticlePDG)==13) {
    partOrig =  defOrigOfMuon(xTruthParticleContainer,thePart);
    partType =  defTypeOfMuon(partOrig);
  } else  if(abs(iParticlePDG)==15) {
    partOrig =  defOrigOfTau(xTruthParticleContainer,thePart);
    partType =  defTypeOfTau(partOrig);
  } else  if(abs(iParticlePDG)==22){
    partOrig =  defOrigOfPhoton(xTruthParticleContainer,thePart);
    partType =  defTypeOfPhoton( partOrig);
  } else  if(abs(iParticlePDG)==12||abs(iParticlePDG)==14||abs(iParticlePDG)==16)  {
    partOrig =  defOrigOfNeutrino(xTruthParticleContainer,thePart);
    partType = Neutrino;
  }


  ATH_MSG_DEBUG( "particleTruthClassifier  succeeded " );
  return std::make_pair(partType,partOrig);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::TrackParticle *trkPtr){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing trackClassifier" );

  m_thePart             = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0;
  m_PhotonMotherPDG     = 0;
  m_PhotonMotherStatus  = 0;
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_isPrompt            = false;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0;
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;

  m_thePart=getGenPart(trkPtr);

  if(!m_thePart)   return std::make_pair(parttype,partorig);

  ATH_MSG_DEBUG( "trackClassifier  succeeded " );
  return particleTruthClassifier(m_thePart);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Electron* elec){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing egamma electron Classifier" );

  m_thePart             = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0;
  m_PhotonMotherPDG     = 0;
  m_PhotonMotherStatus  = 0;
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0;
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;

  m_isPrompt            = false;

  m_egPartPtr.clear();
  m_egPartdR.clear();
  m_egPartClas.clear();

  if(elec->author()!=xAOD::EgammaParameters::AuthorFwdElectron){
    const xAOD::TrackParticle* trkPtr=elec->trackParticle();
    if(!trkPtr)     {return std::make_pair(parttype,partorig);}
    m_thePart=getGenPart(trkPtr);
  }
  else {
#ifndef XAOD_ANALYSIS //Fwd electron available only in Athena
    const xAOD::CaloCluster* clus=elec->caloCluster();
    m_thePart  = egammaClusMatch(clus,true);
#else
    ATH_MSG_WARNING( "Forward Electron classification using extrapolation to Calo is available only in Athena , check your enviroment. " );
#endif
  }
  if(!m_thePart)  {return std::make_pair(parttype,partorig);}
  ATH_MSG_DEBUG( "egamma electron Classifier  succeeded " );
  return particleTruthClassifier(m_thePart);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Photon* phot){
  //-----------------------------------------------------------------------------------------


  ATH_MSG_DEBUG( "Executing egamma photon Classifier" );

  m_thePart             = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0;
  m_PhotonMotherPDG     = 0;
  m_PhotonMotherStatus  = 0;
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0;
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;

  m_isPrompt            = false;

  m_cnvPhtTrkPtr.clear();
  m_cnvPhtTrPart.clear();
  m_cnvPhtPartType.clear();
  m_cnvPhtPartOrig.clear();

  m_egPartPtr.clear();
  m_egPartdR.clear();
  m_egPartClas.clear();

  const xAOD::CaloCluster* clus=phot->caloCluster();
  if(!clus) return std::make_pair(parttype,partorig);
  if( std::fabs(clus->eta())> 10.0 ||std::fabs(clus->phi())> 6.28||(clus->et())<=0.) return std::make_pair(parttype,partorig);

  const xAOD::Vertex* VxCvPtr=phot->vertex();

  if(VxCvPtr!=0) {
    for(int itrk=0; itrk<(int) VxCvPtr->nTrackParticles(); itrk++){
      if(itrk>1) continue;
      const xAOD::TrackParticle* trkPtr  = VxCvPtr->trackParticle(itrk);
      if(!trkPtr)  continue;
      m_cnvPhtTrkPtr.push_back(trkPtr);
      const xAOD::TruthParticle* thePart=getGenPart(trkPtr);

      std::pair<ParticleType,ParticleOrigin>  classif=particleTruthClassifier(thePart);
      m_cnvPhtTrPart.push_back(thePart);

      m_cnvPhtPartType.push_back(classif.first);
      m_cnvPhtPartOrig.push_back(classif.second);
    } // end cycle itrk

  } // VxCvPtr!=0

#ifndef XAOD_ANALYSIS
  m_thePart = egammaClusMatch(clus,false);
#else
  ATH_MSG_WARNING( "Photon  Classification using extrapolation to Calo is  available only in Athena , check your enviroment. " );
#endif
  if(!m_thePart)  {return std::make_pair(parttype,partorig);}
  ATH_MSG_DEBUG( "egamma photon  Classifier  succeeded " );

  return particleTruthClassifier(m_thePart);

}
//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Muon* mu ){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing muon  Classifier" );

  m_thePart             = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0;
  m_PhotonMotherPDG     = 0;
  m_PhotonMotherStatus  = 0;
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_isPrompt            = false;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0;
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;

  const xAOD::TrackParticle* trkPtr=0;

  if( mu->primaryTrackParticleLink().isValid())                 trkPtr = *mu->primaryTrackParticleLink();
  else if( mu->combinedTrackParticleLink().isValid() )          trkPtr = *mu->combinedTrackParticleLink();
  else if (mu->inDetTrackParticleLink().isValid())              trkPtr = *mu->inDetTrackParticleLink();
  else if( mu->muonSpectrometerTrackParticleLink().isValid() )  trkPtr = *mu->muonSpectrometerTrackParticleLink();

  if(!trkPtr)   return std::make_pair(parttype,partorig);

  m_thePart=getGenPart(trkPtr);
  if(!m_thePart)  return std::make_pair(parttype,partorig);

  ATH_MSG_DEBUG( "muon Classifier  succeeded " );
  return particleTruthClassifier(m_thePart);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::CaloCluster* clus){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing egamma photon Classifier with cluster Input" );

  m_thePart             = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0;
  m_PhotonMotherPDG     = 0;
  m_PhotonMotherStatus  = 0;
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0;
  m_probability         = 0;
  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;
  m_isPrompt            = false;
  m_egPartPtr.clear();
  m_egPartdR.clear();
  m_egPartClas.clear();

  if(!clus) return std::make_pair(parttype,partorig);
  if( std::fabs(clus->eta())> 10.0 ||std::fabs(clus->phi())> M_PI||(clus->et())<=0.) return std::make_pair(parttype,partorig);

#ifndef XAOD_ANALYSIS
  m_thePart = egammaClusMatch(clus,false);
#else
  ATH_MSG_WARNING( "Cluster  Classification using extrapolation to Calo is available only in Athena , check your enviroment. " );
#endif

  if(!m_thePart)  {return std::make_pair(parttype,partorig);}
  ATH_MSG_DEBUG( "Calo Cluster  Classifier  succeeded " );

  return particleTruthClassifier(m_thePart);
}
//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Jet* jet, bool DR = true){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing Classifier with jet Input" );

  m_thePart             = 0;
  m_deltaRMatch         =-999.;
  m_ParticleOutCome           = UnknownOutCome;
  ParticleType   parttype     = UnknownJet;
  ParticleOrigin partorig     = NonDefined;
  ParticleType   tempparttype = UnknownJet;
  std::set<const xAOD::TruthParticle*>      allJetMothers;
  std::pair<ParticleType,ParticleOrigin>   res;

  if(!jet) return std::make_pair(parttype,partorig);

  allJetMothers.clear();

  if (!DR) {

  } else {
    // use a DR matching scheme (default)
    // retrieve collection and get a pointer
    const xAOD::TruthParticleContainer  * xTruthParticleContainer;
    StatusCode sc = evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName);
    if (sc.isFailure()||!xTruthParticleContainer){
      ATH_MSG_WARNING( "No  xAODTruthParticleContainer "<<m_xaodTruthParticleContainerName<<" found" );
      return std::make_pair(parttype,partorig);
    }

    ATH_MSG_DEBUG( "xAODTruthParticleContainer  " << m_xaodTruthParticleContainerName<<" successfully retrieved " );

    // find the matching truth particles
    for(const auto thePart : *xTruthParticleContainer){
      // do not look at intermediate particles
      if(thePart->status()!=2&&thePart->status()!=3&&thePart->status()!=10902) continue;
      // match truth particles to the jet
      if (deltaR((*thePart),(*jet)) > m_jetPartDRMatch) continue;
      // determine jet origin
      findAllJetMothers(thePart,allJetMothers);
      // determine jet type
      if(thePart->status()==3) continue;
      // determine jet type
      tempparttype = particleTruthClassifier(thePart).first;
      if(tempparttype==Hadron) tempparttype=defTypeOfHadron(thePart->pdgId());
      // classify the jet
      if (tempparttype==BBbarMesonPart || tempparttype==BottomMesonPart || tempparttype==BottomBaryonPart)
            {parttype = BJet;}
      else if (tempparttype==CCbarMesonPart || tempparttype==CharmedMesonPart || tempparttype==CharmedBaryonPart )
            {if (parttype==BJet) {} else { parttype = CJet;}}
      else if (tempparttype==StrangeBaryonPart||tempparttype==LightBaryonPart||tempparttype==StrangeMesonPart||tempparttype==LightMesonPart)
            {if (parttype==BJet||parttype==CJet) {} else { parttype = LJet;}}
      else {if (parttype==BJet||parttype==CJet||parttype==LJet) {} else { parttype = UnknownJet;}}

    } // end loop over truth particles

  }// end DR scheme

  // clasify the jet origin
  partorig = defJetOrig(allJetMothers);

  allJetMothers.clear();

  ATH_MSG_DEBUG( " jet Classifier succeeded" );
  return std::make_pair(parttype,partorig);
}

double MCTruthClassifier::deltaR(const xAOD::TruthParticle& v1, const xAOD::Jet & v2) {
  double dphi = std::fabs(v1.phi()-v2.phi()) ;
  dphi = (dphi<=M_PI)? dphi : 2*M_PI-dphi;
  double deta = std::fabs(v1.eta()-v2.eta()) ;
  return std::sqrt(dphi*dphi+deta*deta) ;
}

//---------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defJetOrig(std::set<const xAOD::TruthParticle*> allJetMothers){
  //---------------------------------------------------------------------------------
  ParticleOrigin             partOrig = NonDefined;
  std::set<const xAOD::TruthParticle*>::iterator it;

  for (it=allJetMothers.begin(); it!=allJetMothers.end(); ++it) {
    int pdg = abs((*it)->pdgId());
    if ( pdg == 6  )                        { partOrig = top;         }
    if ( pdg == 23 )                        { partOrig = ZBoson;      }
    if ( pdg == 24 && !(partOrig==top) )    { partOrig = WBoson;      }
    if ( (pdg < 6 || pdg == 21) &&
         !( partOrig==top    ||
            partOrig==ZBoson ||
            partOrig==WBoson    ) )         { partOrig = QCD;         }
    if ( pdg == 25 )                        { partOrig = Higgs;      return partOrig; }
    if ( pdg == 35 ||
         pdg == 36 ||
         pdg == 37 )                        { partOrig = HiggsMSSM;  return partOrig; }
    if ( pdg == 32 ||
         pdg == 33 ||
         pdg == 34 )                        { partOrig = HeavyBoson; return partOrig; }
    if ( pdg == 42 )                        { partOrig = LQ;         return partOrig; }
    if ( MC::PID::isSUSY(pdg) )             { partOrig = SUSY;       return partOrig; }
    if ( MC::PID::isBSM(pdg) )              { partOrig = OtherBSM;   return partOrig; }
  }
  return partOrig;
}
//---------------------------------------------------------------------------------------------------------
void MCTruthClassifier::findAllJetMothers(const xAOD::TruthParticle* thePart,
                                          std::set<const xAOD::TruthParticle*>&allJetMothers){
  //---------------------------------------------------------------------------------------------------------
  const xAOD::TruthVertex*  partOriVert = thePart->hasProdVtx() ? thePart->prodVtx():0;

  if (partOriVert!=0) {
    for(unsigned int ipIn=0;ipIn<partOriVert->nIncomingParticles();ipIn++){
      const xAOD::TruthParticle* theMoth=partOriVert->incomingParticle(ipIn);
      if(!theMoth) continue;
      allJetMothers.insert(theMoth);
      findAllJetMothers(theMoth,allJetMothers);
    }
  }
  return ;
}

//-----------------------------------------------------------------------------------------
const xAOD::TruthParticle* MCTruthClassifier::getGenPart(const xAOD::TrackParticle *trk){
  //-----------------------------------------------------------------------------------------
  //return GenParticle corresponding to given TrackParticle

  ATH_MSG_DEBUG( "Executing getGenPart " );

  if (!trk) return 0;

  m_deltaRMatch =-999.;
  m_deltaPhi    =-999.;
  m_NumOfSiHits = 0;
  m_probability = 0;

  uint8_t NumOfPixHits = 0;
  uint8_t NumOfSCTHits = 0;
  typedef ElementLink<xAOD::TruthParticleContainer> TruthLink_t;

  static SG::AuxElement::Accessor<TruthLink_t>  tPL ("truthParticleLink");
  if(!tPL.isAvailable(*trk)){
    ATH_MSG_DEBUG("Track particle is not associated to truth particle");
    return 0;
  }

  auto truthLink = tPL(*trk);
  if (!truthLink.isValid()){
    ATH_MSG_DEBUG("Invalid link to truth particle");
    return 0;
  }

  const xAOD::TruthParticle* theGenParticle=(*truthLink);
  if (!theGenParticle){
    ATH_MSG_DEBUG("Could not find truth matching for track");
    return 0;
  }

  static SG::AuxElement::Accessor<float >  tMP ("truthMatchProbability");
  if(tMP.isAvailable(*trk)){
    m_probability = tMP(*trk);
  }
  else {
    ATH_MSG_DEBUG("Truth match probability not available");
  }

  if(theGenParticle->status()==2||theGenParticle->status()==3){
    ATH_MSG_WARNING("track matched to the truth with status "<<theGenParticle->status());
  }

  if(theGenParticle->status()==2 &&
     (abs(theGenParticle->pdgId())==11||abs(theGenParticle->pdgId())==13)){
    const xAOD::TruthVertex* EndVrtx=theGenParticle->decayVtx();
    const xAOD::TruthParticle* theGenPartTmp(0);

    if(EndVrtx!=0){
      int itr=0;
      do {
        theGenPartTmp=0;
        for(unsigned int ipOut=0; ipOut<EndVrtx->nOutgoingParticles();ipOut++){
          const xAOD::TruthParticle* theDaugt=EndVrtx->outgoingParticle(ipOut);
          if(!theDaugt) {
            continue;
          }
          if(theDaugt->pdgId()==theGenParticle->pdgId()) {
            theGenPartTmp=theDaugt;
          }
          if(theDaugt->pdgId()!=theGenParticle->pdgId()&&theDaugt->pdgId()!=22){
            theGenPartTmp=0;
          }
        }

        itr++;
        if(itr>100) {
          ATH_MSG_WARNING ("getGenPart infinite while");  break;
        }

        if(theGenPartTmp!=0) {
          EndVrtx = theGenPartTmp->decayVtx();
        }
        else  {
          EndVrtx = 0;
        }
      } while (theGenPartTmp && theGenPartTmp->pdgId()==theGenParticle->pdgId()
               &&theGenPartTmp->status()==2&&EndVrtx!=0);

      if(theGenPartTmp && theGenPartTmp->pdgId()==theGenParticle->pdgId()) {
        theGenParticle=theGenPartTmp;
      }
    }
  }

  if (!trk->summaryValue(NumOfSCTHits, xAOD::numberOfSCTHits) )
    ATH_MSG_DEBUG("Could not retrieve number of SCT hits");
  if (!trk->summaryValue(NumOfPixHits, xAOD::numberOfPixelHits) )
    ATH_MSG_DEBUG("Could not retrieve number of Pixel hits");

  m_NumOfSiHits=NumOfSCTHits+NumOfPixHits;

  m_deltaPhi  = detPhi(theGenParticle->phi(),trk->phi());
  float  deteta  = detEta(theGenParticle->eta(),trk->eta());
  m_deltaRMatch   = rCone(m_deltaPhi, deteta);
  if(m_NumOfSiHits>m_NumOfSiHitsCut&&m_deltaRMatch >m_deltaRMatchCut) theGenParticle=NULL;
  else if(m_NumOfSiHits<=m_NumOfSiHitsCut && m_deltaPhi > m_deltaPhiMatchCut)  theGenParticle=NULL;

  ATH_MSG_DEBUG( "getGenPart  succeeded " );
  return(theGenParticle);
}

//-------------------------------------------------------------------------------
ParticleType MCTruthClassifier::defTypeOfElectron(ParticleOrigin EleOrig){
  //-------------------------------------------------------------------------------

  if( EleOrig == NonDefined)  return UnknownElectron;

  if (EleOrig == WBoson     || EleOrig == ZBoson     || EleOrig == top       ||
      EleOrig == SingleElec || EleOrig == Higgs      || EleOrig == HiggsMSSM ||
      EleOrig == HeavyBoson || EleOrig == WBosonLRSM || EleOrig == NuREle    ||
      EleOrig == NuRMu      || EleOrig == NuRTau     || EleOrig == LQ        ||
      EleOrig == SUSY       || EleOrig == DiBoson    || EleOrig == ZorHeavyBoson ||
      EleOrig == OtherBSM   || EleOrig == MultiBoson || m_isPrompt ) {
    return IsoElectron;
  }
  if (EleOrig == JPsi          || EleOrig == BottomMeson  ||
      EleOrig == CharmedMeson  || EleOrig == BottomBaryon ||
      EleOrig == CharmedBaryon || EleOrig == TauLep       ||
      EleOrig == Mu            || EleOrig == QuarkWeakDec){
    return  NonIsoElectron;
  }
  return BkgElectron;
}

//2345678901234567890123456789012345678901234567890123456789012345678901234567890
//-------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defOrigOfElectron(const xAOD::TruthParticleContainer* mcTruthTES,
                                                    const xAOD::TruthParticle  *thePart)
//-------------------------------------------------------------------------------
{

  ATH_MSG_DEBUG( "Executing DefOrigOfElectron " );

  int PriBarcode  = thePart->barcode()%m_barcodeShift;
  const xAOD::TruthParticle* thePriPart = barcode_to_particle(mcTruthTES,PriBarcode);
  if(!thePriPart) return NonDefined;
  if(abs(thePriPart->pdgId())!=11) return NonDefined;

  const xAOD::TruthVertex*   partOriVert = thePriPart->hasProdVtx() ?  thePriPart->prodVtx() : 0;

  //-- to define electron outcome status
  m_ParticleOutCome = defOutComeOfElectron(thePriPart);

  if(!partOriVert) return NonDefined;

  m_NumOfParents=-1;
  m_NumOfParents=partOriVert->nIncomingParticles();
  if(m_NumOfParents>1)
    ATH_MSG_DEBUG( "DefOrigOfElectron:: electron  has more than one mother " );

  m_Mother      = getMother(thePriPart);
  if(!m_Mother) {
    return NonDefined;
  }
  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();
  m_MothOriVert   =  m_Mother->hasProdVtx() ? m_Mother->prodVtx() : 0 ;

  //---patch to fix LQ dataset problem
  if(m_LQpatch){
    if(abs(m_MotherPDG) == 11&&m_Mother->status()==2&&m_MothOriVert==0&&m_MotherPDG==thePriPart->pdgId()&&
       m_NumOfParents == 1 &&(partOriVert->nOutgoingParticles()==1||partOriVert->nOutgoingParticles()==2) ) {
      const xAOD::TruthParticle* theP(0);
      int itr=0;
      float dRMin(99999.);
      do {
        const xAOD::TruthParticle* thePtmp = barcode_to_particle(mcTruthTES,m_MotherBarcode-(itr+2));
        if(abs(thePtmp->pdgId())==11&&thePtmp->pdgId()==m_MotherPDG&&thePtmp->status()==3&&thePtmp->decayVtx()==0){
          float dphi=detPhi(m_Mother->phi(),thePtmp->phi());
          float deta=m_Mother->eta()-thePtmp->eta();
          float dR=rCone(dphi,deta);
          if(dR<dRMin) {dRMin=dR; theP=thePtmp;};

        }
        itr++;
      } while (itr<4);

      if(dRMin>0.1) theP=0;
      if(theP!=0){
        thePriPart = theP;
        m_Mother        = getMother(thePriPart);
        if(!m_Mother)   {
          return NonDefined;
        }
        m_MotherPDG     = m_Mother->pdgId();
        m_MotherBarcode = m_Mother->barcode();
        m_MothOriVert   = m_Mother->hasProdVtx() ? m_Mother->prodVtx() : 0 ;
      }
    }
  }
  //----

  // to exclude interactions mu(barcode<10^6)->mu(barcode10^6)+e
  bool samePart=false;
  for (unsigned int ipOut=0; ipOut<partOriVert->nOutgoingParticles(); ++ipOut) {
    const xAOD::TruthParticle* theDaug=partOriVert->outgoingParticle(ipOut);
    if(!theDaug) continue;
    if( m_MotherPDG==theDaug->pdgId()&&m_MotherBarcode%m_barcodeShift==theDaug->barcode()%m_barcodeShift) samePart=true;
  }

  //to resolve Sherpa loop
  if(m_MothOriVert&&m_MothOriVert->barcode() == partOriVert->barcode()) samePart=true;
  //

  if( ( abs(m_MotherPDG)==13||abs(m_MotherPDG)==15||abs(m_MotherPDG)==24)&& m_MothOriVert!=0&&!samePart){
    long pPDG(0);
    const xAOD::TruthParticle*  MotherParent(0);
    do {
      pPDG=0;
      MotherParent=getMother(m_Mother);
      //to prevent Sherpa loop
      const xAOD::TruthVertex*  mother_prdVtx(0);
      const xAOD::TruthVertex*  mother_endVtx(0);
      if(m_Mother) {
        mother_prdVtx=m_Mother->hasProdVtx() ? m_Mother->prodVtx():0;
        mother_endVtx=m_Mother->decayVtx();
      }
      const xAOD::TruthVertex*  parent_prdVtx(0);
      const xAOD::TruthVertex*  parent_endVtx(0);
      if(MotherParent){
        parent_prdVtx=MotherParent->hasProdVtx() ? MotherParent->prodVtx():0;
        parent_endVtx=MotherParent->decayVtx();
      }
      if(mother_endVtx==parent_prdVtx&&mother_prdVtx==parent_endVtx){ MotherParent=m_Mother; break;}
      //
      if(MotherParent) pPDG = MotherParent->pdgId();
      //to prevent Sherpa loop
      if(m_Mother == MotherParent) break;

      if(abs(pPDG)==13 || abs(pPDG)==15 || abs(pPDG)==24)  m_Mother = MotherParent;

    }  while ((abs(pPDG)==13 || abs(pPDG)==15 || abs(pPDG)==24) );

    if(abs(pPDG)==13 || abs(pPDG)==15 || abs(pPDG)==24 || abs(pPDG)==23 ||
       abs(pPDG)==25 || abs(pPDG)==35 || abs(pPDG)==36 || abs(pPDG)==37 ||
       abs(pPDG)==32 || abs(pPDG)==33 || abs(pPDG)==34 || abs(pPDG)==6  ||
       abs(pPDG)==9900024 || abs(pPDG)== 9900012 || abs(pPDG)== 9900014 || abs(pPDG)== 9900016 ||
       (abs(pPDG)<2000040&& abs(pPDG)>1000001))  m_Mother = MotherParent;
  }


  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();
  m_MotherStatus  = m_Mother->status();
  partOriVert   = m_Mother->decayVtx();
  m_MothOriVert   = m_Mother->hasProdVtx() ? m_Mother->prodVtx() : 0;
  m_NumOfParents  = partOriVert->nIncomingParticles();
  m_NumOfDaug     = partOriVert->nOutgoingParticles();

  int  NumOfPhot(0),NumOfEl(0),NumOfPos(0),NumOfNucFr(0);
  int  NumOfquark(0),NumOfgluon(0),NumOfElNeut(0),NumOfLQ(0);
  int  NumOfMuPl(0),NumOfMuMin(0),NumOfMuNeut(0);
  int  NumOfTau(0),NumOfTauNeut(0);
  long DaugType(0);
  samePart=false;

  for (unsigned int ipOut=0; ipOut<partOriVert->nOutgoingParticles(); ++ipOut) {
    const xAOD::TruthParticle* theDaug=partOriVert->outgoingParticle(ipOut);
    if(!theDaug) continue;
    DaugType = theDaug->pdgId();
    if( abs(DaugType)  < 7  ) NumOfquark++;
    if( abs(DaugType) == 21 ) NumOfgluon++;
    if( abs(DaugType) == 12 ) NumOfElNeut++;
    if( abs(DaugType) == 14 ) NumOfMuNeut++;
    if( DaugType      == 22 ) NumOfPhot++;
    if( DaugType      == 11 ) NumOfEl++;
    if( DaugType      ==-11 ) NumOfPos++;
    if( DaugType      == 13 ) NumOfMuMin++;
    if( DaugType      ==-13 ) NumOfMuPl++;
    if( abs(DaugType) == 15 ) NumOfTau++;
    if( abs(DaugType) == 16 ) NumOfTauNeut++;

    if( abs(DaugType) == 42 ) NumOfLQ++;
    if( abs(DaugType) == abs(m_MotherPDG)&&
        theDaug->barcode()%m_barcodeShift == m_MotherBarcode%m_barcodeShift ) samePart=true;
    if(m_NumOfParents==1&&(m_MotherPDG==22||abs(m_MotherPDG)==11||abs(m_MotherPDG)==13||abs(m_MotherPDG)==211)&&
       (DaugType > 1000000000||DaugType==0 ||DaugType == 2212||DaugType == 2112 || abs(DaugType) == 211|| abs(DaugType) == 111))  NumOfNucFr++;
  } // cycle itrDaug

  if (m_MotherPDG == 22&&m_MothOriVert!=0){
    // get mother of photon
    for (unsigned int ipIn=0; ipIn<m_MothOriVert->nIncomingParticles(); ++ipIn) {
      const xAOD::TruthParticle* theMother=m_MothOriVert->incomingParticle(ipIn);
      if(!theMother) continue;
      m_PhotonMotherPDG     = theMother->pdgId();
      m_PhotonMotherBarcode = theMother->barcode();
      m_PhotonMotherStatus  = theMother->status();
      m_PhotonMother        = theMother;

    }
  }

  // if ( m_NumOfParents == 0 && m_NumOfDaug == 1 )  return  SingleElec;
  if ( (m_MotherPDG == 22 && m_NumOfDaug == 2 &&  NumOfEl == 1 && NumOfPos == 1)||
       (m_MotherPDG == 22 && m_NumOfDaug == 1 && (NumOfEl == 1 || NumOfPos == 1))){
    return PhotonConv;
  }

  //e,gamma,pi+Nuclear->NuclearFragments+nuclons+e
  if ( ( m_NumOfParents == 1&&(abs(m_MotherPDG)==22||abs(m_MotherPDG)==11||abs(m_MotherPDG)==15 ))
       &&m_NumOfDaug>1&&NumOfNucFr!=0)     return ElMagProc;

  if ( m_NumOfParents == 1&&abs(m_MotherPDG)==211
       &&m_NumOfDaug>2&&NumOfNucFr!=0)     return ElMagProc;

  // nuclear photo fission
  if ( m_MotherPDG == 22 && m_NumOfDaug > 4&&NumOfNucFr!=0)
    return ElMagProc;

  // unknown process el(pos)->el+pos??
  if( abs(m_MotherPDG) == 11 && m_NumOfDaug == 2 && NumOfEl == 1 && NumOfPos == 1 )
    return ElMagProc;

  // unknown process el->el+el??
  if( m_MotherPDG == 11 && m_NumOfDaug == 2 && NumOfEl == 2 && NumOfPos == 0 )
    return ElMagProc;

  // unknown process pos->pos+pos??
  if( m_MotherPDG == -11 && m_NumOfDaug == 2 && NumOfEl == 0 && NumOfPos == 2 )
    return ElMagProc;

  // unknown process pos/el->pos/el??
  if( abs(m_MotherPDG) == 11&&m_Mother->status()!=2&&m_MotherPDG==thePriPart->pdgId() &&  m_NumOfDaug == 1 &&!samePart)
    return ElMagProc;

  // pi->pi+e+/e-; mu->mu+e+/e- ;
  // gamma+ atom->gamma(the same) + e (compton scattering)
  if( m_NumOfDaug == 2 && (NumOfEl == 1 || NumOfPos == 1 )
      && abs(m_MotherPDG)!=11&&samePart)    return ElMagProc;


  if ( (m_MotherPDG == 111 && m_NumOfDaug == 3 && NumOfPhot == 1 &&
        NumOfEl == 1 && NumOfPos == 1)|| (m_MotherPDG == 111 && m_NumOfDaug == 4 &&
                                          NumOfPhot == 0 && NumOfEl == 2 && NumOfPos == 2) )
    return  DalitzDec;

  // Quark weak decay
  if ( abs(m_MotherPDG)< 7 &&m_NumOfParents == 1&&m_NumOfDaug==3&&NumOfquark==1&&NumOfElNeut==1 ) return QuarkWeakDec;

  if( abs(m_MotherPDG) == 13&&NumOfNucFr!=0 )  return ElMagProc;

  if( abs(m_MotherPDG)==6 )                    return top;

  if( abs(m_MotherPDG)==24&&m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()!=0 ) {

    const xAOD::TruthVertex* prodVert = m_MothOriVert;
    const xAOD::TruthParticle* ptrPart;
    do {
      ptrPart=prodVert->incomingParticle(0);
      prodVert=ptrPart->hasProdVtx() ? ptrPart->prodVtx() : 0 ;
    } while(abs(ptrPart->pdgId())==24&&prodVert!=0);

    if(prodVert&&prodVert->nIncomingParticles()==1&&abs(ptrPart->pdgId())==9900012)      return NuREle;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(ptrPart->pdgId())==9900014) return NuRMu;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(ptrPart->pdgId())==9900016) return NuRTau;
    return WBoson;
  }else if( abs(m_MotherPDG)==24) return WBoson;

  if( abs(m_MotherPDG)==23 ) return ZBoson;

  //-- Exotics
  if( abs(m_MotherPDG)<7&&m_NumOfParents==2&&m_NumOfDaug==2&&NumOfEl==1&&NumOfPos==1&&
      partOriVert->barcode()==-5){
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    const xAOD::TruthVertex* TmpVert= partOriVert->outgoingParticle(0)->decayVtx();

    if(abs(pdg1)==abs(pdg2)&&TmpVert->nIncomingParticles()==2)   return ZorHeavyBoson;
  }

  if(abs(m_MotherPDG)<7&&m_NumOfParents==2&&m_NumOfDaug==2&&(NumOfEl==1||NumOfPos==1)&&
     NumOfElNeut==1&&partOriVert->barcode()==-5)  {
    const xAOD::TruthVertex* TmpVert= partOriVert->outgoingParticle(0)->decayVtx();
    if(TmpVert->nIncomingParticles()==2) return HeavyBoson;
  }

  //MadGraphPythia ZWW*->lllnulnu
  if(m_NumOfParents==1&&m_NumOfDaug>4&&(abs(m_MotherPDG)<7||m_MotherPDG==21)){

    const xAOD::TruthParticle* thePartToCheck=thePriPart;
    const xAOD::TruthParticle* theMother=thePriPart->hasProdVtx() ? thePriPart->prodVtx()->incomingParticle(0) : 0;
    if(theMother!=0&&abs(theMother->pdgId())==11&&theMother->status()==2) thePartToCheck=theMother;

    bool isZboson=false;
    bool isWboson=false;
    bool skipnext=false;

    for (unsigned int ipOut=0; ipOut<partOriVert->nOutgoingParticles()-1; ++ipOut) {
      const xAOD::TruthParticle* theDaug=partOriVert->outgoingParticle(ipOut);
      if(!theDaug) continue;
      const xAOD::TruthParticle* theNextDaug=0;
      for(unsigned int ipOut1=ipOut+1;ipOut1<partOriVert->nOutgoingParticles();ipOut1++) {
        theNextDaug=partOriVert->outgoingParticle(ipOut1);
        if(theNextDaug!=0) break;
      }
      if(!theNextDaug) continue;
      if(skipnext) {skipnext=false; continue;}

      if(abs(theDaug->pdgId())==11&&abs(theNextDaug->pdgId())==11 ) {
        //Zboson
        if(thePartToCheck==theDaug||thePartToCheck==theNextDaug) { isZboson=true; break;}
        skipnext=true;
      } else if(abs(theDaug->pdgId())==11&&abs(theNextDaug->pdgId())==12) {
        //WBoson
        if(thePartToCheck==theDaug||thePartToCheck==theNextDaug) { isWboson=true; break;}
        skipnext=true;
      }
    }
    if(isWboson) return WBoson;
    if(isZboson) return ZBoson;
  }

  //--Sherpa Z->ee
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&NumOfEl==1&&NumOfPos==1) return ZBoson;


  //--Sherpa W->enu ??
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&(NumOfEl==1||NumOfPos==1)&&NumOfElNeut==1)  return WBoson;


  //--Sherpa ZZ,ZW
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==4&&
     (NumOfEl+NumOfPos+NumOfMuPl+NumOfMuMin+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut==4) ) {
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return DiBoson;
  }

  //--Sherpa VVV -- Note, have to allow for prompt photon radiation or these get lost
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon-NumOfPhot)==6&&
     (NumOfEl+NumOfPos+NumOfMuPl+NumOfMuMin+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut==6) ) {
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return MultiBoson;
  }

  //New Sherpa Z->ee
  if(partOriVert==m_MothOriVert&&partOriVert!=0){
    int NumOfEleLoop=0;
    int NumOfLepLoop=0;
    int NumOfEleNeuLoop=0;
    for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles(); ipOut++){
      for(unsigned int ipIn=0;ipIn<partOriVert->nIncomingParticles();ipIn++){
        if(!partOriVert->outgoingParticle(ipOut)) continue;
        if(!partOriVert->incomingParticle(ipIn)) continue;
        if (partOriVert->outgoingParticle(ipOut)->barcode()==partOriVert->incomingParticle(ipIn)->barcode()){
          if(std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==11) NumOfEleLoop++;
          if(std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==12) NumOfEleNeuLoop++;
          if(std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==11||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==12||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==13||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==14||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==15||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==16) NumOfLepLoop++;
        }
      }
    }
    if(NumOfEleLoop==2&&NumOfEleNeuLoop==0) return ZBoson;
    if(NumOfEleLoop==1&&NumOfEleNeuLoop==1) return WBoson;
    if((NumOfEleLoop==4&&NumOfEleNeuLoop==0)||
       (NumOfEleLoop==3&&NumOfEleNeuLoop==1)||
       (NumOfEleLoop==2&&NumOfEleNeuLoop==2)) return DiBoson;
    if(NumOfLepLoop==4) return DiBoson;
  }

  //-- McAtNLo
  if( abs(m_MotherPDG)<7&&m_NumOfParents==2&&NumOfEl==1&&NumOfPos==1&&partOriVert->barcode()==-1){
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if(abs(pdg1)==abs(pdg2))                   return ZBoson;
  }

  if( abs(m_MotherPDG)==25 )                   return Higgs;

  if( abs(m_MotherPDG)==35||
      abs(m_MotherPDG)==36||
      abs(m_MotherPDG)==37)                    return HiggsMSSM;

  if( abs(m_MotherPDG)==32||
      abs(m_MotherPDG)==33||
      abs(m_MotherPDG)==34)                    return HeavyBoson;

  if( abs(m_MotherPDG)==13)                    return Mu;
  if( abs(m_MotherPDG)==15){

    ParticleOrigin  tauOrig= defOrigOfTau (mcTruthTES,m_Mother);
    ParticleType    tautype=defTypeOfTau(tauOrig);
    if(tautype==IsoTau) return tauOrig;
    else return TauLep;
  }

  if(abs(m_MotherPDG)==9900024)                return WBosonLRSM;
  if(abs(m_MotherPDG)==9900012)                return NuREle;
  if(abs(m_MotherPDG)==9900014)                return NuRMu;
  if(abs(m_MotherPDG)==9900016)                return NuRTau;

  if (abs(m_MotherPDG) == 42 || NumOfLQ!=0 )   return LQ;

  if( MC::PID::isSUSY(m_MotherPDG) )           return SUSY;

  if( MC::PID::isBSM(m_MotherPDG) )            return OtherBSM;

  ParticleType pType = defTypeOfHadron(m_MotherPDG);
  if( (pType==BBbarMesonPart || pType==CCbarMesonPart )
      && m_MothOriVert!=0&&isHardScatVrtx(m_MothOriVert))  m_isPrompt=true;

  //----
  return convHadronTypeToOrig(pType);
}

//-------------------------------------------------------------------------------
ParticleType MCTruthClassifier::defTypeOfMuon(ParticleOrigin MuOrig){
  //-------------------------------------------------------------------------------


  if( MuOrig == NonDefined)  return UnknownMuon;

  if (MuOrig == WBoson     || MuOrig == ZBoson     || MuOrig == top       ||
      MuOrig == SingleMuon || MuOrig == Higgs      || MuOrig == HiggsMSSM ||
      MuOrig == HeavyBoson || MuOrig == WBosonLRSM || MuOrig == NuREle    ||
      MuOrig == NuRMu      || MuOrig == NuRTau     || MuOrig == LQ        ||
      MuOrig == SUSY       || MuOrig == DiBoson    || MuOrig == ZorHeavyBoson ||
      MuOrig == OtherBSM   || MuOrig == MultiBoson || m_isPrompt)  {
    return IsoMuon;
  }
  if (MuOrig == JPsi          || MuOrig == BottomMeson  ||
      MuOrig == CharmedMeson  || MuOrig == BottomBaryon ||
      MuOrig == CharmedBaryon ||  MuOrig == TauLep      ||
      MuOrig == QuarkWeakDec){
    return  NonIsoMuon;
  }
  //  if (MuOrig == Pion  || MuOrig == Kaon ) return  DecayMuon;
  return BkgMuon;

}

//2345678901234567890123456789012345678901234567890123456789012345678901234567890
//-------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defOrigOfMuon(const xAOD::TruthParticleContainer* mcTruthTES,
                                                const xAOD::TruthParticle  *thePart){
  //-------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing DefOrigOfMuon " );

  int PriBarcode     = thePart->barcode()%m_barcodeShift;
  const xAOD::TruthParticle* thePriPart =barcode_to_particle(mcTruthTES,PriBarcode);
  if(!thePriPart) return NonDefined;
  if(abs(thePriPart->pdgId())!=13) return NonDefined;

  const xAOD::TruthVertex*   partOriVert =  thePriPart->hasProdVtx() ? thePriPart->prodVtx() : 0 ;

  //-- to define muon  outcome status
  m_ParticleOutCome = defOutComeOfMuon(thePriPart);

  if(!partOriVert) return NonDefined;


  m_NumOfParents=partOriVert->nIncomingParticles();
  if(m_NumOfParents>1)
    ATH_MSG_DEBUG ( "DefOrigOfMuon:: muon  has more than one mother " );

  m_Mother      = getMother(thePriPart);
  if(!m_Mother) {
    return NonDefined;
  }

  m_MothOriVert   = m_Mother->hasProdVtx() ? m_Mother->prodVtx() : 0;
  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();

  if( ( abs(m_MotherPDG)==15||abs(m_MotherPDG)==24)&& m_MothOriVert!=0){
    long pPDG(0);
    const xAOD::TruthParticle*  MotherParent(0);
    do {
      //
      pPDG=0;
      //
      const xAOD::TruthVertex*  mother_prdVtx(0);
      const xAOD::TruthVertex*  mother_endVtx(0);
      MotherParent=getMother(m_Mother);
      //to prevent Sherpa loop
      mother_prdVtx=m_Mother->hasProdVtx() ? m_Mother->prodVtx():0;
      mother_endVtx=m_Mother->decayVtx();
      //
      const xAOD::TruthVertex*  parent_prdVtx(0);
      const xAOD::TruthVertex*  parent_endVtx(0);
      if(MotherParent){
        parent_prdVtx=MotherParent->hasProdVtx() ? MotherParent->prodVtx():0;
        parent_endVtx=MotherParent->decayVtx();
      }
      //
      if(mother_endVtx==parent_prdVtx&&mother_prdVtx==parent_endVtx){
        MotherParent=m_Mother;
        break;
      }
      //
      //to prevent Sherpa loop
      if(m_Mother == MotherParent) {
        break;
      }

      if(MotherParent) {
        pPDG = MotherParent->pdgId();
        if(abs(pPDG)==13 || abs(pPDG)==15 || abs(pPDG)==24)  {
          m_Mother = MotherParent;
        }
      }
    }  while ((abs(pPDG)==13 || abs(pPDG)==15 || abs(pPDG)==24) );


    if(abs(pPDG)==15 || abs(pPDG)==24 || abs(pPDG)==23 || abs(pPDG)==25 ||
       abs(pPDG)==35 || abs(pPDG)==36 || abs(pPDG)==37 || abs(pPDG)==32 ||
       abs(pPDG)==33 || abs(pPDG)==34 || abs(pPDG)==6  ||
       abs(pPDG)==9900024 || abs(pPDG)==9900012 || abs(pPDG)==9900014 || abs(pPDG)==9900016 ||
       (abs(pPDG)<2000040&& abs(pPDG)>1000001))  {
      m_Mother = MotherParent;
    }
  }

  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();
  m_MotherStatus  = m_Mother->status();
  m_MothOriVert   = m_Mother->hasProdVtx() ? m_Mother->prodVtx() : 0;
  partOriVert   = m_Mother->decayVtx();
  m_NumOfParents  = partOriVert->nIncomingParticles();
  m_NumOfDaug     = partOriVert->nOutgoingParticles();

  //---
  long DaugType(0);
  int  NumOfPhot(0),NumOfEl(0),NumOfPos(0);
  int  NumOfElNeut(0),NumOfMuNeut(0),NumOfLQ(0),NumOfquark(0),NumOfgluon(0);
  int  NumOfMuPl(0),NumOfMuMin(0);
  int  NumOfTau(0),NumOfTauNeut(0);
  for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles();ipOut++){
    const xAOD::TruthParticle * theDaug=partOriVert->outgoingParticle(ipOut);
    if(!theDaug) continue;
    DaugType  = theDaug->pdgId();
    if( DaugType      == 11 ) NumOfEl++;
    else if( DaugType      ==-11 ) NumOfPos++;
    else if( DaugType      == 13 ) NumOfMuMin++;
    else if( DaugType      ==-13 ) NumOfMuPl++;
    else if( abs(DaugType) == 12 ) NumOfElNeut++;
    else if( abs(DaugType) == 14 ) NumOfMuNeut++;
    else if( abs(DaugType) == 15 ) NumOfTau++;
    else if( abs(DaugType) == 16 ) NumOfTauNeut++;
    else if( abs(DaugType) == 42 ) NumOfLQ++;
    else if( abs(DaugType)  < 7  ) NumOfquark++;
    else if( abs(DaugType) == 21 ) NumOfgluon++;
    else if( DaugType      == 22 ) NumOfPhot++;
  } // cycle itrDaug

  // if ( m_NumOfParents == 0 && m_NumOfDaug == 1 )   return  SingleMuon;

  if ( abs(m_MotherPDG) == 211&&m_NumOfDaug == 2 && NumOfMuNeut == 1)
    return   PionDecay;

  if ( abs(m_MotherPDG) == 321&&m_NumOfDaug == 2 && NumOfMuNeut == 1)
    return   KaonDecay;

  if( abs(m_MotherPDG)==15 ) {
    ParticleOrigin  tauOrig= defOrigOfTau (mcTruthTES,m_Mother);
    ParticleType    tautype=defTypeOfTau(tauOrig);
    if(tautype==IsoTau) return tauOrig;
    else return TauLep;
  }

  if( abs(m_MotherPDG)==6  )                   return top;

  // Quark weak decay
  if ( abs(m_MotherPDG)< 7 &&m_NumOfParents == 1&&m_NumOfDaug==3&&NumOfquark==1&&NumOfMuNeut==1 ) return QuarkWeakDec;

  if( abs(m_MotherPDG)==24&&m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()!=0 ) {
    const xAOD::TruthVertex* prodVert = m_MothOriVert;
    const xAOD::TruthParticle* itrP;
    do {
      itrP =prodVert->incomingParticle(0);
      prodVert= itrP->hasProdVtx() ? itrP->prodVtx() : 0;
    } while(abs(itrP->pdgId())==24&&prodVert!=0);

    if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900012)      return NuREle;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900014) return NuRMu;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900016) return NuRTau;
    return WBoson;
  } else  if( abs(m_MotherPDG)==24) return WBoson;

  if( abs(m_MotherPDG)==23 )                   return ZBoson;

  if ( m_MotherPDG == 22 && m_NumOfDaug == 2 &&  NumOfMuMin == 1 && NumOfMuPl == 1 ){
    return PhotonConv;
  }

  //-- Exotics
  if( abs(m_MotherPDG)<7&&m_NumOfParents==2&&NumOfMuPl==1&&NumOfMuMin==1&&
      NumOfEl+NumOfPos==0&&NumOfTau==0&&partOriVert->barcode()==-5){
    int pdg1= partOriVert->incomingParticle(0)->pdgId();
    int pdg2= partOriVert->incomingParticle(1)->pdgId();
    const xAOD::TruthParticle* theDaug =partOriVert->outgoingParticle(0);
    const xAOD::TruthVertex* TmpVert=theDaug->decayVtx();
    if(abs(pdg1)==abs(pdg2)&&TmpVert->nIncomingParticles()==2)  return ZorHeavyBoson;
  }

  if(abs(m_MotherPDG)<7&&m_NumOfParents==2&&(NumOfMuMin==1||NumOfMuPl==1)&&
     NumOfMuNeut==1&&NumOfEl+NumOfPos==0&&NumOfTau==0&&partOriVert->barcode()==-5) {
    const xAOD::TruthParticle* theDaug =partOriVert->outgoingParticle(0);
    const xAOD::TruthVertex* TmpVert=theDaug->decayVtx();
    if(TmpVert->nIncomingParticles()==2) return HeavyBoson;
  }

  //MadGraphPythia ZWW*->lllnulnu

  if(m_NumOfParents==1&&m_NumOfDaug>4&&(abs(m_MotherPDG)<7||m_MotherPDG==21)){
    bool isZboson=false;
    bool isWboson=false;
    bool skipnext=false;
    for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles()-1;ipOut++){
      if(skipnext) {skipnext=false; continue;}
      const xAOD::TruthParticle* theDaug=partOriVert->outgoingParticle(ipOut);
      if(!theDaug) continue;
      const xAOD::TruthParticle* theNextDaug=0;
      for(unsigned int ipOut1=ipOut+1;ipOut1<partOriVert->nOutgoingParticles();ipOut1++) {
        theNextDaug=partOriVert->outgoingParticle(ipOut1);
        if(theNextDaug!=0) break;
      }
      if(!theNextDaug) continue;
      if(  abs(theDaug->pdgId())==13&&abs(theNextDaug->pdgId())==13) {
        //Zboson
        if(thePriPart==theDaug||thePriPart==theNextDaug) { isZboson=true; break;}
        skipnext=true;
      } else if(abs(theDaug->pdgId())==13&&abs(theNextDaug->pdgId())==14) {
        //WBoson
        if(thePriPart==theDaug||thePriPart==theNextDaug) { isWboson=true; break;}
        skipnext=true;
      }
    }
    if(isWboson) return WBoson;
    if(isZboson) return ZBoson;
  }

  //--Sherpa Z->mumu
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&NumOfMuPl==1&&NumOfMuMin==1) return ZBoson;

  //--Sherpa W->munu ??
  // if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&(NumOfEl==1||NumOfPos==1)&&NumOfElNeut==1) return WBoson;
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&(NumOfMuPl==1||NumOfMuMin==1)&&NumOfMuNeut==1) return WBoson;


  //--Sherpa ZZ,ZW
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==4&&
     (NumOfEl+NumOfPos+NumOfMuPl+NumOfMuMin+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut==4) ) {
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return DiBoson;
  }


  //--Sherpa VVV -- Note, have to allow for prompt photon radiation or these get lost
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon-NumOfPhot)==6&&
     (NumOfEl+NumOfPos+NumOfMuPl+NumOfMuMin+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut==6) ) {
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return MultiBoson;
    std::cout << "First part was good..." << std::endl;
  }
  else std::cout << "Fell through..." << m_NumOfParents << " " << m_NumOfDaug-NumOfquark-NumOfgluon << " " << NumOfEl+NumOfPos+NumOfMuPl+NumOfMuMin+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut << std::endl;

    if( DaugType      == 11 ) NumOfEl++;
    if( DaugType      ==-11 ) NumOfPos++;
    if( DaugType      == 13 ) NumOfMuMin++;
    if( DaugType      ==-13 ) NumOfMuPl++;
    if( abs(DaugType) == 12 ) NumOfElNeut++;
    if( abs(DaugType) == 14 ) NumOfMuNeut++;
    if( abs(DaugType) == 15 ) NumOfTau++;
    if( abs(DaugType) == 16 ) NumOfTauNeut++;
    if( abs(DaugType) == 42 ) NumOfLQ++;
    if( abs(DaugType)  < 7  ) NumOfquark++;
    if( abs(DaugType) == 21 ) NumOfgluon++;

  //--New Sherpa Z->mumu
  if(partOriVert==m_MothOriVert){
    int NumOfMuLoop=0;
    int NumOfMuNeuLoop=0;
    int NumOfLepLoop=0;
    for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles(); ipOut++){
      for(unsigned int ipIn=0;ipIn<partOriVert->nIncomingParticles();ipIn++){
        if(!partOriVert->outgoingParticle(ipOut)) continue;
        if(!partOriVert->incomingParticle(ipIn)) continue;
        if (partOriVert->outgoingParticle(ipOut)->barcode()==partOriVert->incomingParticle(ipIn)->barcode()){
          if(std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==13) NumOfMuLoop++;
          if(std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==14) NumOfMuNeuLoop++;
          if(std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==11||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==12||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==13||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==14||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==15||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==16) NumOfLepLoop++;
         }
      }
    }
    if(NumOfMuLoop==2&&NumOfMuNeuLoop==0) return ZBoson;
    if(NumOfMuLoop==1&&NumOfMuNeuLoop==1) return WBoson;
    if((NumOfMuLoop==4&&NumOfMuNeuLoop==0)||
       (NumOfMuLoop==3&&NumOfMuNeuLoop==1)||
       (NumOfMuLoop==2&&NumOfMuNeuLoop==2)) return DiBoson;
    if(NumOfLepLoop==4) return DiBoson;


  }


  //-- McAtNLo
  if( abs(m_MotherPDG)<7&&m_NumOfParents==2&&NumOfMuPl==1&&NumOfMuMin==1&&partOriVert->barcode()==-1){
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(0)->pdgId();
    if(abs(pdg1)==abs(pdg2))                   return ZBoson;
  }


  if( abs(m_MotherPDG)==25 )                   return Higgs;

  if( abs(m_MotherPDG)==35||
      abs(m_MotherPDG)==36||
      abs(m_MotherPDG)==37)                    return HiggsMSSM;

  if( abs(m_MotherPDG)==32||
      abs(m_MotherPDG)==33||
      abs(m_MotherPDG)==34)                    return HeavyBoson;

  if(abs(m_MotherPDG)==9900024)                return WBosonLRSM;
  if(abs(m_MotherPDG)==9900012)                return NuREle;
  if(abs(m_MotherPDG)==9900014)                return NuRMu;
  if(abs(m_MotherPDG)==9900016)                return NuRTau;

  if (abs(m_MotherPDG) == 42 || NumOfLQ!=0  )  return LQ;

  if( MC::PID::isSUSY(m_MotherPDG) )           return SUSY;
  if( MC::PID::isBSM(m_MotherPDG) )            return OtherBSM;

  ParticleType pType = defTypeOfHadron(m_MotherPDG);
  if( (pType==BBbarMesonPart || pType==CCbarMesonPart )
      && m_MothOriVert!=0&&isHardScatVrtx(m_MothOriVert))  m_isPrompt=true;

  return convHadronTypeToOrig(pType);


}
//-------------------------------------------------------------------------------
ParticleType MCTruthClassifier::defTypeOfTau(ParticleOrigin TauOrig){
  //-------------------------------------------------------------------------------

  if( TauOrig == NonDefined)  return UnknownTau;

  if (TauOrig == WBoson     || TauOrig == ZBoson     || TauOrig == top       ||
      TauOrig == SingleMuon || TauOrig == Higgs      || TauOrig == HiggsMSSM ||
      TauOrig == HeavyBoson || TauOrig == WBosonLRSM || TauOrig ==  NuREle   ||
      TauOrig == NuRMu      || TauOrig ==  NuRTau    || TauOrig == SUSY      ||
      TauOrig == DiBoson    || TauOrig == ZorHeavyBoson || TauOrig == OtherBSM ||
      TauOrig == MultiBoson )
    return IsoTau;

  if (TauOrig == JPsi          || TauOrig == BottomMeson  ||
      TauOrig == CharmedMeson  || TauOrig == BottomBaryon ||
      TauOrig == CharmedBaryon || TauOrig == QuarkWeakDec)
    return  NonIsoTau;

  return BkgTau;
}

//2345678901234567890123456789012345678901234567890123456789012345678901234567890
//-------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defOrigOfTau(const xAOD::TruthParticleContainer* mcTruthTES,
                                               const xAOD::TruthParticle  *thePart){
  //-------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing DefOrigOfTau " );

  int PriBarcode     = thePart->barcode()%m_barcodeShift;
  const xAOD::TruthParticle* thePriPart = barcode_to_particle(mcTruthTES,PriBarcode);
  if(!thePriPart) return NonDefined;
  if(abs(thePriPart->pdgId())!=15) return NonDefined;

  const xAOD::TruthVertex*   partOriVert = thePriPart->hasProdVtx() ? thePriPart->prodVtx() : 0;


  //-- to define tau  outcome status
  if(thePriPart->status()<3) m_ParticleOutCome = defOutComeOfTau(thePriPart);

  if(!partOriVert) return NonDefined;

  m_NumOfParents=partOriVert->nIncomingParticles();
  if(m_NumOfParents>1)
    ATH_MSG_DEBUG( "DefOrigOfTau:: tau  has more than one mother " );

  m_Mother=getMother(thePriPart);
  if(!m_Mother) {
    return NonDefined;
  }

  m_MothOriVert =  m_Mother->hasProdVtx() ? m_Mother->prodVtx() : 0;

  const xAOD::TruthParticle* MotherParent(0);

  if( abs(m_MotherPDG)==24 && m_MothOriVert!=0){
    MotherParent=getMother(m_Mother);
    long pPDG(0);
    if(MotherParent) {
      pPDG = MotherParent->pdgId();
      if(abs(pPDG)==6) {
        m_Mother = MotherParent;
      }
    }
  }

  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();
  m_MotherStatus  = m_Mother->status();
  m_MothOriVert   = m_Mother->hasProdVtx() ? m_Mother->prodVtx() : 0;
  partOriVert   = m_Mother->decayVtx();
  if(!partOriVert) {
    return NonDefined;
  }

  m_NumOfParents  = partOriVert->nIncomingParticles();
  m_NumOfDaug     = partOriVert->nOutgoingParticles();

  int  NumOfPhot(0),NumOfEl(0),NumOfPos(0);
  int  NumOfquark(0),NumOfgluon(0),NumOfElNeut(0);
  int  NumOfMuPl(0),NumOfMuMin(0),NumOfMuNeut(0);
  int  NumOfTau(0),NumOfTauNeut(0);
  long DaugType(0);

  for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles(); ipOut++){
    if(!partOriVert->outgoingParticle(ipOut)) continue;
    DaugType = partOriVert->outgoingParticle(ipOut)->pdgId();
    if( abs(DaugType)  < 7  ) NumOfquark++;
    if( abs(DaugType) == 21 ) NumOfgluon++;
    if( abs(DaugType) == 12 ) NumOfElNeut++;
    if( abs(DaugType) == 14 ) NumOfMuNeut++;
    if( DaugType      == 22 ) NumOfPhot++;
    if( DaugType      == 11 ) NumOfEl++;
    if( DaugType      ==-11 ) NumOfPos++;
    if( DaugType      == 13 ) NumOfMuMin++;
    if( DaugType      ==-13 ) NumOfMuPl++;
    if( abs(DaugType) == 15 ) NumOfTau++;
    if( abs(DaugType) == 16 ) NumOfTauNeut++;
  }

  if( abs(m_MotherPDG)==6  )                   return top;

  if( abs(m_MotherPDG)==24&&m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()!=0 ){
    const xAOD::TruthVertex* prodVert = m_MothOriVert;
    const xAOD::TruthParticle* itrP;
    do {
      itrP =prodVert->incomingParticle(0);
      prodVert= itrP->hasProdVtx() ? itrP->prodVtx() : 0;
    } while(abs(itrP->pdgId())==24&&prodVert!=0);

    if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900012)      return NuREle;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900014) return NuRMu;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900016) return NuRTau;
    return WBoson;
  }
  else  if( abs(m_MotherPDG)==24) {
    return WBoson;
  }

  if( abs(m_MotherPDG)==23 ){
    return ZBoson;
  }
  if(m_NumOfParents==1&&m_NumOfDaug>4&&(abs(m_MotherPDG)<7||m_MotherPDG==21)){
    bool isZboson=false;
    bool isWboson=false;
    bool skipnext=false;
    for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles()-1;ipOut++){
      if(skipnext) {
        skipnext=false;
        continue;
      }
      const xAOD::TruthParticle* theDaug=partOriVert->outgoingParticle(ipOut);
      if(!theDaug) continue;
      const xAOD::TruthParticle* theNextDaug=0;
      for(unsigned int ipOut1=ipOut+1;ipOut1<partOriVert->nOutgoingParticles();ipOut1++) {
        theNextDaug=partOriVert->outgoingParticle(ipOut1);
        if(theNextDaug!=0)
          break;
      }
      if(!theNextDaug) {
        continue;
      }
      if(  abs(theDaug->pdgId())==15&&abs(theNextDaug->pdgId())==15) {
        //Zboson
        if(thePriPart==theDaug||thePriPart==theNextDaug) { isZboson=true; break;}
        skipnext=true;
      } else if(abs(theDaug->pdgId())==15&&abs(theNextDaug->pdgId())==16) {
        //WBoson
        if(thePriPart==theDaug||thePriPart==theNextDaug) { isWboson=true; break;}
        skipnext=true;
      }
    }
    if(isWboson) return WBoson;
    if(isZboson) return ZBoson;
  }
  //--Sherpa Z->tautau
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&NumOfTau==2){
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return ZBoson;
  }

  //--Sherpa W->taunu  new
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&NumOfTau==1&&NumOfTauNeut==1) return WBoson;

  //--Sherpa ZZ,ZW
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==4&&
     (NumOfEl+NumOfPos+NumOfMuPl+NumOfMuMin+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut==4) ) {
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return DiBoson;
  }

  //--Sherpa VVV -- Note, have to allow for prompt photon radiation or these get lost
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon-NumOfPhot)==6&&
     (NumOfEl+NumOfPos+NumOfMuPl+NumOfMuMin+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut==6) ) {
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return MultiBoson;
  }

  //New Sherpa Z->tautau
  if(partOriVert==m_MothOriVert){
    int NumOfTauLoop=0;
    int NumOfTauNeuLoop=0;
    int NumOfLepLoop=0;
    for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles(); ipOut++){
      for(unsigned int ipIn=0;ipIn<partOriVert->nIncomingParticles();ipIn++){
        if(!partOriVert->outgoingParticle(ipOut)) continue;
        if(!partOriVert->incomingParticle(ipIn)) continue;
        if (partOriVert->outgoingParticle(ipOut)->barcode()==partOriVert->incomingParticle(ipIn)->barcode()){
          if(std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==15) NumOfTauLoop++;
          if(std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==16) NumOfTauNeuLoop++;
          if(std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==11||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==12||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==13||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==14||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==15||
             std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==16) NumOfLepLoop++;
        }
      }
    }
    if(NumOfTauLoop==2&&NumOfTauNeuLoop==0) return ZBoson;
    if(NumOfTauLoop==1&&NumOfTauNeuLoop==1) return WBoson;
    if((NumOfTauLoop==4&&NumOfTauNeuLoop==0)||
       (NumOfTauLoop==3&&NumOfTauNeuLoop==1)||
       (NumOfTauLoop==2&&NumOfTauNeuLoop==2)) return DiBoson;
    if(NumOfLepLoop==4) return DiBoson;

  }


  if( abs(m_MotherPDG)==25 )                   return Higgs;

  if( abs(m_MotherPDG)==35||
      abs(m_MotherPDG)==36||
      abs(m_MotherPDG)==37)                    return HiggsMSSM;

  if( abs(m_MotherPDG)==32||
      abs(m_MotherPDG)==33||
      abs(m_MotherPDG)==34)                    return HeavyBoson;


  if(abs(m_MotherPDG)==9900024)                return WBosonLRSM;
  if(abs(m_MotherPDG)==9900016)                return NuRTau;

  if( MC::PID::isSUSY(m_MotherPDG) )           return SUSY;

  if( MC::PID::isBSM(m_MotherPDG) )            return OtherBSM;

  if ( abs(m_MotherPDG) == 443 )               return JPsi;

  ParticleType pType = defTypeOfHadron(m_MotherPDG);
  return convHadronTypeToOrig(pType);
}


//-------------------------------------------------------------------------------
ParticleType MCTruthClassifier::defTypeOfPhoton(ParticleOrigin PhotOrig){
  //-------------------------------------------------------------------------------

  if (PhotOrig == NonDefined)   return UnknownPhoton;

  if (PhotOrig == WBoson     || PhotOrig == ZBoson  || PhotOrig == SinglePhot ||
      PhotOrig == Higgs      || PhotOrig == HiggsMSSM  || PhotOrig == HeavyBoson  ||
      PhotOrig == PromptPhot || PhotOrig == SUSY || PhotOrig == OtherBSM)   return IsoPhoton;

  if (PhotOrig == ISRPhot ||  PhotOrig == FSRPhot || PhotOrig == TauLep ||
      PhotOrig == Mu      ||  PhotOrig == NuREle   || PhotOrig == NuRMu ||
      PhotOrig == NuRTau  )
    return  NonIsoPhoton;

  return BkgPhoton;

}
//2345678901234567890123456789012345678901234567890123456789012345678901234567890
//-------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defOrigOfPhoton(const xAOD::TruthParticleContainer* mcTruthTES,
                                                  const xAOD::TruthParticle  *thePart)
//-------------------------------------------------------------------------------
{

  ATH_MSG_DEBUG( "Executing DefOrigOfPhoton " );

  m_Mother              = 0;
  m_MotherPDG           = 0;
  m_MotherBarcode       = 0;
  m_MotherStatus        = 0;
  m_PhotonMotherPDG     = 0;
  m_PhotonMotherBarcode = 0;

  m_ParticleOutCome = UnknownOutCome;


  int PriBarcode = thePart->barcode()%m_barcodeShift;

  const xAOD::TruthParticle* thePriPart = barcode_to_particle(mcTruthTES,PriBarcode);
  if(!thePriPart) return NonDefined;
  if(abs(thePriPart->pdgId())!=22) return  NonDefined;

  const xAOD::TruthVertex*   partOriVert = thePriPart->hasProdVtx() ? thePriPart->prodVtx() : 0;

  //-- to define photon outcome status
  m_ParticleOutCome = defOutComeOfPhoton(thePriPart);

  if(!partOriVert) {
    return NonDefined;
  }

  m_NumOfParents=partOriVert->nIncomingParticles();
  if(partOriVert->nIncomingParticles()>1)
    ATH_MSG_DEBUG( "DefOrigOfPhoton:: photon  has more than one mother " );

  m_Mother        = getMother(thePriPart);
  if(!m_Mother) return NonDefined;
  m_MotherPDG     = m_Mother->pdgId();
  m_MothOriVert   = m_Mother->hasProdVtx() ? m_Mother->prodVtx() : 0;
  m_MotherBarcode = m_Mother->barcode();
  m_MotherStatus  = m_Mother->status();
  partOriVert   = m_Mother->decayVtx();
  m_NumOfParents  = partOriVert->nIncomingParticles();
  m_NumOfDaug     = partOriVert->nOutgoingParticles();

  int  NumOfNucFr(0),NumOfEl(0),NumOfPos(0),NumOfMu(0),NumOfTau(0), NumOfPht(0),NumOfLQ(0);
  long DaugBarcode(0),DaugType(0), NumOfLep(0), NumOfNeut(0), NumOfPartons(0);

  for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles();ipOut++){
    if(!partOriVert->outgoingParticle(ipOut)) continue;
    DaugType = partOriVert->outgoingParticle(ipOut)->pdgId();
    if(m_NumOfParents==1&&(m_MotherPDG==22||abs(m_MotherPDG)==11||abs(m_MotherPDG)==211)&&
       (DaugType > 1000000000 ||DaugType==0 ||DaugType == 2212||DaugType == 2112))  NumOfNucFr++;
    if(DaugType      == 22) NumOfPht++;
    if(DaugType      == 11) NumOfEl++;
    if(DaugType      ==-11) NumOfPos++;
    if(abs(DaugType) == 13) NumOfMu++;
    if(abs(DaugType) == 15) NumOfTau++;
    if(abs(DaugType) == 42) NumOfLQ++;
    if(abs(DaugType) == 11||abs(DaugType) == 13||abs(DaugType) == 15) NumOfLep++;
    if(abs(DaugType) == 12||abs(DaugType) == 14||abs(DaugType) == 16) NumOfNeut++;
    if(abs(DaugType)<11 || (abs(DaugType)>16&&abs(DaugType)<43&&abs(DaugType)!=22)) NumOfPartons++;

    if(DaugType == m_MotherPDG ) DaugBarcode = partOriVert->outgoingParticle(ipOut)->barcode() ;
  } // cycle itrDaug

  bool foundISR=false;
  bool foundFSR=false;

  if ( m_NumOfParents == 1 && m_NumOfDaug == 2 &&
       DaugBarcode%m_barcodeShift==m_MotherBarcode%m_barcodeShift )   return  BremPhot;

  if ( m_NumOfParents == 1 && m_NumOfDaug == 2 &&
       abs(m_MotherPDG)==11&& NumOfPht==2 )   return  ElMagProc;


  // decay of W,Z and Higgs to lepton with FSR generated by Pythia
  if(m_NumOfParents == 1 && m_NumOfDaug==2&&
     (abs(m_MotherPDG)==11||abs(m_MotherPDG)==13||abs(m_MotherPDG)==15)&&
     DaugBarcode%m_barcodeShift!=m_MotherBarcode%m_barcodeShift&& m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()==1){
    int itr=0;
    long PartPDG=0;
    const xAOD::TruthVertex* prodVert = m_MothOriVert;
    const xAOD::TruthVertex*Vert=0;
    do {
      Vert = prodVert;
      for(unsigned int ipIn=0;ipIn<Vert->nIncomingParticles();ipIn++){
        if(!Vert->incomingParticle(ipIn)) continue;
        PartPDG   = abs(Vert->incomingParticle(ipIn)->pdgId());
        prodVert  =  Vert->incomingParticle(ipIn)->hasProdVtx() ?  Vert->incomingParticle(ipIn)->prodVtx() : 0;
        if(PartPDG==23||PartPDG==24||PartPDG==25) foundFSR=true;
      }// cycle itrMother
      itr++;
      if(itr>100) {ATH_MSG_WARNING( "DefOrigOfPhoton:: infinite while" ); break;}
    }  while (prodVert!=0&&abs(m_MotherPDG)==PartPDG);

    if(foundFSR) return  FSRPhot;

  }

  // Nucl reaction
  // gamma+Nuclear=>gamma+Nucl.Fr+Nuclons+pions
  // e+Nuclear=>e+gamma+Nucl.Fr+Nuclons+pions
  // pi+Nuclear=>gamma+Nucl.Fr+Nuclons+pions

  if ( ( m_NumOfParents == 1&&(abs(m_MotherPDG)==22||abs(m_MotherPDG)==11)&&m_NumOfDaug>2&&NumOfNucFr!=0)||
       ( m_NumOfParents == 1&&abs(m_MotherPDG)==211&&m_NumOfDaug>10&&NumOfNucFr!=0) ||
       ( m_NumOfParents == 1&&m_MotherPDG==22&&m_NumOfDaug>10&&m_MotherStatus==1)||
       ( m_NumOfParents == 1&&m_MotherPDG>1000000000)
       )     return  NucReact;

  if ( abs(m_MotherPDG)==13 &&  NumOfMu==0 )        return  Mu;

  if ( abs(m_MotherPDG)==15 &&  NumOfTau==0 )       return  TauLep;

  //-- to find prompt photon
  if (abs(partOriVert->barcode())==5){
    int nqin(0),ngin(0),nqout(0),ngout(0),npartin(0),npartout(0),nphtout(0);
    if(partOriVert!=0){
      npartin = partOriVert->nIncomingParticles();
      for(unsigned int ipIn=0;ipIn<partOriVert->nIncomingParticles();ipIn++){
        if(!partOriVert->incomingParticle(ipIn)) continue;
        if(abs(partOriVert->incomingParticle(ipIn)->pdgId())<7) nqin++;
        if(abs(partOriVert->incomingParticle(ipIn)->pdgId())==21) ngin++;
      }

      npartout = partOriVert->nOutgoingParticles();
      for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles();ipOut++){
        if(!partOriVert->outgoingParticle(ipOut)) continue;
        if(abs(partOriVert->outgoingParticle(ipOut)->pdgId())<7) nqout++;
        if(abs(partOriVert->outgoingParticle(ipOut)->pdgId())==21) ngout++;
        if(abs(partOriVert->outgoingParticle(ipOut)->pdgId())==22) nphtout++;

      }
    }

    if(npartout==2&&npartin==2&&
       ( ((nqin==2&&ngin==0)&&(nqout==0&&ngout==1&&nphtout==1)) ||
         ((nqin==2&&ngin==0)&&(nqout==0&&ngout==0&&nphtout==2)) ||
         ((nqin==1&&ngin==1)&&(nqout==1&&ngout==0&&nphtout==1)) ||
         ((nqin==0&&ngin==2)&&(nqout==0&&ngout==0&&nphtout==2)) ||
         ((nqin==0&&ngin==2)&&(nqout==0&&ngout==1&&nphtout==1))
         )  ) return  PromptPhot;
  }

  //-- to find initial and final state raiation and underline photons
  //-- dijets and min bias
  if(m_NumOfParents==1&&m_MotherStatus==3){

    //  const xAOD::TruthParticle* thePhotMoth = (*mcEventItr)->barcode_to_particle(m_MotherBarcode);
    //  const xAOD::TruthVertex*   MothOriVert = thePhotMoth->prodVtx();
    if(m_MothOriVert!=0&&abs(m_MothOriVert->barcode())==5) return FSRPhot;

    for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles();ipOut++){
      if(!partOriVert->outgoingParticle(ipOut)) continue;
      if(partOriVert->outgoingParticle(ipOut)->status()!=3||
         m_MotherPDG!=partOriVert->outgoingParticle(ipOut)->pdgId()) continue;
      const xAOD::TruthVertex* Vrtx=partOriVert->outgoingParticle(ipOut)->decayVtx();
      if(!Vrtx) continue;
      if(abs(Vrtx->barcode())==5) foundISR=true;
    }

    if( foundISR ) return ISRPhot;
    else           return UndrPhot;

  }

  //-- to find initial and final state radiation and underline photons
  //-- SUSY
  if(m_NumOfParents==1&&(abs(m_MotherPDG)<7||m_MotherPDG==21)&&
     !(m_NumOfDaug==NumOfPht+NumOfPartons&&(m_MotherStatus==62||m_MotherStatus==52||m_MotherStatus==21||m_MotherStatus==22))){
    for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles();ipOut++){
      if(!partOriVert->outgoingParticle(ipOut)) continue;
      if(m_MotherPDG!=partOriVert->outgoingParticle(ipOut)->pdgId()) continue;
      const xAOD::TruthVertex* Vrtx = partOriVert->outgoingParticle(ipOut)->decayVtx();
      if(!Vrtx) continue;
      if(Vrtx->nOutgoingParticles()!=1&&Vrtx->nIncomingParticles()==1) continue;
      if(!Vrtx->outgoingParticle(0)) continue;
      if(Vrtx->outgoingParticle(0)->pdgId()==91) foundISR=true;
    }

    if( foundISR ) return ISRPhot;
    else           return UndrPhot;

  }

  //-- to find final  state radiation
  //-- Exotics
  if(m_NumOfParents==1&&abs(m_MotherPDG)==11&&m_MotherStatus==2&&m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()!=0){
    const xAOD::TruthParticle*  itrP =m_MothOriVert->incomingParticle(0);
    const xAOD::TruthVertex* Vrtx= itrP->hasProdVtx() ? itrP->prodVtx() : 0 ;
    if(m_MothOriVert->nIncomingParticles()==1&&abs(itrP->pdgId())==11&&Vrtx!=0&&
       abs(Vrtx->barcode())==5&&itrP->status()==3) return FSRPhot;
  }


  // FSR  from Photos
  //-- Exotics- CompHep
  if(m_NumOfParents==2&&((abs(m_MotherPDG)==11&&NumOfEl==1&&NumOfPos==1)||
                         (abs(m_MotherPDG)==13&&NumOfMu==2)||(abs(m_MotherPDG)==15&&NumOfTau==2))) {
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if (abs(pdg1)==abs(pdg2))              return FSRPhot;
  }


  if(m_NumOfParents==2&&NumOfLep==1&&NumOfNeut==1&&(abs(m_MotherPDG)==11||abs(m_MotherPDG)==12))  return FSRPhot;




  //-- Exotics - CompHep
  if (abs(m_MotherPDG)==11&& m_NumOfParents == 1 && m_NumOfDaug == 2 && (NumOfEl==1||NumOfPos==1)&&NumOfPht==1&&
      DaugBarcode%m_barcodeShift!=m_MotherBarcode%m_barcodeShift&&DaugBarcode<20000&&m_MotherBarcode<20000 )   return  FSRPhot;


  // FSR  from Photos
  if ( abs(m_MotherPDG)==23&&( (NumOfEl+NumOfPos==2 || NumOfEl+NumOfPos==4 )||
                               (NumOfMu==2||NumOfMu==4) ||
                               (NumOfTau==2||NumOfTau==4)) &&NumOfPht>0)
    return  FSRPhot;

  if( abs(m_MotherPDG)==9900024&&NumOfPht>0) return  FSRPhot;

  if( abs(m_MotherPDG)==9900012&&NumOfPht>0) return  FSRPhot;
  if( abs(m_MotherPDG)==9900014&&NumOfPht>0) return  FSRPhot;
  if( abs(m_MotherPDG)==9900016&&NumOfPht>0) return  FSRPhot;

  if(m_NumOfParents==2&&NumOfLQ==1)          return  FSRPhot;


  //--- other process


  if ( abs(m_MotherPDG)==23)                return  ZBoson;


  if ( abs(m_MotherPDG)==24 ) {

    if(NumOfLep==1&&NumOfNeut==1&&m_NumOfDaug==NumOfLep+NumOfNeut+NumOfPht) return  FSRPhot;

    if(m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()!=0){
      const xAOD::TruthVertex* prodVert = m_MothOriVert;
      const xAOD::TruthParticle*  itrP;
      do {
        itrP =prodVert->incomingParticle(0);
        prodVert= itrP->hasProdVtx() ? itrP->prodVtx() : 0;
      } while(abs(itrP->pdgId())==24&&prodVert!=0);

      if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==15)           return TauLep;
      else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==13)      return Mu;
      else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900012) return NuREle;
      else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900014) return NuRMu;
      else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(itrP->pdgId())==9900016) return NuRTau;

    } else  return  WBoson;
  }

  //MadGraphPythia ZWW*->lllnulnu
  if(m_NumOfParents==1&&m_NumOfDaug>4&&(abs(m_MotherPDG)<7||m_MotherPDG==21)){
    bool isZboson=false;
    bool isWboson=false;
    bool skipnext=false;
    for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles()-1;ipOut++){
      if(skipnext) {skipnext=false; continue;}
      const xAOD::TruthParticle* theDaug=partOriVert->outgoingParticle(ipOut);
      if(!theDaug) continue;
      const xAOD::TruthParticle* theNextDaug=0;
      for(unsigned int ipOut1=ipOut+1;ipOut1<partOriVert->nOutgoingParticles();ipOut1++) {
        theNextDaug=partOriVert->outgoingParticle(ipOut1);
        if(theNextDaug!=0) break;
      }
      if(!theNextDaug) continue;

      if(abs(theDaug->pdgId())==15&&abs(theNextDaug->pdgId())==15 ){
        //Zboson
        if(thePriPart==theDaug||thePriPart==theNextDaug) { isZboson=true; break;}
        skipnext=true;
      } else if( abs(theDaug->pdgId())==15&&abs(theNextDaug->pdgId())==16) {
        //WBoson
        if(thePriPart==theDaug||thePriPart==theNextDaug) { isWboson=true; break;}
        skipnext=true;
      }
    }
    if(isWboson) return WBoson;
    if(isZboson) return ZBoson;
  }

  //--Sherpa ZZ,ZW+FSR
  if(m_NumOfParents==4&&(m_NumOfDaug-NumOfPht)==4&&(NumOfLep+NumOfNeut==4)) {
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    int pdg3=partOriVert->incomingParticle(2)->pdgId();
    int pdg4=partOriVert->incomingParticle(3)->pdgId();
    if(abs(pdg1)<17&&abs(pdg1)>10&&abs(pdg2)<17&&abs(pdg2)>10&&abs(pdg3)<17&&abs(pdg3)>10&&
       abs(pdg4)<17&&abs(pdg4)>10 ) return FSRPhot;
  }

  //--New Sherpa single photon
  if(partOriVert==m_MothOriVert&&partOriVert!=0){
    int NumOfPhtLoop=0;
    for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles(); ipOut++){
      if(!partOriVert->outgoingParticle(ipOut)) continue;
      for(unsigned int ipIn=0;ipIn<partOriVert->nIncomingParticles();ipIn++){
        if(!partOriVert->incomingParticle(ipIn)) continue;
        if (partOriVert->outgoingParticle(ipOut)->barcode()==partOriVert->incomingParticle(ipIn)->barcode()&&
            std::abs(partOriVert->outgoingParticle(ipOut)->pdgId())==22) NumOfPhtLoop++;
        if(NumOfPhtLoop==1) return SinglePhot;
      }
    }
  }


  if ( abs(m_MotherPDG)==25 )               return  Higgs;

  if ( abs(m_MotherPDG)==111 )              return  PiZero;

  if( abs(m_MotherPDG)==35||
      abs(m_MotherPDG)==36||
      abs(m_MotherPDG)==37)                return HiggsMSSM;

  if( abs(m_MotherPDG)==32||
      abs(m_MotherPDG)==33||
      abs(m_MotherPDG)==34||
      abs(m_MotherPDG)==5100039 // KK graviton
      )                                    return HeavyBoson;

  if( MC::PID::isSUSY(m_MotherPDG) )       return SUSY;

  if( MC::PID::isBSM(m_MotherPDG) )        return OtherBSM;

  // Pythia8 gamma+jet samples
  if ((m_MotherStatus==62||m_MotherStatus==52||m_MotherStatus==21||m_MotherStatus==22) &&
      thePriPart->status()==1 && NumOfPht==1 && m_NumOfDaug==(NumOfPht+NumOfPartons) ){
    return PromptPhot;
  }

  ParticleType pType = defTypeOfHadron(m_MotherPDG);
  if( (pType==BBbarMesonPart || pType==CCbarMesonPart )
      && m_MothOriVert!=0&&isHardScatVrtx(m_MothOriVert))  m_isPrompt=true;
  return convHadronTypeToOrig(pType);
}
//2345678901234567890123456789012345678901234567890123456789012345678901234567890
//-------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defOrigOfNeutrino(const xAOD::TruthParticleContainer* mcTruthTES,
                                                    const xAOD::TruthParticle  *thePart)
//-------------------------------------------------------------------------------
{
  //author - Pierre-Antoine Delsart
  //
  ATH_MSG_DEBUG( "Executing DefOrigOfNeutrino " );

  int PriBarcode  = thePart->barcode()%m_barcodeShift;
  int nuFlav = abs(thePart->pdgId());
  const xAOD::TruthParticle* thePriPart = barcode_to_particle(mcTruthTES,PriBarcode);
  if(!thePriPart) return NonDefined;
  if(abs(thePriPart->pdgId())!=nuFlav) return NonDefined;

  const xAOD::TruthVertex*   partOriVert = thePriPart->hasProdVtx() ? thePriPart->prodVtx() : 0;

  //-- to define neutrino outcome status
  m_ParticleOutCome = NonInteract;

  if(!partOriVert) return NonDefined;

  m_NumOfParents=-1;
  m_NumOfParents=partOriVert->nIncomingParticles();
  if(m_NumOfParents>1)
    ATH_MSG_DEBUG( "DefOrigOfNeutrino:: neutrino  has more than one mother " );

  m_Mother      = getMother(thePriPart);
  if(!m_Mother) {
    return NonDefined;
  }
  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();
  m_MothOriVert   = m_Mother->hasProdVtx() ? m_Mother->prodVtx() : 0;

  //to resolve Sherpa loop
  bool samePart=false;
  if(m_MothOriVert&&m_MothOriVert->barcode() == partOriVert->barcode()) samePart=true;
  //
  if( ( abs(m_MotherPDG)==nuFlav||abs(m_MotherPDG)==15||abs(m_MotherPDG)==24)&& m_MothOriVert!=0&&!samePart){
    long pPDG(0);
    const xAOD::TruthParticle*  MotherParent(0);
    do {
      pPDG=0;
      MotherParent=getMother(m_Mother);
      //to prevent Sherpa loop
      const xAOD::TruthVertex*  mother_prdVtx(0);
      const xAOD::TruthVertex*  mother_endVtx(0);
      if(m_Mother) {
        mother_prdVtx=m_Mother->hasProdVtx() ? m_Mother->prodVtx():0;
        mother_endVtx=m_Mother->decayVtx();
      }
      const xAOD::TruthVertex*  parent_prdVtx(0);
      const xAOD::TruthVertex*  parent_endVtx(0);
      if(MotherParent){
        parent_prdVtx=MotherParent->hasProdVtx() ? MotherParent->prodVtx():0;
        parent_endVtx=MotherParent->decayVtx();
      }
      if(mother_endVtx==parent_prdVtx&&mother_prdVtx==parent_endVtx){
        MotherParent=m_Mother;
        break;
      }
      //
      if(MotherParent) {
        pPDG = MotherParent->pdgId();
      }
      //to prevent Sherpa loop
      if(m_Mother == MotherParent) {
        break;
      }
      if(abs(pPDG)==nuFlav || abs(pPDG)==15 || abs(pPDG)==24)  {
        m_Mother = MotherParent;
      }

    }  while ((abs(pPDG)==nuFlav || abs(pPDG)==15 || abs(pPDG)==24) );

    if(abs(pPDG)==nuFlav || abs(pPDG)==15 || abs(pPDG)==24 || abs(pPDG)==23 ||
       abs(pPDG)==25 || abs(pPDG)==35 || abs(pPDG)==36 || abs(pPDG)==37 ||
       abs(pPDG)==32 || abs(pPDG)==33 || abs(pPDG)==34 || abs(pPDG)==6  ||
       abs(pPDG)==9900024 || abs(pPDG)== 9900012 || abs(pPDG)== 9900014 || abs(pPDG)== 9900016 ||
       (abs(pPDG)<2000040&& abs(pPDG)>1000001))  m_Mother = MotherParent;
  }


  m_MotherPDG     = m_Mother->pdgId();
  m_MotherBarcode = m_Mother->barcode();
  m_MotherStatus  = m_Mother->status();
  partOriVert   = m_Mother->decayVtx();
  m_MothOriVert   = m_Mother->hasProdVtx() ? m_Mother->prodVtx() : 0;
  m_NumOfParents  = partOriVert->nIncomingParticles();
  m_NumOfDaug     = partOriVert->nOutgoingParticles();

  int  NumOfPhot(0),NumOfNucFr(0);
  int  NumOfquark(0),NumOfgluon(0),NumOfLQ(0);
  int  NumOfElNeut(0),NumOfMuNeut(0), NumOfTauNeut(0);
  int  NumOfEl(0),NumOfMu(0), NumOfTau(0);
  long DaugType(0);
  samePart=false;

  for (unsigned int ipOut=0; ipOut<partOriVert->nOutgoingParticles(); ++ipOut) {
    const xAOD::TruthParticle* theDaug=partOriVert->outgoingParticle(ipOut);
    if(!theDaug) continue;
    DaugType = theDaug->pdgId();
    if( abs(DaugType)  < 7  ) NumOfquark++;
    if( abs(DaugType) == 21 ) NumOfgluon++;
    if( abs(DaugType) == 12 ) NumOfElNeut++;
    if( abs(DaugType) == 14 ) NumOfMuNeut++;
    if( abs(DaugType) == 16 ) NumOfTauNeut++;
    if( DaugType      == 22 ) NumOfPhot++;
    if( abs(DaugType) == 11 ) NumOfEl++;
    if( abs(DaugType) == 13 ) NumOfMu++;
    if( abs(DaugType) == 15 ) NumOfTau++;

    if( abs(DaugType) == 42 ) NumOfLQ++;
    if( abs(DaugType) == abs(m_MotherPDG)&&
        theDaug->barcode()%m_barcodeShift == m_MotherBarcode%m_barcodeShift ) samePart=true;
    if(m_NumOfParents==1&&(m_MotherPDG==22||abs(m_MotherPDG)==11||abs(m_MotherPDG)==13||abs(m_MotherPDG)==211)&&
       (DaugType > 1000000000||DaugType==0 ||DaugType == 2212||DaugType == 2112 || abs(DaugType) == 211|| abs(DaugType) == 111))  NumOfNucFr++;
  } // cycle itrDaug

  // Quark weak decay
  if ( abs(m_MotherPDG)< 7 &&m_NumOfParents == 1&&m_NumOfDaug==3&&NumOfquark==1&&
       (NumOfEl==1 || NumOfMu==1 || NumOfTau==1) ) return QuarkWeakDec;

  if( abs(m_MotherPDG)==6 )                    return top;

  if( abs(m_MotherPDG)==24&&m_MothOriVert!=0&&m_MothOriVert->nIncomingParticles()!=0 ) {

    const xAOD::TruthVertex* prodVert = m_MothOriVert;
    const xAOD::TruthParticle* ptrPart;
    do {
      ptrPart=prodVert->incomingParticle(0);
      prodVert=ptrPart->hasProdVtx() ? ptrPart->prodVtx() : 0;
    } while(abs(ptrPart->pdgId())==24&&prodVert!=0);

    if(prodVert&&prodVert->nIncomingParticles()==1&&abs(ptrPart->pdgId())==9900012)      return NuREle;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(ptrPart->pdgId())==9900014) return NuRMu;
    else if(prodVert&&prodVert->nIncomingParticles()==1&&abs(ptrPart->pdgId())==9900016) return NuRTau;
    return WBoson;
  }else if( abs(m_MotherPDG)==24) return WBoson;

  if( abs(m_MotherPDG)==23 ) return ZBoson;

  //-- Exotics
  if( abs(m_MotherPDG)<7&&m_NumOfParents==2&&m_NumOfDaug==2&&(NumOfEl==1||NumOfMu==1||NumOfTau==1)&&
      partOriVert->barcode()==-5){
    int pdg1 = partOriVert->incomingParticle(0)->pdgId();
    int pdg2 = partOriVert->incomingParticle(1)->pdgId();
    const xAOD::TruthVertex* TmpVert= partOriVert->outgoingParticle(0)->decayVtx();

    if(abs(pdg1)==abs(pdg2)&&TmpVert->nIncomingParticles()==2)   return ZorHeavyBoson;
  }

  if(abs(m_MotherPDG)<7&&m_NumOfParents==2&&m_NumOfDaug==2&&(NumOfEl==1||NumOfMu==1||NumOfTau==1)&&
     (NumOfElNeut==1||NumOfMuNeut==1||NumOfTauNeut==1)&&partOriVert->barcode()==-5)  {
    const xAOD::TruthVertex* TmpVert= partOriVert->outgoingParticle(0)->decayVtx();
    if(TmpVert->nIncomingParticles()==2) return HeavyBoson;
  }

  //MadGraphPythia ZWW*->lllnulnu or ZWW*->nunulnulnu (don't even know if the latter is generated)
  if(m_NumOfParents==1&&m_NumOfDaug>4&&(abs(m_MotherPDG)<7||m_MotherPDG==21)){

    const xAOD::TruthParticle* thePartToCheck=thePriPart;
    const xAOD::TruthParticle* theMother= thePriPart->hasProdVtx() ? thePriPart->prodVtx()->incomingParticle(0) : 0;

    if( abs(theMother->pdgId())==11&&theMother->status()==2) thePartToCheck=theMother;

    bool isZboson=false;
    bool isWboson=false;
    bool skipnext=false;

    for (unsigned int ipOut=0; ipOut<partOriVert->nOutgoingParticles()-1; ++ipOut) {
      const xAOD::TruthParticle* theDaug=partOriVert->outgoingParticle(ipOut);
      if(!theDaug) continue;
      const xAOD::TruthParticle* theNextDaug=0;
      for(unsigned int ipOut1=ipOut+1;ipOut1<partOriVert->nOutgoingParticles();ipOut1++) {
        theNextDaug=partOriVert->outgoingParticle(ipOut1);
        if(theNextDaug!=0) break;
      }
      if(!theNextDaug) continue;

      if(skipnext) {skipnext=false; continue;}

      int apdgID1 = abs(theDaug->pdgId());
      int apdgID2 = abs(theNextDaug->pdgId());
      if( apdgID1==apdgID2 &&
         (apdgID1==12||apdgID1==14||apdgID1==16) ) {
        //Zboson
        if(thePartToCheck==theDaug||thePartToCheck==theNextDaug) { isZboson=true; break;}
        skipnext=true;
      } else if( (apdgID1==11&&apdgID2==12) ||
                 (apdgID1==13&&apdgID2==14) ||
                 (apdgID1==15&&apdgID2==16) ) {
        //WBoson
        if(thePartToCheck==theDaug||thePartToCheck==theNextDaug) { isWboson=true; break;}
        skipnext=true;
      }
    }
    if(isWboson) return WBoson;
    if(isZboson) return ZBoson;
  }

  //--Sherpa Z->nunu
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&
     (NumOfElNeut==2 || NumOfMuNeut==2 || NumOfTauNeut==2) ) return ZBoson;

  //--Sherpa W->enu ??
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==2&&
     ((NumOfEl==1&&NumOfElNeut==1) || (NumOfMu==1&&NumOfMuNeut==1) || (NumOfTau==1&&NumOfTauNeut==1)) )  return WBoson;

  //--Sherpa ZZ,ZW
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon)==4&&
     (NumOfEl+NumOfMu+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut==4) ) {
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return DiBoson;
  }

  //--Sherpa VVV -- Note, have to allow for prompt photon radiation or these get lost
  if(m_NumOfParents==2&&(m_NumOfDaug-NumOfquark-NumOfgluon-NumOfPhot)==6&&
     (NumOfEl+NumOfMu+NumOfTau+NumOfElNeut+NumOfMuNeut+NumOfTauNeut==6) ) {
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if((abs(pdg1)==21&&abs(pdg2)==21)||(abs(pdg1)<7&&abs(pdg2)<7)||
       (pdg1==21&&abs(pdg2)<7)||(pdg2==21&&abs(pdg1)<7))  return MultiBoson;
  }

  //New Sherpa Z->nunu
  if(partOriVert==m_MothOriVert&&partOriVert!=0){
    int NumOfLepLoop=0;
    int NumOfNeuLoop=0;
    for(unsigned int ipOut=0;ipOut<partOriVert->nOutgoingParticles(); ipOut++){
      if(!partOriVert->outgoingParticle(ipOut)) continue;
      for(unsigned int ipIn=0;ipIn<partOriVert->nIncomingParticles();ipIn++){
        if(!partOriVert->incomingParticle(ipIn)) continue;
        if (partOriVert->outgoingParticle(ipOut)->barcode()==partOriVert->incomingParticle(ipIn)->barcode()){
          int apdgid = abs(partOriVert->outgoingParticle(ipOut)->pdgId());
          if(apdgid==12 || apdgid==14 || apdgid==16) NumOfNeuLoop++;
          if(apdgid==11||
             apdgid==13||
             apdgid==15) NumOfLepLoop++;
        }
      }
    }
    if(NumOfNeuLoop==2&&NumOfLepLoop==0) return ZBoson;
    if(NumOfNeuLoop==1&&NumOfLepLoop==1) return WBoson;
    if(NumOfNeuLoop+NumOfLepLoop==4) return DiBoson;
  }

  //-- McAtNLo
  if( abs(m_MotherPDG)<7&&m_NumOfParents==2&&(NumOfElNeut==2||NumOfMuNeut==2||NumOfTauNeut==2)&&partOriVert->barcode()==-1){
    int pdg1=partOriVert->incomingParticle(0)->pdgId();
    int pdg2=partOriVert->incomingParticle(1)->pdgId();
    if(abs(pdg1)==abs(pdg2))                   return ZBoson;
  }

  if( abs(m_MotherPDG)==25 )                   return Higgs;

  if( abs(m_MotherPDG)==35||
      abs(m_MotherPDG)==36||
      abs(m_MotherPDG)==37)                    return HiggsMSSM;

  if( abs(m_MotherPDG)==32||
      abs(m_MotherPDG)==33||
      abs(m_MotherPDG)==34)                    return HeavyBoson;

  if( abs(m_MotherPDG)==15){

    ParticleOrigin  tauOrig= defOrigOfTau (mcTruthTES,m_Mother);
    ParticleType    tautype=defTypeOfTau(tauOrig);
    if(tautype==IsoTau) return tauOrig;
    else return TauLep;
  }

  if(abs(m_MotherPDG)==9900024)                return WBosonLRSM;
  if(abs(m_MotherPDG)==9900012)                return NuREle;
  if(abs(m_MotherPDG)==9900014)                return NuRMu;
  if(abs(m_MotherPDG)==9900016)                return NuRTau;

  if (abs(m_MotherPDG) == 42 || NumOfLQ!=0 )   return LQ;

  if( MC::PID::isSUSY(m_MotherPDG) )           return SUSY;

  if( MC::PID::isBSM(m_MotherPDG) )            return OtherBSM;

  ParticleType pType = defTypeOfHadron(m_MotherPDG);
  if( (pType==BBbarMesonPart || pType==CCbarMesonPart )
      && m_MothOriVert!=0&&isHardScatVrtx(m_MothOriVert))  m_isPrompt=true;

  //----
  return convHadronTypeToOrig(pType);

}
//---------------------------------------------------------------------------------
float MCTruthClassifier::detPhi(float x, float y){
  //---------------------------------------------------------------------------------
  float det;
  det=x-y;
  if(det  >  M_PI) det=det-2.*M_PI;
  if(det < - M_PI) det=det+2.*M_PI;
  return std::abs(det);
}
//---------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::convHadronTypeToOrig(ParticleType pType){
  //---------------------------------------------------------------------------------
  if(pType==BBbarMesonPart&&
     std::abs(m_MotherPDG)==443)         return JPsi;
  else if(pType==BBbarMesonPart)    return BBbarMeson;
  else if(pType==BottomMesonPart)   return BottomMeson;
  else if(pType==BottomBaryonPart)  return BottomBaryon;
  else if(pType==CCbarMesonPart)    return CCbarMeson;
  else if(pType==CharmedMesonPart)  return CharmedMeson;
  else if(pType==CharmedBaryonPart) return CharmedBaryon;
  else if(pType==StrangeBaryonPart) return StrangeBaryon;
  else if(pType==StrangeMesonPart)  return StrangeMeson;
  else if(pType==LightBaryonPart)   return LightBaryon;
  else if(pType==LightMesonPart)    return LightMeson;
  else                              return NonDefined;
}
//---------------------------------------------------------------------------------
ParticleOrigin MCTruthClassifier::defHadronType(long pdg){
  //---------------------------------------------------------------------------------

  if ( abs(pdg) == 443 )     return JPsi;

  int q1=(abs(pdg)/1000)%10;
  int q2=(abs(pdg)/100)%10;
  int q3=(abs(pdg)/10)%10;

  // di quark
  // if( q3 == 0 && q2 >=q3 )   cout<<"di quark"<<endl;

  if( q1 == 0 && q2 == 5 && q3 == 5 )            return BBbarMeson;
  else if( q1 == 0 && q3<5 && q3>0 && q2 == 5 )  return BottomMeson;
  else if( q1 == 0 && q3 == 4 && q2 == 4 )       return CCbarMeson;
  else if(q1==0&&q3<4&&q3>0&&q2==4)              return CharmedMeson;
  else if( q1 == 5 )                             return BottomBaryon;
  else if( q1 == 4 )                             return CharmedBaryon;
  else if( q1 == 3 )                             return StrangeBaryon;
  else if( q1 == 2 || q1 == 1 )                  return LightBaryon;
  else if((q1==0&&q2==3&&q3<3&&q3>0)||abs(pdg)==130)
    return StrangeMeson;
  else if( (q1==0&&(q3==1||q3==2) && (q2==1||q2==2)) || (q1==0&&q3==3&&q2==3))
    return  LightMeson;
  else                                           return  NonDefined;

}

//---------------------------------------------------------------------------------
ParticleType MCTruthClassifier::defTypeOfHadron(long pdg){
  //---------------------------------------------------------------------------------

  int q1=(abs(pdg)/1000)%10;
  int q2=(abs(pdg)/100)%10;
  int q3=(abs(pdg)/10)%10;

  // di quark
  // if( q3 == 0 && q2 >=q3 )   cout<<"di quark"<<endl;

  if( q1 == 0 && q2 == 5 && q3 == 5 )            return BBbarMesonPart;
  else if( q1 == 0 && q3<5 && q3>0 && q2 == 5 )  return BottomMesonPart;
  else if( q1 == 0 && q3 == 4 && q2 == 4 )       return CCbarMesonPart;
  else if(q1==0&&q3<4&&q3>0&&q2==4)              return CharmedMesonPart;
  else if( q1 == 5 )                             return BottomBaryonPart;
  else if( q1 == 4 )                             return CharmedBaryonPart;
  else if( q1 == 3 )                             return StrangeBaryonPart;
  else if( q1 == 2 || q1 == 1 )                  return LightBaryonPart;
  else if((q1==0&&q2==3&&q3<3&&q3>0)||abs(pdg)==130)
    return StrangeMesonPart;
  else if( (q1==0&&(q3==1||q3==2) && (q2==1||q2==2)) || (q1==0&&q3==3&&q2==3))
    return  LightMesonPart;
  else                                           return  Unknown;

}

//-------------------------------------------------------------------------------
bool  MCTruthClassifier::isHardScatVrtx(const xAOD::TruthVertex* pVert){
  //-------------------------------------------------------------------------------

  if(pVert==0) return false;

  const xAOD::TruthVertex* pV = pVert;
  int numOfPartIn(0),pdg(0),NumOfParton(0);

  do {
    pVert = pV;
    numOfPartIn = pVert->nIncomingParticles();
    pdg = numOfPartIn!=0&&pVert->incomingParticle(0)!=0 ? pVert->incomingParticle(0)->pdgId() : 0;
    pV  = numOfPartIn!=0&&pVert->incomingParticle(0)!=0&&
      pVert->incomingParticle(0)->hasProdVtx() ? pVert->incomingParticle(0)->prodVtx(): 0;

  }  while (numOfPartIn==1&&(abs(pdg)<81||abs(pdg)>100)&&pV!=0);

  if(numOfPartIn==2) {
    for(unsigned int ipIn=0;ipIn<pVert->nIncomingParticles();ipIn++){
      if(!pVert->incomingParticle(ipIn)) continue;
      if(abs(pVert->incomingParticle(ipIn)->pdgId())<7||
         pVert->incomingParticle(ipIn)->pdgId()==21) NumOfParton++;
    }
    if(NumOfParton==2) return true;
  }

  return false;
}

//---------------------------------------------------------------------------------
bool MCTruthClassifier::isHadron(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------


  bool isPartHadron = false;
  if(thePart!=0) {
    long pdg=thePart->pdgId();
    ParticleType pType = defTypeOfHadron(pdg);
    // ParticleOrigin  partOrig=convHadronTypeToOrig(pType);
    //if(partOrig!=NonDefined) isPartHadron = true;
    if(pType!=Unknown) isPartHadron = true;
    //--exclude protons from beam
    if(pdg==2212&&thePart->status()==3) isPartHadron=false;
  }
  return isPartHadron;
}

//---------------------------------------------------------------------------------
const xAOD::TruthParticle*  MCTruthClassifier::getMother(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------
  ATH_MSG_DEBUG( "executing getMother" );

  const xAOD::TruthVertex*   partOriVert = thePart->hasProdVtx() ? thePart->prodVtx() : 0 ;

  long partPDG       = thePart->pdgId();
  int  partBarcode   = thePart->barcode()%m_barcodeShift;
  long MotherPDG(0);

  const xAOD::TruthVertex*   MothOriVert(0);
  const xAOD::TruthParticle* theMoth(0);

  if(!partOriVert) return theMoth;

  int itr=0;
  do {
    if(itr!=0)  partOriVert = MothOriVert;
    for(unsigned int ipIn=0;ipIn<partOriVert->nIncomingParticles();ipIn++){
      theMoth=partOriVert->incomingParticle(ipIn);
      if(!theMoth) continue;
      MotherPDG   = theMoth->pdgId();
      MothOriVert = theMoth->hasProdVtx() ? theMoth->prodVtx() : 0;
      if(MotherPDG==partPDG) break;
    }
    itr++;
    if(itr>100) { ATH_MSG_WARNING( "getMother:: infinite while" );  break;}
  }  while (MothOriVert!=0&&MotherPDG==partPDG&&partBarcode<m_barcodeG4Shift&&MothOriVert!=partOriVert);


  ATH_MSG_DEBUG( "succeded getMother" );
  return theMoth;
}

//---------------------------------------------------------------------------------
const xAOD::TruthVertex*  MCTruthClassifier::findEndVert(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------
  const xAOD::TruthVertex*  EndVert = thePart->decayVtx();
  const xAOD::TruthVertex* pVert(0);
  if(EndVert!=0){
    do {
      bool samePart=false;
      pVert=0;
      for(unsigned int ipOut=0;ipOut<EndVert->nOutgoingParticles();ipOut++){
        const xAOD::TruthParticle * itrDaug=EndVert->outgoingParticle(ipOut);
        if(!itrDaug) continue;
        if(( (itrDaug->barcode()%m_barcodeShift==thePart->barcode()%m_barcodeShift)||
             // brem on generator level for tau
             (EndVert->nOutgoingParticles()==1&&EndVert->nIncomingParticles()==1&&
              itrDaug->barcode()<m_barcodeG4Shift&&thePart->barcode()<m_barcodeG4Shift)
             ) &&
           itrDaug->pdgId() == thePart->pdgId()) {samePart=true; pVert=itrDaug->decayVtx();}
      } // cycle itrDaug
      if(samePart) EndVert=pVert;
    }  while (pVert!=0&&pVert!=EndVert);  //pVert!=EndVert to prevent Sherpa loop

  } // EndVert

  return EndVert;
}
//---------------------------------------------------------------------------------
ParticleOutCome MCTruthClassifier::defOutComeOfElectron(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------

  ParticleOutCome PartOutCome=UnknownOutCome;

  const xAOD::TruthVertex* EndVert = findEndVert(thePart);

  if(EndVert==0) return  NonInteract;

  int   NumOfElecDaug(0),ElecOutNumOfNucFr(0),ElecOutNumOfElec(0),NumOfHadr(0);
  long  EndDaugType(0);
  NumOfElecDaug=EndVert->nOutgoingParticles();
  for(unsigned int ipOut=0;ipOut<EndVert->nOutgoingParticles();ipOut++){
    if(!EndVert->outgoingParticle(ipOut)) continue;
    EndDaugType    =  EndVert->outgoingParticle(ipOut)->pdgId();
    if(abs(EndDaugType)==11)  ElecOutNumOfElec++;
    if(isHadron(EndVert->outgoingParticle(ipOut))) NumOfHadr++;
    if(EndDaugType >  1000000000||EndDaugType==0||abs(EndDaugType) == 2212||abs(EndDaugType) == 2112) ElecOutNumOfNucFr++;
  } // cycle itrDaug

  if(ElecOutNumOfNucFr!=0||NumOfHadr!=0)     PartOutCome = NuclInteraction;
  if(ElecOutNumOfElec==1&&NumOfElecDaug==1)  PartOutCome = ElectrMagInter;

  return  PartOutCome;
}
//---------------------------------------------------------------------------------
ParticleOutCome MCTruthClassifier::defOutComeOfMuon(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------

  ParticleOutCome PartOutCome=UnknownOutCome;

  const xAOD::TruthVertex*   EndVert = findEndVert(thePart);

  if(EndVert==0) return  NonInteract;
  int   NumOfMuDaug(0),MuOutNumOfNucFr(0),NumOfHadr(0);
  int   NumOfEleNeutr(0),NumOfMuonNeutr(0),NumOfElec(0);
  long  EndDaugType(0);

  NumOfMuDaug=EndVert->nOutgoingParticles();
  for(unsigned int ipOut=0;ipOut<EndVert->nOutgoingParticles();ipOut++){
    if(!EndVert->outgoingParticle(ipOut)) continue;
    EndDaugType    =  EndVert->outgoingParticle(ipOut)->pdgId();
    if(abs(EndDaugType)==11)  NumOfElec++;
    if(abs(EndDaugType)==12)  NumOfEleNeutr++;
    if(abs(EndDaugType)==14)  NumOfMuonNeutr++;
    if(isHadron(EndVert->outgoingParticle(ipOut))) NumOfHadr++;
    if(EndDaugType >  1000000000||EndDaugType==0||abs(EndDaugType) == 2212||abs(EndDaugType) == 2112) MuOutNumOfNucFr++;
  } // cycle itrDaug

  if(MuOutNumOfNucFr!=0||NumOfHadr!=0)                                   PartOutCome = NuclInteraction;
  if(NumOfMuDaug==3&&NumOfElec==1&&NumOfEleNeutr==1&&NumOfMuonNeutr==1)  PartOutCome = DecaytoElectron;

  return  PartOutCome;
}
//---------------------------------------------------------------------------------
ParticleOutCome MCTruthClassifier::defOutComeOfTau(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing defOutComeOfTau" );

  ParticleOutCome PartOutCome=UnknownOutCome;

  const xAOD::TruthVertex*   EndVert = findEndVert(thePart);

  if(EndVert==0) return  NonInteract;
  int   NumOfTauDaug(0),NumOfNucFr(0);
  int   NumOfElec(0),NumOfElecNeut(0),NumOfTauNeut(0);
  int   NumOfMuon(0),NumOfMuonNeut(0),NumOfPhot(0),NumOfPi(0),NumOfKaon(0);

  NumOfTauDaug=EndVert->nOutgoingParticles();
  m_tauFinalStatePart = findFinalStatePart(EndVert);

  for(int i=0;i<(int) m_tauFinalStatePart.size();i++){
    long pdg=m_tauFinalStatePart[i]->pdgId();
    if(abs(pdg)==11)       NumOfElec++;
    else if(abs(pdg)==13)  NumOfMuon++;
    else if(abs(pdg)==12)  NumOfElecNeut++;
    else if(abs(pdg)==14)  NumOfMuonNeut++;
    else if(abs(pdg)==16)  NumOfTauNeut++;
    else if(pdg==22)       NumOfPhot++;
    else if(abs(pdg)==211) NumOfPi++;
    else if(abs(pdg)==321) NumOfKaon++;
    else if(pdg > 1000000000||pdg==0) NumOfNucFr++;
  }


  if(NumOfNucFr!=0)                                         PartOutCome = NuclInteraction;
  if((NumOfTauDaug==3&&NumOfElec==1&&NumOfElecNeut==1)||
     (NumOfTauDaug==(3+NumOfPhot)&&NumOfElecNeut==1))       PartOutCome = DecaytoElectron;
  if((NumOfTauDaug== 3&&NumOfMuon==1&&NumOfMuonNeut==1)||
     (NumOfTauDaug==(3+NumOfPhot)&&NumOfMuonNeut==1))       PartOutCome = DecaytoMuon;

  if(NumOfPi==1||NumOfKaon==1)    PartOutCome = OneProng;
  if(NumOfPi+NumOfKaon==3)        PartOutCome = ThreeProng;
  if(NumOfPi+NumOfKaon==5)        PartOutCome = FiveProng;


  ATH_MSG_DEBUG( "defOutComeOfTau succeeded" );

  return  PartOutCome;

}
//---------------------------------------------------------------------------------
std::vector<const xAOD::TruthParticle*>
MCTruthClassifier::findFinalStatePart(const xAOD::TruthVertex* EndVert){
  //---------------------------------------------------------------------------------

  std::vector<const xAOD::TruthParticle*> finalStatePart;

  if(EndVert==0) return finalStatePart ;

  for(unsigned int ipOut=0;ipOut<EndVert->nOutgoingParticles();ipOut++){

    const xAOD::TruthParticle* thePart=EndVert->outgoingParticle(ipOut);
    if(!thePart) continue;

    finalStatePart.push_back(thePart);
    if(thePart->status()!=1) {

      const xAOD::TruthVertex* pVert = findEndVert(thePart);
      if(pVert==EndVert) break; // to prevent Sherpa  loop
      std::vector<const xAOD::TruthParticle*> vecPart;
      if(pVert!=0) {
        vecPart  = findFinalStatePart(pVert);
         if(vecPart.size()!=0)  for(int i=0;i<(int)vecPart.size();i++) finalStatePart.push_back(vecPart[i]);
      }
    }

  } // cycle itrDaug

  return finalStatePart;
}
//---------------------------------------------------------------------------------
ParticleOutCome MCTruthClassifier::defOutComeOfPhoton(const xAOD::TruthParticle* thePart){
  //---------------------------------------------------------------------------------

  ParticleOutCome PartOutCome=UnknownOutCome;
  const xAOD::TruthVertex*   EndVert = findEndVert(thePart);
  if(EndVert==0) return  UnConverted;

  int  PhtOutNumOfNucFr(0),PhtOutNumOfEl(0),PhtOutNumOfPos(0);
  int  NumOfPhtDaug(0),PhtOutNumOfHadr(0);
  // int  PhtOutNumOfPi(0);
  long EndDaugType(0);

  NumOfPhtDaug=EndVert->nOutgoingParticles();
  for(unsigned int ipOut=0;ipOut<EndVert->nOutgoingParticles();ipOut++){
    if(!EndVert->outgoingParticle(ipOut)) continue;
    EndDaugType    =  EndVert->outgoingParticle(ipOut)->pdgId();
    if(EndDaugType >  1000000000||EndDaugType==0||abs(EndDaugType) == 2212||abs(EndDaugType) == 2112)  PhtOutNumOfNucFr++;
    if(EndDaugType == 11) PhtOutNumOfEl++;
    if(EndDaugType ==-11) PhtOutNumOfPos++;
    // if(abs(EndDaugType)==211||EndDaugType==111) PhtOutNumOfPi++;
    if(isHadron( EndVert->outgoingParticle(ipOut))) PhtOutNumOfHadr++;
  } // cycle itrDaug

  if(PhtOutNumOfEl==1&&PhtOutNumOfPos==1&&NumOfPhtDaug==2)      PartOutCome = Converted;
  if((NumOfPhtDaug>1&&PhtOutNumOfNucFr!=0)||PhtOutNumOfHadr>0 ) PartOutCome = NuclInteraction;

  return  PartOutCome;

}
//---------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::checkOrigOfBkgElec(const xAOD::TruthParticle* theEle){
  //---------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "executing CheckOrigOfBkgElec  " << theEle );

  m_BkgElecMother = 0;

  std::pair<ParticleType,ParticleOrigin> part;
  part.first  = Unknown;
  part.second = NonDefined;

  if(theEle==0) return part;

  // Short-circuit the do-while in case we have G4 information that we can directly use
  if (theEle->barcode()>m_barcodeG4Shift && // Made in G4
      theEle->hasProdVtx() && theEle->prodVtx()->barcode()<-200000 && // For sure made in G4
      theEle->prodVtx()->id()>1000){ // And in a version where we stored the info we need
    // Process IDs from http://www-geant4.kek.jp/lxr/source//processes/electromagnetic/utils/include/G4EmProcessSubType.hh
    if (theEle->prodVtx()->id()==1014){ // from gamma conversion
      // We want the classification of the photon in this case
      part=particleTruthClassifier( theEle->prodVtx()->incomingParticle(0) );
      m_BkgElecMother = theEle->prodVtx()->incomingParticle(0);
      return part;
    }
    // Other uses would go here
  }

  const xAOD::TruthParticleContainer  * xTruthParticleContainer;
  StatusCode sc = evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName);
  if (sc.isFailure()||!xTruthParticleContainer){
    ATH_MSG_WARNING( "No  xAODTruthParticleContainer "<<m_xaodTruthParticleContainerName<<" found" );
    return part;
  }
  ATH_MSG_DEBUG( "xAODTruthParticleContainer  " << m_xaodTruthParticleContainerName<<" successfully retrieved " );

  part=particleTruthClassifier(theEle);

  if(part.first!=BkgElectron||part.second!=PhotonConv) return part;

  const xAOD::TruthParticle* thePart(0);

  if((abs(m_PhotonMotherPDG)==11||abs(m_PhotonMotherPDG)==13|| abs(m_PhotonMotherPDG)==15||isHadron(m_PhotonMother))&&m_PhotonMotherStatus<3){
    do {
      const xAOD::TruthParticle *theMotherPart = barcode_to_particle(xTruthParticleContainer,m_PhotonMotherBarcode%m_barcodeShift);
      if(theMotherPart==0 || theMotherPart==thePart) break;
      thePart = theMotherPart;

      part.first=Unknown; part.second=NonDefined;
      part=particleTruthClassifier(thePart);
    } while (part.first == BkgElectron&& part.second==PhotonConv&&
             (abs(m_PhotonMotherPDG)==11||abs(m_PhotonMotherPDG)==13||
              abs(m_PhotonMotherPDG)==15||isHadron(m_PhotonMother)));


    if(part.first == BkgElectron&& part.second==PhotonConv) {
      // in case of photon from gen particle  classify photon
      //part=particleTruthClassifier(m_Mother);
      thePart = barcode_to_particle(xTruthParticleContainer,m_MotherBarcode%m_barcodeShift);
      if(thePart!=0) part=particleTruthClassifier(thePart);

    } else if(part.first == GenParticle&&isHadron(thePart)){
      // to fix Alpgen hadrons with status code !=1 (>100)
      part.first=Hadron; part.second=NonDefined;
    }

  } else {
    // in case of photon from gen particle  classify photon
    thePart = barcode_to_particle(xTruthParticleContainer,m_MotherBarcode%m_barcodeShift);
    if(thePart!=0) part=particleTruthClassifier(thePart);
  }

  m_BkgElecMother = thePart;
  return part;

}
//------------------------------------------------------------------------
double MCTruthClassifier::partCharge(const xAOD::TruthParticle* thePart){
  //------------------------------------------------------------------------
  if ( thePart == 0 ) return 0. ;
  return thePart->charge();
}

//------------------------------------------------------------------------
const xAOD::TruthParticle* MCTruthClassifier::
barcode_to_particle(const xAOD::TruthParticleContainer* TruthTES,
                    int bc){
  //------------------------------------------------------------------------
  // temporary solution?
  const xAOD::TruthParticle* ptrPart=NULL;

  for(const auto truthParticle : *TruthTES){
    if(truthParticle->barcode()==bc) {ptrPart=truthParticle; break;}
  }
  return ptrPart;
}
