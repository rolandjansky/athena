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
    return StatusCode::SUCCESS;
  }

  StatusCode TriggerProcessorTool::mergeRDO(const Muon::NSW_PadTriggerDataContainer* padTriggerContainer,
                                            const Muon::NSW_TrigRawDataContainer* MMTriggerContainer,
                                            Muon::NSW_TrigRawDataContainer* trigRdoContainer) {

    ATH_MSG_DEBUG("Pad Trigger Container size: " << padTriggerContainer->size());
    for ( const Muon::NSW_PadTriggerData* padTriggerData : *padTriggerContainer ) {
      ATH_MSG_DEBUG("Pad Trigger data: " << *padTriggerData);

      char sectorSide = (padTriggerData->endcap() == Muon::NSW_PadTriggerData::Endcap::A) ? 'A' : 'C';
      auto sectorID = (padTriggerData->sectorSize() == Muon::NSW_PadTriggerData::SectorSize::SMALL) ? padTriggerData->sectorID()*2-1 : padTriggerData->sectorID()*2-2;
      Muon::NSW_TrigRawData* trigRawData = new Muon::NSW_TrigRawData(sectorID, sectorSide, padTriggerData->BCID());
      for ( const Muon::NSW_PadTriggerSegment* padTriggerSegment : *padTriggerData) {
        ATH_MSG_DEBUG("Pad Trigger segment: " << *padTriggerSegment);

        Muon::NSW_TrigRawDataSegment* trigRawDataSegment = new Muon::NSW_TrigRawDataSegment();
        uint8_t bandID = padTriggerSegment->bandID();
        uint8_t phiID  = padTriggerSegment->phiID();
        trigRawDataSegment->setRIndex(bandID);
        trigRawDataSegment->setPhiIndex(phiID);

        // set STGC
        trigRawDataSegment->setPhiRes(true);
        trigRawData->push_back(trigRawDataSegment);
      }
      trigRdoContainer->push_back(trigRawData);
    }
    ATH_MSG_DEBUG("After PadTrigger filling -> NSW Trigger RDO size: " << trigRdoContainer->size());

    for (const auto rawData : *MMTriggerContainer) {
      Muon::NSW_TrigRawData* trigRawData = new Muon::NSW_TrigRawData(*rawData);
      trigRdoContainer->push_back(trigRawData);
    }
    ATH_MSG_DEBUG("After MMTrigger filling -> NSW Trigger RDO size: " << trigRdoContainer->size());

    return StatusCode::SUCCESS;
  }
}
