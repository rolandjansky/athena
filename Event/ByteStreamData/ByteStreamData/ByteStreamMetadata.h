/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMDATA_BYTESTREAMMETADATA_H
#define BYTESTREAMDATA_BYTESTREAMMETADATA_H

/** @file ByteStreamMetadata.h
 *  @brief This file contains the class definition for the ByteStreamMetadata class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  @author Frank Berghaus <fberghaus@anl.gov>
 *  $Id: ByteStreamMetadata.h,v 1.2 2009-04-02 14:47:40 gemmeren Exp $
 **/

#include <vector>
#include <string>
#include <stdint.h>
#include <iostream>

namespace EventStorage { class DataReader; }

/** @class ByteStreamMetadata 
 *  @brief This class is the StoreGate data object for bytestream metadata. 
 **/
class ByteStreamMetadata {
public:


  ByteStreamMetadata();


  /**
   * Constructure for 64 bit det mask constructor
   */
  ByteStreamMetadata(
      unsigned int       runNumber,
      unsigned int       numEvents,
      unsigned int       maxEvents,
      unsigned int       recEnable,
      unsigned int       triggerType,
      uint64_t           detectorMask,
      unsigned int       beamType,
      unsigned int       beamEnergy,
      const std::string& guid,
      const std::string& stream,
      const std::string& project,
      unsigned int       lumiBlock,
      const std::vector<std::string>& freeMetaDataStrings);


  /**
   * Constructor using 128 bit det mask
   */
  ByteStreamMetadata(
      unsigned int       runNumber,
      unsigned int       numEvents,
      unsigned int       maxEvents,
      unsigned int       recEnable,
      unsigned int       triggerType,
      uint64_t           detectorMask,
      uint64_t           detectorMask2,
      unsigned int       beamType,
      unsigned int       beamEnergy,
      const std::string& guid,
      const std::string& stream,
      const std::string& project,
      unsigned int       lumiBlock,
      const std::vector<std::string>& freeMetaDataStrings);

  /**
   * Construct bytestream metadata directly from data reader provided by online
   * EventStorage
   */
  ByteStreamMetadata(EventStorage::DataReader& reader);


  virtual ~ByteStreamMetadata();


  unsigned int       getRunNumber() const;
  unsigned int       getNumEvents() const;
  unsigned int       getMaxEvents() const;
  unsigned int       getRecEnable() const;
  unsigned int       getTriggerType() const;
  uint64_t           getDetectorMask() const;
  uint64_t           getDetectorMask2() const;
  unsigned int       getBeamType() const;
  unsigned int       getBeamEnergy() const;
  const std::string& getGuid() const;
  const std::string& getStream() const;
  const std::string& getProject() const;
  unsigned int       getLumiBlock() const;
  const std::vector<std::string>& getFreeMetaDataStrings() const;


  /**
   * pretty print bytestream metadata content
   */
  friend std::ostream& operator<<(std::ostream& os,
      const ByteStreamMetadata& bsmd);


private:
  unsigned int m_runNumber;
  unsigned int m_numEvents;
  unsigned int m_maxEvents;
  unsigned int m_recEnable;
  unsigned int m_triggerType;
  uint64_t     m_detectorMask;
  uint64_t     m_detectorMask2;
  unsigned int m_beamType;
  unsigned int m_beamEnergy;
  std::string  m_guid;
  std::string  m_stream;
  std::string  m_project;
  unsigned int m_lumiBlock;
  std::vector<std::string> m_freeMetaDataStrings;
};


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(ByteStreamMetadata, 170625359, 1)
#endif
