/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigGenericAlgs/SeededSuperRoiAllTEAlgo.h"
#include "TrigNavigation/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "CxxUtils/phihelper.h"

using namespace PESA;

SeededSuperRoiAllTEAlgo::SeededSuperRoiAllTEAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_runOncePerEvent(true),
    m_was_run(false),
    m_useRoiSizes(true)
{
  declareProperty("NumberOfOutputTEs", m_numberOfOutputTEs = 1, "configure the number of output TEs this algorithm will create");
  declareProperty("createRoIDescriptors",  m_createRoIDescriptors=true, "create SuperRoi descriptor if set true");
  declareProperty("runOncePerEvent",  m_runOncePerEvent=true, "run opnly once per event");
  declareProperty("prefetchPIX",  m_prefetchPIX=false, "register ROB requests for pixels");
  declareProperty("prefetchSCT",  m_prefetchSCT=false, "register ROB requests for SCT");
  declareProperty("UseRoiSizes",  m_useRoiSizes=true, "User RoI sizes rather than width parameters");
  declareProperty("EtaHalfWidth",  m_etaHalfWidth=0.1, "RoI eta half width");
  declareProperty("PhiHalfWidth",  m_phiHalfWidth=0.1, "RoI phi half width");
}


HLT::ErrorCode SeededSuperRoiAllTEAlgo::hltInitialize(){

  m_was_run=false; 

  return HLT::OK; 
}


HLT::ErrorCode SeededSuperRoiAllTEAlgo::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
						  unsigned int type_out)
{
  if ( m_runOncePerEvent && m_was_run ) {
    return HLT::OK;
  }
  m_was_run = true;


  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  beforeExecMonitors().ignore();


  if ( msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Executing SeededSuperRoiAllTEAlgo (" << name()
	  << "). This is an seeded algorithm that will produce exactly "
	  << m_numberOfOutputTEs << " output TEs." << endmsg;
  }


  /// check if the input is ok ...
  if(tes_in.size()==0) return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::USERDEF_1);

  // create the output TEs:
  for ( unsigned i=0 ; i<m_numberOfOutputTEs ; ++i ) {

    // roiDescriptor position:
    HLT::TriggerElement* te;

    TrigRoiDescriptor* superRoi = new TrigRoiDescriptor(true); /// create a fullscan roi - don't need to specify eta and phi any longer
    
    superRoi->setComposite(true);
    superRoi->clear();

    if ( m_createRoIDescriptors ) {

      /// if we want to create our own RoiDescriptors, we need to ensure 
      /// they are deleted at the end
      //      if ( !m_useRoiSizes ) superRoi->manageConstituents(true);
      superRoi->manageConstituents(true);

      // if the input is valid, take _the_ last TEVec and process it
      const HLT::TEVec &tev = tes_in.back();
      
      /// what does this do ?? is it really needed ?? I don;t think so if you 
      /// use the addRoi method later
      //  if ((getUniqueKey( m_superRoi, m_superRoIUniqueKey, m_superRoIOutputKey )) != HLT::OK) {
      //	(*m_log) << MSG::DEBUG << "Unable to retrieve the superRoI unique key" << endmsg;
      //  }
      //  if (evtStore()->record(m_superRoi, m_superRoIUniqueKey).isFailure()) {
      //	(*m_log) << MSG::DEBUG << "Unable to record the superRoI with the key - " << m_superRoIUniqueKey << endmsg;
      //  }
      
      
      //--------------------- begin superRoI fill ---------------------

      for ( unsigned iTe=0; iTe<tev.size() ; iTe++ ){

	const HLT::TriggerElement* inputTe = tev[iTe];
	
	if(!inputTe){
	  msg() << MSG::ERROR << "Invalid TriggerElement pointer = "<< inputTe << endmsg;
	  return HLT::ERROR;
	} // end if(!inputTe)
	
	// get the RoiDescriptor
	const TrigRoiDescriptor* roiDescriptor = 0;
	HLT::ErrorCode hltStatus = getFeature(inputTe, roiDescriptor);
	if( hltStatus != HLT::OK ){
	  msg()<<MSG::ERROR<<" Failed to find RoiDescriptor "<<endmsg;
	  return hltStatus;
	} 
	
	if ( m_useRoiSizes ) { 
	  if ( roiDescriptor ) superRoi->push_back( new TrigRoiDescriptor( *roiDescriptor) );
	}
	else { 
	  double eta = roiDescriptor->eta();
	  double phi = roiDescriptor->phi();
	  TrigRoiDescriptor* tmpRoiDescriptor = new TrigRoiDescriptor( eta,
                                                                   eta-m_etaHalfWidth,
                                                                   eta+m_etaHalfWidth,
                                                                   phi,
                                                                   CxxUtils::wrapToPi(phi-m_phiHalfWidth),
                                                                   CxxUtils::wrapToPi(phi+m_phiHalfWidth) );
       
	  if ( tmpRoiDescriptor ) superRoi->push_back(tmpRoiDescriptor);
	}
	  
      } // end for(iTe)
      
    }
    
    te = addRoI(type_out, superRoi ); // note: addRoI is defined in AllTEAlgo base class
    
    te->setActiveState(  (superRoi->size()>0) ); // set this trigger element to be active

  }
  

  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  afterExecMonitors().ignore();

  // everything's ok
  return HLT::OK;
}

HLT::ErrorCode SeededSuperRoiAllTEAlgo::hltEndEvent() {
  m_was_run = false;
  return HLT::OK;
}

HLT::ErrorCode SeededSuperRoiAllTEAlgo::prepareRobRequests(const std::vector<HLT::TEConstVec>& /* inputTE */){


  return HLT::OK;
} 

