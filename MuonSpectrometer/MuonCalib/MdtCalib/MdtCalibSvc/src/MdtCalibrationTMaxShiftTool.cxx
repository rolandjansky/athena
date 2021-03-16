/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibSvc/MdtCalibrationTMaxShiftTool.h"

// TRandom2 for random smearing
// should later be replaced with std
#include "TRandom2.h"

MdtCalibrationTMaxShiftTool::MdtCalibrationTMaxShiftTool(const std::string& type, const std::string& name, const IInterface* parent) :
  MdtCalibrationShiftMapBase(type, name, parent),
  m_tUpper(688.1818) {
}

StatusCode MdtCalibrationTMaxShiftTool::initializeMap() {
  if (m_mapIsInitialized) {
    ATH_MSG_WARNING(
        "Map already initalized. Multiple calls of initalizeMap "
        "should not happen.");
    return StatusCode::SUCCESS;
  }

  /* First try loading the map from its default location */
  if (!m_forceMapRecreate) {
    ATH_CHECK(loadMapFromFile());
    return StatusCode::SUCCESS;
  }

  /* map was not found as a file */
  // TODO: What if multiple jobs started in parallel?
  //       (DataRace on MapFile because of parallel jobs)

  /* initialize random number generator that creates the shift values */
  TRandom2 rng(/*seed*/ 20160211);

  SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_mdtCab, Gaudi::Hive::currentContext()};
  const MuonMDT_CablingMap* mdtCabling{*readHandle};
  if(!mdtCabling){
    ATH_MSG_ERROR("Null pointer to the MDT cabling conditions object");
    return StatusCode::FAILURE;
  }

  /* Get ROBs */
  std::vector<uint32_t> robVector = mdtCabling->getAllROBId();

  MuonMDT_CablingMap::MapOfItems *listOfSubdet;
  MuonMDT_CablingMap::MapOfItems::const_iterator it_sub;

  MdtSubdetectorMap::MapOfItems *listOfROD;
  MdtSubdetectorMap::MapOfItems::const_iterator it_rod;

  MdtRODMap::MapOfItems *listOfCsm;
  MdtRODMap::MapOfItems::const_iterator it_csm;

  MdtCsmMap::MapOfItems *listOfAmt;
  MdtCsmMap::MapOfItems::const_iterator it_amt;

  listOfSubdet = mdtCabling->getListOfElements();

  int subdetectorId, rodId, csmId, amtId;
  int stationName, stationEta, stationPhi, multiLayer, layer, tube;

  for (it_sub = listOfSubdet->begin(); it_sub != listOfSubdet->end();
       ++it_sub) {
    subdetectorId = it_sub->first;
    listOfROD = it_sub->second->getListOfElements();

    for (it_rod = listOfROD->begin(); it_rod != listOfROD->end(); ++it_rod) {
      rodId = it_rod->first;
      listOfCsm = it_rod->second->getListOfElements();

      for (it_csm = listOfCsm->begin(); it_csm != listOfCsm->end(); ++it_csm) {
        csmId = it_csm->first;
        listOfAmt = it_csm->second->getListOfElements();

        for (it_amt = listOfAmt->begin(); it_amt != listOfAmt->end();
             ++it_amt) {
          amtId = it_amt->first;
          amtId = it_amt->second->moduleId();

          for (int tubeId = 0; tubeId < 24; ++tubeId) {
            /* Get the offline ID, given the current detector element */
            if (!mdtCabling->getOfflineId(
                    subdetectorId, rodId, csmId, amtId, tubeId, stationName,
                    stationEta, stationPhi, multiLayer, layer, tube)) {
              std::ostringstream ss;
              ss << "Trying to initialize non-existing tube with\n";
              ss << "  subdetectorId " << subdetectorId << "\n";
              ss << "  rodId         " << rodId << "\n";
              ss << "  csmId         " << csmId << "\n";
              ss << "  amtId         " << amtId << "\n";
              ss << "  tubeId        " << tubeId;
              ATH_MSG_WARNING(ss.str());
              continue;
            }

            bool isValid = false;
            const Identifier channelIdentifier =
                m_idHelperSvc->mdtIdHelper().channelID(stationName, stationEta, stationPhi,
                                    multiLayer, layer, tube, true, &isValid);

            // this debug msg can be removed eventually
            std::ostringstream ss;
            ss << "Trying to set from online IDs\n";
            ss << "  subdetectorId " << subdetectorId << "\n";
            ss << "  rodId         " << rodId << "\n";
            ss << "  csmId         " << csmId << "\n";
            ss << "  amtId         " << amtId << "\n";
            ss << "  tubelId       " << tubeId << "\n";
            ss << " .. that are converted to offline IDs\n";
            ss << "  stationName   " << stationName << "\n";
            ss << "  stationEta    " << stationEta << "\n";
            ss << "  stationPhi    " << stationPhi << "\n";
            ss << "  multiLayer    " << multiLayer << "\n";
            ss << "  layer         " << layer << "\n";
            ss << "  tube          " << tube << "\n";
            ss << " .. with channelID\n";
            ss << "  Identifier    " << channelIdentifier << "\n";
            ss << "  Identifier32  " << channelIdentifier.get_identifier32();
            ATH_MSG_DEBUG(ss.str());
            // Throw fatal msg if entry exists
            if (!isValid) {
              ATH_MSG_FATAL(ss.str());
              ATH_MSG_FATAL("Conversion to Identifier is NOT valid.");
              return StatusCode::FAILURE;
            }
            if (m_shiftValues.find(channelIdentifier) != m_shiftValues.end()) {
              ATH_MSG_FATAL("Double counting in initilization of map");
              return StatusCode::FAILURE;
            }
            float rn = rng.Gaus(m_centralValue, m_sigma);
            float shift =
                rn / m_tUpper;  // store relative variation: Delta_t / t
            m_shiftValues[channelIdentifier] = shift;
          }
        }
      }
    }
  }

  /* initalization was successful */
  m_mapIsInitialized = true;

  /* write map to the default location */
  ATH_CHECK(dumpMapAsFile());

  return StatusCode::SUCCESS;
}

StatusCode MdtCalibrationTMaxShiftTool::setTUpper(const float tUpper) {
  if (m_mapIsInitialized) {
    ATH_MSG_FATAL("You cannot change m_tUpper once the map is initialized.");
    return StatusCode::FAILURE;
  }

  m_tUpper = tUpper;
  return StatusCode::SUCCESS;
}
