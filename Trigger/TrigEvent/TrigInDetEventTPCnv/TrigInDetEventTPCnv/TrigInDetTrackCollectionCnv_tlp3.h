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
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackCollectionCnv_tlp3.h,v 1.3 demelian Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTIONCNV_TLP3_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKCOLLECTIONCNV_TLP3_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigInDetTrackCollectionCnv_p1.h"
#include "TrigInDetTrackCnv_p4.h"
#include "TrigInDetTrackFitParCnv_p3.h"
#include "TrigInDetTrackCollection_tlp3.h"


class TrigInDetTrackCollectionCnv_tlp3 : public AthenaPoolTopLevelTPConverter<TrigInDetTrackCollectionCnv_p1, TrigInDetTrackCollection_tlp3>
{

 public:

  TrigInDetTrackCollectionCnv_tlp3();
  virtual ~TrigInDetTrackCollectionCnv_tlp3() {}

  virtual void setPStorage( TrigInDetTrackCollection_tlp3 *storage );


 protected:

  //tp converters for ALL types used in TrigInDetTrackCollection
  TrigInDetTrackCollectionCnv_p1  m_trigInDetTrackCollectionCnv;
  TrigInDetTrackCnv_p4            m_trigInDetTrackCnv;
  TrigInDetTrackFitParCnv_p3      m_trigInDetTrackFitParCnv;

};



class TrigInDetTrackCollectionCnvARA_tlp3
  : public TrigInDetTrackCollectionCnv_tlp3
{
public:
  TrigInDetTrackCollectionCnvARA_tlp3();


private:
  PixelID m_pixId;
};


#endif
