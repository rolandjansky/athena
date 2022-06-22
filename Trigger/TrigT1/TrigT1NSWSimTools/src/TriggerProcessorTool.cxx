/*                                                                                                                                                     
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration                                                                              
*/

#include "TrigT1NSWSimTools/TriggerProcessorTool.h"

namespace NSWL1 {

  TriggerProcessorTool::TriggerProcessorTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type,name,parent) 
  {
  }

  StatusCode TriggerProcessorTool::initialize() {

    ATH_MSG_DEBUG("In initialize()");
    return StatusCode::SUCCESS;
  }

  StatusCode TriggerProcessorTool::mergeRDO(const Muon::NSW_PadTriggerDataContainer* padTriggerContainer,
                                            const Muon::NSW_TrigRawDataContainer* stripTriggerContainer,
                                            const Muon::NSW_TrigRawDataContainer* MMTriggerContainer,
                                            Muon::NSW_TrigRawDataContainer* trigRdoContainer) const {
    ATH_MSG_DEBUG("------------- TriggerProcessorTool::mergeRDO ---------------------");
    ATH_MSG_DEBUG("Pad Trigger Container size: " << padTriggerContainer->size());
    for ( const auto padTriggerData : *padTriggerContainer ) {
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

        trigRawDataSegment->setPhiRes(true); // set sTGC bit
        trigRawDataSegment->setSpare(1);     // set spare bit to 1, to distinguish between strips (default, i.e. 0) and pads
        trigRawData->push_back(trigRawDataSegment);
      }
      ATH_MSG_DEBUG("L1NSW-Pad Trigger Output: "
		    << "sectorSide=" << trigRawData->sectorSide() << " "
		    << "sectorId=" << trigRawData->sectorId() << " "
		    << "bcId=" << trigRawData->bcId());
      for(const auto seg : *trigRawData){
	ATH_MSG_DEBUG("\tPadSegment: "
		      << "deltaTheta=" << static_cast<int16_t>(seg->deltaTheta()) << " "
		      << "phiIndex=" << static_cast<int16_t>(seg->phiIndex()) << " "
		      << "rIndex=" << static_cast<int16_t>(seg->rIndex()) << " "
		      << "spare=" << static_cast<int16_t>(seg->spare()) << " "
		      << "lowRes=" << seg->lowRes() << " "
		      << "phiRes=" << seg->phiRes() << " "
		      << "monitor=" << seg->monitor());
      }
      trigRdoContainer->push_back(trigRawData);
    }
    ATH_MSG_DEBUG("After PadTrigger filling -> NSW Trigger RDO size: " << trigRdoContainer->size());

    for (const auto rawData : *stripTriggerContainer) {
      Muon::NSW_TrigRawData* trigRawData = new Muon::NSW_TrigRawData(*rawData, true);
      ATH_MSG_DEBUG("L1NSW-Strip Trigger Output: "
		    << "sectorSide=" << trigRawData->sectorSide() << " "
		    << "sectorId=" << trigRawData->sectorId() << " "
		    << "bcId=" << trigRawData->bcId());
      for(const auto seg : *trigRawData){
	ATH_MSG_DEBUG("\tStripSegment: "
		      << "deltaTheta=" << static_cast<int16_t>(seg->deltaTheta()) << " "
		      << "phiIndex=" << static_cast<int16_t>(seg->phiIndex()) << " "
		      << "rIndex=" << static_cast<int16_t>(seg->rIndex()) << " "
		      << "spare=" << static_cast<int16_t>(seg->spare()) << " "
		      << "lowRes=" << seg->lowRes() << " "
		      << "phiRes=" << seg->phiRes() << " "
		      << "monitor=" << seg->monitor());
      }
      trigRdoContainer->push_back(trigRawData);
    }
    ATH_MSG_DEBUG("After sTGC strip trigger filling -> NSW Trigger RDO size: " << trigRdoContainer->size());

    for (const auto rawData : *MMTriggerContainer) {
      Muon::NSW_TrigRawData* trigRawData = new Muon::NSW_TrigRawData(*rawData, false);
      ATH_MSG_DEBUG("L1NSW-MM Trigger Output: "
		    << "sectorSide=" << trigRawData->sectorSide() << " "
		    << "sectorId=" << trigRawData->sectorId() << " "
		    << "bcId=" << trigRawData->bcId());
      for(const auto seg : *trigRawData){
	ATH_MSG_DEBUG("\tMMSegment: "
		      << "deltaTheta=" << static_cast<int16_t>(seg->deltaTheta()) << " "
		      << "phiIndex=" << static_cast<int16_t>(seg->phiIndex()) << " "
		      << "rIndex=" << static_cast<int16_t>(seg->rIndex()) << " "
		      << "spare=" << static_cast<int16_t>(seg->spare()) << " "
		      << "lowRes=" << seg->lowRes() << " "
		      << "phiRes=" << seg->phiRes() << " "
		      << "monitor=" << seg->monitor());
      }
      trigRdoContainer->push_back(trigRawData);
    }
    ATH_MSG_DEBUG("After MMTrigger filling -> NSW Trigger RDO size: " << trigRdoContainer->size());

    return StatusCode::SUCCESS;
  }
}
