/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibIdentifier/MdtRegionTool.h"

#include "MuonIdHelpers/MdtIdHelper.h"

#include <iostream>

MdtRegionTool::MdtRegionTool() 
{

}

bool MdtRegionTool::initialize( const MdtRegionSet& set, const MdtIdHelper* idHelper ) 
{

  // clear what is there
  clear();

  m_regions = set;

  // initialize index vector to correct size
  unsigned int nhashes = idHelper->module_hash_max();
  m_basicHashToRegionHash.resize(nhashes);

  //std::cout << "New basicHashToRegionHash vector with size " <<  m_basicHashToRegionHash.size() << std::endl;

  // count the number of regions
  unsigned int nregion(0);
  

  // loop over MdtRegions

  MdtRegionSet::const_iterator it = set.begin();
  MdtRegionSet::const_iterator it_end = set.end();
  for( ; it!=it_end;++it ){
    MdtRegionHash hashRegion(nregion);

    // loop over Identifiers in Region
    MdtRegion::const_iterator rit = it->begin();
    MdtRegion::const_iterator rit_end = it->end();

    for( ; rit!=rit_end; ++rit ){
      MdtBasicRegionHash hash;
      IdContext idCont = idHelper->module_context();
      idHelper->get_hash( *rit, hash, &idCont );
      
      if( hash >= m_basicHashToRegionHash.size() ){
	//std::cout << "ERROR hash out of range, skipping it " << std::endl;
	continue;
      }
      
      m_basicHashToRegionHash[hash] = hashRegion;
    }

    ++nregion;
  }
  
  //std::cout << "Initialization complete " << std::endl;
  return true;
}


void MdtRegionTool::clear()
{
  m_basicHashToRegionHash.clear();
  m_regions.clear();
}
