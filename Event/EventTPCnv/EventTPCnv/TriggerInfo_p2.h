/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_TRIGGERINFO_P2_H
#define EVENTTPCNV_TRIGGERINFO_P2_H
/**
 * @file TriggerInfo_p2.h
 *
 * @brief This class is the persistent representation of TriggerInfo
 *
 * @author R.D.Schaffer@cern.ch
 */

#include <vector>

class TriggerInfo_p2 {
public:
    class StreamTag_p2
    {
    public:
        std::string  m_name;
        std::string  m_type;
        bool         m_obeysLumiblock;
    };

    typedef unsigned int number_type;
    TriggerInfo_p2();

    number_type               m_statusElement;
    number_type               m_extendedLevel1ID;
    number_type               m_level1TriggerType;
    std::vector<number_type>  m_level1TriggerInfo;
    std::vector<number_type>  m_level2TriggerInfo;
    std::vector<number_type>  m_eventFilterInfo;
    std::vector<StreamTag_p2> m_streamTags;
};

// inline

inline
TriggerInfo_p2::TriggerInfo_p2()
	:
	m_statusElement(0),
	m_extendedLevel1ID(0),
	m_level1TriggerType(0)
{}


#endif 
