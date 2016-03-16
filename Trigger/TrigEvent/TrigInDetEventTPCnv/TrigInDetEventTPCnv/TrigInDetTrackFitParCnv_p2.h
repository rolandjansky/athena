/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigInDetTrackFitParCnv_p2
 *
 * @brief persistent partner for TrigInDetTrackFitParCnv
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackFitParCnv_p2.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGINDETTRACKFITPARCNV_P2_H
#define TRIGINDETEVENTTPCNV_TRIGINDETTRACKFITPARCNV_P2_H

#include <math.h>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p2.h"

class MsgStream;

class TrigInDetTrackFitParCnv_p2
   : public T_AthenaPoolTPCnvBase<TrigInDetTrackFitPar, TrigInDetTrackFitPar_p2>
{
    public:
        TrigInDetTrackFitParCnv_p2() {}
        virtual void persToTrans( const TrigInDetTrackFitPar_p2 *persObj,
                                        TrigInDetTrackFitPar    *transObj,
                                        MsgStream            &log );
        virtual void transToPers( const TrigInDetTrackFitPar    *transObj,
                                        TrigInDetTrackFitPar_p2 *persObj,
                                        MsgStream            &log );
 private:
	bool CholeskyDecomposition(double a[5][5], float L[5][5]);
        std::unique_ptr<std::vector<double> >
	restoreCovariance(const TrigInDetTrackFitPar_p2*);
	bool calculateSquareRoot(const TrigInDetTrackFitPar*, TrigInDetTrackFitPar_p2*);
};

#endif 
