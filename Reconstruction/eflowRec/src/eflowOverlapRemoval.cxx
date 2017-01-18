/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowOverlapRemoval.h"

#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFODefs.h"

#include "CaloUtils/CaloClusterOverlapHelper.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaContainer.h"

#include "StoreGate/StoreGateSvc.h"

#include <string>
#include <cstdlib>

#include <algorithm>
#include <numeric>
#include <iterator>
#include "FourMomUtils/P4Helpers.h"

eflowOverlapRemoval::eflowOverlapRemoval(const std::string& name,ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_PFOName("JetETMissNeutralParticleFlowObjects"),
  m_storeGate(nullptr),  
  m_egammaContainerName("Photons"),
  m_eflowElectronContainerName("eflowRec_selectedElectrons_EM"),
  m_radialDist(0.2),
  m_longSig(1.0),           // 3.0
  m_radialSig(1.0),         // 3.0 
  m_eFracThreshold(0.1)    // 0.2
{
  declareProperty("PFOInputName", m_PFOName);
  declareProperty("ElectronInputName",m_eflowElectronContainerName);
}

eflowOverlapRemoval::~eflowOverlapRemoval() {}

StatusCode eflowOverlapRemoval::initialize(){

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())
    {
      msg(MSG::WARNING )
	  << "Unable to retrieve pointer to StoreGateSvc"
	  << endmsg;
      return StatusCode::SUCCESS;
    }




  return StatusCode::SUCCESS;
}

StatusCode eflowOverlapRemoval::execute(){

  const xAOD::PFOContainer* pfoContainer;

  StatusCode sc = m_storeGate->retrieve(pfoContainer,m_PFOName);
  if(sc.isFailure() || !pfoContainer)
  {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)
	<<" no neutral PFO container"
	<<endmsg; 
    return StatusCode::SUCCESS;
  }

  const xAOD::EgammaContainer* selectedElectrons; 
  sc = evtStore()->retrieve(selectedElectrons, m_eflowElectronContainerName);
  if (sc.isFailure() || !selectedElectrons){
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)
      << " No eflowRec selectedElectrons container found in TDS"
				<< endmsg;
  }

  const xAOD::EgammaContainer* egammaColl = 0;
  if ( (m_storeGate->retrieve(egammaColl,m_egammaContainerName)).isFailure() )
    {
      msg(MSG::WARNING )<< "cannot allocate egamma Container with key "
			<< m_egammaContainerName
			<< endmsg;
      
    } 

  xAOD::PFOContainer::const_iterator firstPFO = pfoContainer->begin();
  xAOD::PFOContainer::const_iterator lastPFO = pfoContainer->end();

  const std::string leptonString = "LeptonType";

  for (; firstPFO != lastPFO; ++firstPFO){

    const xAOD::PFO* thisPFO = *firstPFO;

    const xAOD::CaloCluster* thisClus = thisPFO->cluster(0);

    if (thisClus){
      bool IsEleClu = false;
      bool IsGammaClu = false;
      if (selectedElectrons){
	IsEleClu = this->IsEleOverlap(thisClus,selectedElectrons);
	if (true == IsEleClu) {
	  //const_cast because we want to set the particle type (and I'd rather not make the function non-const)
	  xAOD::PFODetails::PFOLeptonType leptonType = xAOD::PFODetails::PFO_electron;
	  const_cast<xAOD::PFO*>(thisPFO)->setAttribute<xAOD::PFODetails::PFOLeptonType>(leptonString, leptonType) ;
	}
      }
      if (egammaColl){
	IsGammaClu = this->IsGammaOverlap(thisClus,egammaColl);
	//const_cast because we want to set the particle type (and I'd rather not make the function non-const)
	if (true == IsGammaClu){
	  xAOD::PFODetails::PFOLeptonType leptonType = xAOD::PFODetails::PFO_photon;
	  const_cast<xAOD::PFO*>(thisPFO)->setAttribute<xAOD::PFODetails::PFOLeptonType>(leptonString, leptonType) ;
	}
      }
      if (false == IsGammaClu && false == IsEleClu) const_cast<xAOD::PFO*>(thisPFO)->setAttribute<xAOD::PFODetails::PFOLeptonType>(leptonString, xAOD::PFODetails::PFO_nonLeptonic) ;

    }//if valid cluster pointer
    else {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get a pointer to a xAOD::CaloCluster" << endmsg;
    }
  
}//loop on eflowObjects
  
  return StatusCode::SUCCESS;
}

