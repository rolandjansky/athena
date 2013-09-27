/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrackCollectionCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigInDetTrackCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackCollectionCnv_tlp1.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTIONCNV_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTIONCNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigInDetTrackCollectionCnv_p1.h"
#include "TrigInDetTrackCnv_p1.h"
#include "TrigInDetTrackCnv_p2.h"
#include "TrigInDetTrackFitParCnv_p1.h"
#include "TrigInDetTrackFitParCnv_p2.h"
#include "TrigInDetTrackCollection_tlp1.h"


class TrigInDetTrackCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter<TrigInDetTrackCollectionCnv_p1, TrigInDetTrackCollection_tlp1>
{

 public:

  TrigInDetTrackCollectionCnv_tlp1();
  virtual ~TrigInDetTrackCollectionCnv_tlp1() {}

  virtual void setPStorage( TrigInDetTrackCollection_tlp1 *storage );


 protected:

  //tp converters for ALL types used in TrigInDetTrackCollection
  TrigInDetTrackCollectionCnv_p1  m_trigInDetTrackCollectionCnv;
  TrigInDetTrackCnv_p1            m_trigInDetTrackCnv;
  TrigInDetTrackCnv_p2            m_trigInDetTrackCnv_p2;
  TrigInDetTrackFitParCnv_p1      m_trigInDetTrackFitParCnv;
  TrigInDetTrackFitParCnv_p2      m_trigInDetTrackFitParCnv_p2;

};




#endif
