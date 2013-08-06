/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/CscRawDataCollection.h"
#include "GaudiKernel/MsgStream.h"

uint16_t CscRawDataCollection::numSamples() const {

  /** expected number of samples
      this is the first word in the ROD data block */
  return (m_eventType & 0xFF);

}

uint16_t CscRawDataCollection::latency() const {

  /** latency - from the detector Event Type */
  return (m_eventType >> 8) & 0xFF;

}

/** the rate could be 25 or 50 ns */
uint8_t CscRawDataCollection::rate() const {

  /** the sampling rate - 50 or 25 ns depending on the last bit of EventType */
  bool rateBit = (m_eventType >> 31) & 0x1;
  uint8_t samplingRate = 50;
  if ( rateBit ) samplingRate = 25;
  return samplingRate;
}

/** Calibration layer */
uint8_t CscRawDataCollection::calLayer() const {
    return (m_eventType >> 16) & 0x3F;
}

/** Calibration pulser amplitude */
uint8_t CscRawDataCollection::calAmplitude() const {
    return (m_eventType >> 24) & 0x3F;
}

/** is sparsified data? */
bool CscRawDataCollection::sparsified () const {
  return (m_eventType >> 22) & 0x1;
}

/** is neutron rejection ON? */
bool CscRawDataCollection::neutron () const {
  return (m_eventType >> 23) & 0x1;
}

/** is calibration enabled? */
bool CscRawDataCollection::calEnabled() const {
  return (m_eventType >> 30) & 0x1;
}

/** insert the rate into the detector event type word - the rate can 20 or 40 MHz */
void CscRawDataCollection::setSamplingRate (const uint16_t rate) {
  uint32_t rt = ( rate == 20 ) ? 0x0 : 0x1;
  uint32_t rateWord = 0x7FFF & m_eventType;
  m_eventType = rateWord | ( rt << 31 );

}

/** insert the number of samples into the detector event type word */
void CscRawDataCollection::setNumSamples (const uint16_t n) {
 uint32_t ns = m_eventType & 0xFFFFFF00;
 m_eventType = ns | n;
}

/** insert the latency into the detector event type word */
void CscRawDataCollection::setLatency (const uint16_t latency) {
 uint32_t lt = m_eventType & 0xFFFF00FF;
 m_eventType = lt | latency;
}

/** is this sparsified data? */
void CscRawDataCollection::set_sparsified () {
 uint32_t sp = m_eventType & 0xFFDFFFFF;
 m_eventType =  sp | (1 << 22);
}

/** neutron rejection applied */
void CscRawDataCollection::set_neutron () {
 uint32_t ne = m_eventType & 0xFFBFFFFF;
 m_eventType =  ne | (1 << 23);
}

/** calibration data? */
void CscRawDataCollection::set_enableCal () {
 uint32_t en = m_eventType & 0xBFFFFFFF;
 m_eventType =  en | (1 << 30);
}

/** set the calibration amplitude */
void CscRawDataCollection::set_calAmplitude ( const uint8_t calAmp ) {
 uint32_t la = m_eventType & 0xC0FFFFFF;
 m_eventType =  la | (calAmp << 24);
}

/** set the chamber layer number where calibration is being done */
void CscRawDataCollection::set_calLayer     ( const uint8_t calLay ) {
 uint32_t la = m_eventType & 0xFFC0FFFF;
 m_eventType =  la | (calLay << 16);

}

/** obsolete - just backward compatibility with old data */
void CscRawDataCollection::setNumDPU  (const uint16_t numDPU) {
  m_numRPU = numDPU;
}

std::ostream& operator << ( std::ostream& sl, const CscRawDataCollection& coll) 
{
    sl << "CscRawDataCollection: "
    << "identify()="<< coll.identify()
    << ", rodId()="<< coll.rodId()
    << ", subDetectorId()="<< coll.subDetectorId()
    << ", numRPU()="<< coll.numRPU()
    << ", detector event type="<< coll.eventType()
    << ", sampling phase="<< coll.samplingPhase()
    << ", trigger type="<< coll.triggerType()
    << ", first bit summary="<< coll.firstBitSummary()
    <<" CscRawData=[";
    CscRawDataCollection::const_iterator it = coll.begin();
    CscRawDataCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;}