StatusCode eflowOverlapRemoval::finalize(){ 

  return StatusCode::SUCCESS;

}


bool eflowOverlapRemoval::IsEleOverlap(const xAOD::CaloCluster* fClus, const xAOD::EgammaContainer* selElectrons){
  bool IsEleClu = false;
  xAOD::EgammaContainer::const_iterator firstElectron = selElectrons->begin();
  xAOD::EgammaContainer::const_iterator lastElectron =  selElectrons->end();
  for (; firstElectron != lastElectron; ++firstElectron){
    const xAOD::Egamma* theElectron = *firstElectron;
    const xAOD::CaloCluster* pCluster = theElectron->caloCluster();
    //cellList input celle elettrone
   
    // cList input
    typedef std::vector<const CaloCell*> cell_list_t;
    cell_list_t cList; //output
    if ( CaloClusterOverlapHelper::isNear(pCluster,fClus,m_radialDist) ){
     
      if ( CaloClusterOverlapHelper::getOverlap(pCluster,fClus,m_longSig,m_radialSig,cList) ) {
	
	double refEnergy = pCluster->e();
	if (0.0 != refEnergy){
	  double ef(std::accumulate(cList.begin(),cList.end(),0.,Energy())/refEnergy);
	  if (ef > m_eFracThreshold) IsEleClu = true;
	}//if have non-zero reference energy
      }            
    }
  }
  return IsEleClu;
}


bool eflowOverlapRemoval::IsGammaOverlap(const xAOD::CaloCluster* fClus, const xAOD::EgammaContainer* photonCollection){
  bool IsGammaClu = false;
  xAOD::EgammaContainer::const_iterator fEgamma(photonCollection->begin());
  xAOD::EgammaContainer::const_iterator lEgamma(photonCollection->end());

  for ( ; fEgamma != lEgamma; fEgamma++)
    {	
      const xAOD::Egamma* pEgamma = (*fEgamma);

      if (pEgamma->pt()<10000) continue;
      // if author Photon or RConv (latter now called ambiguous in R19)
      if (! ( pEgamma->author(xAOD::EgammaParameters::AuthorPhoton ) ) || pEgamma->author(xAOD::EgammaParameters::AuthorAmbiguous) ) continue; 
      
      bool passTight = false;
      bool gotID = pEgamma->passSelection(passTight,"Tight");
      if (!gotID) {
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get Photon ID " << endmsg;
        continue;
      }
      if (!passTight) continue;

      // go on only if photon is NOT in ele collection
      const xAOD::CaloCluster* pCluster = pEgamma->caloCluster();
      //cellList input celle elettrone
      
      // cList input
      typedef std::vector<const CaloCell*> cell_list_t;
      cell_list_t cList; //output
      
      if ( CaloClusterOverlapHelper::isNear(pCluster,fClus,m_radialDist) ){
	
	if ( CaloClusterOverlapHelper::getOverlap(pCluster,fClus,m_longSig,m_radialSig,cList) ) {
	  
	  double refEnergy = pCluster->e();
	  if (0.0 != refEnergy){
	    double ef(std::accumulate(cList.begin(),cList.end(),0.,Energy())/refEnergy);
	    if (ef > m_eFracThreshold) IsGammaClu = true;
	  }//if have non-zero reference energy
	}            
      }
    }
  return IsGammaClu;
}

