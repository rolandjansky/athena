/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//File Including the Athena only methods of the MCTruthClassifier Class

#ifndef XAOD_ANALYSIS

//
#include "MCTruthClassifier/MCTruthClassifier.h"
//
// xAOD EDM includes
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTruth/TruthParticle.h"
//
//Athena only includes 
#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "HepMC/GenParticle.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "AthenaKernel/Units.h"
//
//std includes
#include <cmath>

using Athena::Units::GeV;
using namespace MCTruthPartClassifier;
using std::abs;

//Old EDM interface, just for Athena version
//---------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const HepMC::GenParticle  *thePart){
  //---------------------------------------------------------------------------------------
  ParticleType   partType = Unknown;     
  ParticleOrigin partOrig = NonDefined;  

  if (!thePart) return std::make_pair(partType,partOrig); 
	
  // Retrieve the links between HepMC and xAOD::TruthParticle
  const xAODTruthParticleLinkVector* truthParticleLinkVec = 0;
  if(evtStore()->retrieve(truthParticleLinkVec, m_truthLinkVecName).isFailure()){
    ATH_MSG_WARNING("Cannot retrieve TruthParticleLinkVector");
    return std::make_pair(partType,partOrig); 
  }
 
  for( const auto& entry : *truthParticleLinkVec ) {
    if(entry->first.isValid() && entry->second.isValid()
       && entry->first.cptr()->barcode() == thePart->barcode()) {
      const xAOD::TruthParticle* truthParticle = *entry->second;
      if ( !compareTruthParticles(thePart, truthParticle) ) { 
	//if the barcode/pdg id / status of the pair does not match
	//return default
	return std::make_pair(partType,partOrig); 
      }
      return particleTruthClassifier(truthParticle);
    }
  }

  return std::make_pair(partType,partOrig); 
}
//------------------------------------------------------------------------
bool MCTruthClassifier::compareTruthParticles(const HepMC::GenParticle *genPart,
					      const xAOD::TruthParticle *truthPart){
  //------------------------------------------------------------------------
  if (!genPart || !truthPart) return false;

  if (genPart->barcode() != truthPart->barcode() ||
      genPart->pdg_id() != truthPart->pdgId() ||
      genPart->status() != truthPart->status()) {
    ATH_MSG_DEBUG("HepMC::GenParticle and xAOD::TruthParticle do not match");
    return false;
  }

  return true;
}
//End of old EDM just for Athena


