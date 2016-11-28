/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowVertexInformationSetter.h"

#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFO.h"

#include "xAODCaloEvent/CaloCluster.h"

eflowVertexInformationSetter::eflowVertexInformationSetter(const std::string& name,ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator), m_PFOName("JetETMissNeutralParticleFlowObjects") {
declareProperty("PFOInputName",m_PFOName,"Name of container of PFO to use");
 m_storeGate = 0;
}

eflowVertexInformationSetter::~eflowVertexInformationSetter() {}

StatusCode eflowVertexInformationSetter::initialize(){

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

StatusCode eflowVertexInformationSetter::execute(){

  const xAOD::PFOContainer* PFOContainer;

  StatusCode sc = m_storeGate->retrieve(PFOContainer,m_PFOName);
  if(sc.isFailure() || ! PFOContainer)
    {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)
				  <<" no PFO container"
				  <<endmsg;
      return StatusCode::SUCCESS;
    }

  xAOD::PFOContainer::const_iterator firstPFO = PFOContainer->begin();
  xAOD::PFOContainer::const_iterator lastPFO = PFOContainer->end();

  for (; firstPFO != lastPFO; ++firstPFO){

    const xAOD::PFO* thisPFO = *firstPFO;
    
    if (0 == thisPFO->charge()){

      const xAOD::CaloCluster* thisClus = thisPFO->cluster(0);

      if (thisClus){

	double center_mag = 0.0;
	bool isRetrieved = thisClus->retrieveMoment(xAOD::CaloCluster::CENTER_MAG, center_mag );
	if (true == isRetrieved) {
	  xAOD::PFO* nonConstPFO = const_cast<xAOD::PFO*>(thisPFO);
	  nonConstPFO->setCenterMag(center_mag);
	}
	else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not retrieved ENG_FRAC_CORE from the CaloCluster " << endmsg;
      }
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Got neutral PFO with invalid cluster pointer " << endmsg;
    }//neutral
  }//PFO loop


  return StatusCode::SUCCESS;
}

StatusCode eflowVertexInformationSetter::finalize(){
  return StatusCode::SUCCESS;
}
