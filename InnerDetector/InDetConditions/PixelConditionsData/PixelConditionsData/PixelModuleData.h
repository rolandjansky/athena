/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDEADMAPCONDDATA_H
#define PIXELDEADMAPCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <map>

class PixelModuleData {
  public:
    PixelModuleData();
    virtual ~PixelModuleData();

    void setModuleStatus(const int chanNum, const int value);
    int getModuleStatus(const int chanNum) const;

    void setChipStatus(const int chanNum, const int value);
    int getChipStatus(const int chanNum) const;

    enum DCSModuleStatus{OK,WARNING,ERROR,FATAL,NOSTATUS};
    enum DCSModuleState{READY,ON,UNKNOWN,TRANSITION,UNDEFINED,NOSTATE};

    void setBarrelAnalogThreshold(std::vector<int> BarrelAnalogThreshold);
    void setEndcapAnalogThreshold(std::vector<int> EndcapAnalogThreshold);
    void setDBMAnalogThreshold(std::vector<int>    DBMAnalogThreshold);
    int getAnalogThreshold(int bec, int layer) const;

    void setBarrelToTThreshold(std::vector<int> BarrelToTThreshold);
    void setEndcapToTThreshold(std::vector<int> EndcapToTThreshold);
    void setDBMToTThreshold(std::vector<int>    DBMToTThreshold);
    int getToTThreshold(int bec, int layer) const;

    void setBarrelLatency(std::vector<int> BarrelLatency);
    void setEndcapLatency(std::vector<int> EndcapLatency);
    void setDBMLatency(std::vector<int>    DBMLatency);
    int getLatency(int bec, int layer) const;

    void setBarrelCrossTalk(std::vector<double> BarrelCrossTalk);
    void setEndcapCrossTalk(std::vector<double> EndcapCrossTalk);
    void setDBMCrossTalk(std::vector<double>    DBMCrossTalk);
    double getCrossTalk(int bec, int layer) const;

    void setBarrelThermalNoise(std::vector<double> BarrelThermalNoise);
    void setEndcapThermalNoise(std::vector<double> EndcapThermalNoise);
    void setDBMThermalNoise(std::vector<double>    DBMThermalNoise);
    double getThermalNoise(int bec, int layer) const;

    void setBarrelHitDuplication(std::vector<bool> BarrelHitDuplication);
    void setEndcapHitDuplication(std::vector<bool> EndcapHitDuplication);
    void setDBMHitDuplication(std::vector<bool>    DBMHitDuplication);
    bool getHitDuplication(int bec, int layer) const;

    void setBarrelSmallHitToT(std::vector<int> BarrelSmallHitToT);
    void setEndcapSmallHitToT(std::vector<int> EndcapSmallHitToT);
    void setDBMSmallHitToT(std::vector<int>    DBMSmallHitToT);
    int getSmallHitToT(int bec, int layer) const;

    void setIBLHitDiscConfig(const int hitDiscConfig);
    int getIBLHitDiscConfig() const;
    int getIBLOverflowToT() const;

    void clear();

  private:
    typedef std::map<int, int> IntConditions;
    IntConditions  m_moduleStatus;
    IntConditions  m_chipStatus;

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
