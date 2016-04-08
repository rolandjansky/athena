/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ByteStreamMetadata.cxx
 *  @brief This file contains the implementation for the ByteStreamMetadata class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadata.cxx,v 1.2 2009-04-02 14:47:41 gemmeren Exp $
 **/

#include "ByteStreamData/ByteStreamMetadata.h"

//______________________________________________________________________________
ByteStreamMetadata::ByteStreamMetadata() : m_runNumber(0U),
	m_numEvents(0U),
	m_maxEvents(0U),
	m_recEnable(0U),
	m_triggerType(0U),
	m_detectorMask(0L),
	m_detectorMask2(0L),
	m_beamType(0U),
	m_beamEnergy(0U),
	m_guid(),
	m_stream(),
	m_project(),
	m_lumiBlock(0U),
	m_freeMetaDataStrings() {
}
//______________________________________________________________________________
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
	const std::vector<std::string>& freeMetaDataStrings) : m_runNumber(runNumber),
		m_numEvents(numEvents),
		m_maxEvents(maxEvents),
		m_recEnable(recEnable),
		m_triggerType(triggerType),
		m_detectorMask(detectorMask),
		m_detectorMask2(0),
		m_beamType(beamType),
		m_beamEnergy(beamEnergy),
		m_guid(guid),
		m_stream(stream),
		m_project(project),
		m_lumiBlock(lumiBlock),
		m_freeMetaDataStrings(freeMetaDataStrings) {
}
//______________________________________________________________________________
ByteStreamMetadata::ByteStreamMetadata(unsigned int runNumber,
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
	const std::vector<std::string>& freeMetaDataStrings) : m_runNumber(runNumber),
		m_numEvents(numEvents),
		m_maxEvents(maxEvents),
		m_recEnable(recEnable),
		m_triggerType(triggerType),
		m_detectorMask(detectorMask),
		m_detectorMask2(detectorMask2),
		m_beamType(beamType),
		m_beamEnergy(beamEnergy),
		m_guid(guid),
		m_stream(stream),
		m_project(project),
		m_lumiBlock(lumiBlock),
		m_freeMetaDataStrings(freeMetaDataStrings) {
}
//______________________________________________________________________________
ByteStreamMetadata::~ByteStreamMetadata() {
}
unsigned int ByteStreamMetadata::getRunNumber() const {
   return(m_runNumber);
}
unsigned int ByteStreamMetadata::getNumEvents() const {
   return(m_numEvents);
}
unsigned int ByteStreamMetadata::getMaxEvents() const {
   return(m_maxEvents);
}
unsigned int ByteStreamMetadata::getRecEnable() const {
   return(m_recEnable);
}
unsigned int ByteStreamMetadata::getTriggerType() const {
   return(m_triggerType);
}
uint64_t ByteStreamMetadata::getDetectorMask() const {
   return(m_detectorMask);
}
uint64_t ByteStreamMetadata::getDetectorMask2() const {
   return(m_detectorMask2);
}
unsigned int ByteStreamMetadata::getBeamType() const {
   return(m_beamType);
}
unsigned int ByteStreamMetadata::getBeamEnergy() const {
   return(m_beamEnergy);
}
const std::string& ByteStreamMetadata::getGuid() const {
   return(m_guid);
}
const std::string& ByteStreamMetadata::getStream() const {
   return(m_stream);
}
const std::string& ByteStreamMetadata::getProject() const {
   return(m_project);
}
unsigned int ByteStreamMetadata::getLumiBlock() const {
   return(m_lumiBlock);
}
const std::vector<std::string>& ByteStreamMetadata::getFreeMetaDataStrings() const {
   return(m_freeMetaDataStrings);
}
//______________________________________________________________________________
