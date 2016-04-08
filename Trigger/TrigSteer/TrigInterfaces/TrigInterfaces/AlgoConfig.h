/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINTERFACES_ALGOCONFIG_H
#define TRIGINTERFACES_ALGOCONFIG_H

#include <string>
#include "TrigSteeringEvent/Enums.h"

class StoreGateSvc;
class MsgStream;
class PartialEventBuildingInfo;
class ScoutingInfo;
class TrigOperationalInfo;

namespace HLT
{
  class Navigation;
  class RobRequestInfo;

  /**
   * @brief Class describing the configuration for HLT algorithms.
   *
   * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - U. of California, Irvine
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   *
   * This class holds all the common configuration variables for the HLT steering and
   * the HLT algorithms.
   */

  class AlgoConfig {

  public:

    /** @brief Constructor setting all the variables to default values. */
    AlgoConfig()
      : m_hltLevel(HLT::L2),
      m_useL1Simulation(true),
      m_useL1CaloSimulation(true),
      m_useL1MuonSimulation(false),
      m_doTiming(false),
      m_updateCaloRoIs(false),
      m_doPoolReferencing(false),
      m_ratioForcedAccept(0.),
      m_lvl1Id(0),
      m_lumiBlockNumber(0),
      m_triggerConfigVectorLocation("storeL2Location"),
      m_instance("_L2"),
      m_lvlConverterStatus(HLT::OK),
      m_hltStatus(HLT::OK),
      m_storeGate(0),
      m_log(0),
      m_loglvl(0),
      m_navigation(0),
      m_pebi(0),
      m_scouting(0),
      m_doOperationalInfo(0),
      m_steeringOPI(0),
      m_robi(0)
      
    {}

    /** @brief Destructor. */
    ~AlgoConfig() {}

    /**
     * @brief Event reset method.
     * Method resetting all the relevant status variables to their default value for each processed event.
     */
    void reset() {
      m_hltStatus = HLT::OK;
      m_lvlConverterStatus = HLT::OK;
    }


    /**
     * @brief Getter method for the HLT level flag. 
     * @return HLT level flag for this algorithm.
     * The HLT level flag specifies at which HLT level (LVL2 or EF) the algorithm will execute.
     */
    inline HLTLevel getHLTLevel() const { return m_hltLevel; }
    /**
     * @brief Getter method for the instance flag.
     * @return instance flag for this algorithm.
     * The instance flag specifies which is the level ("_L2", "_EF" or "_UNKNOWN") of the current instance of this algorithm.
     */
    inline const std::string& getInstance() const { return m_instance; }
    /**
     * @brief Getter method for the L1 simulation enable/disable flag.
     * @return L1 simulation enable/disable flag for this algorithm.
     * The L1 simulation enable/disable flag specifies whether the LVL1 simulation is going to be used by this algorithm.
     */
    inline bool getUseL1Simulation() const { return m_useL1Simulation; }
    /**
     * @brief Getter method for the L1 calo simulation enable/disable flag.
     * @return L1 calo simulation enable/disable flag for this algorithm.
     * The L1 calo simulation enable/disable flag specifies whether the LVL1 calorimeter simulation is going to be used by this algorithm.
     */
    inline bool getUseL1CaloSimulation() const { return m_useL1CaloSimulation; }
    /**
     * @brief Getter method for the L1 muon simulation enable/disable flag.
     * @return L1 muon simulation enable/disable flag for this algorithm.
     * The L1 muon simulation enable/disable flag specifies whether the LVL1 muon simulation is going to be used by this algorithm.
     */
    inline bool getUseL1MuonSimulation() const { return m_useL1MuonSimulation; }
    /**
     * @brief Getter method for the timing enable/disable flag.
     * @return timing enable/disable flag for this algorithm.
     * The timing enable/disable flag specifies whether timing measurements are going to be performed for this algorithm.
     */
    inline bool getDoTiming() const { return m_doTiming; }
    /**
     * @brief Getter method for the calo RoI update enable/disable flag.
     * @return calo RoI update enable/disable flag for this algorithm.
     * The calo RoI update enable/disable flag specifies whether this algorithm should use updated calorimeter RoI positions or not
     * (needed for running on bytestream before 12.x.x).
     */
    inline bool getUpdateCaloRoIs() const { return m_updateCaloRoIs; }
    /**
     * @brief Getter method for the Pool referencing enable/disable flag.
     * @return Pool referencing enable/disable flag for this algorithm.
     * The Pool referencing enable/disable flag specifies whether this algorithm should use Pool referencing or not.
     */
    inline bool getDoPoolReferencing() const { return  m_doPoolReferencing; }
    /**
     * @brief Getter method for the forced accept rate flag.
     * @return forced accept rate flag for this algorithm.
     * The forced accept rate specifies which is the rate of forced accepted events for this algorithm.
     */
    inline float getRatioForcedAccept() const { return m_ratioForcedAccept; }
    /**
     * @brief Getter method for the LVL1 Id.
     * @return current  LVL1 Id.
     */
    inline unsigned int getLvl1Id() const { return m_lvl1Id; }

