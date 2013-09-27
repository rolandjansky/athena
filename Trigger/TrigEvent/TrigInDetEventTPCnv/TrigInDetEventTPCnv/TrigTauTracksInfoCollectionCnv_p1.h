/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTauTracksInfoCollectionCnv_p1
 *
 * @brief transient-persistent converter for TrigTauTracksInfoCollection_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauTracksInfoCollectionCnv_p1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCONTAINER_CNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigInDetEvent/TrigTauTracksInfoCollection.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCnv_p1.h"

class TrigTauTracksInfoCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigTauTracksInfoCollection, TrigTauTracksInfoCollection_p1, TrigTauTracksInfoCnv_p1 >

{
 
 public:
  TrigTauTracksInfoCollectionCnv_p1() {}
 
}; //end of class definitions
 
#endif
