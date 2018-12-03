/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PartialEventBuildingInfo.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "xAODMuon/MuonContainer.h"


#include "TrigCheckForMuons.h"

using namespace std;

TrigCheckForMuons::TrigCheckForMuons(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    //m_clidSvc("ClassIDSvc", name),
    //m_types(), m_names(), m_momType(), m_clid(),
    m_robSelector("TrigROBSelector", this ),
    m_addCTPResult(1), m_addHLTResult(1),
    m_addOppositePhiRoI(false), m_addOppositeEtaPhiRoI(false),
    m_nRoIs(0),
    m_mindRSqr(0.01*0.01)
{
  declareProperty("muonLabels",  m_muonLabels, "List of labels for xAOD::Muon collection");
  declareProperty("ROBSelector", m_robSelector, "The ROB Selector Tool");
  declareProperty("AddCTPResult", m_addCTPResult, "Add the CTP result to the list of ROBs");
  declareProperty("AddOppositePhiRoI", m_addOppositePhiRoI = false, "Add also RoIs opposite in phi but same eta");
  declareProperty("AddOppositeEtaPhiRoI", m_addOppositeEtaPhiRoI = false, "Add also RoIs opposite in phi and opposite in eta");
  declareProperty("AddHLTResult",  m_addHLTResult,  "Add the HLT result to the list of ROBs");
  declareProperty("etaEdge", m_etaEdge = 5.0, "Upper limit of |eta| range");
  declareProperty("etaWidth", m_etaWidth = 0.1, "Width of the RoI in eta");
  declareProperty("phiWidth", m_phiWidth = 0.1, "Width of the RoI in phi");
  declareProperty("PEBInfoLabel", m_pebLabel = "dunno", "The label under which the PartialEventBuildingInfo object is stored");
  declareProperty("MaxRoIsPerEvent", m_maxRoIsPerEvent = -1);
  declareProperty("extraROBs", m_extraROBs);
  declareProperty("rejectAll", m_rejectAll = false);
  declareProperty("overlapdR"     , m_mindRSqr    = 0.01*0.01  );  


  declareMonitoredStdContainer("nROBs", m_nROBs);
  declareMonitoredStdContainer("detectors", m_dets);
  declareMonitoredVariable("roIEta", m_roIEta);
  declareMonitoredVariable("roIPhi", m_roIPhi);
}


HLT::ErrorCode TrigCheckForMuons::hltInitialize()
{
  msg() << MSG::INFO << "Initializing CPTResult "<<m_addCTPResult
                     << " HLTResult "<< m_addHLTResult<< endmsg;
  if (m_addCTPResult){
    m_trigResults.push_back(eformat::TDAQ_CTP);
    m_trigResults.push_back(eformat::TDAQ_MUON_CTP_INTERFACE);
    msg() << MSG::INFO << " Will add TDAQ_CTP, TDAQ_MUON_CTP_INTERFACE to event record" << endmsg;
  }
  if (m_addHLTResult){
    m_trigResults.push_back(eformat::TDAQ_HLT);
    msg() << MSG::INFO << " Will add TDAQ_HLT to event record " << endmsg;
  }
  return HLT::OK;
}


HLT::ErrorCode TrigCheckForMuons::hltBeginRun()
{
  msg() << MSG::INFO << "in hltBeginRun() " << endmsg;
  return m_robSelector->setupMonitoring();
}


HLT::ErrorCode TrigCheckForMuons::hltFinalize()
{
  /* OI-- Not using this so far 
  if (!m_clidSvc.retrieve().isSuccess()) {
    msg() << MSG::FATAL << "Cannot get ClassIDSvc " << m_clidSvc << endmsg;
    return HLT::FATAL;    
  }
  
  std::vector< std::string> coll = m_collectionTypeName.value();

  m_types.clear();
  m_names.clear();

  for (std::vector<std::string>::const_iterator it = coll.begin(), it_e = coll.end(); it != it_e; it++) {
    std::size_t found = (*it).find_first_of('#');
    
    if (found != std::string::npos)
      {
	m_types.push_back((*it).substr(0,found));
	m_names.push_back((*it).substr(found+1,(*it).size()-found+1));    
	if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "Will look for Collection " << m_types.back() << " with " << m_names.back() << " for PEB stream"<< endmsg;
      }else{
      
      msg() << MSG::FATAL << "Collection type and collection name not configured correctly. " << endmsg;
      return  HLT::BAD_ALGO_CONFIG;
    }
  }
  
  if (m_types.size() != m_names.size()){
    msg() << MSG::FATAL << "Collection type and collection name vectors have different size. " << endmsg;
    return HLT::BAD_ALGO_CONFIG;
  }   

  m_clid.clear();
  
  for (std::vector<std::string>::const_iterator it = m_types.begin(), it_e = m_types.end(); it != it_e; it++) {
    CLID classid;
    if (!m_clidSvc->getIDOfTypeName(*it, classid).isSuccess()){
      msg() << MSG::WARNING << "Cannot convert type. No CLID for " << (*it) << endmsg;      
      return HLT::BAD_ALGO_CONFIG;
    } else {
      int momType = -1;
      const SG::BaseInfoBase* baseInfo = SG::BaseInfoBase::find( classid );
      if( ! baseInfo ){
	msg() << MSG::FATAL << "Please fix me: Cannot convert type. No BaseInfo for " << (*it) << endmsg;      
	return HLT::BAD_ALGO_CONFIG;    
     }

      if( baseInfo->is_base( ClassID_traits< xAOD::IParticleContainer >::ID() ) ) {
	momType = 0;
      } else if( baseInfo->is_base( ClassID_traits< I4MomentumContainer >::ID() ) ) {
	// this is an "old" container, handle it through I4MomentumContainer
	momType = 1;
      } else {
	msg() << MSG::FATAL << "Please fix me: I do not know how to extract eta/phi for this class  " << (*it) << endmsg;  
	return HLT::BAD_ALGO_CONFIG;    
      }

      m_clid.push_back(classid);
      m_momType.push_back(momType);
      if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "CLID "<< classid << " is OK, accept for PEB " << endmsg;

    }
  }
  */
  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigCheckForMuons::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output)
{
    
  bool debug = msg().level()<=MSG::DEBUG;

  m_nRoIs++;
  if( debug ){
    msg() << MSG::DEBUG << "Executing this TrigCheckForMuons " ;
    if (m_maxRoIsPerEvent > -1) msg() << " RoI " << m_nRoIs << "/" << m_maxRoIsPerEvent;
    msg() << endmsg;
  }

  if (m_maxRoIsPerEvent > -1 && m_nRoIs > m_maxRoIsPerEvent) {
    if( debug )
      msg() << MSG::DEBUG << "RoI limit ("  << m_maxRoIsPerEvent 
	    << ") reached for this event : will not process this RoI" << endmsg;
    return HLT::OK;
  }

  // Get Objects

  //HLT::ErrorCode status = HLT::BAD_ALGO_CONFIG;


  // create new partial EB directive
  PartialEventBuildingInfo* pebInfo = config()->getPEBI();
  if(!pebInfo){
    if( debug )
      msg() << MSG::DEBUG << "*** Not Executing this TrigCheckForMuons " << name() << ", not a calib chain" << endmsg;
    return HLT::OK;
  }


  //--------- Get list of eta/phi of interesting objects ---------- 

  // FIXME : can trace number of recorded muons per event and only add new once

  ElementLinkVector<xAOD::MuonContainer> elvmuon;
  std::vector<ElementLink<xAOD::MuonContainer> > muonVec;

  for( auto mLabel : m_muonLabels ){
    bool foundLabel = false;
    size_t nMu0 = muonVec.size();
    size_t nMu1 = 0;
    for( auto TEvec : inputTE)
      for( auto oneTE : TEvec ){
	// use getFeatureLinks instead of "Link" to get them all recursively, otherwise Bphys topo will stop search
	HLT::ErrorCode status = getFeaturesLinks<xAOD::MuonContainer,xAOD::MuonContainer>(oneTE, elvmuon);
	if(status==HLT::OK ) {
	  foundLabel = true;
	  bool found = false;
	  nMu1 += elvmuon.size();
	  for( const auto & efmu : elvmuon ){
	    if( found ) break;
	    for(const auto& part : muonVec ){
	       if( (*part)->charge() *  (*efmu)->charge() < 0 ) continue;
	       double deta = (*part)->eta() - (*efmu)->eta();
	       double dphi = (*part)->phi() - (*efmu)->phi();
	       double deltaR2 = deta*deta +dphi*dphi;
	       if( deltaR2 <= m_mindRSqr){
		 found = true; 
		 if(debug) msg() << MSG::DEBUG << " SKIP (Already have) muon with pt="<<(*efmu)->pt() << " eta="
				 << (*efmu)->eta() <<  " phi=" << (*efmu)->phi() << " charge="<< (*efmu)->charge() << endmsg;
		 break;
	       }
	    } // loop over muonVec	    
	    if( !found ){
	      muonVec.push_back(efmu);
	      if(debug) msg() << MSG::DEBUG << " Found muon pt="<<(*efmu)->pt() << " eta="
			      << (*efmu)->eta() <<  " phi=" << (*efmu)->phi() << " charge="<< (*efmu)->charge() << endmsg;
	    }
	  }// end loop over muons
	  if(debug){
	    if( !foundLabel )  msg() << " No collection with label "<< mLabel << " is found " << endmsg;
	    else
	      msg() << MSG::DEBUG << " Found "<< nMu1 <<   " muons with label "<< mLabel
		  <<" recorded "<< muonVec.size() - nMu0 << endmsg; 
	  }
	  
	}
      }// end loop over TEs
  } // end loop over labels
  

  for(const auto& muon : muonVec ){
	
    m_roIEta = (*muon)->eta();
    m_roIPhi = (*muon)->phi();

	if (debug) msg() << MSG::DEBUG << "Processing muon " << m_roIEta << " " << m_roIPhi << endmsg;

	double etaMin = std::max(-m_etaEdge,m_roIEta - m_etaWidth);
	double etaMax = std::min( m_etaEdge,m_roIEta + m_etaWidth);
  
	double phiMin = m_roIPhi - m_phiWidth;
	double phiMax = m_roIPhi + m_phiWidth;
	while (phiMin < 0)      phiMin += 2*M_PI;
	while (phiMax > 2*M_PI) phiMax -= 2*M_PI;  // AH HA!! This is wrong! Need in the range -pi .. pi
  
	TrigRoiDescriptor _roi( m_roIEta, etaMin, etaMax, m_roIPhi, phiMin, phiMax );

	// now add ROBs
	HLT::ErrorCode ec = m_robSelector->fillPEBInfo(*pebInfo, _roi, &m_dets, &m_nROBs);
	if (ec != HLT::OK) {
	  if (debug) msg() << MSG::DEBUG << "Failed to record PEB " << endmsg;
	return ec;
	}

	if( m_addOppositePhiRoI || m_addOppositeEtaPhiRoI) {
	  m_roIPhi_2 = (*muon)->phi();
	  while (m_roIPhi_2 < 0)      m_roIPhi_2 += 2*M_PI;
	  while (m_roIPhi_2 > 2*M_PI) m_roIPhi_2 -= 2*M_PI; 

	  double phiMin_2 =( m_roIPhi_2 + M_PI) - m_phiWidth; //Making a few redefintions of phi to look at muons produced in the oppoisite direction. Keeping eta the same, but changing phi.
	  double phiMax_2 = (m_roIPhi_2 + M_PI) + m_phiWidth;
	  while (phiMin_2 < 0)      phiMin_2 += 2*M_PI;
	  while (phiMax_2 > 2*M_PI) phiMax_2 -= 2*M_PI; 
	
	  if( m_addOppositePhiRoI ) { //To now look for muons in the opposite phi direction, create a second TrigRoiDescriptor
	    TrigRoiDescriptor _roi2( m_roIEta, etaMin, etaMax, m_roIPhi_2 + M_PI, phiMin_2, phiMax_2 );

	    // now add ROBs
	    ec = m_robSelector->fillPEBInfo(*pebInfo, _roi2, &m_dets, &m_nROBs);
	    if (ec != HLT::OK) {
	      if (debug) msg() << MSG::DEBUG << "Failed to record PEB for _roi2" << endmsg;
	      return ec;
	    }
	  }
	  if( m_addOppositeEtaPhiRoI ) {//To now look for muons in the opposite eta and phi direction, 
	    double etaMin_2 = std::max(-m_etaEdge,-m_roIEta - m_etaWidth);
	    double etaMax_2 = std::min( m_etaEdge,-m_roIEta + m_etaWidth);
	    TrigRoiDescriptor _roi2( m_roIEta, etaMax_2, etaMin_2, m_roIPhi_2 + M_PI, phiMin_2, phiMax_2 ); 

	    // now add ROBs
	    ec = m_robSelector->fillPEBInfo(*pebInfo, _roi2, &m_dets, &m_nROBs);
	    if (ec != HLT::OK) {
	      if (debug) msg() << MSG::DEBUG << "Failed to record PEB for _roi2" << endmsg;
	      return ec;
	    }
	  }
	}


	if (debug) msg() << MSG::DEBUG << "Recorded " << m_nROBs.back() << " ROBs " << endmsg;


      } //  end loop over muons

  // Add extra ROBs
    for (std::vector<uint32_t>::const_iterator rob = m_extraROBs.begin(); rob != m_extraROBs.end(); rob++)
    pebInfo->add(*rob);

  // Add trig results as specified in properties
  pebInfo->addSubDetector(m_trigResults);
  
  if(! m_rejectAll){  // Flag used for commissioning
    HLT::TriggerElement* te = addRoI(output);
    te->setActiveState(true);  
  }
  return HLT::OK;
}


HLT::ErrorCode TrigCheckForMuons::hltEndEvent()
{
  m_robSelector->reset();
  m_dets.clear();
  m_nROBs.clear();
  m_nRoIs = 0;
  return HLT::OK;
}