    /**
     * @brief Getter method for the Lumi Block Number.
     * @return current  Lumi Block Number.
     */
    inline unsigned int getLumiBlockNumber() const { return m_lumiBlockNumber; }
    /**
     * @brief Getter method for the L2 config vector location.
     * @return L2 config vector location for this algorithm.
     * The forced accept rate specifies the store location of the LVL2 config vector.
     */
    inline const std::string& getTriggerConfigVectorL2Location() const { return m_triggerConfigVectorLocation; }
    /**
     * @brief Getter method for the pointer to the StoreGate service.
     * @return pointer to the StoreGate service.
     */
    inline StoreGateSvc* getStoreGate() const { return m_storeGate; }
    /**
     * @brief Getter method for the pointer to the MsgStream service.
     * @return pointer to the MsgStream service.
     */
    inline MsgStream&    getMsgStream() const { return (*m_log); }
    /**
     * @brief Getter method for the message level flag.
     * @return message level flag for this algorithm.
     */
    inline unsigned int getMsgLvl() const { return m_loglvl; }
    /**
     * @brief Getter method for the pointer to the Navigation service.
     * @return pointer to the Navigation service.
     */
    inline Navigation*   getNavigation() const { return m_navigation; }
    /**
     * @brief Getter method for the level converter status variable.
     * @return level converter status variable.
     */
    inline ErrorCode     getLvlConverterStatus() const { return m_lvlConverterStatus; }
    /**
     * @brief Getter method for the HLT status variable.
     * @return HLT status variable.
     */
    inline ErrorCode     getHltStatus() const { return m_hltStatus; }


    /**
     * @brief Setter method for the HLT level flag. 
     * The HLT level flag specifies at which HLT level (LVL2 or EF) the algorithm will execute.
     */
    void setHLTLevel(HLTLevel l);
    /**
     * @brief Setter method for the L1 simulation enable/disable flag.
     * The L1 simulation enable/disable flag specifies whether the LVL1 simulation is going to be used by this algorithm.
     */
    void setUseL1Simulation(bool l1) { m_useL1Simulation = l1; }
    /**
     * @brief Setter method for the L1 calo simulation enable/disable flag.
     * The L1 calo simulation enable/disable flag specifies whether the LVL1 calorimeter simulation is going to be used by this algorithm.
     */
    void setUseL1CaloSimulation(bool l1) { m_useL1CaloSimulation = l1; }
    /**
     * @brief Setter method for the L1 muon simulation enable/disable flag.
     * The L1 muon simulation enable/disable flag specifies whether the LVL1 muon simulation is going to be used by this algorithm.
     */
    void setUseL1MuonSimulation(bool l1) { m_useL1MuonSimulation = l1; }
    /**
     * @brief Setter method for the timing enable/disable flag.
     * The timing enable/disable flag specifies whether timing measurements are going to be performed for this algorithm.
     */
    void setUseTiming(bool t) { m_doTiming = t; }
    /**
     * @brief Setter method for the calo RoI update enable/disable flag.
     * The calo RoI update enable/disable flag specifies whether this algorithm should use updated calorimeter RoI positions or not
     * (needed for running on bytestream before 12.x.x).
     */
    void setUpdateCaloRoIs(bool u) { m_updateCaloRoIs = u; }
    /**
     * @brief Setter method for the Pool referencing enable/disable flag.
     * The Pool referencing enable/disable flag specifies whether this algorithm should use Pool referencing or not.
     */
    void setDoPoolReferencing(bool p) { m_doPoolReferencing = p; }
    /**
     * @brief Setter method for the forced accept rate flag.
     * The forced accept rate specifies which is the rate of forced accepted events for this algorithm.
     */
    void setRatioForcedAccept(float f) { m_ratioForcedAccept = f; }
    /**
     * @brief Setter method for the LVL1 Id.
     */
    void setLvl1Id(unsigned int l) { m_lvl1Id = l; }
    /**
     * @brief Setter method for the LumiBlockNumber.
     */    
    void setLumiBlockNumber(unsigned int l) { m_lumiBlockNumber = l; }
    /**
     * @brief Setter method for the trigger config vector location.
     * The trigger config vector location specifies the store location of the trigger config vector.
     */
    void setTriggerConfigVectorLocation(const std::string& s) { m_triggerConfigVectorLocation = s; }
    /**
     * @brief Setter method for the pointer to the StoreGate service.
     */
    void setStoreGate(StoreGateSvc* p) { m_storeGate = p; }
    /**
     * @brief Setter method for the pointer to the MsgStream service.
     */
    void setMsgStream(MsgStream* p) { m_log = p; }
    /**
     * @brief Setter method for the message level flag.
     */
    void setMsgLvl(unsigned int i) { m_loglvl = i; }
    /**
     * @brief Setter method for the pointer to the Navigation service.
     */
    void setNavigation(Navigation* p) { m_navigation = p; }
    /**
     * @brief Setter method for the level converter status variable.
     */
    void setLvlConverterStatus(HLT::ErrorCode ec) { m_lvlConverterStatus = ec; }
    /**
     * @brief Setter method for the HLT status variable.
     */
    void setHltStatus(HLT::ErrorCode ec) { m_hltStatus = ec; }

