// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/setConverterLibrary.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2014
 * @brief Helper to explicitly set library for a converter.
 */


#include "GaudiKernel/ClassID.h"
#include <string>


#ifndef DATAMODELTESTDATACOMMON_SETCONVERTERLIBRARY_H
#define DATAMODELTESTDATACOMMON_SETCONVERTERLIBRARY_H


namespace DMTest {


void setPluginLibrary (const std::string& name, const std::string& lib);
void setConverterLibrary (CLID clid, const std::string& lib);
void setConverterLibrary (const std::string& lib);
void setTrigConverterLibrary (CLID clid, const std::string& lib);
void setTrigConverterLibrary (const std::string& lib);


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_SETCONVERTERLIBRARY_H
