/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMEVENTTPCNV_BYTESTREAMMETADATA_P1_H
#define BYTESTREAMEVENTTPCNV_BYTESTREAMMETADATA_P1_H

/** @file ByteStreamEventTPCnv/ByteStreamMetadata_p1.h
 *  @brief This file contains the class definition for the ByteStreamMetadata_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadata_p1.h,v 1.1 2009-03-18 17:42:44 gemmeren Exp $
 **/

#include <string>
#include <vector>
#include <stdint.h>

/** @class ByteStreamMetadata_p1
 *  @brief This class is the persistent representation of ByteStreamMetadata
 **/
class  ByteStreamMetadata_p1 {
public: // Constructor and Destructor
   /// Default Constructor
   ByteStreamMetadata_p1();
   /// Copy Constructor
   ByteStreamMetadata_p1(const ByteStreamMetadata_p1& rhs);
   /// Destructor
   virtual ~ByteStreamMetadata_p1();

public: // Non-static members
   /// Overloaded Assignment Operator
   ByteStreamMetadata_p1& operator=(const ByteStreamMetadata_p1& rhs);

friend class ByteStreamMetadataCnv_p1;

private:
   unsigned int m_runNumber;
   unsigned int m_numEvents;
   unsigned int m_maxEvents;
   unsigned int m_recEnable;
   unsigned int m_triggerType;
   uint64_t m_detectorMask;
   uint64_t m_detectorMask2;
   unsigned int m_beamType;
   unsigned int m_beamEnergy;
   std::string m_guid;
   std::string m_stream;
   std::string m_project;
   unsigned int m_lumiBlock;
   std::vector<std::string> m_freeMetaDataStrings;
};

class  ByteStreamMetadataContainer_p1 : public std::vector<ByteStreamMetadata_p1> {
};

inline ByteStreamMetadata_p1::ByteStreamMetadata_p1() : m_runNumber(0),
	m_numEvents(0),
	m_maxEvents(0),
	m_recEnable(0),
	m_triggerType(0),
	m_detectorMask(0L),
	m_detectorMask2(0L),
	m_beamType(0),
	m_beamEnergy(0),
	m_guid(),
	m_stream(),
	m_project(),
	m_lumiBlock(0),
	m_freeMetaDataStrings() {}
inline ByteStreamMetadata_p1::ByteStreamMetadata_p1(const ByteStreamMetadata_p1& rhs) : m_runNumber(rhs.m_runNumber),
	m_numEvents(rhs.m_numEvents),
	m_maxEvents(rhs.m_maxEvents),
	m_recEnable(rhs.m_recEnable),
	m_triggerType(rhs.m_triggerType),
	m_detectorMask(rhs.m_detectorMask),
	m_detectorMask2(rhs.m_detectorMask2),
	m_beamType(rhs.m_beamType),
	m_beamEnergy(rhs.m_beamEnergy),
	m_guid(rhs.m_guid),
	m_stream(rhs.m_stream),
	m_project(rhs.m_project),
	m_lumiBlock(rhs.m_lumiBlock),
	m_freeMetaDataStrings(rhs.m_freeMetaDataStrings) {}
inline ByteStreamMetadata_p1::~ByteStreamMetadata_p1() {}

inline ByteStreamMetadata_p1& ByteStreamMetadata_p1::operator=(const ByteStreamMetadata_p1& rhs) {
   if (this != &rhs) {
      m_runNumber = rhs.m_runNumber;
      m_numEvents = rhs.m_numEvents;
      m_maxEvents = rhs.m_maxEvents;
      m_recEnable = rhs.m_recEnable;
      m_triggerType = rhs.m_triggerType;
      m_detectorMask = rhs.m_detectorMask;
      m_detectorMask2 = rhs.m_detectorMask2;
      m_beamType = rhs.m_beamType;
      m_beamEnergy = rhs.m_beamEnergy;
      m_guid = rhs.m_guid;
      m_stream = rhs.m_stream;
      m_project = rhs.m_project;
      m_lumiBlock = rhs.m_lumiBlock;
      m_freeMetaDataStrings = rhs.m_freeMetaDataStrings;
   }
   return *this;
}

#endif
