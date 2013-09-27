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
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTIONCNV_TLP2_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTIONCNV_TLP2_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigInDetTrackCollectionCnv_p1.h"
#include "TrigInDetTrackCnv_p3.h"
#include "TrigInDetTrackFitParCnv_p3.h"
#include "TrigInDetTrackCollection_tlp2.h"


class TrigInDetTrackCollectionCnv_tlp2 : public AthenaPoolTopLevelTPConverter<TrigInDetTrackCollectionCnv_p1, TrigInDetTrackCollection_tlp2>
{

 public:

  TrigInDetTrackCollectionCnv_tlp2();
  virtual ~TrigInDetTrackCollectionCnv_tlp2() {}

  virtual void setPStorage( TrigInDetTrackCollection_tlp2 *storage );


 protected:

  //tp converters for ALL types used in TrigInDetTrackCollection
  TrigInDetTrackCollectionCnv_p1  m_trigInDetTrackCollectionCnv;
  TrigInDetTrackCnv_p3            m_trigInDetTrackCnv;
  TrigInDetTrackFitParCnv_p3      m_trigInDetTrackFitParCnv;

};





#endif
