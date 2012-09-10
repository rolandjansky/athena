/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALORAMPDATACONTAINER_H
#define L1CALORAMPDATACONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"

#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloRampData.h"

#include <map>
#include <string>

/**
 * Container of L1CaloRampData objects.
 *
 * @author Veit Scharf <veit.scharf@kip.uni-heidelberg.de>
 **/

class L1CaloRampDataContainer
{
 public:
    typedef std::map<unsigned int, L1CaloRampData> L1CaloRampDataMap;
    typedef L1CaloRampDataMap::const_iterator const_iterator;
    //    typedef L1CaloRampDataMap::value_type value_type;

    L1CaloRampDataContainer(): m_runNumber(0) {}
    virtual ~L1CaloRampDataContainer() {}
    
    const L1CaloRampData* rampData(unsigned int channelId) const;
    const L1CaloRampData* rampData(const L1CaloCoolChannelId& channelId) const;
    L1CaloRampData* rampData(const L1CaloCoolChannelId& channelId);
    L1CaloRampData* rampData(unsigned int channelId);
    
    void addRampData(unsigned int channelId, const L1CaloRampData& rampData);
    void addRampData(const L1CaloCoolChannelId& channelId, const L1CaloRampData& rampData);
    
    inline const_iterator begin() const { return m_rampDataMap.begin(); }
    inline const_iterator end() const   { return m_rampDataMap.end(); }

    unsigned int runNumber()   const    { return m_runNumber; }
    std::string gainStrategy() const    { return m_gainStrategy; }
    void setRunNumber(unsigned int run) { m_runNumber = run; }
    void setGainStrategy(const std::string& strategy) { m_gainStrategy = strategy; }

 private:
    typedef L1CaloRampDataMap::iterator iterator;
    L1CaloRampDataMap m_rampDataMap;
    unsigned int m_runNumber;
    std::string m_gainStrategy;
};

CLASS_DEF( L1CaloRampDataContainer , 1308570360 , 1 )

#endif // L1CALORAMPDATACONTAINER_H
