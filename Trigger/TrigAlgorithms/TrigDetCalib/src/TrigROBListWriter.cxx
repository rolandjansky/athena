/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PartialEventBuildingInfo.h"
#include "IRegionSelector/IRegSelSvc.h"

#include "TrigROBListWriter.h"

TrigROBListWriter::TrigROBListWriter(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator),
    m_robSelector("TrigROBSelector", this ),
    m_addCTPResult(0), m_addL2Result(0), m_addEFResult(0),
    m_nRoIs(0)
{
  declareProperty("ROBSelector", m_robSelector, "The ROB Selector Tool");
  declareProperty("AddCTPResult", m_addCTPResult, "Add the CTP result to the list of ROBs");
  declareProperty("AddL2Result",  m_addL2Result,  "Add the L2 result to the list of ROBs");
  declareProperty("AddEFResult",  m_addEFResult,  "Add the EF result to the list of ROBs");
  declareProperty("etaEdge", m_etaEdge = 5.0, "Upper limit of |eta| range");
  declareProperty("etaWidth", m_etaWidth = 0.1, "Width of the RoI in eta");
  declareProperty("phiWidth", m_phiWidth = 0.1, "Width of the RoI in phi");
  declareProperty("PEBInfoLabel", m_pebLabel = "dunno", "The label under which the PartialEventBuildingInfo object is stored");
  declareProperty("RoIDescriptorLabels", m_roILabels, "The labels used to search for RoI features");
  declareProperty("MaxRoIsPerEvent", m_maxRoIsPerEvent = -1);
  declareProperty("extraROBs", m_extraROBs);
  declareProperty("rejectAll", m_rejectAll = false);

  declareMonitoredStdContainer("nROBs", m_nROBs);
  declareMonitoredStdContainer("detectors", m_dets);
  declareMonitoredVariable("roIEta", m_roIEta);
  declareMonitoredVariable("roIPhi", m_roIPhi);
}


HLT::ErrorCode TrigROBListWriter::hltInitialize()
{
  if (m_addCTPResult) m_trigResults.push_back(eformat::TDAQ_CTP);
  if (m_addL2Result)  m_trigResults.push_back(eformat::TDAQ_LVL2);
  if (m_addEFResult)  m_trigResults.push_back(eformat::TDAQ_EVENT_FILTER);

  return HLT::OK;
}


HLT::ErrorCode TrigROBListWriter::hltBeginRun()
{
  msg() << MSG::INFO << "in hltBeginRun() " << endreq;
  return m_robSelector->setupMonitoring();
}


HLT::ErrorCode TrigROBListWriter::hltFinalize()
{
  return HLT::OK;
}


HLT::ErrorCode TrigROBListWriter::hltExecute(const HLT::TriggerElement* te_in,
						   HLT::TriggerElement* te_out)
{
  m_nRoIs++;
  msg() << MSG::DEBUG << "Executing this TrigROBListWriter " << name() << " for types "
	<< te_in->getId() << " -> " << te_out->getId();
  if (m_maxRoIsPerEvent > -1) msg() << " RoI " << m_nRoIs << "/" << m_maxRoIsPerEvent;
  msg() << endreq;
  
  if (m_maxRoIsPerEvent > -1 && m_nRoIs > m_maxRoIsPerEvent) {
    msg() << MSG::DEBUG << "RoI limit ("  << m_maxRoIsPerEvent 
          << ") reached for this event : will not process this RoI" << endreq;
    return HLT::OK;
  }

  // Get RoI descriptor
  const TrigRoiDescriptor* roIDescriptor = 0;
  HLT::ErrorCode status = HLT::BAD_ALGO_CONFIG;
  
  for (std::vector<std::string>::const_iterator label = m_roILabels.begin();
       label != m_roILabels.end(); label++) {
    status = getFeature(te_out, roIDescriptor, *label);
    if (status == HLT::OK && roIDescriptor) break;
  }

  if (status != HLT::OK || !roIDescriptor) {
    msg() << MSG::ERROR << "Could not find RoI descriptor - labels checked : "; 
    for (std::vector<std::string>::const_iterator label = m_roILabels.begin();
         label != m_roILabels.end(); label++) 
      msg() << *label << " ";
    msg()<< endreq;
    return status;
  }

  // from dummyAlgoForCalibration
  // create new partial EB directive
  PartialEventBuildingInfo* pebInfo = config()->getPEBI();
  if(!pebInfo){
    msg() << MSG::DEBUG << "*** Not Executing this TrigROBListWriter " << name() << ", not a calib chain" << endreq;
    return HLT::OK;
  }
  /*PartialEventBuildingInfo* pebInfo = 0;
  if (store()->transientContains<PartialEventBuildingInfo>(m_pebLabel))
    store()->retrieve(pebInfo, m_pebLabel).ignore();
  else {
    pebInfo = new PartialEventBuildingInfo();
    store()->record(pebInfo, m_pebLabel, true).ignore();
  }*/

  // Some monitoring

  m_roIEta = roIDescriptor->eta();
  m_roIPhi = roIDescriptor->phi();

  //  bool createRoI = false;
  // if ( roIDescriptor->eta()==roIDescriptor->etaMinus() &&  roIDescriptor->eta()==roIDescriptor->etaPlus() &&
  //     roIDescriptor->phi()==roIDescriptor->phiMinus() &&  roIDescriptor->phi()==roIDescriptor->phiPlus() ) {

  double etaMin = std::max(-m_etaEdge,roIDescriptor->eta() - m_etaWidth);
  double etaMax = std::min( m_etaEdge,roIDescriptor->eta() + m_etaWidth);
  
  double phiMin = roIDescriptor->phi() - m_phiWidth;
  double phiMax = roIDescriptor->phi() + m_phiWidth;
  while (phiMin < 0)      phiMin += 2*M_PI;
  while (phiMax > 2*M_PI) phiMax -= 2*M_PI;  // AH HA!! This is wrong! Need in the range -pi .. pi
  
  TrigRoiDescriptor roi( roIDescriptor->eta(), etaMin, etaMax, roIDescriptor->phi(), phiMin, phiMax );

  // now add ROBs
  HLT::ErrorCode ec = m_robSelector->fillPEBInfo(*pebInfo, roi, &m_dets, &m_nROBs);
  if (ec != HLT::OK) return ec;

  // Add extra ROBs
  for (std::vector<uint32_t>::const_iterator rob = m_extraROBs.begin(); rob != m_extraROBs.end(); rob++)
    pebInfo->add(*rob);

  // Add trig results as specified in properties
  pebInfo->addSubDetector(m_trigResults);
  
  if(m_rejectAll)  // Flag used for commissioning
     te_out->setActiveState(false);
  else
     te_out->setActiveState(true);
  return HLT::OK;
}


HLT::ErrorCode TrigROBListWriter::hltEndEvent()
{
  m_robSelector->reset();
  m_dets.clear();
  m_nROBs.clear();
  m_nRoIs = 0;
  return HLT::OK;
}
