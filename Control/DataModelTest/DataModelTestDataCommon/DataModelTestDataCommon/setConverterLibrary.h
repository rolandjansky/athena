// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/setConverterLibrary.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2014
 * @brief Helper to explicitly set library for a converter.
 */

#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ClassID.h"
#include <string>


#ifndef DATAMODELTESTDATACOMMON_SETCONVERTERLIBRARY_H
#define DATAMODELTESTDATACOMMON_SETCONVERTERLIBRARY_H


namespace DMTest {


void setPluginLibrary ATLAS_NOT_THREAD_SAFE (const std::string& name, const std::string& lib);
void setConverterLibrary ATLAS_NOT_THREAD_SAFE (CLID clid, const std::string& lib);
void setConverterLibrary ATLAS_NOT_THREAD_SAFE (const std::string& lib);
void setTrigConverterLibrary ATLAS_NOT_THREAD_SAFE (CLID clid, const std::string& lib);
void setTrigConverterLibrary ATLAS_NOT_THREAD_SAFE (const std::string& lib);


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_SETCONVERTERLIBRARY_H
