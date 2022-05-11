/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibSvc/MdtCalibrationT0ShiftTool.h"

// TRandom3 for random smearing
// should later be replaced with std
#include "TRandom3.h"

MdtCalibrationT0ShiftTool::MdtCalibrationT0ShiftTool(const std::string& type, const std::string& name, const IInterface* parent) :
  MdtCalibrationShiftMapBase(type, name, parent) {
}

StatusCode MdtCalibrationT0ShiftTool::initializeMap()
{
  if (m_mapIsInitialized) {
    ATH_MSG_WARNING("Map already initalized. Multiple calls of initalizeMap should not happen.");
    return StatusCode::SUCCESS;
  }

  /* First try loading the map from its default location */
  if (!m_forceMapRecreate) {
    ATH_CHECK(loadMapFromFile());
    return StatusCode::SUCCESS;
  }

  /* map was not found as a file or needs to be recreated */
  // TODO: What if multiple jobs started in parallel?
  //       (DataRace on MapFile because of parallel jobs)

  /* initialize random number generator that creates the shift values */
  TRandom3 rng(/*seed*/ 20120704);

  SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_mdtCab, Gaudi::Hive::currentContext()};
  const MuonMDT_CablingMap* mdtCabling{*readHandle};
  if(!mdtCabling){
    ATH_MSG_ERROR("Null pointer to the MDT cabling conditions object");
    return StatusCode::FAILURE;
  }
  
  for (const auto& onl_cab : mdtCabling->getOnlineConvMap()) {
      MuonMDT_CablingMap::CablingData cabling_data{};
      cabling_data.MdtCablingOnData::operator=(onl_cab.first);
      /// Loop over all tdc channels
      for (const auto& tdc : onl_cab.second) {
          if (!tdc) continue;
          cabling_data.tdcId = tdc->moduleId();          
          for (cabling_data.channelId = 0; cabling_data.channelId < 24; ++cabling_data.channelId) {
             /* Get the offline ID, given the current detector element */
            if (!mdtCabling->getOfflineId(cabling_data, msgStream())) {
              ATH_MSG_WARNING(cabling_data);
              continue;
            }
            Identifier channelIdentifier;
            bool isValid  = mdtCabling->convert(cabling_data,channelIdentifier);
            // this debug msg can be removed eventually
            ATH_MSG_DEBUG("Trying to set from online IDs "<<
                          std::endl<<
                          cabling_data<< 
                          " .. with channelID"<<
                          std::endl << 
                          "  Identifier    " << 
                          channelIdentifier <<std::endl<<"  Identifier32  " << channelIdentifier.get_identifier32());

            if (!isValid) {
              ATH_MSG_FATAL(cabling_data<<" --- Conversion to Identifier is NOT valid.");
              return StatusCode::FAILURE;
            }

            double rn = rng.Gaus(m_centralValue, m_sigma);
            // Fatal if entry exists
            if (m_shiftValues.find(channelIdentifier) != m_shiftValues.end()) {
              ATH_MSG_FATAL("Double counting in initialization of map");
              return StatusCode::FAILURE;
            }
            m_shiftValues[channelIdentifier] = rn;
          
          }
      }
  }
  /* initalization was successful */
  m_mapIsInitialized = true;

  /* write map to the default location */
  ATH_CHECK( dumpMapAsFile() );

  return StatusCode::SUCCESS;
}
