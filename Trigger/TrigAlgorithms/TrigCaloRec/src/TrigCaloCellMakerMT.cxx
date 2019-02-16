/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigCaloCellMakerMT.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   P.A. Delsart
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
#include "AthenaMonitoring/Monitored.h"

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

#include "TrigCaloCellMakerMT.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"
#include "TrigCaloRec/TrigCaloQuality.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthContainers/ConstDataVector.h"
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"

//
class ISvcLocator;
// EM mid layer cell size.
const double TrigCaloCellMakerMT::m_detas2 = 0.025;
const double TrigCaloCellMakerMT::m_dphis2 = 0.0245436926;


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

TrigCaloCellMakerMT::TrigCaloCellMakerMT(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_counter(0),
    m_dataAccessSvc( "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", name ),
    m_fullScanEnabled(false),
    m_trustRoiLimits(false),
    m_createRoiForID(false),
    m_etaWidthForID(0),
    m_phiWidthForID(0)
 {

   // Name of Tools to check and finalize the cell container...
    declareProperty("ContainerTools",m_containerToolNames);

   // Eta and Phi size of the RoI window...
    declareProperty("FullScanEnabled",m_fullScanEnabled);
    declareProperty("eta_size",m_neta);
    declareProperty("phi_size",m_nphi);

    /// Trust the limits encoded in the RoI already
    declareProperty("TrustRoiLimits", m_trustRoiLimits=false);

    /// create an RoI for the ID trigger
    declareProperty("CreateRoiForID", m_createRoiForID=false);
    declareProperty("EtaWidthForID",  m_etaWidthForID=0);
    declareProperty("PhiWidthForID",  m_phiWidthForID=0);

     
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigCaloCellMakerMT::~TrigCaloCellMakerMT()
{ }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NSSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

StatusCode TrigCaloCellMakerMT::initialize()
{
  ATH_MSG_INFO("in initialize()");

  m_counter = 0;
  
  // Cache pointer to ToolSvc
  IToolSvc* toolSvc = 0;// Pointer to Tool Service
  if (service("ToolSvc", toolSvc).isFailure()) {
    ATH_MSG_FATAL (" Tool Service not found ");
    //    return HLT::TOOL_FAILURE;
    return StatusCode::SUCCESS;
  }
  if (!m_monTool.empty()) {
    ATH_MSG_DEBUG("Retrieving monTool");
    CHECK(m_monTool.retrieve());
  } else {
    ATH_MSG_INFO("No monTool configured => NO MONITOING");
  }



  CHECK( m_dataAccessSvc.retrieve() );

  std::vector<std::string>::iterator itrName;
  std::vector<std::string>::iterator endName;

  // access CellContainer tools and store them
  itrName = m_containerToolNames.begin() ;
  endName = m_containerToolNames.end() ;

  for (; itrName!=endName; ++itrName) {
     
    ListItem theItem(*itrName);
    IAlgTool* algtool;
     
    if( toolSvc->retrieveTool(theItem.type(), theItem.name(), algtool,this).isFailure() ) {
      ATH_MSG_FATAL("Unable to find tool for ");
      //return HLT::TOOL_FAILURE;
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_INFO ((*itrName) << " successfully retrieved" );
      // Save the cell container tools (only for checking purposes) in m_containerTools
      m_containerTools.push_back(dynamic_cast<ICaloCellMakerTool*>(algtool) );
    }
  }
 // end of helpers...
 
  ATH_MSG_DEBUG ("Initialization of TrigCaloCellMakerMT completed successfully");


 if (!m_fullScanEnabled) {
   ATH_MSG_DEBUG("In RoI mode");
   ATH_CHECK( m_inputRoiKey.initialize() );
 
   ATH_CHECK( m_outputCaloRoiKey.initialize() );
   ATH_CHECK( m_outputIDRoiKey.initialize() );
 } else {
   ATH_MSG_DEBUG("In FullScan mode");
 }

 ATH_CHECK( m_outputCellsKey.initialize() );
 ATH_CHECK( m_outputCDVcellsKey.initialize() );
 ATH_CHECK( m_outputTrigCaloQualityKey.initialize() );
 
 return StatusCode::SUCCESS;
}


StatusCode TrigCaloCellMakerMT::finalize()
{
  ATH_MSG_DEBUG ("in finalize()");

  return StatusCode::SUCCESS;
}

StatusCode TrigCaloCellMakerMT::execute()
{
  // Monitoring initialization...
  auto timer = Monitored::Timer("TIME_execute");
  auto time_tools = Monitored::Timer("TIME_ContainerTools");
  auto mon_CellContainerSize = Monitored::Scalar("CellContainerSize", 0.);
  auto mon_roi_phi = Monitored::Scalar("RoIPhi", 0.);
  auto mon_roi_eta = Monitored::Scalar("RoIEta", 0.);

  auto monitorIt = Monitored::Group( m_monTool, timer, time_tools, mon_roi_eta,  mon_roi_phi,  mon_CellContainerSize);

  //  for(int id=0; id<IAlgToolEFCalo::MAXNEFCALO; id++) m_conversionError[id]=0;
  
  ATH_MSG_DEBUG("in execute()");
  
  
  auto ctx = getContext();
  ATH_MSG_DEBUG(" Make Output Handles");
  auto caloCellContainer =   SG::makeHandle (m_outputCellsKey, ctx); 
  auto trigCaloQuality   =  SG::makeHandle (m_outputTrigCaloQualityKey, ctx); 
      

  ATH_CHECK(caloCellContainer.record (std::make_unique<CaloCellContainer>()));
  ATH_CHECK(trigCaloQuality.record (std::make_unique<TrigCaloQuality>()));

  //  caloCellContainer->reserve(190000);

  const TrigRoiDescriptor* roiDescriptor=nullptr;
  if (!m_fullScanEnabled) {
    ATH_MSG_DEBUG(" Make RoI Handles");
    auto outputCaloRoiColl =   SG::makeHandle (m_outputCaloRoiKey, ctx);  
  
    auto outputIDRoiColl =   SG::makeHandle (m_outputIDRoiKey, ctx);  

    ATH_CHECK( outputCaloRoiColl.record (std::make_unique<TrigRoiDescriptorCollection>()));
    ATH_CHECK( outputIDRoiColl.record (std::make_unique<TrigRoiDescriptorCollection>()));

    auto roiCollection = SG::makeHandle(m_inputRoiKey, ctx);
    
    if (roiCollection->size()==0) {
      ATH_MSG_DEBUG(" RoI collection size = 0");
      return StatusCode::SUCCESS;
    }
      
    roiDescriptor = *(roiCollection->begin());
    if (roiCollection->size() > 1) ATH_MSG_WARNING("Misconfiguration - Called with " << roiCollection->size() << " RoI, but it should be called with 1 RoI - Will only process the first RoI"); 
      
    mon_roi_eta=roiDescriptor->eta();
    mon_roi_phi=roiDescriptor->phi();

    if ( !roiDescriptor->composite() )  { 
      if ( !m_trustRoiLimits ){ 
	
	double   eta0 = roiDescriptor->eta();
	double   phi0 = HLT::wrapPhi(roiDescriptor->phi());

	double etamin = eta0-(m_detas2*(float)m_neta)/2.;
	double etamax = eta0+(m_detas2*(float)m_neta)/2.;
	// What if phimin < 0 or phimax > 2Pi ??  question
	double phimin = HLT::wrapPhi(phi0 - (m_dphis2*(float)m_nphi)/2.);
	double phimax = HLT::wrapPhi(phi0 + (m_dphis2*(float)m_nphi)/2.);
	
	ATH_MSG_DEBUG(" eta0 = "<< eta0);
	ATH_MSG_DEBUG("phi0 = "<< phi0);
	ATH_MSG_DEBUG(" etamin = "<< etamin);
	ATH_MSG_DEBUG(" etamax = "<< etamax);
	ATH_MSG_DEBUG(" phimin = " << phimin);
	ATH_MSG_DEBUG(" phimax = " <<phimax);
      
	TrigRoiDescriptor* roi = new TrigRoiDescriptor( roiDescriptor->roiWord(), roiDescriptor->l1Id(), roiDescriptor->roiId(),
						       eta0, etamin, etamax, phi0, phimin, phimax );

	       
	ATH_MSG_DEBUG(" pusing back output Roi");
	outputCaloRoiColl->push_back(roi);
	roiDescriptor = roi; 
      }
    }
    // create and attache an RoI for the ID Trigger
    if (m_createRoiForID ) {

      if ( roiDescriptor->composite() ) { 
	msg() << MSG::WARNING << " Request to build an RoI for the ID From a composite RoiDescriptor" << *roiDescriptor  << endmsg;
      }
    

      TrigRoiDescriptor* roi = new TrigRoiDescriptor( roiDescriptor->roiWord(), 
						      roiDescriptor->l1Id(), 
						      roiDescriptor->roiId(),
						      roiDescriptor->eta(),
						      roiDescriptor->eta()-m_etaWidthForID, 
						      roiDescriptor->eta()+m_etaWidthForID, 
						      roiDescriptor->phi(), 
						      HLT::wrapPhi(roiDescriptor->phi()-m_phiWidthForID), 
						      HLT::wrapPhi(roiDescriptor->phi()+m_phiWidthForID) ); 
      
      if ( m_etaWidthForID==0 || m_phiWidthForID==0 )  {
	msg() << MSG::WARNING << "ZERO width RoI requested for the ID: " <<  *roi << endmsg;
      }
      
      ATH_MSG_DEBUG("REGTEST: push_back RoI for ID " << *roi);
      outputIDRoiColl->push_back(roi);
    }
  }
  

  // Creating the cells containers...
  int index=0;
  uint32_t error = 0;
  
  //  if (timerSvc()) m_timer[1]->start();
  SG::WriteHandle<ConstDataVector<CaloCellContainer> > CDVcellContainer = SG::WriteHandle<ConstDataVector<CaloCellContainer> > ( m_outputCDVcellsKey, ctx );

  auto cdv = CxxUtils::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS);
  if(m_fullScanEnabled) {
    ATH_MSG_DEBUG("Getting  ConstDataVector<CaloCellContainer> from " << m_dataAccessSvc.name());
    m_dataAccessSvc->loadFullCollections( ctx, *cdv );
    ATH_MSG_DEBUG("ConstDataVector<CaloCellContainer> size " << (*cdv).size());
  } else {
    ATH_MSG_DEBUG("ROI etaMinus "<< roiDescriptor->etaMinus() << "ROI etaPlus "<< roiDescriptor->etaPlus() << " phiMinus "<< roiDescriptor->phiMinus() << "ROI phiPlus "<< roiDescriptor->phiPlus() );
    LArTT_Selector<LArCellCont> sel;
    m_dataAccessSvc->loadCollections( ctx, *roiDescriptor, TTEM, 2, sel );
    for( const auto cell : sel ) {cdv->push_back( cell ); }
  }

  // FIXME: Need  to convert from the ConstDataVector<CaloCellContainer> returned by the DataAccessSvc to a CaloCellContainer needed by downstream algorithms
  for ( const CaloCell* c : (*cdv)) {
    CaloCell* newcell= new CaloCell(*c);
    caloCellContainer->push_back(newcell);
  }

  // Also record the cdv (but could probably delete it instead).
  auto ss = CDVcellContainer.record( std::move(cdv) );
  ATH_CHECK( ss );
  


  if (caloCellContainer->size() < 1) {
    //return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
    ATH_MSG_WARNING("caloCellContainer->size()  is 0");
    return StatusCode::SUCCESS;
  }

  // Check conversion status
  trigCaloQuality->setPersistencyFlag(false);
  //  trigCaloQuality->setPersistencyFlag(isPersistent);
  trigCaloQuality->setPersistencyFlag(true);
  trigCaloQuality->setError(error);

  // Looping over cell container tools... this could be used for calibrations...
  time_tools.start();
  std::vector<ICaloCellMakerTool*> ::const_iterator itrcont=m_containerTools.begin();
  std::vector<ICaloCellMakerTool*> ::const_iterator endcont=m_containerTools.end();
  
  index=0;
  for (; itrcont!=endcont; ++itrcont) {
    //    if (timerSvc()) m_timer[2+index]->start();
    ATH_MSG_DEBUG("Executing container tool " <<(*itrcont)->name());
    if ( (*itrcont)->process(caloCellContainer.ptr(), ctx).isFailure() ) {
      //msg() << MSG::ERROR << "Error executing tool " << m_containerToolNames[index] << endmsg;
      ATH_MSG_ERROR("Error executing container tool " <<(*itrcont)->name());
    } 
else {
  ATH_MSG_DEBUG("Executed container tool ");
    }
    //    if (timerSvc()) m_timer[2+index]->stop();
    ++index;
  }
  time_tools.stop();

  if ( m_fullScanEnabled) caloCellContainer->updateCaloIterators();


    ATH_MSG_DEBUG(" REGTEST: Produced a Cell Container of Size= " << caloCellContainer->size() );

  //monitoring
   mon_CellContainerSize = (float)caloCellContainer->size();


  if (error) {
    //return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::USERDEF_2);
    return StatusCode::SUCCESS;
  }
  ++m_counter;
  
  return StatusCode::SUCCESS; 
}

