/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ENERGY_LOSS_CNV_P1_H
#define ENERGY_LOSS_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLoss_p1.h"

class MsgStream;

/** @brief T/P converter for class EnergyLoss
    @author Wolfgang Liebig <>
*/

class EnergyLossCnv_p1: public T_AthenaPoolTPCnvBase<
  Trk::EnergyLoss, Trk::EnergyLoss_p1>
{
    public:
        EnergyLossCnv_p1() {}
        virtual void persToTrans( const Trk :: EnergyLoss_p1 *persObj,
                                        Trk :: EnergyLoss    *transObj,
                                        MsgStream            &log );
        virtual void transToPers( const Trk :: EnergyLoss    *transObj,
                                        Trk :: EnergyLoss_p1 *persObj,
                                        MsgStream            &log );
};

#endif
