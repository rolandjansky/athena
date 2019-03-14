/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SCT_GeoModel/SCT_FSIHelper.h"
#include "SCT_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

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


FSIHelper::FSIHelper(SCT_DataBase * rdb)
  : m_rdb(rdb)
{
  int numWheels =  m_rdb->fwdGeneral()->getInt("NUMWHEELS");
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
  for (int iLocIndex = 0; iLocIndex < m_rdb->fwdFSILocationSize(); iLocIndex++) {
    std::string locType =  m_rdb->fwdFSILocation(iLocIndex)->getString("LOCTYPE");
    double radius =  m_rdb->fwdFSILocation(iLocIndex)->getDouble("LOCR") * Gaudi::Units::mm;
    double rphi = m_rdb->fwdFSILocation(iLocIndex)->getDouble("LOCPHI") * Gaudi::Units::deg;
    int side =  m_rdb->fwdFSILocation(iLocIndex)->getInt("SIDE");
    FSILocation * location = new  FSILocation(locType, radius, rphi, side);
    m_locationTypes[locType] = location;
  }

  // Loop through all fsi's
  for (int index = 0; index < m_rdb->fwdFSISize(); index++) {
    unsigned int iWheel = m_rdb->fwdFSI(index)->getInt("WHEEL") - 1;
    int simType = m_rdb->fwdFSI(index)->getInt("SIMTYPE");
    std::string simTypeString = m_rdb->fwdFSIType(simType)->getString("TYPENAME");
    int simTypeCheck = m_rdb->fwdFSIType(simType)->getInt("SIMTYPE");
    std::string locationType =  m_rdb->fwdFSI(index)->getString("LOCTYPE");
    std::string actualType =  m_rdb->fwdFSI(index)->getString("ACTUALTYPE");

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
  

  
