/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTruthEventTPCnv
 * @Class  : TrigInDetTrackTruthCnv_p1
 *
 * @brief transient-persistent converter for TrigInDetTrackTruth
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackTruthCnv_p1.h,v 1.2 2008-04-16 16:40:19 ahamil Exp $
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGINDETTRACKTRUTHCNV_P1_H
#define TRIGEVENTTPCNV_TRIGINDETTRACKTRUTHCNV_P1_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigInDetTruthEvent/TrigInDetTrackTruth.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruth_p1.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"
#include "TrigTruthEventTPCnv/TrigIDHitStatsCnv_p1.h"

class MsgStream;


class TrigInDetTrackTruthCnv_p1 : public T_AthenaPoolTPCnvBase<TrigInDetTrackTruth, TrigInDetTrackTruth_p1> 
{

 public:

  TrigInDetTrackTruthCnv_p1() : m_hepMcParticleLinkCnv_p1(0), m_trigIDHitStatsCnv_p1(0) {}

  virtual void persToTrans(const TrigInDetTrackTruth_p1* persObj, TrigInDetTrackTruth* transObj, MsgStream &log);
  virtual void transToPers(const TrigInDetTrackTruth* transObj, TrigInDetTrackTruth_p1* persObj, MsgStream &log);

 private:

  ITPConverterFor<HepMcParticleLink>*  m_hepMcParticleLinkCnv_p1;
  ITPConverterFor<TrigIDHitStats>*     m_trigIDHitStatsCnv_p1;
  //HepMcParticleLinkCnv_p1* m_hepMcParticleLinkCnv_p1;
  //TrigIDHitStatsCnv_p1* m_trigIDHitStatsCnv_p1;

};



#endif
