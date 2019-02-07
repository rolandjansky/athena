/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigFullCaloCellMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   C. Santamarina
//           This is an Hlt algorithm that creates a cell container
//           with calorimeter cells.
//
// ********************************************************************
//

#include <sstream>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//
#include "CaloEvent/CaloCellContainer.h"
//
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//
#include "CaloUtils/CaloCollectionHelper.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigCaloRec/TrigFullCaloCellMaker.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"
#include "CaloInterface/ICaloCellMakerTool.h"

//
class ISvcLocator;
// EM mid layer cell size.
const double TrigFullCaloCellMaker::m_detas2 = 0.025;
const double TrigFullCaloCellMaker::m_dphis2 = 0.0245436926;


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

TrigFullCaloCellMaker::TrigFullCaloCellMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_tcrAlgTools(this), 
    m_pCaloCellContainer(NULL),
    m_useCachedResult(false), m_cachedTE(0)
{
  
   // Name of Tools to check and finalize the cell container...
    declareProperty("ContainerTools",m_containerToolNames);

   // Eta and Phi size of the RoI window...
    declareProperty("eta_size",m_neta);
    declareProperty("phi_size",m_nphi);
    
    declareProperty ("TCRTools", m_tcrAlgTools);
    //declareProperty ("ContainerTools", m_containerTools);

    declareMonitoredVariable("CellContainerSize", m_CellContainerSize);
    declareMonitoredVariable("ConversionErrorInFullCalo", m_conversionError);

}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigFullCaloCellMaker::~TrigFullCaloCellMaker()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NSSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////q////////////
//

HLT::ErrorCode TrigFullCaloCellMaker::hltInitialize()
{
  msg() << MSG::INFO << "in initialize()" << endmsg;

  // Cache pointer to ToolSvc
  IToolSvc* toolSvc = 0;// Pointer to Tool Service
  if (service("ToolSvc", toolSvc).isFailure()) {
    msg() << MSG::FATAL << " Tool Service not found " << endmsg;
    return HLT::TOOL_FAILURE;
  }

  if (timerSvc()) { // Global timers
    m_timer.push_back(addTimer("TCCF_TimerTot"));
    m_timer.push_back(addTimer("TCCF_RoICellContainer"));
    //declareMonitoredObject("TCCF.TimerTot",*(m_timer[0]), &TrigTimer::lastElapsed);
    //declareMonitoredObject("TCCF.RoICellContainer",*(m_timer[1]), &TrigTimer::lastElapsed);
  }


  /*if ( m_tcrAlgTools.retrieve().isFailure() ) {
    msg() << MSG::ERROR << "Failed to retrieve helper tools: " << m_tcrAlgTools << endmsg;
    return HLT::BAD_JOB_SETUP;
   } else {
    msg() << MSG::INFO << "Retrieved " << m_tcrAlgTools << endmsg;
  }*/

  std::vector<std::string>::iterator itrName;
  std::vector<std::string>::iterator endName;

  ToolHandleArray<IAlgToolEFCalo>::iterator itrtcr = m_tcrAlgTools.begin();
  ToolHandleArray<IAlgToolEFCalo>::iterator endtcr = m_tcrAlgTools.end();

  for (; itrtcr!=endtcr; ++itrtcr) {

    if( (itrtcr->retrieve()).isFailure() ) {
        msg() << MSG::ERROR << "Unable to find tool named " << (*itrtcr).name() << endmsg;
        return HLT::BAD_JOB_SETUP;
    } else {
      msg() << MSG::INFO << " successfully retrieved " << (*itrtcr).name() << endmsg;
      std::vector<TrigTimer*> tooltimers = (*itrtcr)->getTimers();
      std::vector<TrigTimer*>::iterator timer = tooltimers.begin();
      for (; timer != tooltimers.end(); timer++){
          declareMonitoredObject((*timer)->name(),*(*timer), &TrigTimer::lastElapsed);
          //declareMonitoredObject((*timer)->name(),*(*timer), &TrigTimer::elapsed);
          if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found timer. Added to monitoring " << endmsg;
      }
    }
  }
  // access CellContainer tools and store them
  itrName = m_containerToolNames.begin() ;
  endName = m_containerToolNames.end() ;

  for (; itrName!=endName; ++itrName) {
     
    ListItem theItem(*itrName);
    IAlgTool* algtool;
     
    if( toolSvc->retrieveTool(theItem.type(), theItem.name(), algtool,this).isFailure() ) {
      msg() << MSG::FATAL << "Unable to find tool for " << (*itrName) << endmsg;
      return HLT::TOOL_FAILURE;
    } else {
      msg() << MSG::INFO << (*itrName) << " successfully retrieved" << endmsg;
 // Save the cell container tools (only for checking purposes) in m_containerTools
      m_containerTools.push_back(dynamic_cast<ICaloCellMakerTool*>(algtool) );
      if(timerSvc() ) m_timer.push_back(addTimer("TCCF_"+theItem.name())); // One timer per tool
    }
  }
 // end of helpers...
 
 if (msgLvl() <= MSG::DEBUG)
   msg() << MSG::DEBUG
         << "Initialization of TrigFullCaloCellMaker completed successfully"
         << endmsg;
 
 return HLT::OK;
}


HLT::ErrorCode TrigFullCaloCellMaker::hltFinalize()
{
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in finalize()" << endmsg;

  return HLT::OK;
}

