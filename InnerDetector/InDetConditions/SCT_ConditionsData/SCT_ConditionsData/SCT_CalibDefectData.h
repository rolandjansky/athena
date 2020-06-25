// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibDefectData.h
 * header file for data object
 * @author P. Johansson
 * @date 26/10/07
 **/

#ifndef SCT_CALIBDEFECTDATA_H
#define SCT_CALIBDEFECTDATA_H

// Include Athena stuff
#include "Identifier/Identifier.h"

#include <map>
#include <string>
#include <vector>

/**
 * @class SCT_CalibDefectData
 * @brief Container with a list of defects derived from calibration data
 * and used in SCT_ReadCalibDataCondAlg and SCT_ReadCalibDataTool.
 **/
class SCT_CalibDefectData {
public:

  typedef struct{ 
    std::vector<unsigned int> begDefects;
    std::vector<unsigned int> endDefects;
    std::vector<std::string> typeOfDefect;
    std::vector<float> parValue;
  } CalibModuleDefects;

  /// Constructor
  SCT_CalibDefectData();

  /// Destructor
  virtual ~SCT_CalibDefectData() = default;

  /// Add defects for a module
  bool addModule( const Identifier & moduleId, const CalibModuleDefects & defects );
 
  /// Search the map for a module
  CalibModuleDefects findModule( const Identifier & moduleId ) const;
  
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

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_CalibDefectData , 103608210, 1 )

// Condition container definition for CondInputLoader
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_CalibDefectData , 1073500 );

#endif // SCT_CALIBDEFECTDATA_H
