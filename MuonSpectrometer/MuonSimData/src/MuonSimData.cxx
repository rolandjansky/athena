/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Simulation data associated with an MuonRawData object
 ------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: MuonSimData.cxx,v 1.1 2004-05-29 15:17:53 ketevi Exp $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonSimData/MuonSimData.h"
#include <iostream>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>


MuonSimData::MuonSimData() :
  m_word(0),
  x(0.0),y(0.0),z(0.0)
{
  
}

MuonSimData::MuonSimData (const std::vector<Deposit>& deposits, 
			    int simDataWord)
    : m_word((deposits.size()<<29 & 0xe0000000) | (simDataWord & 0x1fffffff)), x(0.0),y(0.0),z(0.0)
{
    m_deposits = deposits;
}

MuonSimData::MuonSimData (const MuonSimData& other)
    :m_word(other.m_word),
     m_deposits(other.m_deposits), x(other.x),y(other.y),z(other.z)
{}


MuonSimData::~MuonSimData()
{
}

