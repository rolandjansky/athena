// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TConverterStreamer.h,v 1.3 2008-11-04 12:42:10 ssnyder Exp $
/**
 * @file TConverterStreamer.h
 * @author scott snyder <snyder@bnl.gov>, from code by Marcin Nowak.
 * @date Dec, 2006
 * @brief Root streamer that calls our converters when reading
 *        in non-split mode.
 *
 * When we read objects with a converter in non-split mode, we get
 * root to call the converter by installing a custom streamer.
 * For writing, it will just do the normal thing.
 * For reading, it checks the persistent class checksum.
 * If there's a match with one of our registered converters, then
 * rather than trying to read the object directly, we instead
 * read it into a temporary instance of the persistent class,
 * then call the converter to change it to a transient object.
 *
 * Note that as long as the transient class has a ShowMembers method,
 * then will not prevent the class from being split on output.
 */


#ifndef TCONVERTERSTREAMER_H
#define TCONVERTERSTREAMER_H

#include "TClassStreamer.h"
#include "TVirtualConverter.h"
#include <map>
class TClass;
class TBuffer;
class TFile;

/**
 * @brief Root streamer that calls our converter when reading
 *        in non-split mode.
 */
class TConverterStreamer
  : public TClassStreamer
{
public:
  typedef std::pair<TVirtualConverter*, bool> Payload;
  typedef std::map<UInt_t, Payload> CheckSumMap;

  /**
   * @brief Constructor.
   * @param convmap  Map from checksums to converter instances.
   * @param cls      The transient class that we're reading.
   */
  TConverterStreamer (const CheckSumMap& convmap, TClass* cls);

  /**
   * @brief Standard Root streamer interface.
   * @param b   Buffer for Root I/O.
   * @param obj Object being read or written.
   */
  virtual void operator() (TBuffer& b, void* obj);


private:
  /**
   * @brief Read the initial byte count/version from an object.
   * @param buf The Root I/O buffer.
   * @param[out] startpos The starting position of the object.
   * @param[out] bcnt     The length of the object, in bytes.
   * @param[out] version  The version of the object being read.
   *
   * As a side effect, this sets @c fStreamerChecksum.
   */
  void FindVersion(TBuffer& buf,
                   UInt_t& startpos,
                   UInt_t& bcnt,
                   Version_t& version);

  /**
   * @brief Translate a Root checksum to a class version number.
   * @param cl The class being examined.
   * @param checksum The checksum to translate.
   * @return The class version number of @c cl with checksum @c checksum,
   *         or 0 if there's no match.
   */
  Version_t R__FindStreamerInfoVersion(const TClass* cl, UInt_t checksum);

  /// The checksum -> converter map passed to the constructor.
  const CheckSumMap& fConvmap;

  /// The transient class we're reading.
  TClass* fClass;

  /// Cached checksum for the last object read.
  UInt_t fStreamerChecksum;

  /// Cached file for the last object read.
  TFile* fLastFile;
};


#endif // not TCONVERTERSTREAMER_H

