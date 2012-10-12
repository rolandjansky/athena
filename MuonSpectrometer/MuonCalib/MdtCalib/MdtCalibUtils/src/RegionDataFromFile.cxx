/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibUtils/RegionDataFromFile.h"
#define MAX_VALUE 100000

namespace MuonCalib {

  RegionDataFromFile::id_vec RegionDataFromFile::getIdentifiers(int regionId) const
  {
    if( regionId >= 0 && regionId < (int)m_identifiersPerRegions.size() ){
      return m_identifiersPerRegions[regionId];
    }else{
      std::cout << "RegionDataFromFile::getIdentifiers: WARNING <index out of range> " 
		<< regionId << " size " << m_identifiersPerRegions.size() << std::endl;
      return id_vec();
    }
  }

  bool RegionDataFromFile::addRegion( int regionId, const id_vec& ids )
  {
    if( regionId >= 0 && regionId < (int)m_identifiersPerRegions.size() ){
      m_identifiersPerRegions[regionId] = ids;
      return true;
    }else{
      std::cout << "RegionDataFromFile::addRegion: WARNING <index out of range> " 
		<< regionId << " size " << m_identifiersPerRegions.size() << std::endl;
      return false;      
    }    
  }

  void RegionDataFromFile::clear() 
  {
    m_nregions = 0;
    m_identifiersPerRegions.clear();
  }

  std::istream& RegionDataFromFile::read(std::istream& is)
  {
    // clear vector before starting to read  
    m_identifiersPerRegions.clear(); 

    std::string dummy; 

    // read header containing the total number of regions
    is >> dummy >> m_nregions;
    m_nregions = (m_nregions > MAX_VALUE) ? MAX_VALUE : m_nregions;
    // loop over all regions in file
    int regionId,nelements;
    for( int i=0;i<static_cast<int>(m_nregions);++i){
      // read region header containing the region id and the number of identifiers
      is >> dummy >> regionId >> nelements;
      nelements = (nelements > MAX_VALUE) ? MAX_VALUE : nelements;
      if( regionId != i ){
	std::cout << "RegionDataFromFile::read ERROR <read wrong regionId> " 
		  << regionId << " current " << i << std::endl;
	std::cout << "Aborting reading " << std::endl;
	clear();
	return is;
      }      

      id_vec ids;
      ids.reserve(nelements);

      // loop over identifiers in file 
      int s,e,p,m;
      for(int j=0;j<nelements;++j){
	// read stName,eta,phi,ml
	is >> s >> e >> p >> m;
	ids.push_back( MultilayerId(s,e,p,m) );
      }

      m_identifiersPerRegions.push_back(ids);
    }
    
    return is;
  }
  
  std::ostream& RegionDataFromFile::write(std::ostream& os) const
  {
    // write header 
    os << "Regions " << m_identifiersPerRegions.size() << std::endl;
    
    // write regions
    for( unsigned int i=0;i<m_identifiersPerRegions.size();++i){
      unsigned int ids = m_identifiersPerRegions[i].size();
      os << "RegionId " << i << " " << ids << std::endl;

      // write ids
      for( unsigned int j=0;j<ids;++j){
	MultilayerId id = m_identifiersPerRegions[i][j];
	os << "   " << id.stName << " " << id.eta << " " << id.phi << " " << id.ml << std::endl;
      }
    }
    return os;
  }

}

std::istream& operator>> (std::istream& is, MuonCalib::RegionDataFromFile& data)
{
  return data.read(is);
}

std::ostream& operator<< (std::ostream& os, const MuonCalib::RegionDataFromFile& data)
{
  return data.write(os);
}
