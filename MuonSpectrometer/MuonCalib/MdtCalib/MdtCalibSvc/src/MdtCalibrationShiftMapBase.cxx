/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// std
#include <sys/stat.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

// other packages
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"
#include "Identifier/Identifier.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "PathResolver/PathResolver.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

// this package
#include "MdtCalibSvc/MdtCalibrationShiftMapBase.h"

#include "TTree.h"
// TRandom3 for random shifting
// should later be replaced with std
#include "TRandom3.h"

//
// private helper functions
//

MdtCalibrationShiftMapBase::MdtCalibrationShiftMapBase(const std::string &name,
                                                       ISvcLocator *pSvcLocator)
    : base_class(name, pSvcLocator),
      m_cablingSvc("MuonMDT_CablingSvc", name),
      m_mapIsInitialized(false),
      m_mapFileName(""),
      m_centralValue(0),
      m_sigma(10),
      m_forceMapRecreate(false) {
  declareProperty("MapFile", m_mapFileName);
  declareProperty("CentralValue", m_centralValue = 0);
  declareProperty("Sigma", m_sigma = 10);
  declareProperty("ForceMapRecreate", m_forceMapRecreate = false);
}

MdtCalibrationShiftMapBase::~MdtCalibrationShiftMapBase() { ; }

// return failure if not overloaded
StatusCode MdtCalibrationShiftMapBase::initializeMap() {
  return StatusCode::FAILURE;
}

StatusCode MdtCalibrationShiftMapBase::dumpMapAsFile() {
  /* initialize map if it's not there */
  if (!m_mapIsInitialized) {
    initializeMap();
  }

  /* write the map to a file */
  {
    std::ofstream file(m_mapFileName.c_str(), std::ios::out | std::ios::trunc);

    /* see if opening the file was successful */
    if (!file.is_open()) {
      ATH_MSG_FATAL(
          "Cannot open map output file for writing. Tried accessing file at "
          "\"./"
          << m_mapFileName.c_str() << "\"");
      return StatusCode::FAILURE;
    }

    /* write header, comments can start with '#' or '//' */
    file << "# This file contains shift values for MDT tubes\n";
    file << "# Each Identifier is mapped to a float\n";
    file << "# Below are comma separated values with formatting\n";
    file << "# Identifier.get_compact(),float\n";
    file << "# ------------------------------------------------\n";

    /* dump map contents */
    for (auto shift : m_shiftValues) {
      Identifier::value_type identifierCompact = shift.first.get_compact();
      float shiftValue = shift.second;
      file << identifierCompact;
      file << ",";
      file << shiftValue;
      file << "\n";
    }
  }  // '}' flushes file
  return StatusCode::SUCCESS;
}

StatusCode MdtCalibrationShiftMapBase::loadMapFromFile() {
  /* check if map was already initialized */
  if (m_mapIsInitialized) {
    ATH_MSG_WARNING("Map already initialized, won't overwrite it.");
    return StatusCode::SUCCESS;
  }

  /* resolve path */
  std::string fileWithPath = PathResolver::find_file(m_mapFileName, "DATAPATH");
  if (fileWithPath == "") {
    ATH_MSG_ERROR("Cannot find file " << m_mapFileName << " in $DATAPATH");
    return StatusCode::FAILURE;
  }

  /* check if the file exists */
  struct stat buffer;
  if (stat(fileWithPath.c_str(), &buffer) != 0) {
    ATH_MSG_ERROR("Cannot stat the file \""
                  << fileWithPath.c_str()
                  << "\" -> map can not be initialized from this file.");
    return StatusCode::FAILURE;
  }

  /* check if the map is already stored */
  std::ifstream fin(fileWithPath.c_str(), std::ios::in);
  std::string line;
  bool initializedWithWarnings = false;
  // get all lines in file
  while (std::getline(fin, line)) {
    // check if file is empty, begins with '#', or '//'
    if (line.empty() || line.compare(0, 1, "#") == 0 ||
        line.compare(0, 2, "//")) {
      continue;
    }
    // need a stringstream for readline
    std::stringstream lineStream(line);
    // get all csv tokens in line
    std::string token;
    std::vector<std::string> tokenVector;
    while (std::getline(lineStream, token, ',')) {
      tokenVector.push_back(token);
    }
    // expect exactly two tokens: identifier and value
    if (tokenVector.size() == 2) {
      // we are careful about the type here
      // watch out for compiler warnings warning about casting ull to value_type
      // they might occur of Identifier::value_type is changed at some point
      Identifier::value_type identifierCompact = std::stoull(tokenVector[0]);
      Identifier id(identifierCompact);
      float shift = std::stof(tokenVector[1]);
      m_shiftValues[id] = shift;
    } else {
      ATH_MSG_WARNING("Unexpected input format in shift map file "
                      << fileWithPath.c_str());
      ATH_MSG_WARNING("Expected 2 tokens, got " << tokenVector.size());
      ATH_MSG_WARNING("Broken line: \"" << line.c_str() << "\"");
      initializedWithWarnings = true;
    }
  }
  m_mapIsInitialized = true;
  if (initializedWithWarnings) {
    ATH_MSG_WARNING("Initialized shift map WITH WARNINGS from file \""
                    << fileWithPath.c_str() << "\"");
  } else {
    ATH_MSG_INFO("Successfully initialized shift map from file \""
                 << fileWithPath.c_str() << "\"");
  }
  return StatusCode::SUCCESS;
}

float MdtCalibrationShiftMapBase::getValue(const Identifier &id) {
  if (!m_mapIsInitialized) {
    StatusCode sc = initializeMap();
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Could not initialize the shift map.");
      throw GaudiException("Cannot run without shift map.", "", sc);
    }
  }

  return m_shiftValues[id];
}
