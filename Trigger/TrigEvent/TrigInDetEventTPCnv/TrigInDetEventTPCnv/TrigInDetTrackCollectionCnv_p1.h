/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrackCollectionCnv_p1
 *
 * @brief transient persistent converter for TrigInDetTrackCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackCollectionCnv_p1.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTIONCNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_p1.h"


class MsgStream;

class TrigInDetTrackCollectionCnv_p1 
  : public T_AthenaPoolTPCnvBase<TrigInDetTrackCollection, TrigInDetTrackCollection_p1>
{

 public:
  
  TrigInDetTrackCollectionCnv_p1() {}

  virtual void persToTrans( const TrigInDetTrackCollection_p1*, TrigInDetTrackCollection*, MsgStream& );
  virtual void transToPers( const TrigInDetTrackCollection*, TrigInDetTrackCollection_p1*, MsgStream& );
  
  virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) {
    m_trigInDetTrackVectorCnv.setTopConverter( topCnv, 0 );
  }

 private:

 typedef T_AthenaPoolTPPtrVectorCnv< TrigInDetTrackCollection,
                                     std::vector<TPObjRef>,
                                     ITPConverterFor<TrigInDetTrack>  >   TrigInDetTrackVectorCnv_p1;

  TrigInDetTrackVectorCnv_p1 m_trigInDetTrackVectorCnv;

};


#endif
