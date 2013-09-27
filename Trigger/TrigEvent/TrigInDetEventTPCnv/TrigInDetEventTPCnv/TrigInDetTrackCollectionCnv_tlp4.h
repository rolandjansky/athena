/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrackCollectionCnv_tlp3
 *
 * @brief "top level" transient-persistent converter for TrigInDetTrackCollection
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTIONCNV_tlp4_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTIONCNV_tlp4_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigInDetTrackCollectionCnv_p1.h"
#include "TrigInDetTrackCnv_p5.h"
#include "TrigInDetTrackFitParCnv_p3.h"
#include "TrigInDetTrackCollection_tlp4.h"


class TrigInDetTrackCollectionCnv_tlp4 : public AthenaPoolTopLevelTPConverter<TrigInDetTrackCollectionCnv_p1, TrigInDetTrackCollection_tlp4>
{

 public:

  TrigInDetTrackCollectionCnv_tlp4();
  virtual ~TrigInDetTrackCollectionCnv_tlp4() {}

  virtual void setPStorage( TrigInDetTrackCollection_tlp4 *storage );


 protected:

  //tp converters for ALL types used in TrigInDetTrackCollection
  TrigInDetTrackCollectionCnv_p1  m_trigInDetTrackCollectionCnv;
  TrigInDetTrackCnv_p5            m_trigInDetTrackCnv;
  TrigInDetTrackFitParCnv_p3      m_trigInDetTrackFitParCnv;

};





#endif
