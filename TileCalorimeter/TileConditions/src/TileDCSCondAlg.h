//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEDCSCONDALG_H
#define TILECONDITIONS_TILEDCSCONDALG_H

// Tile includes
#include "TileConditions/TileDCSState.h"
#include "TileConditions/TileEMScale.h"
#include "TileConditions/TileCablingSvc.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "GaudiKernel/ICondSvc.h"

class TileCablingService;

/**
 * @class TileDCSCondAlg
 * @brief Condition algorithm to prepare TileDCSState object and put it into condition store
 *
 * This condition algorithm reads information from Tile DCS DB.
 * Reference HV can be get from Tile calibration Cesium or Laser DB folder (by default)
 * instead of from Tile DCS DB. In this case reference HV is used from Laser folder
 * if it is not negative otherwise reference HV is used from Cesium folder. Using this information
 * the algorithm prepares TileDCSState condition object and put it into condition store.
 */
class TileDCSCondAlg: public AthReentrantAlgorithm {
  public:

    TileDCSCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TileDCSCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

  private:

    enum TileModuleType {LONG_BARREL = 0, // LB
                         EXTENDED_BARREL = 1, // EB
                         EXTENDED_BARREL_NO_D4 = 2, // EB without D4: EBA15, EBC18
                         EXTENDED_BARREL_SPECIAL_C10 = 3 // EB with special C10
    };

    enum TileLimits {
      NUMBER_OF_CHANNELS = 48,
      NUMBER_OF_HVSET_CHANNELS = 68,
      NUMBER_OF_MODULE_TYPES = 2, // LB, EB
      NUMBER_OF_SPECIAL_MODULE_TYPES = 4 // // LB, EB, EB without D4, EB with special C10
    };

   /**
    * @brief Read mapping of DB COOL channels into Tile ROS and drawer pairs
    * @param fileName name of file with mapping of COOL channels to Tile ROS and drawer pairs
    * @param subStr sub string to look for
    * @param[out] rosDrawer vector of Tile ROS and drawer pairs indexed by COOL channel
    * @return 0 in the case of success
    */
    int readConfig(const std::string& fileName, const std::string& subStr, std::vector<std::pair<int, int>>& rosDrawer);

   /**
    * @brief Read special deltas for few unstable PMTs
    * @param fileName name of file with special deltas for few unstable PMTs
    * @return 0 in the case of success
    */
    int readBadHV(const std::string& fileName);

   /**
    * @brief Store reference HV from Tile CES or Laser DB folder in TileDCSState
    * @param[out] dcsState TileDCSState to store reference HV
    * @param[out] eventRange Event range within which reference HV is valid
    */
    StatusCode fillReferenceHV(TileDCSState& dcsState, SG::WriteCondHandle<TileDCSState>& wh, const EventContext& ctx) const;

   /**
    * @ brief Return Tile channel key used as index in internal caches
    * @ ros Tile ROS in range [1..4]
    * @ drawer Tile drawer number in range [0..63]
    * @ channel Tile channel number
    */

    unsigned int getChannelKey(unsigned int ros, unsigned int drawer, unsigned int channel) const;

   /**
    * @brief The Tile DCS HV COOL folder name
    */
    SG::ReadCondHandleKey<CondAttrListCollection> m_hvKey{this,
        "HV", "/TILE/DCS/HV", "The Tile DCS HV COOL folder name"};

   /**
    * @brief The Tile DCS HVSET COOL folder name
    */
    SG::ReadCondHandleKey<CondAttrListCollection> m_hvSetKey{this,
        "HVSET", "/TILE/DCS/HVSET", "The Tile DCS HVSET COOL folder name"};

   /**
    * @brief The Tile DCS STATES COOL folder name
    */
    SG::ReadCondHandleKey<CondAttrListCollection> m_statesKey{this,
        "STATES", "/TILE/DCS/STATES", "The Tile DCS STATES COOL folder name"};

   /**
    * @brief The name of TileDCSState object to produce and put into condition store
    */
    SG::WriteCondHandleKey<TileDCSState> m_dcsStateKey{this,
        "TileDCS", "TileDCS", "Output Tile DCS status"};

   /**
    * @brief The name of TileEMScale object to get reference HV from Cesium or Laser folder
    */
    SG::ReadCondHandleKey<TileEMScale> m_emScaleKey{this,
        "TileEMScale", "TileEMScale", "Input Tile EMS conditions"};

