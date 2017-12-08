/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibDefectData.h
 * header file for data object
 * @author P. Johansson - 26/10/07
 **/

#ifndef SCT_CALIBDEFECTDATA_H
#define SCT_CALIBDEFECTDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <map>
#include <vector>
#include <string>

// Include Athena stuff
#include "Identifier/Identifier.h"

class SCT_CalibDefectData {
public:

  typedef struct{ 
    std::vector<unsigned int> begDefects;
    std::vector<unsigned int> endDefects;
    std::vector<std::string> typeOfDefect;
    std::vector<float> parValue;
  } CalibModuleDefects;

  // Constructor
  SCT_CalibDefectData();

  // Destructor
  virtual ~SCT_CalibDefectData();

  /// Add defects for a module
  bool addModule( const Identifier & moduleId, const CalibModuleDefects & defects );
 
  /// Search the map for a module
  CalibModuleDefects findModule( const Identifier & moduleId );
  
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

  typedef std::map<Identifier, CalibModuleDefects> CalibConditions;
  CalibConditions m_defectMap;

};

CLASS_DEF( SCT_CalibDefectData , 103608210, 1 )

#endif // SCT_CALIBDEFECTDATA_H