    /**
     * @brief accessors for PartialEventBuildingInfo object
     */
    PartialEventBuildingInfo* getPEBI() const { return m_pebi; }
    void setPEBI(PartialEventBuildingInfo* p) { m_pebi = p; }
    
    /**
     * @brief accessors for ScoutingInfo object
     */
    ScoutingInfo* getScoutingInfo() const { return m_scouting; }
    void setScoutingInfo(ScoutingInfo* sI) { m_scouting = sI; }

    /**
     * @brief accessors for OPI verbosity level
     */    
    void setSteeringOPILevel(unsigned int i) { m_doOperationalInfo = i; }
    inline unsigned int getSteeringOPILevel() const { return m_doOperationalInfo; }

    /**
     * @brief accessors for currently filled by steering OPI object
     */      
    inline TrigOperationalInfo* getSteeringOPI() const     { return m_steeringOPI; } 
    inline void setSteeringOPI( TrigOperationalInfo* opi ) { m_steeringOPI = opi; } 

    RobRequestInfo* robRequestInfo() const { return m_robi; }
    void setRobRequestInfo(RobRequestInfo* robi) { m_robi = robi; }
    
  private:

    /** @brief HLT level flag. */
    HLTLevel m_hltLevel;
    /** @brief L1 simulation enable/disable flag. */
    bool m_useL1Simulation;
    /** @brief L1 calo simulation enable/disable flag. */
    bool m_useL1CaloSimulation;
    /** @brief L1 muon simulation enable/disable flag. */
    bool m_useL1MuonSimulation;
    /** @brief Timing enable/disable flag. */
    bool m_doTiming;
    /** @brief Calo RoI update enable/disable flag. */
    bool m_updateCaloRoIs;
    /** @brief Pool referencing enable/disable flag. */
    bool m_doPoolReferencing;
    /** @brief Forced accept rate flag. */
    float m_ratioForcedAccept;
    /** @brief LVL1 Id. */
    unsigned int m_lvl1Id;
    /** @brief Lumi Block Number */
    unsigned int m_lumiBlockNumber;
    /** @brief Getter method for the L2 config vector location. */
    std::string m_triggerConfigVectorLocation;
    /** @brief Getter method for the instance flag. */
    std::string m_instance;
    /** @brief Getter method for the level converter status variable. */
    ErrorCode m_lvlConverterStatus;
    /** @brief Getter method for the HLT status variable. */
    ErrorCode m_hltStatus;

    /** @brief Pointer to the StoreGate service. */
    StoreGateSvc* m_storeGate;
    /** @brief Pointer to the MsgStream service. */
    MsgStream* m_log;
    /** @brief Message level flag. */
    unsigned int m_loglvl;
    /** @brief Pointer to the Navigation service. */
    Navigation* m_navigation;
    /** @brief pointer to partial event building info */
    PartialEventBuildingInfo* m_pebi;
    /** @brief pointer to scouting info */
    ScoutingInfo* m_scouting; 

    /** @brief flag to collect operational information 
     * 0 - none
     * 1... - more
     */
    unsigned int m_doOperationalInfo;
    TrigOperationalInfo* m_steeringOPI;

    /** @brief pointer to partial event building info */
    RobRequestInfo* m_robi;
  };

  /**
   * @brief Setter method for the HLT level flag. 
   * The HLT level flag specifies at which HLT level (LVL2 or EF) the algorithm will execute.
   */
  inline void AlgoConfig::setHLTLevel(HLTLevel l) {
    m_hltLevel = l;
    if(l == HLT::L2)         m_instance = "_L2";
    else if (l == HLT::EF)   m_instance = "_EF";
    else if (l == HLT::HLT)  m_instance = "_HLT";
    else m_instance = "_UNKNOWN";
  }

} // end of namespace
#endif
 
