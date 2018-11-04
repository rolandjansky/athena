/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDEADMAPCONDDATA_H
#define PIXELDEADMAPCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <map>

class PixelModuleData {
  public:
    PixelModuleData();
    virtual ~PixelModuleData();

    void setModuleStatus(const CondAttrListCollection::ChanNum& chanNum, const int value);
    int getModuleStatus(const CondAttrListCollection::ChanNum& chanNum) const;

    void setChipStatus(const CondAttrListCollection::ChanNum& chanNum, const int value);
    int getChipStatus(const CondAttrListCollection::ChanNum& chanNum) const;

    void setTDAQModuleStatus(const CondAttrListCollection::ChanNum& chanNum, const int value);
    int getTDAQModuleStatus(const CondAttrListCollection::ChanNum& chanNum) const;

    void setBarrelAnalogThreshold(std::vector<int> BarrelAnalogThreshold);
    void setEndcapAnalogThreshold(std::vector<int> EndcapAnalogThreshold);
    void setDBMAnalogThreshold(std::vector<int>    DBMAnalogThreshold);
    std::vector<int> getBarrelAnalogThreshold() const;
    std::vector<int> getEndcapAnalogThreshold() const;
    std::vector<int> getDBMAnalogThreshold() const;

    void setBarrelToTThreshold(std::vector<int> BarrelToTThreshold);
    void setEndcapToTThreshold(std::vector<int> EndcapToTThreshold);
    void setDBMToTThreshold(std::vector<int>    DBMToTThreshold);
    std::vector<int> getBarrelToTThreshold() const;
    std::vector<int> getEndcapToTThreshold() const;
    std::vector<int> getDBMToTThreshold() const;

    void setBarrelLatency(std::vector<int> BarrelLatency);
    void setEndcapLatency(std::vector<int> EndcapLatency);
    void setDBMLatency(std::vector<int>    DBMLatency);
    std::vector<int> getBarrelLatency() const;
    std::vector<int> getEndcapLatency() const;
    std::vector<int> getDBMLatency() const;

    void setBarrelCrossTalk(std::vector<double> BarrelCrossTalk);
    void setEndcapCrossTalk(std::vector<double> EndcapCrossTalk);
    void setDBMCrossTalk(std::vector<double>    DBMCrossTalk);
    std::vector<double> getBarrelCrossTalk() const;
    std::vector<double> getEndcapCrossTalk() const;
    std::vector<double> getDBMCrossTalk() const;

    void setBarrelThermalNoise(std::vector<double> BarrelThermalNoise);
    void setEndcapThermalNoise(std::vector<double> EndcapThermalNoise);
    void setDBMThermalNoise(std::vector<double>    DBMThermalNoise);
    std::vector<double> getBarrelThermalNoise() const;
    std::vector<double> getEndcapThermalNoise() const;
    std::vector<double> getDBMThermalNoise() const;

    void setBarrelHitDuplication(std::vector<bool> BarrelHitDuplication);
    void setEndcapHitDuplication(std::vector<bool> EndcapHitDuplication);
    void setDBMHitDuplication(std::vector<bool>    DBMHitDuplication);
    std::vector<bool> getBarrelHitDuplication() const;
    std::vector<bool> getEndcapHitDuplication() const;
    std::vector<bool> getDBMHitDuplication() const;

    void setBarrelSmallHitToT(std::vector<int> BarrelSmallHitToT);
    void setEndcapSmallHitToT(std::vector<int> EndcapSmallHitToT);
    void setDBMSmallHitToT(std::vector<int>    DBMSmallHitToT);
    std::vector<int> getBarrelSmallHitToT() const;
    std::vector<int> getEndcapSmallHitToT() const;
    std::vector<int> getDBMSmallHitToT() const;

    void setIBLHitDiscConfig(const int hitDiscConfig);
    int getIBLHitDiscConfig() const;
    int getIBLOverflowToT() const;

    void clear();

  private:
    typedef std::map<CondAttrListCollection::ChanNum, int> IntConditions;
    IntConditions  m_moduleStatus;
    IntConditions  m_chipStatus;
    IntConditions  m_tdaqStatus;
    std::vector<int> m_BarrelAnalogThreshold;
    std::vector<int> m_EndcapAnalogThreshold;
    std::vector<int> m_DBMAnalogThreshold;
    std::vector<int> m_BarrelToTThreshold;
    std::vector<int> m_EndcapToTThreshold;
    std::vector<int> m_DBMToTThreshold;
    std::vector<int> m_BarrelLatency;
    std::vector<int> m_EndcapLatency;
    std::vector<int> m_DBMLatency;
    std::vector<double> m_BarrelCrossTalk; 
    std::vector<double> m_EndcapCrossTalk; 
    std::vector<double> m_DBMCrossTalk; 
    std::vector<double> m_BarrelThermalNoise;
    std::vector<double> m_EndcapThermalNoise;
    std::vector<double> m_DBMThermalNoise;
    std::vector<bool> m_BarrelHitDuplication;
    std::vector<bool> m_EndcapHitDuplication;
    std::vector<bool> m_DBMHitDuplication;
    std::vector<int>  m_BarrelSmallHitToT;
    std::vector<int>  m_EndcapSmallHitToT;
    std::vector<int>  m_DBMSmallHitToT;
    int m_hitDiscConfig;

};

CLASS_DEF( PixelModuleData , 345932873 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( PixelModuleData, 578988393 );

#endif
