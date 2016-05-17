/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigValAlgs/TrigProblemFinder.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigDecisionTool/ExpertMethods.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigDecisionEvent/TrigDecision.h"

TrigProblemFinder::TrigProblemFinder(const std::string &name, ISvcLocator *pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareProperty("checkHeaders", m_checkHeaders=true, "Checks error words in the HLT Result headers");
  declareProperty("checkChains", m_checkChains=true,   "Checks error words in the HLT chains");
  declareProperty("checkNavigation", m_checkNavigation=true,   "Checks error words in the HLT Navigation");
}


TrigProblemFinder::~TrigProblemFinder() {}


// IAlgorithm virtual methods to implement
StatusCode TrigProblemFinder::initialize() {
  
  ATH_CHECK( m_trigDec.retrieve() );

  // enable expert access to item and chain info
  m_trigDec->ExperimentalAndExpertMethods()->enable();
  return StatusCode::SUCCESS; 
}
 


StatusCode TrigProblemFinder::execute() {

  // headers
  if ( m_checkHeaders ) {
    const DataHandle<TrigDec::TrigDecision> td;
    ATH_CHECK( evtStore()->retrieve(td) );
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
    ATH_MSG_WARNING (std::setw(50) << context << " " << strErrorCode(ec) );
    ATH_MSG_DEBUG   (std::setw(50) << context << " " << strErrorCode(ec) );
  } 
}


void TrigProblemFinder::checkHeader(const HLT::HLTResult* r) {
  ATH_MSG_DEBUG ("HLTResult Header: starting check" );

  if (r == 0){
    ATH_MSG_WARNING (std::setw(50) <<"HLTResult: missing" );
    return;
  }

  if ( r->isCreatedOutsideHLT() ) {
    ATH_MSG_WARNING ("HLTResult Header: careated outside HLT"  );
  }


  checkAndPrintErrorCode( r->getHLTStatus(), "HLTResult Header: overlall" );
  checkAndPrintErrorCode( r->getLvlConverterStatus(), "HLTResult Header: converter" );
  if ( r->getHLTStatus() != HLT::OK ) {
    unsigned int chainCounter=0xffffffff;
    unsigned int step=0;
    const_cast<HLT::HLTResult*>(r)->getErrorCoordinates(chainCounter, step);
    ATH_MSG_WARNING (std::setw(50) <<"HLTResult Header: most severe chain"  << chainCounter );
    ATH_MSG_WARNING (std::setw(50) <<"HLTResult Header: last step"  << step );
    return;
  }
  if ( r->isHLTResultTruncated() ) {
    ATH_MSG_WARNING ("HLTResult Header: truncated"  );
    return;
  }   
}



void TrigProblemFinder::checkChains() {

  ATH_MSG_DEBUG ("L2 and EF chains: starting check" );
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
      ATH_MSG_WARNING ("Navigation: TE marked as seeing error " << roif.label() << " " << *roi );
    else 
      ATH_MSG_WARNING ("Navigation: TE marked as seeing error " << roif.label() <<  ", no RoI attributed" );
  } else {
    if (roi)
      ATH_MSG_DEBUG ("Navigation: TE " << roif.label()  << " " << *roi );
    else 
      ATH_MSG_DEBUG ("Navigation: TE " << roif.label()  << " " << "no RoI attributed" );
  }
  

  const std::vector<TriggerElement*> succ = nav->getDirectSuccessors(te);
  std::vector<TriggerElement*>::const_iterator it;
  for ( it = succ.begin(); it != succ.end(); ++it ) {
    checkTE(*it, nav, td);
  }
}
	       

void TrigProblemFinder::checkNavigation() {
  ATH_MSG_DEBUG ("Navigation: starting check" );
  using namespace HLT;
  const NavigationCore* cnav = m_trigDec->ExperimentalAndExpertMethods()->getNavigation();
  NavigationCore* nav = const_cast<NavigationCore*>(cnav);
  const TriggerElement* initTe = nav->getInitialNode();
  checkTE(initTe, nav, &*m_trigDec);
}



