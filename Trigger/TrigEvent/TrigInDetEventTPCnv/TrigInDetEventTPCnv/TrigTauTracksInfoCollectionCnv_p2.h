/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTauTracksInfoCollectionCnv_p2
 *
 * @brief transient-persistent converter for TrigTauTracksInfoCollection_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauTracksInfoCollectionCnv_p2.h 563349 2013-09-27 13:20:37Z masik $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCONTAINER_CNV_P2_H
#define TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCONTAINER_CNV_P2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigTauTracksInfoCollection.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollection_p2.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCnv_p2.h"

typedef T_AthenaPoolTPCnvVector< TrigTauTracksInfoCollection, TrigTauTracksInfoCollection_p2, TrigTauTracksInfoCnv_p2 > TrigTauTracksInfoCollectionCnv_p2;

 
#endif
