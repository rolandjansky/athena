// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TConverterRegistry.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief Registry for Root converters.
 *
 * There are two types of converters which we manage.
 * The first type is that used by @c TConvertingBranchElement.
 * This can be used when the definition of a class changes
 * but its name stays the same.  These converters are registered
 * with @c AddConverter.
 *
 * The second type is that used by @c TConvertingStreamerInfo.
 * This can handle some conversions in which the type name
 * changes between transient and persistent versions.
 * These converters are registered with @c AddStreamerConverter.
 */


#ifndef TCONVERTERREGISTRY_H
#define TCONVERTERREGISTRY_H


#include <string>
#include <map>
#include <mutex>
#include "Rtypes.h"
class TVirtualConverter;
class TMemberStreamer;


/**
 * @brief Registry for Root converters.
 */
class TConverterRegistry
{
public:
  /**
   * @brief Return a pointer to the global registry instance.
   */
  static TConverterRegistry* Instance();


  //==========================================================================
  // @c TConvertingBranchElement converters.
  //

  /**
   * @brief Add a new converter to the registry.
   * @param conv  The converter to add.
   *
   * This will also add a streamer for the class, to support conversion
   * in non-split mode.
   *
   * The caller will retain ownership of the converter.
   */
  void AddConverter (TVirtualConverter* conv);

  /**
   * @brief Add a new converter to the registry.
   * @param conv  The converter to add.
   * @param takeown If true, the registry takes ownership of the converter.
   *
   * This will also add a streamer for the class, to support conversion
   * in non-split mode.
   */
  void AddConverter (TVirtualConverter* conv, bool takeown);

  /**
   * @brief Add a new converter to the registry.
   * @param convname  The name of the converter class to add.
   * @returns True if successful, false if the class couldn't be found.
   *
   * This will also add a streamer for the class, to support conversion
   * in non-split mode.
   */
  bool AddConverter (const char* convname);

  /**
   * @brief Look up a converter in the registry by name and checksum.
   * @param name     The name of the (transient) class.
   * @param checksum The checksum of the persistent class.
   * @return The converter, or 0 if none.
   */
  TVirtualConverter* GetConverter (const char* name, int checksum) const;


  //==========================================================================
  // @c TConvertingStreamerInfo converters.
  //

  void AddStreamerConverter (const std::string& from_type,
                             const std::string& to_type,
                             TMemberStreamer* streamer);

  TMemberStreamer* GetStreamerConverter (const std::string& from_type,
                                         const std::string& to_type) const;


private:
  /// Destructor.
  ~TConverterRegistry();

  // We store a map of names to maps of checksums to converters.
  // The bool flag tells whether or not we own this converter.
  typedef std::pair<TVirtualConverter*, bool> Payload;
  typedef std::map<UInt_t, Payload> CheckSumMap;
  typedef std::map<std::string, CheckSumMap> MapType;

  /// Map of registered converters.
  MapType fMap;

  /// Protect access to the map.
  mutable std::mutex fMutex;
  using lock_t = std::lock_guard<std::mutex>;

  /// Streamer converters.
#ifndef __CLING__
  typedef std::map<std::string, TMemberStreamer*> SMapType;
  SMapType fSMap;
#endif
};


#endif // not TCONVERTERREGISTRY_H
