/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ByteStreamEventTPCnv/src/ByteStreamMetadataCnv_p1.cxx
 *  @brief This file contains the implementation for the ByteStreamMetadataCnv_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadataCnv_p1.cxx,v 1.1 2009-03-18 17:42:45 gemmeren Exp $
 **/

#include "ByteStreamData/ByteStreamMetadataContainer.h"
#include "ByteStreamEventTPCnv/ByteStreamMetadataCnv_p1.h"

void ByteStreamMetadataCnv_p1::persToTrans(const ByteStreamMetadata_p1* pers, ByteStreamMetadata* trans, MsgStream &/*log*/)
{
  *trans = ByteStreamMetadata (pers->m_runNumber,
                               pers->m_numEvents,
                               pers->m_maxEvents,
                               pers->m_recEnable,
                               pers->m_triggerType,
                               pers->m_detectorMask,
                               pers->m_detectorMask2,
                               pers->m_beamType,
                               pers->m_beamEnergy,
                               pers->m_guid,
                               pers->m_stream,
                               pers->m_project,
                               pers->m_lumiBlock,
                               pers->m_freeMetaDataStrings);
}

void ByteStreamMetadataCnv_p1::transToPers(const ByteStreamMetadata* trans, ByteStreamMetadata_p1* pers, MsgStream &/*log*/) {
   pers->m_runNumber = trans->getRunNumber();
   pers->m_numEvents = trans->getNumEvents();
   pers->m_maxEvents = trans->getMaxEvents();
   pers->m_recEnable = trans->getRecEnable();
   pers->m_triggerType = trans->getTriggerType();
   pers->m_detectorMask = trans->getDetectorMask();
   pers->m_detectorMask2 = trans->getDetectorMask2();
   pers->m_beamType = trans->getBeamType();
   pers->m_beamEnergy = trans->getBeamEnergy();
   pers->m_guid = trans->getGuid();
   pers->m_stream = trans->getStream();
   pers->m_project = trans->getProject();
   pers->m_lumiBlock = trans->getLumiBlock();
   pers->m_freeMetaDataStrings = trans->getFreeMetaDataStrings();
}