//Methods using directly the extrapolator usable only from Athena
//-----------------------------------------------------------------------------------------
const xAOD::TruthParticle*
MCTruthClassifier::egammaClusMatch(const xAOD::CaloCluster* clus, bool isFwrdEle){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing egammaClusMatch " );

  const xAOD::TruthParticle* theMatchPart=0;

  //retrieve collection and get a pointer
 
  const xAOD::TruthParticleContainer  * xTruthParticleContainer;
  StatusCode sc = evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName);
  if (sc.isFailure()||!xTruthParticleContainer){
    ATH_MSG_WARNING( "No  xAODTruthParticleContainer "<<m_xaodTruthParticleContainerName<<" found" ); 
    return theMatchPart;
  }
  ATH_MSG_DEBUG( "xAODTruthParticleContainer  " << m_xaodTruthParticleContainerName<<" successfully retrieved " );

  const xAOD::TruthParticle* theEgamma(0);
  const xAOD::TruthParticle* theLeadingPartInCone(0);
  const xAOD::TruthParticle* theBestPartOutCone(0);
  const xAOD::TruthParticle* theBestPartdR(0);
  double LeadingPhtPT(0),LeadingPartPT(0);
  double LeadingPhtdR(999.),LeadingPartdR(999.),BestPartdR(999.);

  double etaClus = clus->etaBE(2);
  double phiClus = clus->phiBE(2);
  if (etaClus < -900) etaClus = clus->eta();
  if (phiClus < -900) phiClus = clus->phi();

  std::vector<const xAOD::TruthParticle*> tps;
  if( !m_truthInConeTool->particlesInCone(etaClus,phiClus,0.5,tps) ) {
    ATH_MSG_WARNING( "Truth Particle in Cone failed" ); 
    return theMatchPart;
  }

  for(const auto thePart : tps){
    // loop over the stable particle  
    if(thePart->status()!=1) continue;
    // excluding G4 particle
    if(!isFwrdEle || (isFwrdEle && m_FwdElectronUseG4Sel)){
      if(thePart->barcode()>=m_barcodeG4Shift) continue;
    }
    long iParticlePDG = thePart->pdgId(); 
    // excluding neutrino
    if(abs(iParticlePDG)==12||abs(iParticlePDG)==14||abs(iParticlePDG)==16) continue;

    double pt = thePart->pt()/GeV;
    double q  = partCharge(thePart);
    // exclude charged particles with pT<1 GeV
    if(q!=0&&pt<m_pTChargePartCut)  continue;
    if(q==0&&pt<m_pTNeutralPartCut) continue;

    // eleptical cone  for extrapolations m_partExtrConePhi X m_partExtrConeEta 
    if(!isFwrdEle&&m_ROICone&& std::pow((detPhi(phiClus,thePart->phi())/m_partExtrConePhi),2)+
       std::pow((detEta(etaClus,thePart->eta())/m_partExtrConeEta),2)>1.0 ) continue;

    //Also check if the clus and true have different sign , i they need both to be <0 or >0 
    if(isFwrdEle && //It is forward and  
       (((etaClus<0) - (thePart->eta()<0) !=0) //The truth eta has different sign wrt to the fwd electron
	|| (std::fabs(thePart->eta())<m_FwdElectronTruthExtrEtaCut)  //or the truth is less than 2.4 (default cut)
	|| (std::fabs(thePart->eta()-etaClus)> m_FwdElectronTruthExtrEtaWindowCut) //or if the delta Eta between el and truth is  > 0.15
	) //then do no extrapolate this truth Particle for this fwd electron
       ) continue;

    double dR(-999.);
    bool isNCone=false;
    bool isExt = genPartToCalo(clus, thePart,isFwrdEle, dR, isNCone);
    if (!isExt) continue;

    m_egPartPtr.push_back(thePart);
    m_egPartdR.push_back(dR);

    theMatchPart = barcode_to_particle(xTruthParticleContainer,thePart->barcode()%m_barcodeShift);
    m_egPartClas.push_back(particleTruthClassifier(theMatchPart));
    if(!isFwrdEle){
      // the leading photon  inside narrow eleptical cone  m_partExtrConePhi X m_partExtrConeEta 
      if((iParticlePDG==22||abs(iParticlePDG)==11) 
	 &&isNCone&&pt>LeadingPhtPT)   { 
	theEgamma = thePart; 
	LeadingPhtPT=pt; 
	LeadingPhtdR=dR;}
      // leading particle (excluding photon) inside narrow eleptic cone m_partExtrConePhi X m_partExtrConeEta 
      if((iParticlePDG!=22&&abs(iParticlePDG)!=11)&&isNCone&&pt>LeadingPartPT)  { 
	theLeadingPartInCone = thePart; 
	LeadingPartPT = pt; 
	LeadingPartdR = dR;};
    // the best dR matched particle outside  narrow eleptic cone m_partExtrConePhi X m_partExtrConeEta 
      if(!isNCone&&dR<BestPartdR){ 
	theBestPartOutCone = thePart; 
	BestPartdR = dR; 
      }
    }
    else{
      if(dR<BestPartdR) { 
	theBestPartdR = thePart; 
	BestPartdR = dR; 
      }
    }
  }// end cycle for Gen particle 
 
  if(theEgamma!=0) {
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theEgamma->barcode()%m_barcodeShift); m_deltaRMatch=LeadingPhtdR;
  } else if(theLeadingPartInCone!=0) {
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theLeadingPartInCone->barcode()%m_barcodeShift);  m_deltaRMatch=LeadingPartdR;  
  } else if(theBestPartOutCone!=0) {
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theBestPartOutCone->barcode()%m_barcodeShift); m_deltaRMatch=BestPartdR;
  } else if(isFwrdEle&&theBestPartdR!=0) {
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theBestPartdR->barcode()%m_barcodeShift); m_deltaRMatch=BestPartdR;
  } else theMatchPart = 0;
  
  if(isFwrdEle||theMatchPart!=0||!m_inclG4part)   {return theMatchPart;}


  // additional loop over G4 particles for unmatched egamma photons
  // requested by Photon's group people 
  for(const auto thePart : tps){
    // loop over the stable particle  
    if(thePart->status()!=1) continue;
    // only G4 particle including None Primary with barcode > 10^6 
    if(thePart->barcode()<m_barcodeG4Shift) continue;
    long iParticlePDG = thePart->pdgId(); 
    // exclude neutrino
    if(abs(iParticlePDG)==12||abs(iParticlePDG)==14||abs(iParticlePDG)==16) continue;
    // exclude particles interacting into the detector volume
    if(thePart->decayVtx()!=0) continue;

    if( std::pow((detPhi(phiClus,thePart->phi())/m_partExtrConePhi),2)+
	std::pow((detEta(etaClus,thePart->eta())/m_partExtrConeEta),2)>1.0 ) continue;

    double pt = thePart->pt()/GeV;
    double q  = partCharge(thePart);
    // exclude charged particles with pT<1 GeV
    if(q!=0&&pt<m_pTChargePartCut )  continue;
    if(q==0&&pt<m_pTNeutralPartCut)  continue;

    double dR(-999.);
    bool isNCone=false;
    bool isExt = genPartToCalo(clus, thePart, false , dR, isNCone);
    if(!isExt) continue;

    m_egPartPtr.push_back(thePart);
    m_egPartdR.push_back(dR);

    theMatchPart = barcode_to_particle(xTruthParticleContainer,thePart->barcode()%m_barcodeShift);
    m_egPartClas.push_back(particleTruthClassifier(theMatchPart));

    // the leading photon  inside narrow eleptical cone  m_partExtrConePhi X m_partExtrConeEta 
      if((iParticlePDG==22||abs(iParticlePDG)==11) 
	 &&isNCone&&pt>LeadingPhtPT)   { 
	theEgamma = thePart; 
	LeadingPhtPT=pt; 
	LeadingPhtdR=dR;}
      // leading particle (excluding photon) inside narrow eleptic cone m_partExtrConePhi X m_partExtrConeEta 
      if((iParticlePDG!=22&&abs(iParticlePDG)!=11)&&isNCone&&pt>LeadingPartPT)  { 
	theLeadingPartInCone = thePart; 
	LeadingPartPT = pt; 
	LeadingPartdR = dR;};
    // the best dR matched particle outside  narrow eleptic cone m_partExtrConePhi X m_partExtrConeEta 
      if(!isNCone&&dR<BestPartdR){ 
	theBestPartOutCone = thePart; 
	BestPartdR = dR; 
      }
  }// end cycle for G4 particle

  if( theEgamma!=0){
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theEgamma->barcode()%m_barcodeShift); m_deltaRMatch=LeadingPhtdR;
  } else if( theLeadingPartInCone!=0) {
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theLeadingPartInCone->barcode()%m_barcodeShift); m_deltaRMatch=LeadingPartdR;
  } else if( theBestPartOutCone!=0) { 
    theMatchPart = barcode_to_particle(xTruthParticleContainer,theBestPartOutCone->barcode()%m_barcodeShift); m_deltaRMatch=BestPartdR;
  } else theMatchPart = 0;
  ATH_MSG_DEBUG( "succeeded  egammaClusMatch ");
  return theMatchPart;
}

