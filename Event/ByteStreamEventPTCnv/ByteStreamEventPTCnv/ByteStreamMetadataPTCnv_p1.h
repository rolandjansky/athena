/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMEVENTPTCNV_BYTESTREAMMETADATAPTCNV_P1_H
#define BYTESTREAMEVENTPTCNV_BYTESTREAMMETADATAPTCNV_P1_H
/**
 * @file ByteStreamMetadataPTCnv_p1.h
 *
 * @brief Header file for in-file metadata getters (file peeking without Athena)
 *
 * @author  <vaniachine@anl.gov>
 */

#define private public
#include "ByteStreamEventTPCnv/ByteStreamMetadata_p1.h"
#undef private

class ByteStreamMetadataPTCnv_p1 : ByteStreamMetadata_p1 {
public:
    static unsigned int getRunNumber (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_runNumber; };
    static unsigned int getNumEvents (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_numEvents; };
    static unsigned int getMaxEvents (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_maxEvents; };
    static unsigned int getRecEnable (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_recEnable; };
    static unsigned int getTriggerType (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_triggerType; };
    static uint64_t getDetectorMask (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_detectorMask; };
    static uint64_t getDetectorMask2 (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_detectorMask2; };
    static unsigned int getBeamType (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_beamType; };
    static unsigned int getBeamEnergy (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_beamEnergy; };
    static std::string getGuid (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_guid; };
    static std::string getStream (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_stream; };
    static std::string getProject (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_project; };
    static unsigned int getLumiBlock (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_lumiBlock; };
    static std::vector<std::string> getFreeMetaDataStrings (const ::ByteStreamMetadata_p1& bsmdp) { return bsmdp.m_freeMetaDataStrings; };
};

#endif // BYTESTREAMEVENTPTCNV_BYTESTREAMMETADATAPTCNV_P1_H
