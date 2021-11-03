/*                                                                                                                                                     
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                                                              
*/

#include "TrigT1NSWSimTools/TriggerProcessorTool.h"

namespace NSWL1 {

  TriggerProcessorTool::TriggerProcessorTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type,name,parent) 
  {
  }

  TriggerProcessorTool::~TriggerProcessorTool() { }

  StatusCode TriggerProcessorTool::initialize() {

    ATH_MSG_DEBUG("In initialize()");
    ATH_CHECK(m_trigRdoContainerKey.initialize());
    ATH_CHECK(m_padTriggerContainerKey.initialize());

    return StatusCode::SUCCESS;
  }

  
  StatusCode TriggerProcessorTool::mergeRDO() {

    SG::WriteHandle<Muon::NSW_TrigRawDataContainer> trigRdoContainer(m_trigRdoContainerKey);
    ATH_CHECK(trigRdoContainer.record(std::make_unique<Muon::NSW_TrigRawDataContainer>()));
    ATH_MSG_DEBUG("Recorded NSW_TrigRawDataContainer called " << trigRdoContainer.name() << " in store " << trigRdoContainer.store());
 
    /// retrieve the stgc pad trigger container
    SG::ReadHandle<Muon::NSW_PadTriggerDataContainer> padTriggerContainer(m_padTriggerContainerKey);
    if (!padTriggerContainer.isValid()) {
      ATH_MSG_ERROR("Could not find the STGC Pad Trigger container with name: " << padTriggerContainer.name() << 
		    " in store " << padTriggerContainer.store() );
      return StatusCode::SUCCESS;
    } 


    for ( const Muon::NSW_PadTriggerData* padTriggerData : *padTriggerContainer ) {

      Muon::NSW_TrigRawData* trigRawData = new Muon::NSW_TrigRawData(padTriggerData->sectorID(),padTriggerData->BCID());

      for ( const Muon::NSW_PadTriggerSegment* padTriggerSegment : *padTriggerData) {

	Muon::NSW_TrigRawDataSegment* trigRawDataSegment = new Muon::NSW_TrigRawDataSegment();
	uint8_t bandID = padTriggerSegment->bandID();
	uint8_t phiID  = padTriggerSegment->phiID();
	
	trigRawDataSegment->setRIndex(bandID);
	trigRawDataSegment->setPhiIndex(phiID);
	// set STGC
	trigRawDataSegment->setPhiRes(false);

	trigRawData->push_back(trigRawDataSegment);
	
      }

      trigRdoContainer->push_back(trigRawData);

    } 
    
    return StatusCode::SUCCESS;
  }

  
  
}
