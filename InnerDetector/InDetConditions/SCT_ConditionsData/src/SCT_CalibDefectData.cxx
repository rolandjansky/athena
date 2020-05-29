/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
//Implementation file for the data object class
//The object is a map of channel number<->status and defect type/values
// P. Johansson
//----------------------------------------------------------------------
#include "SCT_ConditionsData/SCT_CalibDefectData.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

//----------------------------------------------------------------------
//constructor
SCT_CalibDefectData::SCT_CalibDefectData():
  m_defectMap{}
{}

//----------------------------------------------------------------------
// Add map entries of defects on module
bool SCT_CalibDefectData::addModule(const Identifier & moduleId, const CalibModuleDefects & moduleDefects)
{
  return (m_defectMap.insert(std::make_pair(moduleId, moduleDefects))).second;
}

//----------------------------------------------------------------------
// Search the maps for a module, returns empty module container if not found
SCT_CalibDefectData::CalibModuleDefects SCT_CalibDefectData::findModule(const Identifier & moduleId) const
{
  // Create container for the defect data
  SCT_CalibDefectData::CalibModuleDefects ModuleDefects;
  // Iterate over the map and look for the requested module
  CalibConditions::const_iterator iter(m_defectMap.find(moduleId));
  if (iter not_eq m_defectMap.end()){
    ModuleDefects  = iter->second;
  }
  return ModuleDefects;
}

//----------------------------------------------------------------------
// Method to print the map - useful debug tool
std::string SCT_CalibDefectData::str()
{
  std::ostringstream txt;
  if (m_defectMap.empty()){
    txt << "No modules are in the SCT Calibration defect data map.\n";
  } else {
    CalibConditions::const_iterator itr(m_defectMap.begin());
    for (; itr!=m_defectMap.end(); ++itr){
      txt << itr->first << "  " << moduleText(itr->first);
    }
  }
  return txt.str();
}

//----------------------------------------------------------------------
// Print data for one module
std::string SCT_CalibDefectData::moduleText( const Identifier & moduleId ){
  CalibModuleDefects thisModuleDefects = findModule( moduleId );
  std::ostringstream txt;
  std::ostringstream tmp;

  if ( thisModuleDefects.begDefects.empty() ){
    txt << "The SCT module Calibration defect data map is empty for module " << moduleId <<".\n";
  } else {
    txt << "\n" << std::left << std::setw(15) << "Bad strips" <<  std::setw(15) << "Defect type" << std::setw(15) << "Value:" << "\n";
    for ( unsigned int i = 0; i < thisModuleDefects.begDefects.size(); i++){
      if ( thisModuleDefects.begDefects[i] == thisModuleDefects.endDefects[i] ){
        txt << std::left << std::setw(15) << thisModuleDefects.begDefects[i];
      }
      else{
        tmp << thisModuleDefects.begDefects[i] << " - " << thisModuleDefects.endDefects[i];
        txt << std::left << std::setw(15) << tmp.str();
      }
      txt << std::left << std::setw(15) << thisModuleDefects.typeOfDefect[i];
      txt << std::left << std::setw(15) << thisModuleDefects.parValue[i] << "\n";
      tmp.str("");
    }
  }
  return txt.str();
}

//----------------------------------------------------------------------
// Is the map empty?
bool SCT_CalibDefectData::empty() const{
  return m_defectMap.empty();
}

//----------------------------------------------------------------------
// Clear map
void SCT_CalibDefectData::clear() {
  m_defectMap.clear();
}

//----------------------------------------------------------------------
// The size of the map
unsigned long int SCT_CalibDefectData::size() const{
  return m_defectMap.size();
} 


