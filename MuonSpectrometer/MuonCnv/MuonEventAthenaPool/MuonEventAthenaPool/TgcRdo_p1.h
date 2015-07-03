/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDO_P1_H
#define MUON_TGCRDO_P1_H

/*

Persistent represenation of the TgcRdo class
Author: Marcin Nowak
        CERN, January, 2006

*/

#include <vector>
#include "MuonEventAthenaPool/TgcRawData_p1.h"


class TgcRdo_p1
   : public std::vector<TgcRawData_p1>
{
public:

  // Default constructor 
  TgcRdo_p1 () :
	m_id(0),  m_subDetectorId(0),  m_rodId(0),
	m_triggerType(0),  m_bcId(0),  m_l1Id(0)
  { }

  // get methods
  // Identifier
  uint16_t identify() 		const {return m_id;}
  
  uint16_t subDetectorId() 	const {return m_subDetectorId;}
  uint16_t rodId()        	const {return m_rodId;}
  uint16_t triggerType()  	const {return m_triggerType;}
  uint16_t bcId()          	const {return m_bcId;}
  uint16_t l1Id()          	const {return m_l1Id;}

friend class TgcRdoCnv_p1;
  
private:
 // ID of this instance
  uint16_t m_id;

  // online IDs
  uint16_t m_subDetectorId;
  uint16_t m_rodId;

  // Trigger Type
  uint16_t m_triggerType;

  // BCID and L1ID on ROD
  uint16_t m_bcId;
  uint16_t m_l1Id;

};

#endif





