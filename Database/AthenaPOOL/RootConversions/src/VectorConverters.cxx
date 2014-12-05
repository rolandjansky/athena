/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file RootConversions/src/VectorConverters.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009, from previous code.
 * @brief Define converters for some standard vector types,
 *        needed to read old data files and for cross-platform
 *        compatibility.
 */


#include "RootConversions/VectorConverters.h"
#include "RootConversions/VectorConverter.h"
#include "RootConversions/TConverterRegistry.h"

#ifdef XAOD_ANALYSIS
#include "TError.h"
#endif


namespace RootConversions {


/**
 * @brief Register standard vector converters.
 */
void VectorConverters::initialize()
{
#ifdef XAOD_ANALYSIS
long tmpError = gErrorIgnoreLevel;
gErrorIgnoreLevel=kError; //silences the warnings about duplicate entries from vector.dll and vector-bool.dll
#endif
  TConverterRegistry::Instance()->AddStreamerConverter
    ("vector<double>", "vector<float>",
     new RootConversions::VectorConverter<double,float> ("double"));

  TConverterRegistry::Instance()->AddStreamerConverter
    ("vector<unsigned long>", "vector<unsigned long long>",
     new RootConversions::VectorConverter<unsigned long,unsigned long long>
       ("unsigned long"));

  TConverterRegistry::Instance()->AddStreamerConverter
    ("vector<unsigned long long>", "vector<unsigned long>",
     new RootConversions::VectorConverter<unsigned long long,unsigned long>
       ("unsigned long long"));

  TConverterRegistry::Instance()->AddStreamerConverter
    ("vector<unsigned short>", "vector<int>",
     new RootConversions::VectorConverter<unsigned short,int>
       ("unsigned short"));

  TConverterRegistry::Instance()->AddStreamerConverter
    ("vector<unsigned short>", "vector<long long>",
     new RootConversions::VectorConverter<unsigned short,long long>
       ("unsigned short"));

  TConverterRegistry::Instance()->AddStreamerConverter
    ("vector<unsigned short>", "vector<Long64_t>",
     new RootConversions::VectorConverter<unsigned short,Long64_t>
       ("unsigned short"));

  TConverterRegistry::Instance()->AddStreamerConverter
    ("vector<unsigned int>", "vector<ULong64_t>",
     new RootConversions::VectorConverter<unsigned int,ULong64_t>
       ("unsigned int"));
#ifdef XAOD_ANALYSIS
gErrorIgnoreLevel=tmpError;
#endif
}


} // namespace RootConversions
