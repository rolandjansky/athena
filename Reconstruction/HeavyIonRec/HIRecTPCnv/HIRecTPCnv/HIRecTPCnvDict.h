///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// File: HIRecTPCnv/HIRecTPCnvDict.h
/// Dictionary file for persistent representation(s) of AOD containers
/// Author: Sebastien Binet <binet@cern.ch>
/// Date:   December 2006

#ifndef HIRECTPCNV_HIRECTPCNVCNVDICT_H
#define HIRECTPCNV_HIRECTPCNVCNVDICT_H

#include "HIRecTPCnv/EtaPhiBins_p1.h"
#include "HIRecTPCnv/EtaPhiBinsCnv_p1.h"
#include "HIRecTPCnv/HIPixelTrackletsCollection_p1.h"
#include "HIRecTPCnv/HIPixelTracklets_p1.h"
#include "HIRecTPCnv/HIPixelTrackletsCollection_tlp1.h"
#include "HIRecTPCnv/HIPixelTrackletsCollectionCnv_tlp1.h"
#include "HIGlobal/HIPixelTrackletsCollection.h"

#include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p1.h"

#include "HIGlobal/HITRTInfo.h"
#include "HIRecTPCnv/HITRTInfo_p1.h"
#include "HIRecTPCnv/HITRTInfoCnv_p1.h"

//struct dummy {
T_TPCnv<EtaPhiBins,EtaPhiBins_p1> m_etaPhiBinsCnv;

std::vector< HIPixelTracklets_p1 > m_v1;
std::vector< HIPixelTrackletsCollection_p1 > m_v2;

T_TPCnv<HIPixelTrackletsCollection,HIPixelTrackletsCollection_tlp1> m_HIPixelTrackletsCollectionCnv;

T_TPCnv<HITRTInfo, HITRTInfo_p1> m_hitrtinfocnv;

//};

#endif //RECTPCNV_RECTPCNVCNVDICT_H
