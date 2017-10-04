/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 -----------------------------------------
 ***************************************************************************
 this class reads in the electronic mapping from the TBElectronics.map-file to return the Muon-Offline-Identifier to a given channel.
 ***************************************************************************/

#include "IOVDbTestConditions/IOVDbTestMDTEleMap.h"
//#include "IOVDbTestCnv/IOVDbTestMDTEleMap.h"
//#include "Identifier/Identifier.h"
#include <stdio.h>

IOVDbTestMDTEleMap::IOVDbTestMDTEleMap()
	:
	m_runNumber(0),
	m_eventNumber(0),
	m_timeStamp(0)
{ }

IOVDbTestMDTEleMap::~IOVDbTestMDTEleMap() 
{ }

std::string IOVDbTestMDTEleMap::name() const
{
    return (m_name);
}

void 
IOVDbTestMDTEleMap::set(int runNumber, int eventNumber, const std::string& name)
{
    m_runNumber   = runNumber;
    m_eventNumber = eventNumber;
    m_name        = name;    
}

void 
IOVDbTestMDTEleMap::set(int timeStamp, const std::string& name)
{
    m_timeStamp   = timeStamp;
    m_name        = name;    
}

int         IOVDbTestMDTEleMap::runNumber() const
{
    return (m_runNumber);
}

int         IOVDbTestMDTEleMap::eventNumber() const
{
    return (m_eventNumber);
}

int         IOVDbTestMDTEleMap::timeStamp() const
{
    return (m_timeStamp);
}




