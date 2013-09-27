/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrackFitParCnv_p1
 *
 * @brief persistent partner for TrigInDetTrackFitParCnv
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackFitParCnv_p1.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKFITPARCNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKFITPARCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p1.h"

class MsgStream;

class TrigInDetTrackFitParCnv_p1
   : public T_AthenaPoolTPCnvBase<TrigInDetTrackFitPar, TrigInDetTrackFitPar_p1>
{
    public:
        TrigInDetTrackFitParCnv_p1() {}
        virtual void persToTrans( const TrigInDetTrackFitPar_p1 *persObj,
                                        TrigInDetTrackFitPar    *transObj,
                                        MsgStream            &log );
        virtual void transToPers( const TrigInDetTrackFitPar    *transObj,
                                        TrigInDetTrackFitPar_p1 *persObj,
                                        MsgStream            &log );
};

#endif 
