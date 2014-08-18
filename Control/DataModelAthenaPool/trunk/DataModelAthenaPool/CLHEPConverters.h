// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelAthenaPool/CLHEPConverters.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009, from previous code.
 * @brief Define converters for CLHEP types needed to read old data files.
 */


#ifndef DATAMODELATHENAPOOL_CLHEPCONVERTERS_H
#define DATAMODELATHENAPOOL_CLHEPCONVERTERS_H


namespace DataModelAthenaPool {


/**
 * @brief Register all CLHEP converters.
 *
 * This is a class rather than a free function so that it can be autoloaded.
 */
class CLHEPConverters
{
public:
  /// Register all CLHEP converters.
  static void initialize();
};


} // namespace DataModelAthenaPool


#endif // not DATAMODELATHENAPOOL_CLHEPCONVERTERS_H