HLT::ErrorCode TrigFullCaloCellMaker::hltExecute( std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
                              unsigned int type_out) {

  // Time total TrigFullCaloCellMaker execution time.
  if (timerSvc()) m_timer[0]->start();
  m_conversionError=0;

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in execute()" << endmsg;

  // CACHING
   if (msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "Executing TrigFullCaloCellMaker/" << name() << " in cached mode" << endmsg;

  // get all input TEs (for seeding relation of navigation structure)
  HLT::TEVec allTEs;

  for (std::vector<HLT::TEVec>::const_iterator it = tes_in.begin(); it != tes_in.end(); ++it) {

    for (HLT::TEVec::const_iterator inner_it = (*it).begin(); inner_it != (*it).end(); ++inner_it) {
      allTEs.push_back(*inner_it);
      const TrigRoiDescriptor* roiDescriptor = 0;
      HLT::ErrorCode sc = getFeature(*inner_it, roiDescriptor, "");
      if (sc != HLT::OK) return sc;
      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << " REGTEST: RoI id " << roiDescriptor->roiId()
              << " located at   phi = " <<  roiDescriptor->phi()
              << ", eta = " << roiDescriptor->eta() << endmsg;
    }
  }

    // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);

  if (m_useCachedResult) {
  // save (cached) met feature to output TE:
    m_config->getNavigation()->copyAllFeatures( m_cachedTE, outputTE );
    outputTE->setActiveState(true);
    //outputTE->setActiveState(false); // We do not want the algorithms on top of this one to run on the Cached RoIs
    return HLT::OK;
  }

  beforeExecMonitors().ignore();
  // Monitoring initialization...
  m_CellContainerSize = 1000.;
   
  if (timerSvc()) m_timer[1]->start(); // Measures the time to retrieve the cells in the RoI  

  // The CellContainer is not the cell owner (SG::VIEW_ELEMENTS)...
  //CaloCellContainer* m_pCaloCellContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);
  m_pCaloCellContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);
  m_pCaloCellContainer->reserve(190000); // if you are doing the whole calo..

  std::string cellCollKey;
  HLT::ErrorCode sc = getUniqueKey( m_pCaloCellContainer, cellCollKey, "TrigCaloCellMaker" );
  if (sc != HLT::OK) { 
    msg() << MSG::DEBUG << "Could not retrieve the cell collection key" << endmsg;
  return sc;                                                                                                                                                   
  }     

  if ( store()->record(m_pCaloCellContainer, cellCollKey).isFailure() ) {
    msg() << MSG::ERROR << "Could not record a cell container in the RoI with key " << cellCollKey << endmsg;
  }


  //  HLT::ErrorCode sc =recordAndAttachFeature(outputTE, m_pCaloCellContainer, cellCollKey, "TrigCaloCellMaker");
  //  if (sc != HLT::OK) {
  //    msg() << MSG::ERROR << "Could not record a cell container in the RoI with key " << cellCollKey << endmsg;
  //    return sc;
  //  } else {
  //    if (msgLvl() <= MSG::DEBUG)
  //      msg() << MSG::DEBUG << " REGTEST: Recorded the cell container in the RoI " << endmsg;
  //  }


// Creating the cells containers...
  ToolHandleArray<IAlgToolEFCalo>::iterator itrtcr = m_tcrAlgTools.begin();
  ToolHandleArray<IAlgToolEFCalo>::iterator endtcr = m_tcrAlgTools.end();
  // Use specific Tools  to fill the container :
  int index=0;
  uint32_t error = 0;
  for (; itrtcr!=endtcr; ++itrtcr) {

    //if (timerSvc()) m_timer[2+index]->start();
    if ( ((*itrtcr)->execute(*m_pCaloCellContainer)).isFailure() ) {
      msg() << MSG::ERROR << "Error executing TCR tool " << (*itrtcr).name() << endmsg;
      return HLT::TOOL_FAILURE;
    } else {  
      uint32_t in_error = (*itrtcr)->report_error();
      if (0x0FFFFFFF & in_error) ++m_conversionError;
      error|=(in_error&0x0FFFFFFF);
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executed TCR tool " << (*itrtcr).name() << endmsg;
    }
    //if (timerSvc()) m_timer[2+index]->stop();
    
    ++index;
  }

// Looping over cell container tools... this could be used for calibrations...

  index=0;
  const EventContext& ctx = Gaudi::Hive::currentContext();
  for (const ICaloCellMakerTool* tool : m_containerTools) {
    if ( tool->process (m_pCaloCellContainer, ctx).isFailure() ) {
      //msg() << MSG::ERROR << "Error executing tool " << m_containerToolNames[index] << endmsg;
      msg() << MSG::ERROR << "Error executing container tool " << endmsg;
    } else {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executed container tool " << endmsg;
    }
    ++index;
  }

  m_pCaloCellContainer->updateCaloIterators();   // update the iterators (will issue WARNING if indeed cells are mixed up)
  m_pCaloCellContainer->initializeLookUpTable(); // build look up table (to be done by hand otherwise will be triggered by first use)

  if((store()->setConst(m_pCaloCellContainer)).isFailure()) return HLT::TOOL_FAILURE; // lock the container


  if (timerSvc()) m_timer[1]->stop();

  cellCollKey = "";
  sc = reAttachFeature(outputTE, m_pCaloCellContainer, cellCollKey, "TrigCaloCellMaker");
  if (sc != HLT::OK) {
    msg() << MSG::ERROR << "Could not record a cell container in the RoI with key " << cellCollKey << endmsg;
    return sc;
  } else {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " REGTEST: Recorded the cell container in the RoI " << endmsg;
  }


  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " REGTEST: Produced a Cell Container of Size= " << m_pCaloCellContainer->size() << endmsg;

  //monitoring
  m_CellContainerSize = (float)m_pCaloCellContainer->size();

  // Time total TrigCaloCellMaker execution time.
  if (timerSvc()) m_timer[0]->stop();

  m_useCachedResult = true;
  m_cachedTE = outputTE;

 // stop monitoring
  afterExecMonitors().ignore();

  if (error)
    return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::USERDEF_2);

  return HLT::OK;
}
