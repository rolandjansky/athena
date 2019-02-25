/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include <stdint.h>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloEvent/JetElement.h"

#include "IPpmByteStreamSubsetTool.h"
#include "JepByteStreamV2Tool.h"
#include "ITriggerTowerSelectionTool.h"

#include "TrigT1CaloDataAccess.h"

namespace LVL1BS {

// Constructor

TrigT1CaloDataAccess::TrigT1CaloDataAccess(const std::string& type,
                      const std::string& name, const IInterface*  parent)
                    : AthAlgTool(type, name, parent),
 m_robDataProvider("ROBDataProviderSvc/ROBDataProviderSvc", name),
 m_selectionTool("LVL1BS::TriggerTowerSelectionTool/TriggerTowerSelectionTool"),
 m_ppmBSConverter("LVL1BS::PpmByteStreamSubsetTool/PpmByteStreamSubsetTool"),
 m_JetConverter("LVL1BS::JepByteStreamV2Tool/JepByteStreamV2Tool"),
 m_ttCol(0), m_jetCol(0)
{
  declareInterface<ITrigT1CaloDataAccess>(this);

  declareProperty("ROBDataProviderSvc",        m_robDataProvider);
  declareProperty("TriggerTowerSelectionTool", m_selectionTool);
  declareProperty("PpmByteStreamSubsetTool",   m_ppmBSConverter);
  declareProperty("JepByteStreamV2Tool",   m_JetConverter);

}

// Destructor

TrigT1CaloDataAccess::~TrigT1CaloDataAccess()
{
}

// Initialize


StatusCode TrigT1CaloDataAccess::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  // Retrieve data provider service

  StatusCode sc = m_robDataProvider.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Failed to retrieve service " << m_robDataProvider
                    << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved service " << m_robDataProvider << endmsg;

  // Retrieve selection tool

  sc = m_selectionTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_selectionTool << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_selectionTool << endmsg;

  // Retrieve PPM converter tool

  sc = m_ppmBSConverter.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_ppmBSConverter << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_ppmBSConverter << endmsg;

  sc = m_JetConverter.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Failed to retrieve tool " << m_JetConverter << endmsg;
    return sc;
  } else msg(MSG::INFO) << "Retrieved tool " << m_JetConverter << endmsg;

  IIncidentSvc* p_incSvc;
  if ( service("IncidentSvc",p_incSvc, true).isFailure() ) {
          msg(MSG::ERROR) << "Unable to get the IncidentSvc" << endmsg;
  }else{
  // Will use the new event for caching purposes
  p_incSvc->addListener(this, "BeginEvent",100);
  }


  m_robs_full_je.push_back(0x0074000c); m_robs_full_je.push_back(0x0074000d);
  m_robs_full_je.push_back(0x0074001c); m_robs_full_je.push_back(0x0074001d);
  m_robs_full_je.push_back(0x0074002c); m_robs_full_je.push_back(0x0074002d);
  m_robs_full_je.push_back(0x0074003c); m_robs_full_je.push_back(0x0074003d);

  m_ttCol = new DataVector<LVL1::TriggerTower>(SG::VIEW_ELEMENTS);
  // Better reserve the space. Some objects will be used for full
  // unpacking and will be build in the first full event request
  m_ttCol->reserve(5300);
  m_chanIds.reserve(100);
  m_chanIds_full.reserve(7400);
  m_robs.reserve(36);
  m_robs_full.reserve(36);
  // First event selection
  m_jetCol = new DataVector<LVL1::JetElement>;
  m_jetCol->reserve(150);
  m_first=true;

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode TrigT1CaloDataAccess::finalize()
{
  m_chanIds.clear();
  m_chanIds_full.clear();
  m_robs_full_je.clear();
  m_robs_full.clear();
  m_robs.clear();
  m_ttCol->clear(); 
  delete m_ttCol;
  m_jetCol->clear(); 
  delete m_jetCol;
  return StatusCode::SUCCESS;
}

// Return iterators to required trigger towers

StatusCode TrigT1CaloDataAccess::loadCollection(
                     DataVector<LVL1::TriggerTower>::const_iterator& beg,
	             DataVector<LVL1::TriggerTower>::const_iterator& end,
	             const double etaMin, const double etaMax,
	             const double phiMin, const double phiMax, const bool full=true)
{

  beg=end;
  // It is possible to save good time for full scan using special
  // pre-prepared tables. You have to state full=true
  if ( !full ) {  m_chanIds.clear(); m_robs.clear(); }
  // Get PPM sub-block channel IDs for wanted TriggerTowers
  if ( m_first && full )
  m_selectionTool->channelIDs(-5., 5., 0, 2*M_PI, m_chanIds_full);
  if ( ! full )
  m_selectionTool->channelIDs(etaMin, etaMax, phiMin, phiMax, m_chanIds);

  // Get ROB IDs for wanted TriggerTowers
  if ( m_first && full )
  m_selectionTool->robIDs(m_chanIds_full, m_robs_full);
  if ( ! full )
  m_selectionTool->robIDs(m_chanIds, m_robs);

  // Get data
  m_robFrags.clear();

  if ( full ){
  m_robDataProvider->addROBData(m_robs_full);
  m_robDataProvider->getROBData(m_robs_full, m_robFrags);
  }else{
  m_robDataProvider->addROBData(m_robs);
  m_robDataProvider->getROBData(m_robs, m_robFrags);
  }

  // Convert to TriggerTowers
  m_ttCol->clear();
  StatusCode sc;
  if ( full ) sc = m_ppmBSConverter->convert(m_robFrags, m_ttCol, m_chanIds_full);
  else sc = m_ppmBSConverter->convert(m_robFrags, m_ttCol, m_chanIds);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "PPM bytestream conversion failed" << endmsg;
    m_ttCol->clear();
  }
  beg = m_ttCol->begin();
  end = m_ttCol->end();

  if ( full && m_first ) m_first=false;
  return sc;


}

StatusCode TrigT1CaloDataAccess::loadCollection(
                      DataVector<LVL1::JetElement>::const_iterator& beg,
		      DataVector<LVL1::JetElement>::const_iterator& end){

  beg=end;
  if ( m_present_event != m_lC_Jet ){
  // Get data
  m_robFrags.clear();
  m_robDataProvider->addROBData(m_robs_full_je);
  m_robDataProvider->getROBData(m_robs_full_je, m_robFrags);

  m_jetCol->clear();
  StatusCode sc = m_JetConverter->convert(m_robFrags, m_jetCol);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "JET bytestream conversion failed" << endmsg;
    return sc;
  }
  m_lC_Jet = m_present_event;
  }
  beg = m_jetCol->begin();
  end = m_jetCol->end();
  

  return StatusCode::SUCCESS;
}

void TrigT1CaloDataAccess::handle(const Incident & inc ) {
  m_present_event = inc.context().eventID().event_number();
  m_ppmBSConverter->eventNumber(m_present_event);
}



} // end namespace
