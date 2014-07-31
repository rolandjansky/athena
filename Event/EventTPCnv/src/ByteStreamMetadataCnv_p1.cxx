/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ByteStreamMetadataCnv_p1.cxx
 *  @brief This file contains the implementation for the ByteStreamMetadataCnv_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadataCnv_p1.cxx,v 1.1 2009-03-18 17:42:45 gemmeren Exp $
 **/

#define private public
#define protected public
#include "ByteStreamData/ByteStreamMetadataContainer.h"
#undef private
#undef protected

#include "EventTPCnv/ByteStreamMetadataCnv_p1.h"

void ByteStreamMetadataCnv_p1::persToTrans(const ByteStreamMetadata_p1* pers, ByteStreamMetadata* trans, MsgStream &/*log*/)  {
   trans->m_runNumber = pers->m_runNumber;
   trans->m_numEvents = pers->m_numEvents;
   trans->m_maxEvents = pers->m_maxEvents;
   trans->m_recEnable = pers->m_recEnable;
   trans->m_triggerType = pers->m_triggerType;
   trans->m_detectorMask = pers->m_detectorMask;
   trans->m_beamType = pers->m_beamType;
   trans->m_beamEnergy = pers->m_beamEnergy;
   trans->m_guid = pers->m_guid;
   trans->m_stream = pers->m_stream;
   trans->m_project = pers->m_project;
   trans->m_lumiBlock = pers->m_lumiBlock;
   trans->m_freeMetaDataStrings = pers->m_freeMetaDataStrings;
}

void ByteStreamMetadataCnv_p1::transToPers(const ByteStreamMetadata* trans, ByteStreamMetadata_p1* pers, MsgStream &/*log*/) {
   pers->m_runNumber = trans->m_runNumber;
   pers->m_numEvents = trans->m_numEvents;
   pers->m_maxEvents = trans->m_maxEvents;
   pers->m_recEnable = trans->m_recEnable;
   pers->m_triggerType = trans->m_triggerType;
   pers->m_detectorMask = trans->m_detectorMask;
   pers->m_beamType = trans->m_beamType;
   pers->m_beamEnergy = trans->m_beamEnergy;
   pers->m_guid = trans->m_guid;
   pers->m_stream = trans->m_stream;
   pers->m_project = trans->m_project;
   pers->m_lumiBlock = trans->m_lumiBlock;
   pers->m_freeMetaDataStrings = trans->m_freeMetaDataStrings;
}
