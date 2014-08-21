// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file RootConversions/VectorConverters.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009, from previous code.
 * @brief Define converters for some standard vector types,
 *        needed to read old data files and for cross-platform
 *        compatibility.
 *
 *        vector<double> -> vector<float>
 *        vector<unsigned long> -> vector<unsigned long long>
 *        vector<unsigned long long> -> vector<unsigned long>
 */


#ifndef ROOTCONVERSIONS_VECTORCONVERTERS_H
#define ROOTCONVERSIONS_VECTORCONVERTERS_H


namespace RootConversions {


/**
 * @brief Register standard vector converters.
 *
 * This is a class rather than a free function so that it can be autoloaded.
 */
class VectorConverters
{
public:
  /// Register all CLHEP converters.
  static void initialize();
};


} // namespace RootConversions


#endif // not ROOTCONVERSIONS_VECTORCONVERTERS_H
