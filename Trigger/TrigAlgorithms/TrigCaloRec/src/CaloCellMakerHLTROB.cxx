/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CaloCellMakerHLTROB.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   Denis O. Damazio
//           This is an Hlt algorithm that creates a cell container
//           with calorimeter cells within an RoI. Based on Cibran 
//           Santamarina original TrigCaloRec
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
#include "TrigSteeringEvent/PhiHelper.h"
//
#include "CaloEvent/CaloCellContainer.h"
//
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//
#include "CaloUtils/CaloCollectionHelper.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigCaloRec/CaloCellMakerHLTROB.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"
#include "TrigCaloRec/TrigCaloQuality.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"

//
class ISvcLocator;


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloCellMakerHLTROB::CaloCellMakerHLTROB(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  ,  m_tcrAlgTools(this), 
    m_counter(0),
    m_caloCellContainer(NULL),
    m_fullScanEnabled(false)
 {

   // Name of Tools to check and finalize the cell container...
    declareProperty("ContainerTools",m_containerToolNames);

    declareProperty ("TCRTools", m_tcrAlgTools);
    //declareProperty ("ContainerTools", m_containerTools);
    
    m_vec_robs.reserve(1000);
     
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
CaloCellMakerHLTROB::~CaloCellMakerHLTROB()
{ m_vec_robs.clear(); }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NSSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

StatusCode CaloCellMakerHLTROB::initialize()
{
  msg() << MSG::INFO << "in initialize()" << endmsg;

  m_counter = 0;
  
  // Cache pointer to ToolSvc
  IToolSvc* toolSvc = 0;// Pointer to Tool Service
  if (service("ToolSvc", toolSvc).isFailure()) {
    msg() << MSG::FATAL << " Tool Service not found " << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<std::string>::iterator itrName;
  std::vector<std::string>::iterator endName;
  m_fullScanEnabled=false;

  ToolHandleArray<IAlgToolEFCalo>::iterator itrtcr = m_tcrAlgTools.begin();
  ToolHandleArray<IAlgToolEFCalo>::iterator endtcr = m_tcrAlgTools.end();

  for (; itrtcr!=endtcr; ++itrtcr) {

    if( (itrtcr->retrieve()).isFailure() ) {
        msg() << MSG::ERROR << "Unable to find tool named " << (*itrtcr).name() << endmsg;
        return StatusCode::FAILURE;
    } else {
      msg() << MSG::INFO << " successfully retrieved " << (*itrtcr).name() << endmsg;
    }    
    if((*itrtcr).name() == "FullCaloCellContMaker")
        m_fullScanEnabled=true;
  }
  // access CellContainer tools and store them
  itrName = m_containerToolNames.begin() ;
  endName = m_containerToolNames.end() ;

  for (; itrName!=endName; ++itrName) {
     
    ListItem theItem(*itrName);
    IAlgTool* algtool;
     
    if( toolSvc->retrieveTool(theItem.type(), theItem.name(), algtool,this).isFailure() ) {
      msg() << MSG::FATAL << "Unable to find tool for " << (*itrName) << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg() << MSG::INFO << (*itrName) << " successfully retrieved" << endmsg;
 // Save the cell container tools (only for checking purposes) in m_containerTools
      m_containerTools.push_back(dynamic_cast<ICaloCellMakerTool*>(algtool) );
    }
  }
 // end of helpers...
 
 return StatusCode::SUCCESS;
}


StatusCode CaloCellMakerHLTROB::finalize()
{

  return StatusCode::SUCCESS;
}

StatusCode CaloCellMakerHLTROB::execute( )
{
  
  // Time total CaloCellMakerHLTROB execution time.
#ifndef NDEBUG
    ATH_MSG_DEBUG("in execute()");
#endif
  
  // Monitoring initialization...
   m_CellContainerSize = 0.;

  // Some debug output:
   
   // Get RoiDescriptor
  //  bool createdRoI = false;
      
  TrigRoiDescriptor roi( true );
  
  // The CellContainer is not the cell owner (SG::VIEW_ELEMENTS)...
  m_caloCellContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);
  m_caloCellContainer->reserve(190000);


  if ( evtStore()->record(m_caloCellContainer, "AllCaloHLT").isFailure() ) {
    msg() << MSG::ERROR << "Could not record a cell container in the RoI with key AllCaloHLT" << endmsg;
  }
  
  // Creating the cells containers...
  ToolHandleArray<IAlgToolEFCalo>::iterator itrtcr = m_tcrAlgTools.begin();
  ToolHandleArray<IAlgToolEFCalo>::iterator endtcr = m_tcrAlgTools.end();
  // Use specific Tools  to fill the container :
  int index=0;
  uint32_t error = 0;
  

  unsigned idet=0;
  for (; itrtcr!=endtcr; ++itrtcr) {
    
    StatusCode sc;

    if(m_fullScanEnabled) {
      sc= (*itrtcr)->execute(*m_caloCellContainer);
    } else {
      //      sc = (*itrtcr)->execute(*m_caloCellContainer, new TrigRoiDescriptor( eta0, etamin, etamax, phi0, phimin, phimax ) );
      sc = (*itrtcr)->execute(*m_caloCellContainer, roi );
    }
    
    if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Error executing TCR tool " << (*itrtcr).name() << endmsg;
      return StatusCode::FAILURE;
    } else {  
      if ( m_fullScanEnabled ) idet = IAlgToolEFCalo::EFFULLCALO;
    }
    
    ++index;
  }


  /// delete the Roi descriptor if we needed to create another (should really 
  /// add it to the navigation with the appropriate IDs !!! 

  if (m_caloCellContainer->size() < 1) return StatusCode::FAILURE;

  // Looping over cell container tools... this could be used for calibrations...
  
  std::vector<ICaloCellMakerTool*> ::const_iterator itrcont=m_containerTools.begin();
  std::vector<ICaloCellMakerTool*> ::const_iterator endcont=m_containerTools.end();
  
  index=0;
  for (; itrcont!=endcont; ++itrcont) {
    if ( (*itrcont)->process(m_caloCellContainer).isFailure() ) {
      //msg() << MSG::ERROR << "Error executing tool " << m_containerToolNames[index] << endmsg;
      msg() << MSG::ERROR << "Error executing container tool " << endmsg;
    } 
    ++index;
  }
  if ( m_fullScanEnabled) m_caloCellContainer->updateCaloIterators();
  (evtStore()->setConst( m_caloCellContainer ) ).ignore();


  ++m_counter;
  
  return StatusCode::SUCCESS; 
}

