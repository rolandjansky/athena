/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileTrigger.cxx
//  Author   : Jose Maneira
//  Created  : March 2007
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//  BUGS:
//    
//    
//*****************************************************************************

#include "TileEvent/TileTrigger.h"
#include "TileConditions/TileCablingService.h"

#include "CaloIdentifier/CaloLVL1_ID.h"

#include <iostream>
#include <sstream>
#include <iomanip>

// get cabling
namespace {
const TileCablingService * const s_cabling = TileCablingService::getInstance();
}

TileTrigger::TileTrigger(int id, const Identifier& mtid, double mtsum, 
		std::vector<Identifier>& boardtid, std::vector<double>& boardtsum,
		std::vector<Identifier>& backtid, std::vector<double>& backtsum) :	
  m_ID(id),
  m_maxTowerID (mtid),
  m_maxTowerSum (mtsum),
  m_boardTowerID (boardtid),
  m_boardTowerSum (boardtsum),
  m_backTowerID (backtid),
  m_backTowerSum (backtsum)
  { }
  
TileTrigger::TileTrigger(const Identifier& mtid, double mtsum, 
		std::vector<Identifier>& boardtid, std::vector<double>& boardtsum,
		std::vector<Identifier>& backtid, std::vector<double>& backtsum) :	
  m_ID(0),
  m_maxTowerID (mtid),
  m_maxTowerSum (mtsum),
  m_boardTowerID (boardtid),
  m_boardTowerSum (boardtsum),
  m_backTowerID (backtid),
  m_backTowerSum (backtsum)
  { }
  
void TileTrigger::Set(int id, const Identifier& mtid, double mtsum, 
		std::vector<Identifier>& boardtid, std::vector<double>& boardtsum,
		std::vector<Identifier>& backtid, std::vector<double>& backtsum)
{ 
	SetID(id);
	SetMaxTowerID(mtid);
	SetMaxTowerSum(mtsum);
	SetBoardTowerID(boardtid);
	SetBoardTowerSum(boardtsum);
	SetBackTowerID(backtid);
	SetBackTowerSum(backtsum);
}
	
void TileTrigger::Set(const Identifier& mtid, double mtsum, 
		std::vector<Identifier>& boardtid, std::vector<double>& boardtsum,
		std::vector<Identifier>& backtid, std::vector<double>& backtsum)
{ 
	SetMaxTowerID(mtid);
	SetMaxTowerSum(mtsum);
	SetBoardTowerID(boardtid);
	SetBoardTowerSum(boardtsum);
	SetBackTowerID(backtid);
	SetBackTowerSum(backtsum);
}
	
void TileTrigger::print() const
{
  std::cout << (std::string) (*this) << std::endl;
}

TileTrigger::operator std::string() const
{
  std::ostringstream text(std::ostringstream::out);

  text << whoami();
  text << " Id = " << s_cabling->getCaloLVL1_ID()->show_to_string(m_maxTowerID);
  text << "  maxTowerSum = " << m_maxTowerSum << std::endl;

  std::vector<Identifier>::const_iterator ii1=m_boardTowerID.begin();
  std::vector<Identifier>::const_iterator ii2=m_boardTowerID.end();

  std::vector<double>::const_iterator iv1=m_boardTowerSum.begin();
  std::vector<double>::const_iterator iv2=m_boardTowerSum.end();

  text << "  nTriggerBoards = " << m_boardTowerSum.size();
  for ( ; ii1!=ii2 && iv1!=iv2 ; ++ii1, ++iv1) 
    text << " (" << s_cabling->getCaloLVL1_ID()->show_to_string(*ii1) << ", " << (*iv1) << ")";
  text<<std::endl;
  
  ii1=m_backTowerID.begin();
  ii2=m_backTowerID.end();

  iv1=m_backTowerSum.begin();
  iv2=m_backTowerSum.end();

  text << "  nBack2BackTowers = " << m_backTowerSum.size();
  for ( ; ii1!=ii2 && iv1!=iv2 ; ++ii1, ++iv1) 
    text << " (" << s_cabling->getCaloLVL1_ID()->show_to_string(*ii1) << ", " << (*iv1) << ")";
  text<<std::endl;
  
  return text.str();
}
