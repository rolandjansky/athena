/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Simulation data associated with an MuonRawData object
 ------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: MuonSimData.cxx,v 1.1 2004-05-29 15:17:53 ketevi Exp $

#include "MuonSimData/MuonSimData.h"
#include <iostream>


MuonSimData::MuonSimData() :
  m_word(0),
  m_x(0.0),m_y(0.0),m_z(0.0),m_t(0.0)
{
  
}

MuonSimData::MuonSimData (const std::vector<Deposit>& deposits, 
			    int simDataWord)
    : m_word((deposits.size()<<29 & 0xe0000000) | (simDataWord & 0x1fffffff)), m_x(0.0),m_y(0.0),m_z(0.0),m_t(0.0)
{
    m_deposits = deposits;
}

MuonSimData::MuonSimData (std::vector<Deposit>&& deposits, 
                          int simDataWord)
    : m_word((deposits.size()<<29 & 0xe0000000) | (simDataWord & 0x1fffffff)), m_x(0.0),m_y(0.0),m_z(0.0),m_t(0.0)
{
  m_deposits = std::move(deposits);
}

