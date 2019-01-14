/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


 /* *******************************************************
 * NAME : TrigL1FCALTTSumFex.cxx
 * PACKAGE : Trigger/TrigAlgorithms/TrigCaloRec
 * 
 * AUTHOR : Denis Oliveira Damazio
 * CREATED : 2016, September
 *
 **********************************************************/


#include "TrigCaloRec/TrigL1FCALTTSumFex.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"

TrigL1FCALTTSumFex::TrigL1FCALTTSumFex ( const std::string& name,
        ISvcLocator* pSvcLocator) : HLT::FexAlgo(name,pSvcLocator) {
    declareProperty("HIEventShapeContainerKey", m_HIEventShapeContainerKey ="HIFCAL", "Nmae of the output HI FCAL container");

}

TrigL1FCALTTSumFex::~TrigL1FCALTTSumFex() {}

HLT::ErrorCode TrigL1FCALTTSumFex::hltInitialize()
{
        return HLT::OK;
}

HLT::ErrorCode TrigL1FCALTTSumFex::hltFinalize()
{
        return HLT::OK;
}

HLT::ErrorCode TrigL1FCALTTSumFex::hltExecute(
        const HLT::TriggerElement* /*inputTE*/,
	HLT::TriggerElement* outputTE) {


    // prepare event shape for recording
    xAOD::HIEventShapeContainer* shape = new xAOD::HIEventShapeContainer();
    xAOD::HIEventShapeAuxContainer shapeAux = xAOD::HIEventShapeAuxContainer();
    shape->setStore(&shapeAux);

    const xAOD::TriggerTowerContainer* container(0);
    if ( getFeature(outputTE,container) != HLT::OK ) {
         msg() << MSG::WARNING << " Failed to get TT collection." << endmsg;
         return HLT::ERROR;
    }

    int count_all=0;
    int count_fcal=0;
    float eta_min_pos=999.0;
    float eta_max_pos=-999.0;
    float eta_min_neg=999.0;
    float eta_max_neg=-999.0;
    float etPos=0;
    float etNeg=0;
    int nPos=0;
    int nNeg=0;
    for(unsigned int i=0;i<container->size();i++){
       const xAOD::TriggerTower* tt = container->at(i);
       double etaTower = tt->eta();
       count_all++;
       if ( std::fabs(etaTower) < 3.1 ) continue;
       count_fcal++;
       if ( etaTower > 0 ) {
	if ( etaTower < eta_min_pos ) eta_min_pos = etaTower;
	if ( etaTower > eta_max_pos ) eta_max_pos = etaTower;
	etPos+=tt->pt();
        nPos++;
       } else { // negative eta	
	if ( etaTower < eta_min_neg ) eta_min_neg = etaTower;
	if ( etaTower > eta_max_neg ) eta_max_neg = etaTower;
	etNeg+=tt->pt();
        nNeg++;
       }
    }
    etPos*=1e3;
    etNeg*=1e3;

    

   xAOD::HIEventShape* Pos = new xAOD::HIEventShape();
   shape->push_back ( Pos );
   Pos->setEt( etPos );
   Pos->setNCells( nPos );
   Pos->setEtaMin( eta_min_pos );
   Pos->setEtaMax( eta_max_pos );
   xAOD::HIEventShape* Neg = new xAOD::HIEventShape();
   shape->push_back ( Neg );
   Neg->setEt( etNeg );
   Neg->setNCells( nNeg );
   Neg->setEtaMin( eta_min_neg );
   Neg->setEtaMax( eta_max_neg );

   if ( attachFeature(outputTE, shape, m_HIEventShapeContainerKey) != HLT::OK ) {
	return HLT::ERROR;
   }

    return HLT::OK;


}
