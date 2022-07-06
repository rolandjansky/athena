/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILECALIBMONITORALGORITHM_H
#define TILEMONITORING_TILECALIBMONITORALGORITHM_H

#include "TileEvent/TileDQstatus.h"
#include "TileConditions/TileCablingSvc.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"

class TileHWID;
class TileInfo;

/** @class TileCalibMonitorAlgorithm
 *  @brief Base class for Tile calibration runs monitoring
 */

class TileCalibMonitorAlgorithm : public AthMonitorAlgorithm {

  public:

    TileCalibMonitorAlgorithm(const std::string& name, ISvcLocator* svcLocator)
      :AthMonitorAlgorithm(name, svcLocator) {}

    using AthMonitorAlgorithm::AthMonitorAlgorithm;
    virtual ~TileCalibMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override = 0;


  protected:

   /*
    * Function to check that the DMU header format is correct
    * bit_31 of the DMU header must be 1 and
    * bit_17 of the DMU header must be 0
    * Return true in case of error
    */
    bool isHeaderFormatError(uint32_t header) const {
      return (((header >> 31 & 0x1) == 1) && ((header >> 17 & 0x1) == 0)) ? false : true;
    };

   /*
    * Function to check that the DMU header parity is correct
    * Parity of the DMU header should be odd
    * Return true in case of error
    */
    bool isHeaderParityError(uint32_t header) const {
      uint32_t parity(0);
      for (int i = 0; i < 32; ++i) {
        parity += ((header >> i) & 0x1);
      }
      return ((parity % 2) == 1) ? false : true;
    };


   /**
    * @enum RunType
    * @brief Describes Tile Run Type
    */
    enum RunType {
       UNKNOWN_RUN  = 0, // expect monogain
       PHYS_RUN = 1, // expect monogain
       LAS_RUN  = 2, // expect monogain
       LED_RUN  = 3, // expect monogain
       PED_RUN  = 4, // expect bigain
       CIS_RUN  = 8, // expect bigain
       MONO_RUN = 9, // expect monogain
       CIS_RAMP_RUN = 10 // expect monogain
    };

    Gaudi::Property<std::string> m_tileInfoName{this,
        "TileInfo", "TileInfo", "Name of TileInfo object in Detector Store"};

    Gaudi::Property<std::vector<int>> m_fragIDsToIgnoreDMUerrors{this,
        "FragIDsToIgnoreDMUErrors", {}, "List of Tile frag IDs for which ignore DMU errors"};

    Gaudi::Property<std::vector<int>> m_fragIDsDemonstrators{this,
        "FragIDsDemonstrators", {}, "List of Tile frag IDs of Demonstrators"};

    Gaudi::Property<int> m_runType{this,
        "RunType", 0, "Run type: 1 - phys, 2 - las, 4 - ped, 8 - cis, 9- mono"};

    SG::ReadHandleKey<TileDQstatus> m_dqStatusKey{this,
        "TileDQstatus", "TileDQstatus", "Tile DQ status name"};

  /**
    * @brief Name of Tile cabling service
    */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    const TileCablingService* m_cabling{nullptr};
    const TileHWID* m_tileHWID{nullptr};
    const TileInfo* m_tileInfo{nullptr};
};


#endif // TILEMONITORING_TILECALIBMONITORALGORITHM_H
