/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigValAlgs/TrigProblemFinder.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigDecisionTool/ExpertMethods.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

TrigProblemFinder::TrigProblemFinder(const std::string &name, ISvcLocator *pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_log(0),
    m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_storeGate("StoreGateSvc", name)
{
  declareProperty("checkHeaders", m_checkHeaders=true, "Checks error words in the HLT Result headers");
  declareProperty("checkChains", m_checkChains=true,   "Checks error words in the HLT chains");
  declareProperty("checkNavigation", m_checkNavigation=true,   "Checks error words in the HLT Navigation");
}


TrigProblemFinder::~TrigProblemFinder() {}


// IAlgorithm virtual methods to implement
StatusCode TrigProblemFinder::initialize() {
  
  m_log = new MsgStream(messageService(), name());
  StatusCode sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    (*m_log) << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_trigDec.retrieve();
  if ( sc.isFailure() ) {
    (*m_log) << MSG::ERROR << "Could not retrieve TrigDecisionTool!" << endreq;
    return sc;
  }
  // enable expert access to item and chain info
  m_trigDec->ExperimentalAndExpertMethods()->enable();
  return StatusCode::SUCCESS; 
}
 


StatusCode TrigProblemFinder::execute() {

  // headers
  if ( m_checkHeaders ) {
    const DataHandle<TrigDec::TrigDecision> td;
    StatusCode sc = m_storeGate->retrieve(td);
    if (sc.isFailure()) {
      (*m_log) << MSG::FATAL << "Could not find TrigDecision object" << endreq;
      return StatusCode::FAILURE;
    }
    checkHeader(&(td->getL2Result()));
    checkHeader(&(td->getEFResult()));
  }
  if ( m_checkChains ) {
    checkChains();
  }

  if ( m_checkNavigation ) {
    checkNavigation();
  }
  
  return StatusCode::SUCCESS;

}

StatusCode TrigProblemFinder::finalize() {
  return StatusCode::SUCCESS;
}

void TrigProblemFinder::checkAndPrintErrorCode(HLT::ErrorCode ec, const std::string& context) {
  if ( ec != HLT::OK ) {
    (*m_log) << MSG::WARNING << std::setw(50) << context << " " << strErrorCode(ec) << endreq;
    (*m_log) << MSG::DEBUG   << std::setw(50) << context << " " << strErrorCode(ec) << endreq;
  } 
}


void TrigProblemFinder::checkHeader(const HLT::HLTResult* r) {
  (*m_log) << MSG::DEBUG << "HLTResult Header: starting check" << endreq;  

  if (r == 0){
    (*m_log) << MSG::WARNING << std::setw(50) <<"HLTResult: missing" << endreq;
    return;
  }

  if ( r->isCreatedOutsideHLT() ) {
    (*m_log) << MSG::WARNING << "HLTResult Header: careated outside HLT"  << endreq;
  }


  checkAndPrintErrorCode( r->getHLTStatus(), "HLTResult Header: overlall" );
  checkAndPrintErrorCode( r->getLvlConverterStatus(), "HLTResult Header: converter" );
  if ( r->getHLTStatus() != HLT::OK ) {
    unsigned int chainCounter=0xffffffff;
    unsigned int step=0;
    const_cast<HLT::HLTResult*>(r)->getErrorCoordinates(chainCounter, step);
    (*m_log) << MSG::WARNING << std::setw(50) <<"HLTResult Header: most severe chain"  << chainCounter << endreq;
    (*m_log) << MSG::WARNING << std::setw(50) <<"HLTResult Header: last step"  << step << endreq;
    return;
  }
  if ( r->isHLTResultTruncated() ) {
    (*m_log) << MSG::WARNING << "HLTResult Header: truncated"  << endreq;
    return;
  }   
}



void TrigProblemFinder::checkChains() {

  (*m_log) << MSG::DEBUG << "L2 and EF chains: starting check" << endreq;  
  Trig::ExpertMethods* em = m_trigDec->ExperimentalAndExpertMethods();
  std::vector<std::string> chains( m_trigDec->getListOfTriggers("L2_.*, EF_.*"));
  for (std::vector<std::string>::iterator it = chains.begin(); it != chains.end(); ++it ) {
    const HLT::Chain* ch = em->getChainDetails(*it);
    if (ch)
      checkAndPrintErrorCode( ch->getErrorCode(), ch->getChainName() );
  }

}

void TrigProblemFinder::checkTE(const HLT::TriggerElement* te, HLT::NavigationCore* nav, const Trig::TrigDecisionTool* td) {
  using namespace HLT;
  Trig::Feature<TrigRoiDescriptor> roif = td->ancestor<TrigRoiDescriptor>(te,std::string("initialRoI"));
  const TrigRoiDescriptor* roi = roif.cptr();
  if ( te->getErrorState() ) {
    if (roi)
      (*m_log) << MSG::WARNING << "Navigation: TE marked as seeing error " << roif.label() << " " << *roi << endreq;
    else 
      (*m_log) << MSG::WARNING << "Navigation: TE marked as seeing error " << roif.label() <<  ", no RoI attributed" << endreq;    
  } else {
    if (roi)
      (*m_log) << MSG::DEBUG << "Navigation: TE " << roif.label()  << " " << *roi << endreq;
    else 
      (*m_log) << MSG::DEBUG << "Navigation: TE " << roif.label()  << " " << "no RoI attributed" << endreq;    
  }
  

  const std::vector<TriggerElement*> succ = nav->getDirectSuccessors(te);
  std::vector<TriggerElement*>::const_iterator it;
  for ( it = succ.begin(); it != succ.end(); ++it ) {
    checkTE(*it, nav, td);
  }
}
	       

void TrigProblemFinder::checkNavigation() {
  (*m_log) << MSG::DEBUG << "Navigation: starting check" << endreq;  
  using namespace HLT;
  const NavigationCore* cnav = m_trigDec->ExperimentalAndExpertMethods()->getNavigation();
  NavigationCore* nav = const_cast<NavigationCore*>(cnav);
  const TriggerElement* initTe = nav->getInitialNode();
  checkTE(initTe, nav, &*m_trigDec);
}



