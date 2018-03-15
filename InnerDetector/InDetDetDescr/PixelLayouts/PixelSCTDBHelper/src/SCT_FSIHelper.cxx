/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelSCTDBHelper/SCT_FSIHelper.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>

FSILocation:: FSILocation(std::string name,
			  double radius,
			  double phi,
			  int side)
  : m_name(name),
    m_radius(radius),
    m_phi(phi),
    m_side(side)
{}


FSIDetails::FSIDetails(const FSILocation * location, 
		       int simType, 
		       const std::string & simTypeString, 
		       const std::string & locationType, 
		       const std::string & actualType)
  : m_location(location), 
    m_simType(simType),
    m_simTypeStr(simTypeString), 
    m_locType(locationType), 
    m_actualType(actualType)
{}




FSIHelper::FSIHelper(const OraclePixGeoAccessor* geoAccessor)
  : m_geoAccessor(geoAccessor)
{

  m_fwdFSILocation = geoAccessor->getTable("SctFwdFSILocation");
  m_fwdFSIType = geoAccessor->getTable("SctFwdFSIType");
  m_fwdFSI = geoAccessor->getTable("SctFwdFSI");
  m_fwdGeneral = geoAccessor->getTable("SctFwdGeneral");

  int numWheels =  m_geoAccessor->getInt(m_fwdGeneral,"NUMWHEELS");
  m_wheelLocMap.resize(numWheels);
  fill();
}

FSIHelper::~FSIHelper()
{
  std::map<std::string, FSILocation *>::iterator iter;
  for (iter = m_locationTypes.begin(); iter != m_locationTypes.end(); ++iter) {
    delete iter->second;
  }

  for(unsigned int i = 0; i < m_wheelLocMap.size(); i++) {
    if (m_wheelLocMap[i]) {
      for (unsigned int j = 0; j < m_wheelLocMap[i]->size(); j++) {
	delete (*m_wheelLocMap[i])[j];
      }
      delete m_wheelLocMap[i];
    }
  }
}


void
FSIHelper::fill()
{

  // Loop through location types
  for (int iLocIndex = 0; iLocIndex < (int)m_geoAccessor->getTableSize(m_fwdFSILocation); iLocIndex++) {
    std::string locType =  m_geoAccessor->getString(m_fwdFSILocation,"LOCTYPE",iLocIndex);
    double radius =  m_geoAccessor->getDouble(m_fwdFSILocation,"LOCR",iLocIndex) * CLHEP::mm;
    double rphi = m_geoAccessor->getDouble(m_fwdFSILocation,"LOCPHI",iLocIndex) * CLHEP::deg;
    int side =  m_geoAccessor->getInt(m_fwdFSILocation,"SIDE",iLocIndex);
    FSILocation * location = new  FSILocation(locType, radius, rphi, side);
    m_locationTypes[locType] = location;
  }

  // Loop through all fsi's
  for (int index = 0; index < (int)m_geoAccessor->getTableSize(m_fwdFSI); index++) {
    unsigned int iWheel = m_geoAccessor->getInt(m_fwdFSI,"WHEEL",index) - 1;
    int simType = m_geoAccessor->getInt(m_fwdFSI,"SIMTYPE",index);
    std::string simTypeString = m_geoAccessor->getString(m_fwdFSIType,"TYPENAME",simType);
    int simTypeCheck = m_geoAccessor->getInt(m_fwdFSIType,"SIMTYPE",simType);
    std::string locationType =  m_geoAccessor->getString(m_fwdFSI,"LOCTYPE",index);
    std::string actualType =  m_geoAccessor->getString(m_fwdFSI,"ACTUALTYPE",index);

    if (simTypeCheck != simType)  std::cout << "Error in simType to index match in table SctFwdFSIType" << std::endl; 

    FSILocation * location = m_locationTypes[locationType];
    if (!location) std::cout << "Error filling FSI information. No FSI of type " <<  locationType << " found" << std::endl;
    FSIDetails * fsi = new FSIDetails(location, simType, simTypeString, locationType, actualType);
    
    if (iWheel >= m_wheelLocMap.size()) 
      std::cout << "Error in FSIHelper: wheel number is out of range: " << iWheel << std::endl;
    
    if (!m_wheelLocMap[iWheel]) {
      m_wheelLocMap[iWheel] = new std::vector<const FSIDetails *>;
    }
    m_wheelLocMap[iWheel]->push_back(fsi);
    
  }
}


const std::vector<const FSIDetails *> &
FSIHelper::fsiVector(int iWheel) const
{
  return *(m_wheelLocMap[iWheel]);
}
  

  
