/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ByteStreamMetadata.cxx
 *  @brief This file contains the implementation for the ByteStreamMetadata class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadata.cxx,v 1.2 2009-04-02 14:47:41 gemmeren Exp $
 **/

#include "ByteStreamData/ByteStreamMetadata.h"

#include "EventStorage/DataReader.h"

#include <bitset>

/******************************************************************************/
ByteStreamMetadata::ByteStreamMetadata()
  : m_runNumber(0U)
  , m_numEvents(0U)
  , m_maxEvents(0U)
  , m_recEnable(0U)
  , m_triggerType(0U)
  , m_detectorMask(0L)
  , m_detectorMask2(0L)
  , m_beamType(0U)
  , m_beamEnergy(0U)
  , m_guid()
  , m_stream()
  , m_project()
  , m_lumiBlock(0U)
  , m_freeMetaDataStrings()
{}


/******************************************************************************/
ByteStreamMetadata::ByteStreamMetadata(unsigned int runNumber,
    unsigned int numEvents,
    unsigned int maxEvents,
    unsigned int recEnable,
    unsigned int triggerType,
    uint64_t detectorMask,
    unsigned int beamType,
    unsigned int beamEnergy,
    const std::string& guid,
    const std::string& stream,
    const std::string& project,
    unsigned int lumiBlock,
    const std::vector<std::string>& freeMetaDataStrings)
  : m_runNumber(runNumber)
  , m_numEvents(numEvents)
  , m_maxEvents(maxEvents)
  , m_recEnable(recEnable)
  , m_triggerType(triggerType)
  , m_detectorMask(detectorMask)
  , m_detectorMask2(0)
  , m_beamType(beamType)
  , m_beamEnergy(beamEnergy)
  , m_guid(guid)
  , m_stream(stream)
  , m_project(project)
  , m_lumiBlock(lumiBlock)
  , m_freeMetaDataStrings(freeMetaDataStrings)
{}


/******************************************************************************/
ByteStreamMetadata::ByteStreamMetadata(
    unsigned int runNumber,
    unsigned int numEvents,
    unsigned int maxEvents,
    unsigned int recEnable,
    unsigned int triggerType,
    uint64_t detectorMask,
    uint64_t detectorMask2,
    unsigned int beamType,
    unsigned int beamEnergy,
    const std::string& guid,
    const std::string& stream,
    const std::string& project,
    unsigned int lumiBlock,
    const std::vector<std::string>& freeMetaDataStrings)
  : m_runNumber(runNumber)
  , m_numEvents(numEvents)
  , m_maxEvents(maxEvents)
  , m_recEnable(recEnable)
  , m_triggerType(triggerType)
  , m_detectorMask(detectorMask)
  , m_detectorMask2(detectorMask2)
  , m_beamType(beamType)
  , m_beamEnergy(beamEnergy)
  , m_guid(guid)
  , m_stream(stream)
  , m_project(project)
  , m_lumiBlock(lumiBlock)
  , m_freeMetaDataStrings(freeMetaDataStrings)
{}


/******************************************************************************/
ByteStreamMetadata::ByteStreamMetadata(EventStorage::DataReader& reader)
  : m_runNumber          (reader.runNumber())
  , m_numEvents          (reader.eventsInFile())
  , m_maxEvents          (reader.maxEvents())
  , m_recEnable          (reader.recEnable())
  , m_triggerType        (reader.triggerType())
  , m_detectorMask       (0)
  , m_detectorMask2      (0)
  , m_beamType           (reader.beamType())
  , m_beamEnergy         (reader.beamEnergy())
  , m_guid               (reader.GUID())
  , m_stream             (reader.stream())
  , m_project            (reader.projectTag())
  , m_lumiBlock          (reader.lumiblockNumber())
  , m_freeMetaDataStrings(reader.freeMetaDataStrings())
{
  // default goes into ByteStreamMetadata
  std::bitset<64> word1;
  std::bitset<64> word2;
  for (unsigned int i=0; i<64; ++i) {
    word1[i] = reader.detectorMask()[i];
    word2[i] = reader.detectorMask()[i+64];
  }
  m_detectorMask  = word1.to_ulong();
  m_detectorMask2 = word2.to_ulong();
}


/******************************************************************************/
ByteStreamMetadata::~ByteStreamMetadata() {}


/******************************************************************************/
// TODO: inline these getters
unsigned int
ByteStreamMetadata::getRunNumber() const
{return(m_runNumber);}

unsigned int
ByteStreamMetadata::getNumEvents() const
{return(m_numEvents);}

unsigned int
ByteStreamMetadata::getMaxEvents() const
{return(m_maxEvents);}

unsigned int
ByteStreamMetadata::getRecEnable() const
{return(m_recEnable);}

unsigned int
ByteStreamMetadata::getTriggerType() const
{return(m_triggerType);}

uint64_t
ByteStreamMetadata::getDetectorMask() const
{return(m_detectorMask);}

uint64_t
ByteStreamMetadata::getDetectorMask2() const
{return(m_detectorMask2);}

unsigned int
ByteStreamMetadata::getBeamType() const
{return(m_beamType);}

unsigned int
ByteStreamMetadata::getBeamEnergy() const
{return(m_beamEnergy);}

const std::string&
ByteStreamMetadata::getGuid() const
{return(m_guid);}

const std::string&
ByteStreamMetadata::getStream() const
{return(m_stream);}

const std::string&
ByteStreamMetadata::getProject() const
{return(m_project);}

unsigned int
ByteStreamMetadata::getLumiBlock() const
{return(m_lumiBlock);}

const std::vector<std::string>&
ByteStreamMetadata::getFreeMetaDataStrings() const
{return(m_freeMetaDataStrings);}


/******************************************************************************/
std::ostream& operator<<(std::ostream& os, const ByteStreamMetadata& bsmd) {
  std::bitset<64> word1(bsmd.getDetectorMask());
  std::bitset<64> word2(bsmd.getDetectorMask2());

  os << "  runNumber:           " << bsmd.getRunNumber()   << '\n'
     << "  numEvents:           " << bsmd.getNumEvents()   << '\n'
     << "  m_maxEvents:         " << bsmd.getMaxEvents()   << '\n'
     << "  recEnable:           " << bsmd.getRecEnable()   << '\n'
     << "  triggerType:         " << bsmd.getTriggerType() << '\n'
     << "  detectorMask:        " << word1                 << '\n'
     << "  detectorMask2:       " << word2                 << '\n'
     << "  beamType:            " << bsmd.getBeamType()    << '\n'
     << "  beamEnergy:          " << bsmd.getBeamEnergy()  << '\n'
     << "  guid:                " << bsmd.getGuid()        << '\n'
     << "  stream:              " << bsmd.getStream()      << '\n'
     << "  project:             " << bsmd.getProject()     << '\n'
     << "  lumiBlock:           " << bsmd.getLumiBlock()   << '\n';

  os << "  freeMetaDataStrings: ";
  for (const std::string& key : bsmd.getFreeMetaDataStrings())
    os <<  key << "  ";

  return os;
}