//--------------------------------------------------------------
bool MCTruthClassifier::genPartToCalo(const xAOD::CaloCluster* clus, 
				      const xAOD::TruthParticle* thePart,
				      bool isFwrdEle, 
				      double& dRmatch,
				      bool  & isNarrowCone)  {
  dRmatch      = -999.;
  isNarrowCone = false;

  if ( thePart == 0 ) return false ;

  const Trk::CaloExtension* caloExtension = 0;
  if( !m_caloExtensionTool->caloExtension(*thePart,caloExtension,m_useCaching) || caloExtension->caloLayerIntersections().empty() ){
    ATH_MSG_WARNING("extrapolation of Truth Particle with eta  " << thePart->eta() 
		    <<" and Pt " << thePart->pt() << " to calo failed");
    return false;
  }

  Trk::TrackParametersIdHelper parsIdHelper;
  double etaCalo= -99;
  double phiCalo= -99; 

   // define calo sample
  CaloSampling::CaloSample sample= CaloSampling::EMB2;
  if ( (clus->inBarrel()  && !clus->inEndcap()) ||
       (clus->inBarrel()  &&  clus->inEndcap() &&
        clus->eSample(CaloSampling::EMB2) >= clus->eSample(CaloSampling::EME2) ) ) {
    // Barrel
    sample = CaloSampling::EMB2;
    
  } else if( ( !clus->inBarrel()  && clus->inEndcap() && !isFwrdEle) ||
             (  clus->inBarrel()  && clus->inEndcap() && 
                clus->eSample(CaloSampling::EME2) > clus->eSample(CaloSampling::EMB2) ) ) {
    // End-cap
    sample = CaloSampling::EME2; 
  } else if( isFwrdEle && clus->inEndcap()) {            
    // FCAL
    sample= CaloSampling::FCAL2;
    
  } else return false ;

  // loop over calo layers
  for( auto cur = caloExtension->caloLayerIntersections().begin(); cur != caloExtension->caloLayerIntersections().end() ; ++cur ){
      
    // only use entry layer
    if( !parsIdHelper.isEntryToVolume((*cur)->cIdentifier()) ) continue;
      
    CaloSampling::CaloSample sampleEx = parsIdHelper.caloSample((*cur)->cIdentifier());
    if( sampleEx != CaloSampling::EMB2 && sampleEx != CaloSampling::EME2 && sampleEx != CaloSampling::FCAL2 ) continue;
    
    if( sampleEx == sample || etaCalo == -99 ){
      etaCalo = (*cur)->position().eta();
      phiCalo = (*cur)->position().phi();
      if( sampleEx == sample ) break;
    }
  }

  double phiClus = clus->phiBE(2);
  double etaClus = clus->etaBE(2);
  if (etaClus < -900) etaClus = clus->eta();
  if (phiClus < -900) phiClus = clus->phi();

  //--FixMe
  if(isFwrdEle||(etaClus==0.&&phiClus==0.)) {
    phiClus = clus->phi();
    etaClus = clus->eta();  
  }

  double dPhi    = detPhi(phiCalo,phiClus);
  double dEta    = detEta(etaCalo,etaClus);
  dRmatch        = rCone(dPhi,dEta);
  
  if((!isFwrdEle&&dRmatch>m_phtdRtoTrCut)||
     (isFwrdEle&&dRmatch>m_fwrdEledRtoTrCut))    return false ;

  if(!isFwrdEle&&std::pow(dPhi/m_phtClasConePhi,2)+std::pow(dEta/m_phtClasConeEta,2)<=1.0) isNarrowCone=true;

  return  true;
}
//End of methods using directly the extrapolator usable only from Athena
#endif
