/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TConverterRegistry.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief Registry for Root converters.
 *
 * This class serves as a registry for all known Root converter instances.
 * Register each converter with @c AddConverter.
 */


#include "RootConversions/TConverterRegistry.h"
#include "RootConversions/TVirtualConverter.h"
#include "RootConversions/TConverterStreamer.h"
#include "CxxUtils/checker_macros.h"
#include "TMemberStreamer.h"
#include "TClass.h"
#include "TROOT.h"


void TConvertingBranchElement_init();


/**
 * @brief Add a new converter to the registry.
 * @param conv  The converter to add.
 *
 * This will also add a streamer for the class, to support conversion
 * in non-split mode.
 *
 * The caller will retain ownership of the converter.
 */
void TConverterRegistry::AddConverter (TVirtualConverter* conv)
{
  AddConverter (conv, false);
}


/**
 * @brief Add a new converter to the registry.
 * @param conv  The converter to add.
 * @param takeown If true, the registry takes ownership of the converter.
 *
 * This will also add a streamer for the class, to support conversion
 * in non-split mode.
 */
void TConverterRegistry::AddConverter (TVirtualConverter* conv,
                                       bool takeown)
{
  lock_t lock (fMutex);
  // Make sure the branch element is properly initialized.
  TConvertingBranchElement_init();

  // Look up the checksum->converter map.
  // (This will make an empty one if this is the first time we've
  // seen this name.)
  TClass* cls = conv->GetTransClass();
  CheckSumMap& cmap = fMap[cls->GetName()];

  // If this is the first converter for this transient class,
  // set up a streamer for non-split mode conversions.
  if (cmap.size() == 0)
    cls->AdoptStreamer (new TConverterStreamer (cmap, cls));

  // Add this converter to the map.
  cmap[conv->GetCheckSum()] = std::make_pair (conv, takeown);
}


/**
 * @brief Add a new converter to the registry.
 * @param convname  The name of the converter class to add.
 * @returns True if successful, false if the class couldn't be found.
 *
 * This will also add a streamer for the class, to support conversion
 * in non-split mode.
 */
bool TConverterRegistry::AddConverter (const char* convname)
{
  TClass* cl = gROOT->GetClass (convname);
  if (!cl) return false;
  TClass* basecl = gROOT->GetClass ("TVirtualConverter");
  int offs = cl->GetBaseClassOffset (basecl);
  if (offs < 0) return false;
  char* cnv = reinterpret_cast<char*> (cl->New());
  AddConverter (reinterpret_cast<TVirtualConverter*> (cnv + offs), true);
  return true;
}


/**
 * @brief Look up a converter in the registry by name and checksum.
 * @param name     The name of the (transient) class.
 * @param checksum The checksum of the persistent class.
 * @return The converter, or 0 if none.
 */
TVirtualConverter* TConverterRegistry::GetConverter (const char* name,
                                                     int checksum) const
{
  lock_t lock (fMutex);
  MapType::const_iterator i = fMap.find (name);
  if (i != fMap.end()) {
    CheckSumMap::const_iterator i2 = i->second.find (checksum);
    if (i2 != i->second.end())
      return i2->second.first;
  }
  return 0;
}


void TConverterRegistry::AddStreamerConverter (const std::string& from_type,
                                               const std::string& to_type,
                                               TMemberStreamer* streamer)
{
  lock_t lock (fMutex);
  std::string key = from_type + "-" + to_type;
  SMapType::const_iterator i = fSMap.find (key);
  if (i != fSMap.end())
    delete i->second;
  fSMap[key] = streamer;
}


TMemberStreamer*
TConverterRegistry::GetStreamerConverter (const std::string& from_type,
                                          const std::string& to_type) const
{
  lock_t lock (fMutex);
  std::string key = from_type + "-" + to_type;
  SMapType::const_iterator i = fSMap.find (key);
  if (i != fSMap.end())
    return i->second;
  return 0;
}


/**
 * @brief Return a pointer to the global registry instance.
 */
TConverterRegistry* TConverterRegistry::Instance()
{
  // Do it like this so that the object gets destroyed automatically
  // at program termination.
  static TConverterRegistry instance ATLAS_THREAD_SAFE;
  return &instance;
}


/**
 * @brief Destructor.
 *
 * Destroy the registered converters.
 */
TConverterRegistry::~TConverterRegistry()
{
  for (MapType::iterator i = fMap.begin(); i != fMap.end(); ++i) {
    for (CheckSumMap::iterator j = i->second.begin();
         j != i->second.end();
         ++j)
    {
      if (j->second.second)
        delete j->second.first;
    }
  }

  for (SMapType::iterator i = fSMap.begin(); i != fSMap.end(); ++i)
    delete i->second;
}
