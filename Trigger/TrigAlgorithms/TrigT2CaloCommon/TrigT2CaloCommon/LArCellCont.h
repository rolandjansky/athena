/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArCellCont.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
 
 AUTHOR:   Denis Oliveira Damazio

 PURPOSE:  Container with statically allocated LArCellCollections.
	   To be prepared in the Algorithm initialize and kept up
	   to algorithm finalize.
 *******************************************************************/

#ifndef LARCELLCONT_H
#define LARCELLCONT_H

#include "LArRecEvent/LArCellCollection.h"
#include "TrigT2CaloCommon/LArRodIdHash.h"
#include "LArTools/LArCablingService.h"
#include "LArByteStream/Hid2RESrcID.h"
#include "Identifier/HWIdentifier.h"
#include <vector>

class EventInfo;
class StoreGateSvc;
class ILArBadChannelMasker;
class ILArBadChanTool;
/** Class which contains statically allocated LArCellCollections */
class LArCellCont : public std::vector<LArCellCollection*>
{

 public:

  /** Identifiable definition to be compatible with LArCellIDC */
  typedef LArCellCollection IDENTIFIABLE;

  /** Constructor. Does not do anything. Check initialize. */
  LArCellCont( ) ;

  /** @brief Finds a collection by its LArReadoutModuleID
  *   @param[in] ReadOut Module ID
  *   @return @c const_iterator to a pointer to LArCellCollection.
  */
  const std::vector<LArCellCollection*>::const_iterator&
		find(const HWIdentifier& id) const ;
  /** @brief Finds a collection by its ROD ID
  *   @param[in] Read Out ID as provided by RegionSelector.
  *   @return @c const_iterator to a pointer to LArCellCollection.
  */
  const std::vector<LArCellCollection*>::const_iterator&
		find(const unsigned int& id) const ;
  /** @brief Each Collection contains data from 2 FEBs.
  *   @return for each collection the ID of the second FEB.
  */
  HWIdentifier findsec(const unsigned int& id) const ;

  /** destructor */
  virtual ~LArCellCont() { };

  /** initialize method. Builds all cells and collections. */
  StatusCode initialize( void ) ;
  /** finalize method. Destroys all cells and collections. */
  StatusCode finalize( void ) ;
  /** sets Event Number */
  void eventNumber ( const unsigned int eN ) { m_event=eN; };
  /** List of Missing ROBs */
  const std::vector<uint32_t>& MissingROBs( void ) {
	return m_MissingROBs;
  }

private:    

	/** Hash ID Identifier mapping to collection index. */
	LArRodIdHash m_hash;
	/** Hardware to Source ID conversion */
	mutable Hid2RESrcID m_conv;
	/** FEB Hardware Identifier for second FEBs in a
	    Collection */
	std::vector<HWIdentifier> m_second;
	/** @c const_iterator to LArCellCollections */
        mutable std::vector<LArCellCollection*>::const_iterator m_it;
	/** eventNumber of a given Collection */
	mutable std::vector<unsigned int> m_eventNumber;
	/** this event number */
	unsigned int m_event;
	/** One needs to destroy the TT vectors */
	std::vector<std::vector<LArCell*>*> m_vecs;

	/** A tool to help mask cells */
	ILArBadChannelMasker* m_masker;		
	/** A tool to help mask febs */
	ILArBadChanTool* m_badChanTool;		
	/** List of Missing ROBs to be disabled at the RS */
	std::vector<uint32_t> m_MissingROBs;
};

#endif

