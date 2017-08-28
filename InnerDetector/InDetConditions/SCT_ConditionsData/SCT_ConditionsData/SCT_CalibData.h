/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibData.h
 * header file for data object
 * @author P. Johansson - 26/10/07
 **/

#ifndef SCT_CALIBDATA_H
#define SCT_CALIBDATA_H

#include "CLIDSvc/CLASS_DEF.h"

#include <map>
#include <vector>
#include <string>

// Include Athena stuff
#include "Identifier/Identifier.h"

class SCT_CalibData {
 public:
  
  /// Calib module data
  typedef std::map<std::string, std::vector<float> > CalibModuleData;
  
  // Constructor
  SCT_CalibData();
  
  // Destructor
  virtual ~SCT_CalibData();

  /// Add defects for a module
  bool addModule( const Identifier & moduleId, const CalibModuleData & data );
 
  /// Search the map for a module
  CalibModuleData findModule( const Identifier & moduleId );
  
  /// Print the map - might be useful
  std::string str();

  /// Print data for one module
  std::string moduleText( const Identifier & moduleId );
  
  /// Return whether the map is empty
  bool empty() const;
  
  /// Clear the map
  void clear();

  /// Return the size of the map
  unsigned long int size() const;


private:

  typedef std::map<Identifier, CalibModuleData> CalibCondData;
  CalibCondData m_dataMap;

};

CLASS_DEF( SCT_CalibData, 249008643, 1 )

#endif // SCT_CALIBDATA_H
