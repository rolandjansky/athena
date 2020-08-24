/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <list>
#include <iterator>
#include <sstream>
#include <cmath>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"

#include "TrigLongLivedParticlesHypo/TrigNewLoFHypo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"



class ISvcLocator;

TrigNewLoFHypo::TrigNewLoFHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("AcceptAll",       m_acceptAll=false);
  declareProperty("LoFCellContSize", m_LoFCellContSize=4);

  // Monitored variables...
  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("CellContainerSize", m_CellContainerSize);

}

TrigNewLoFHypo::~TrigNewLoFHypo() {}

HLT::ErrorCode TrigNewLoFHypo::hltInitialize() {

  ATH_MSG_INFO( "in initialize()"  );
  
  m_accepted=0;
  m_rejected=0;
  m_errors=0;
  
  return HLT::OK;
}

HLT::ErrorCode TrigNewLoFHypo::hltFinalize(){

  ATH_MSG_INFO( "in finalize()"  );
  ATH_MSG_INFO( "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors );
  return HLT::OK;

}


HLT::ErrorCode TrigNewLoFHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

  ATH_MSG_DEBUG( " in hltExecute() "  );

  m_cutCounter = -1;

  pass=false;

  //* Get TrigOperationalInfo *//
  std::vector<const TrigOperationalInfo*> vectorOperationalInfo;
  if (getFeatures(outputTE, vectorOperationalInfo, "CellLoFInfo") != HLT::OK) {
    ATH_MSG_WARNING( "Failed to get TrigOperationalInfo"  );
    return HLT::MISSING_FEATURE;
  }
  else {
    ATH_MSG_DEBUG( "Number of TrigOperationalInfo objects: " << vectorOperationalInfo.size()  );
  }

  std::vector<const TrigOperationalInfo*>::const_iterator operationalInfo;
  for (operationalInfo=vectorOperationalInfo.begin(); operationalInfo!=vectorOperationalInfo.end(); ++operationalInfo) {   
    
    if ((*operationalInfo)->defined("CellLoF")==1) {
   
      unsigned int size = (*operationalInfo)->infos().first.size();   
      if (size!=1) {
        ATH_MSG_WARNING( "Too much information from the same TrigOperationalInfo object"  );
        return HLT::NAV_ERROR;
      }
   
      m_numberCells = (*operationalInfo)->get("CellLoF");
   
      ATH_MSG_DEBUG( "Number of calo cells passing LoF quality criteria: " << m_numberCells  );
    }
  }

  bool this_event_is_a_LoF_event = false;
  if(int(m_numberCells) >= m_LoFCellContSize){
    this_event_is_a_LoF_event=true;
  }
  
  //monitoring
  m_CellContainerSize = (float)m_numberCells;

  m_cutCounter = 0;
  //Cut on LoF here 
  if(m_acceptAll || !this_event_is_a_LoF_event) {
    pass = true;
    m_accepted++;
    ATH_MSG_DEBUG( " Not LoF event: accepted!"  );

  } else {
    ATH_MSG_DEBUG( " LoF event: rejected!!"  );
    m_rejected++;
    m_cutCounter=1;
  }

  return HLT::OK;

}