   /**
    * @brief Flag to control reading Tile DCS HV folder from DB
    */
    Gaudi::Property<bool> m_readHV{this, "ReadHV", true,
        "Flag to control reading Tile DCS HV folder from DB"};

   /**
    * @brief Flag to control reading Tile DCS HVSET folder from DB
    */
    Gaudi::Property<bool> m_readHVSet{this, "ReadHVSET", false,
        "Flag to control reading Tile DCS HVSET folder from DB"};

   /**
    * @brief Flag to control reading Tile DCS STATES folder from DB
    */
    Gaudi::Property<bool> m_readStates{this, "ReadSTATES", true,
        "Flag to control reading Tile DCS STATES folder from DB"};

   /**
    * @brief Tile DCS Status is OK if difference
    *        between measured and requested HV is below this limit
    */
    Gaudi::Property<float> m_goodHVLimit{this, "GoodHVLimit", 2.0F,
        "Status is OK if HV difference is below this limit"};

   /**
    * @brief Mask channels with difference
    *        between measured and requested HV above this limit
    */
    Gaudi::Property<float> m_warningHVLimit{this, "WarningHVLimit", 10.0F,
        "Mask channels with HV difference above this limit"};

   /**
    * @brief Status is OK if Tile DCS drawer states is like this
    */
    Gaudi::Property<int> m_goodDrawer{this, "GoodDrawer", 212222,
        "Status is OK if Tile DCS drawer states is like this"};

   /**
    * @brief Status is WARNING if Tile DCS drawer states is like this
    */
    Gaudi::Property<std::vector<int>> m_warningDrawer{this, "WarningDrawer",
        {202221, // module in emergency mode
         202222, // 830V HV is on, no communication on HV can-bus, status is unknown (assume that it's good)
         2222,   // no communication on HV can-bus, status is unknown (assume that it's good)
         212020, // no communication on LV can-bus, status is unknown (assume that it's good)
         202020, // 830V HV is on, no communication on LV and HV can-bus, status is unknown (assume that it's good)
         2020},  // no communication on LV and HV can-bus, status is unknown (assume that it's good)
        "Status is WARNING if Tile DCS drawer states is like this"};

   /**
    * @brief File name with mapping Tile DCS HV DB COOL channels to ROS and drawer
    */
    Gaudi::Property<std::string> m_dcsChHvInCool{this,
        "DCSChHVInCOOL", "DCSChHVInCOOL.dat",
        "File name with mapping Tile DCS HV DB COOL channels to ROS and drawer"};

   /**
    * @brief File name with mapping Tile DCS HVSET DB COOL channels to ROS and drawer
    */
    Gaudi::Property<std::string> m_dcsChHvSetInCool{this,
        "DCSChHVSETInCOOL", "DCSChHVSETInCOOL.dat",
        "File name with mapping Tile DCS HVSET DB COOL channels to ROS and drawer"};

   /**
    * @brief File name with mapping Tile DCS STATES DB COOL channels to ROS and drawer
    */
    Gaudi::Property<std::string> m_dcsChStatesInCool{this,
        "DCSChStatesInCOOL", "DCSChStatesInCOOL.dat",
        "File name with mapping Tile DCS STATES DB COOL channels to ROS and drawer"};


   /**
    * @brief Name of conditions service
    */
    ServiceHandle<ICondSvc> m_condSvc{this,
        "CondSvc", "CondSvc", "The conditions service"};

   /**
    * @brief Name of Tile cabling service
    */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service" };

    const TileCablingService* m_cabling;

    std::vector<std::pair<int, int>> m_rosDrawerHV;
    std::vector<std::pair<int, int>> m_rosDrawerHVSet;
    std::vector<std::pair<int, int>> m_rosDrawerStates;

    std::map<int, std::pair<float,float> > m_knownBadHV;

    int m_channel2pmt[2][NUMBER_OF_HVSET_CHANNELS];
    int m_pmt2channel[2][NUMBER_OF_HVSET_CHANNELS];
    bool m_isChannelUsed[4][NUMBER_OF_CHANNELS]; // 0=LB, 1=EB, 2=EB without D4, 3=EB with special C10

};

// inlines

inline
unsigned int TileDCSCondAlg::getChannelKey(unsigned int ros, unsigned int drawer, unsigned int channel) const {
  return ( ((((ros - 1) << 6) | drawer) << 6) | channel );
}


#endif // TILECONDITIONS_TILEDCSCONDALG_H
