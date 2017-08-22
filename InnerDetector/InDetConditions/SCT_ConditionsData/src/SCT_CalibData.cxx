/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
//Implementation file for the data object class
//The object is a map of channel number<->map<parameter, value>
// P. Johansson
//----------------------------------------------------------------------
#include "SCT_ConditionsData/SCT_CalibData.h"
#include <utility>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

//----------------------------------------------------------------------
//constructor
SCT_CalibData::SCT_CalibData()
{}

//----------------------------------------------------------------------
//destructor
SCT_CalibData::~SCT_CalibData()
{ }

//----------------------------------------------------------------------
// Add map entries of Calib data off a module
bool SCT_CalibData::addModule(const Identifier & moduleId, const CalibModuleData & moduleData)
{
  return (m_dataMap.insert(make_pair(moduleId, moduleData))).second;
}

//----------------------------------------------------------------------
// Search the maps for a module, returns empty module container if not found
SCT_CalibData::CalibModuleData SCT_CalibData::findModule(const Identifier & moduleId)
{
  // Create container for the calib data
  SCT_CalibData::CalibModuleData ModuleData;
  // Iterate over the map and look for the requested module
  CalibCondData::const_iterator iter(m_dataMap.find(moduleId));
  if (iter not_eq m_dataMap.end()){
    ModuleData  = iter->second;
  }
  return ModuleData;
}

//----------------------------------------------------------------------
// Method to print the map - useful debug tool
std::string SCT_CalibData::str()
{
  ostringstream txt;
  if (m_dataMap.empty()){
    txt<<"No modules are in the SCT Calibration data map.\n";
  } else {
    CalibCondData::const_iterator itr(m_dataMap.begin());
    for(; itr!=m_dataMap.end(); ++itr){
      txt << "\nCalibData for module: " << itr->first << "\n" << moduleText(itr->first);
    }
  }
  return txt.str();
}

//----------------------------------------------------------------------
// Print data for one module
std::string SCT_CalibData::moduleText( const Identifier & moduleId ){
  CalibModuleData thisModuleData = findModule( moduleId );
  ostringstream txt;
  ostringstream tmp;

  if ( thisModuleData.empty() ){
    txt << "The SCT module Calibration data map is empty for module " << moduleId <<".\n";
  } else {
    txt << setw(15) << left << "Parameter: " << setw(10) << "Chip1" << setw(10) << "Chip2" << 
      setw(10) << "Chip3" << setw(10) << "Chip4" << setw(10) << "Chip5" << setw(10) << 
      "Chip6" << setw(10) << "Chip7" << setw(10) << "Chip8" << setw(10) << "Chip9" << 
      setw(10) << "Chip10" << setw(10) << "Chip11" << setw(10) << "Chip12" << "\n";
    CalibModuleData::const_iterator CalibDataItr(thisModuleData.begin());
    for ( ; CalibDataItr not_eq thisModuleData.end(); ++CalibDataItr){
      txt <<  setw(15) << left << CalibDataItr->first;
      std::vector<float> CalibDataValues = CalibDataItr->second;
      for ( unsigned int i = 0; i < CalibDataValues.size(); i++){
	txt << setw(10) << setprecision (4) << left << CalibDataValues[i];
      }    
      txt << "\n";    
    }
  }
  return txt.str();
}

//----------------------------------------------------------------------
// Is the map empty?
bool SCT_CalibData::empty() const{
  return m_dataMap.empty();
}

//----------------------------------------------------------------------
// Clear map
void SCT_CalibData::clear() {
  m_dataMap.clear();
}

//----------------------------------------------------------------------
// The size of the map
unsigned long int SCT_CalibData::size() const{
  return m_dataMap.size();
} 


