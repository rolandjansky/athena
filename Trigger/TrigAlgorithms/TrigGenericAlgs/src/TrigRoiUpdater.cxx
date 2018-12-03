/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#include "TrigRoiUpdater.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "RoiDescriptor/RoiDescriptor.h"
#include "IRegionSelector/RoiUtil.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"

namespace PESA
{

  TrigRoiUpdater::TrigRoiUpdater(const std::string &name, 
				 ISvcLocator *pSvcLocator)
    : HLT::FexAlgo (name, pSvcLocator),
      m_etaHalfWidth(0.),
      m_phiHalfWidth(0.),
      m_zHalfWidth(0.),
      m_regionSelector("RegSelSvc", name),
      m_requestPIXRobs(true),
      m_requestSCTRobs(true),
      m_monitorDuplicateRoIs(true),
      m_invocations(0),
      m_duplicateRoIs(0)
  {
    declareProperty("EtaHalfWidth",           m_etaHalfWidth);
    declareProperty("PhiHalfWidth",           m_phiHalfWidth);
    declareProperty("RegionSelectorTool",     m_regionSelector);
    declareProperty("RequestPIXRobs",         m_requestPIXRobs);
    declareProperty("RequestSCTRobs",         m_requestSCTRobs);
    declareProperty("MonitorDuplicateRoIs",   m_monitorDuplicateRoIs);

    m_inpPhiMinus = m_inpPhiPlus = m_inpPhiSize = 0.;
    m_inpEtaMinus = m_inpEtaPlus = m_inpEtaSize = 0.;
    m_PhiMinus = m_PhiPlus = m_PhiSize = 0.;
    m_EtaMinus = m_EtaPlus = m_EtaSize = 0.;

  }

  TrigRoiUpdater::~TrigRoiUpdater()
  {}

  //----------------------------------
  //          beginRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TrigRoiUpdater::hltBeginRun() {
    ATH_MSG_DEBUG("TrigRoiUpdater::beginRun()");

