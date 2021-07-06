/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibPedMonAlg.h"


/*---------------------------------------------------------*/
LArCalibPedMonAlg::LArCalibPedMonAlg(const std::string& name,ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{}

/*---------------------------------------------------------*/
LArCalibPedMonAlg::~LArCalibPedMonAlg()
{}

/*---------------------------------------------------------*/
StatusCode 
LArCalibPedMonAlg::initialize()
{
  ATH_MSG_INFO( "Initialize LArCalibPedMonAlg"  );

  ATH_MSG_INFO( "m_accDigitContainerKey.empty() " << m_accDigitContainerKey.empty()
        );
  if(!m_calibDigitContainerKey.empty()) {
    ATH_CHECK( m_calibDigitContainerKey.initialize() );
  } else if(!m_accDigitContainerKey.empty()) {
    ATH_CHECK( m_accDigitContainerKey.initialize() );
  } else if(!m_accCalibDigitContainerKey.empty()) {
    ATH_CHECK( m_accCalibDigitContainerKey.initialize() );
  } else {
     ATH_MSG_FATAL("Either LArCalibDigitContainerKey or LArAccumulatedDigitContainerKey or LArAccumulatedCalibDigitContainerKey must be set");
     return StatusCode::FAILURE;
  }

  return AthMonitorAlgorithm::initialize();
}


/*---------------------------------------------------------*/
StatusCode 
LArCalibPedMonAlg::fillHistograms( const EventContext& ctx ) const
{

  ATH_MSG_DEBUG( "in fillHists()"  );
  
  SG::ReadHandle<LArCalibDigitContainer> pLArCalibDigitContainer;
  SG::ReadHandle<LArAccumulatedDigitContainer> pLArAccDigitContainer;
  SG::ReadHandle<LArAccumulatedCalibDigitContainer> pLArAccCalibDigitContainer;

  std::unordered_set<unsigned int> chanids;

  if(!m_calibDigitContainerKey.empty()) {
    pLArCalibDigitContainer= SG::ReadHandle<LArCalibDigitContainer>{m_calibDigitContainerKey,ctx};
    if(pLArCalibDigitContainer.isValid()){
       ATH_MSG_DEBUG("Got LArCalibDigitContainer with key "<< m_calibDigitContainerKey.key());
    } else {
       ATH_MSG_WARNING("Do not have LArCalibDigitContainer with key "<< m_calibDigitContainerKey.key());
    }  
  }

  if(!m_accDigitContainerKey.empty()) {
    pLArAccDigitContainer= SG::ReadHandle<LArAccumulatedDigitContainer>{m_accDigitContainerKey,ctx};
    if(pLArAccDigitContainer.isValid()){
       ATH_MSG_DEBUG("Got LArAccumulatedDigitContainer with key "<< m_accDigitContainerKey.key());
    } else {
       ATH_MSG_WARNING("Do not have LArAccumulatedDigitContainer with key "<< m_accDigitContainerKey.key());
    }  

    if(pLArAccDigitContainer->empty()) return StatusCode::SUCCESS; // Nothing to fill

    LArAccumulatedDigitContainer::const_iterator itDig = pLArAccDigitContainer->begin();
    LArAccumulatedDigitContainer::const_iterator itDig_e = pLArAccDigitContainer->end();
    const LArAccumulatedDigit* pLArDigit;
    for ( ; itDig!=itDig_e;++itDig) {
        pLArDigit = *itDig;
        unsigned int id = (pLArDigit->hardwareID()).get_identifier32().get_compact();
        chanids.emplace (id);
    }

    ATH_MSG_DEBUG("Filling nbChan: "<<chanids.size());

    auto nbchan = Monitored::Scalar<unsigned int>("nbChan",chanids.size());
    fill(m_MonGroupName,nbchan);
    
  }

  if(!m_accCalibDigitContainerKey.empty()) {
    pLArAccCalibDigitContainer= SG::ReadHandle<LArAccumulatedCalibDigitContainer>{m_accCalibDigitContainerKey,ctx};
    if(pLArAccCalibDigitContainer.isValid()){
       ATH_MSG_DEBUG("Got LArAccumulatedCalibDigitContainer with key "<< m_accCalibDigitContainerKey.key());
    } else {
       ATH_MSG_WARNING("Do not have LArAcumulatedCalibDigitContainer with key "<< m_accCalibDigitContainerKey.key());
    }  

    if(pLArAccCalibDigitContainer->empty()) return StatusCode::SUCCESS; // Nothing to fill

    LArAccumulatedCalibDigitContainer::const_iterator itDig = pLArAccCalibDigitContainer->begin();
    LArAccumulatedCalibDigitContainer::const_iterator itDig_e = pLArAccCalibDigitContainer->end();
    const LArAccumulatedCalibDigit* pLArDigit;
    for ( ; itDig!=itDig_e;++itDig) {
        pLArDigit = *itDig;
        unsigned int id = (pLArDigit->hardwareID()).get_identifier32().get_compact();
        chanids.emplace (id);
    }

    ATH_MSG_DEBUG("Filling nbChan: "<<chanids.size());

    auto nbchan = Monitored::Scalar<unsigned int>("nbChan",chanids.size());
    fill(m_MonGroupName,nbchan);
    
  }
  return StatusCode::SUCCESS;
}



