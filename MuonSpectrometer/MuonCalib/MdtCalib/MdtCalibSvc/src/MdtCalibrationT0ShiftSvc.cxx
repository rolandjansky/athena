/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// std
#include <fstream>
#include <string>
#include <iostream>
#include <cstring>
#include <sys/stat.h>

// other packages
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "Identifier/Identifier.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonIdHelpers/MdtIdHelper.h"

// this package
#include "TTree.h"
#include "MdtCalibSvc/MdtCalibrationShiftMapBase.h"
#include "MdtCalibSvc/MdtCalibrationT0ShiftSvc.h"

// TRandom3 for random smearing
// should later be replaced with std
#include "TRandom3.h"

//
// private helper functions
//

MdtCalibrationT0ShiftSvc::MdtCalibrationT0ShiftSvc(const std::string &name,
                                                   ISvcLocator* pSvcLocator)
    : MdtCalibrationShiftMapBase(name, pSvcLocator) {}

MdtCalibrationT0ShiftSvc::~MdtCalibrationT0ShiftSvc() {}

StatusCode MdtCalibrationT0ShiftSvc::initializeMap()
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

  /* idHelper to retrieve channel Identifiers */
  const MdtIdHelper *idhelper = nullptr;
  const ServiceHandle<StoreGateSvc> detStore("StoreGateSvc/DetectorStore", "detStore");
  ATH_CHECK( detStore->retrieve(idhelper, "MDTIDHELPER") );


  // Cabling Map (part of cablingSvc) must be retrieved AFTER the first event
  // i.e. this can NOT happen in initialize
  /* initialize cabling from which IDs are read */
  ATH_CHECK( m_cablingSvc.retrieve() );

  /* Get ROBs */
  std::vector<uint32_t> robVector = m_cablingSvc->getAllROBId();

  std::map<uint8_t, MdtSubdetectorMap*> *listOfSubdet;
  std::map<uint8_t, MdtSubdetectorMap*>::const_iterator it_sub;

  std::map<uint8_t, MdtRODMap*> *listOfROD;
  std::map<uint8_t, MdtRODMap*>::const_iterator it_rod;

  std::map<uint8_t, MdtCsmMap*> *listOfCsm;
  std::map<uint8_t, MdtCsmMap*>::const_iterator it_csm;

  std::map<uint8_t, MdtAmtMap*> *listOfAmt;
  std::map<uint8_t, MdtAmtMap*>::const_iterator it_amt;

  const MuonMDT_CablingMap* cablingMap = m_cablingSvc->getCablingMap();
  listOfSubdet = cablingMap->getListOfElements();

  int subdetectorId, rodId, csmId, amtId;
  int stationName, stationEta, stationPhi, multiLayer, layer, tube;

  for (it_sub = listOfSubdet->begin(); it_sub != listOfSubdet->end(); ++it_sub) {
    subdetectorId = it_sub->first;
    listOfROD = it_sub->second->getListOfElements();

    for (it_rod = listOfROD->begin(); it_rod != listOfROD->end(); ++it_rod) {
      rodId = it_rod->first;
      listOfCsm = it_rod->second->getListOfElements();

      for (it_csm = listOfCsm->begin(); it_csm != listOfCsm->end(); ++it_csm) {
        csmId = it_csm->first;
        listOfAmt = it_csm->second->getListOfElements();

        for (it_amt = listOfAmt->begin(); it_amt != listOfAmt->end(); ++it_amt) {
          amtId = it_amt->first;
          // amtId = it_amt->second->moduleId();

          for (int tubeId = 0; tubeId < 24; ++tubeId) {

            /* Get the offline ID, given the current detector element */
            if (!m_cablingSvc->getOfflineId(
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

            bool             isValid = false;
            const Identifier channelIdentifier =
                idhelper->channelID(stationName, stationEta, stationPhi,
                                    multiLayer, layer, tube, true, &isValid);
            // this debug msg can be removed eventually
            std::ostringstream ss;
            ss << "Trying to set from online IDs\n";
            ss << "  subdetectorId " << subdetectorId << "\n";
            ss << "  rodId         " << rodId << "\n";
            ss << "  csmId         " << csmId << "\n";
            ss << "  amtId         " << amtId << "\n";
            ss << "  tubeId        " << tubeId << "\n";
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
            ATH_MSG_DEBUG( ss.str() );

            if (!isValid) {
              ATH_MSG_FATAL( ss.str() );
              ATH_MSG_FATAL("Conversion to Identifier is NOT valid.");
              return StatusCode::FAILURE;
            }

            float rn = rng.Gaus(m_centralValue, m_sigma);
            // Fatal if entry exists
            if (m_shiftValues.find(channelIdentifier) != m_shiftValues.end()) {
              ATH_MSG_FATAL("Double counting in initialization of map");
              return StatusCode::FAILURE;
            }
            m_shiftValues[channelIdentifier] = rn;
          }
        }
      }
    }
  }

  /* initalization was successful */
  m_mapIsInitialized = true;

  /* write map to the default location */
  ATH_CHECK( dumpMapAsFile() );

  return StatusCode::SUCCESS;
}
