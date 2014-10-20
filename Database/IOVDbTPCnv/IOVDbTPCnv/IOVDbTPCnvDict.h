/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBTPCNV_IOVDBTPCNVDICT_H
#define IOVDBTPCNV_IOVDBTPCNVDICT_H
/**
 * @file IOVDbTPCnvDict.h
 *
 * @brief Header file for dictionary generation
 *
 * @author  <R.D.Schaffer@cern.ch>
 */
#include "IOVDbTPCnv/IOVMetaDataContainerCnv_p1.h"
#include "IOVDbTPCnv/IOVPayloadContainerCnv_p1.h"
#include "IOVDbTPCnv/IOVMetaDataContainer_p1.h"
#include "IOVDbTPCnv/IOVPayloadContainer_p1.h"
#include "IOVDbTPCnv/AttrListIndexes.h"

struct GCCXML_DUMMY_INSTANTIATION_IOVDBTPCNV {
    T_TPCnv<IOVMetaDataContainer, IOVMetaDataContainer_p1> m_iovmetadatacontainer_p1;
    T_TPCnv<IOVPayloadContainer,  IOVPayloadContainer_p1>  m_iovpayloadcontainer_p1;
};

#endif