    return HLT::OK;
  }
  //----------------------------------------------------------------------------

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigRoiUpdater::hltInitialize() {

    //signature specific modifications
    const std::string instanceName = name();
    
    if (instanceName.find("Cosmics") !=std::string::npos){
      m_zHalfWidth = 1000.;
      ATH_MSG_INFO ("Instance " << instanceName << " z range modification: " << m_zHalfWidth);
    } else if (instanceName.find("BeamSpot") !=std::string::npos){
      m_zHalfWidth = 175.;
      ATH_MSG_INFO ("Instance " << instanceName << " z range modification: " << m_zHalfWidth);
    }


    IIncidentSvc* pIncsvc;
    if ( serviceLocator()->service("IncidentSvc", pIncsvc).isSuccess() ) {
      int priority = 100;
      pIncsvc->addListener( this, "BeginEvent", priority);
    } else {
      msg(MSG::ERROR) << "No connection to incidentSvc used for cleanup" 
                      << endmsg;
      return StatusCode::FAILURE;
    }

    // Retrieving Region Selector Tool:
    if ( (m_requestPIXRobs || m_requestSCTRobs) && m_regionSelector.retrieve().isFailure() ) {
      ATH_MSG_FATAL( m_regionSelector.propertyName()
		     << " : Unable to retrieve RegionSelector tool "  
		     << m_regionSelector.type() );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    return HLT::OK;
  }


  void TrigRoiUpdater::handle(const Incident& inc) {
    if (inc.type() == "BeginEvent") {
      //cleanup stored RoIs
      m_rois.clear();
    }
  }

  ///////////////////////////////////////////////////////////////////
  // Execute HLT Algorithm
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigRoiUpdater::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {
 
    ATH_MSG_DEBUG("execHLTAlgorithm()");

    const TrigRoiDescriptor *roi = 0;
    bool forIDfound=false;
    bool updateNeeded=true;
    bool attach_forID=true;
    
    m_inpPhiMinus = m_inpPhiPlus = m_inpPhiSize = -10.;
    m_inpEtaMinus = m_inpEtaPlus = m_inpEtaSize = -10.;
    m_PhiMinus = m_PhiPlus = m_PhiSize = -10.;
    m_EtaMinus = m_EtaPlus = m_EtaSize = -10.;

    std::vector<std::string> roiNames = {"forID3","forID2","forID1", "forID", ""};
    std::string roiName= "";

    for (size_t k=0; k<roiNames.size(); k++){
      if (HLT::OK != getFeature(outputTE, roi, roiNames[k]) || !roi){
	ATH_MSG_DEBUG("|"  << roiNames[k] << "| not found or zero"); 
      } else {
	roiName = roiNames[k-1>0 ? k-1 : 0];
	ATH_MSG_DEBUG("RoI with label |" << roiNames[k] << "| found and " << roiName << " will be used for an update"); 
	if (roiName.find("forID1")!=std::string::npos) forIDfound = true;
	break;
      }
    }

    if (roi->composite()){
      if (m_requestPIXRobs || m_requestSCTRobs){
	registerROBs(roi);
      }

      ATH_MSG_DEBUG("Not touching a composite RoI");
      updateNeeded = false;
    }

    //signature specific modifications
    const std::string instanceName = name();

    if (forIDfound && instanceName.find("IDTrigRoiUpdater_Muon_")!=std::string::npos){
      ATH_MSG_DEBUG("don't update roiDecriptor from muFast");
      updateNeeded = false;
    }

    if (instanceName.find("Bjet")!=std::string::npos || instanceName.find("bjet")!=std::string::npos){
      ATH_MSG_DEBUG("don't use fixed RoI halfwidths for bjets");
      updateNeeded = false;
      if (instanceName.find("BjetVtx")!=std::string::npos){
	attach_forID=false;	
      }
    }
    
    TrigRoiDescriptor *outroi = 0;

    if (updateNeeded) {
      m_inpPhiMinus= roi->phiMinus(); m_inpPhiPlus = roi->phiPlus();  m_inpPhiSize= m_inpPhiPlus - m_inpPhiMinus;
      m_inpEtaMinus= roi->etaMinus(); m_inpEtaPlus = roi->etaPlus();  m_inpEtaSize= m_inpEtaPlus - m_inpEtaMinus;
      
      float eta = roi->eta();
      float phi = roi->phi();
      
      float oldEtaW = m_inpEtaPlus - m_inpEtaMinus;
      float oldPhiW = m_inpPhiPlus - m_inpPhiMinus;
      
      if (  m_inpPhiPlus < m_inpPhiMinus ) oldPhiW += 2*M_PI;
      
      float diff_eta = 0.5*oldEtaW - m_etaHalfWidth;
      float diff_phi = 0.5*oldPhiW - m_phiHalfWidth;
      
      float zedm(0.), zedp(0.);
      zedm = roi->zedMinus()-m_zHalfWidth;
      zedp = roi->zedPlus()+m_zHalfWidth;
      if (instanceName.find("IDTrigRoiUpdater_BeamSpot_IDTrig")!=std::string::npos){
	zedm = (roi->zedPlus()+roi->zedMinus())/2.-m_zHalfWidth;
	zedp = (roi->zedPlus()+roi->zedMinus())/2.+m_zHalfWidth;
      }

      
      outroi =  new TrigRoiDescriptor(roi->roiWord(), roi->l1Id(), roi->roiId(),
				      eta, m_inpEtaMinus+diff_eta, m_inpEtaPlus-diff_eta,
				      phi, m_inpPhiMinus+diff_phi, m_inpPhiPlus-diff_phi,
				      roi->zed(), zedm, zedp);
    }
    else {
      outroi = new TrigRoiDescriptor(*roi);
    }

    //optional RoI monitoring
    bool thesameroi = false;

    ATH_MSG_DEBUG("Input RoI " << *roi);

    if (attach_forID){
      if ( HLT::OK !=  attachFeature(outputTE, outroi, roiName) ) {
	ATH_MSG_ERROR("Could not attach feature to the TE");
	return HLT::NAV_ERROR;
      }
      else {
	ATH_MSG_DEBUG("REGTEST attached RoI " << roiName << *outroi);
      }
    }

    if (m_requestPIXRobs || m_requestSCTRobs){
      registerROBs(outroi);
    }


    //check whether we are attaching the same RoI again (this execution could be cache)
    if (m_monitorDuplicateRoIs){
      for (auto it = m_rois.begin(); it != m_rois.end(); it++) {
	TrigRoiDescriptor r = (*it);
	if (r == *outroi) {
	  ATH_MSG_DEBUG("This RoI was already processed by the same instance of ID tracking");
	  ATH_MSG_DEBUG(*outroi);
	  ATH_MSG_DEBUG(r);
	  thesameroi=true;
	} 
      }

      if (!thesameroi){
	outroi->manageConstituents(false);
	m_rois.push_back(*outroi);
	//	m_rois.back().manageConstituents(false);
	ATH_MSG_DEBUG("Registering RoI " << m_rois.back());
      } 
      else {
	m_duplicateRoIs++;
      }
    }

    if (!attach_forID){  //if the outroi is not attached to the navigation
      delete outroi;
    }

    m_invocations++;
    return HLT::OK;
  }

  //////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////
      
  HLT::ErrorCode TrigRoiUpdater::hltFinalize() {

    ATH_MSG_DEBUG("finalize() success");
    ATH_MSG_INFO("Invoked " << m_invocations << " with " << m_duplicateRoIs << " duplicate RoIs processed ");
    return HLT::OK;
  }

  //----------------------------------
  //          endRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TrigRoiUpdater::hltEndRun() {
                                 
    ATH_MSG_DEBUG("TrigRoiUpdater::endRun()");
                                        
    return HLT::OK;
  }

  //---------------------------------------------------------------------------

  HLT::ErrorCode TrigRoiUpdater::registerROBs(const TrigRoiDescriptor *roi){

    if (roi == 0) {
      ATH_MSG_WARNING( "REGTEST / Failed to find RoiDescriptor " );
      return HLT::NAV_ERROR;
    }

    ATH_MSG_DEBUG( "REGTEST registerROBs" << *roi );

    std::vector<unsigned int> uIntListOfRobs;

    if (m_requestPIXRobs) {
      m_regionSelector->DetROBIDListUint( PIXEL, *roi, uIntListOfRobs );

      ATH_MSG_DEBUG( "list of ROBs ID in PIX: " );
      for(uint i_lid(0); i_lid<uIntListOfRobs.size(); i_lid++)
	ATH_MSG_DEBUG( "0x" << std::hex << uIntListOfRobs.at(i_lid) << std::dec );

      config()->robRequestInfo()->addRequestScheduledRobIDs( uIntListOfRobs );
      uIntListOfRobs.clear();
    }

    if (m_requestSCTRobs){
      m_regionSelector->DetROBIDListUint( SCT, *roi, uIntListOfRobs );

      ATH_MSG_DEBUG( "list of ROBs ID in SCT: " );
      for(uint i_lid(0); i_lid<uIntListOfRobs.size(); i_lid++)
	ATH_MSG_DEBUG( "0x" << std::hex << uIntListOfRobs.at(i_lid) << std::dec );

      config()->robRequestInfo()->addRequestScheduledRobIDs( uIntListOfRobs );
    }

    return HLT::OK;
  }

} // end namespace
