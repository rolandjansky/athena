/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TriggerInfo.cxx
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: TriggerInfo.cxx,v 1.8 2007-12-03 15:23:56 efeld Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>

#include "EventInfo/TriggerInfo.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>



TriggerInfo::StreamTag::StreamTag(const std::string& name,
                                  const std::string& type,
                                  bool obeysLumiblock,
                                  const std::set<number_type>& robs,
                                  const std::set<number_type>& dets
)
        :
        m_name(name),
        m_type(type),
        m_obeysLumiblock(obeysLumiblock),
        m_robs(robs),
        m_dets(dets)
{}

TriggerInfo::StreamTag::StreamTag()
        :
        m_obeysLumiblock(false)
{}


const std::string&
TriggerInfo::StreamTag::name () const
{
    return (m_name);
}

const std::string&  TriggerInfo::StreamTag::type () const
{
    return (m_type);
}

bool
TriggerInfo::StreamTag::obeysLumiblock () const
{
    return(m_obeysLumiblock);
}

const std::set<TriggerInfo::number_type>  TriggerInfo::StreamTag::robs() const
{
    return (m_robs);
}

const std::set<TriggerInfo::number_type>  TriggerInfo::StreamTag::dets() const
{
    return (m_dets);
}


TriggerInfo::TriggerInfo()
    :
        m_statusElement(0),
        m_extendedLevel1ID(0),
        m_level1TriggerType(0)
{}

TriggerInfo::TriggerInfo(number_type statusElement,
			 number_type extendedLevel1ID,
			 number_type level1TriggerType,
			 const std::vector<number_type>& level1TriggerInfo)
  :
        m_statusElement(statusElement),
        m_extendedLevel1ID(extendedLevel1ID),
        m_level1TriggerType(level1TriggerType),
        m_level1TriggerInfo(level1TriggerInfo)
{}

TriggerInfo::TriggerInfo(number_type statusElement,
                         number_type extendedLevel1ID,
                         number_type level1TriggerType,
                         const std::vector<number_type>& level2TriggerInfo,
                         const std::vector<number_type>& eventFilterInfo,
                         const std::vector<StreamTag>&   streamTags)
        :
        m_statusElement(statusElement),
        m_extendedLevel1ID(extendedLevel1ID),
        m_level1TriggerType(level1TriggerType),
        m_level2TriggerInfo(level2TriggerInfo),
        m_eventFilterInfo(eventFilterInfo),
        m_streamTags(streamTags)
{}

TriggerInfo::TriggerInfo(number_type statusElement,
                         number_type extendedLevel1ID,
                         number_type level1TriggerType,
                         const std::vector<number_type>& level1TriggerInfo,
                         const std::vector<number_type>& level2TriggerInfo,
                         const std::vector<number_type>& eventFilterInfo,
                         const std::vector<StreamTag>&   streamTags)
        :
        m_statusElement(statusElement),
        m_extendedLevel1ID(extendedLevel1ID),
        m_level1TriggerType(level1TriggerType),
        m_level1TriggerInfo(level1TriggerInfo),
        m_level2TriggerInfo(level2TriggerInfo),
        m_eventFilterInfo(eventFilterInfo),
        m_streamTags(streamTags)
{}

TriggerInfo::~TriggerInfo()
{}

TriggerInfo::number_type
TriggerInfo::statusElement     () const
{
    return (m_statusElement);
}


TriggerInfo::number_type 
TriggerInfo::extendedLevel1ID 	() const
{
    return (m_extendedLevel1ID);
}

TriggerInfo::number_type 
TriggerInfo::level1TriggerType	() const
{
    return (m_level1TriggerType);
}

const std::vector<TriggerInfo::number_type>& 
TriggerInfo::level1TriggerInfo () const
{
    return(m_level1TriggerInfo);
}

const std::vector<TriggerInfo::number_type>& 
TriggerInfo::level2TriggerInfo () const
{
    return(m_level2TriggerInfo);
}

const std::vector<TriggerInfo::number_type>& 
TriggerInfo::eventFilterInfo () const
{
    return (m_eventFilterInfo);
}

const std::vector<TriggerInfo::StreamTag>&   
TriggerInfo::streamTags        () const
{
    return (m_streamTags);
}

void 
TriggerInfo::setStatusElement (const number_type statusElement)
{
  m_statusElement = statusElement;
}

void 
TriggerInfo::setExtendedLevel1ID (const number_type extendedLevel1ID)
{
  m_extendedLevel1ID = extendedLevel1ID;
}

void
TriggerInfo::setLevel1TriggerType (const number_type level1TriggerType)
{
  m_level1TriggerType = level1TriggerType;
}

void 
TriggerInfo::setLevel1TriggerInfo (const std::vector<number_type>& level1TriggerInfo)
{
  m_level1TriggerInfo = level1TriggerInfo;
}

void 
TriggerInfo::setLevel2TriggerInfo (const std::vector<number_type>& level2TriggerInfo)
{
  m_level2TriggerInfo = level2TriggerInfo;
}

void
TriggerInfo::setEventFilterInfo (const std::vector<number_type>& eventFilterInfo)
{
  m_eventFilterInfo = eventFilterInfo;
}

void
TriggerInfo::setStreamTags(const std::vector<StreamTag>&   streamTags) 
{
    m_streamTags = streamTags;
}

std::ostream& operator<<(std::ostream& os, const std::vector<TriggerInfo::number_type>& rhs) 
{
  // switch to hex format
  os << std::hex <<  std::setfill( '0' );

  if (rhs.size() > 0) 
    os << " 0x" << std::setw(8) << rhs[0];
  for (size_t i(1); i < rhs.size(); ++i) 
    os << ", 0x" << std::setw(8) << rhs[i];

  // switch back to normal format
  os  << std::dec << std::setfill(' ');

  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<TriggerInfo::StreamTag>& rhs) 
{
  if (rhs.size() > 0) 
    os << "(" << rhs[0].name() << "," << rhs[0].type() << "," << rhs[0].obeysLumiblock() << ")";
  for (size_t i(1); i < rhs.size(); ++i) 
    os << ",(" << rhs[i].name() << "," << rhs[i].type() << "," << rhs[i].obeysLumiblock() << ")";

  return os;
}

std::ostream& operator<<(std::ostream& os, const TriggerInfo& rhs) {

  os << "\n [S,L1ID,L1TT] = [" 
     << rhs.m_statusElement
     << "," << rhs.m_extendedLevel1ID
     << "," << rhs.m_level1TriggerType
     << "]"
     << "\n L1: [" << rhs.m_level1TriggerInfo << "]"
     << "\n L2: [" << rhs.m_level2TriggerInfo << "]"
     << "\n EF: [" << rhs.m_eventFilterInfo << "]"
     << "\n ST: [" << rhs.m_streamTags << "]";

  return os;
}


