/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
* @Project: Trigger
* @Package: TrigEventAthenaPool
* @Class  : TrigTauClusterContainerCnv
*
* @brief transient persistent converter for TrigTauClusterContainer
*
* @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
* @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
*
* File and Version Information:
* $Id: TrigTauClusterContainerCnv.h,v 1.5 2009-03-06 18:08:44 masik Exp $
**********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGTAUCLUSTERCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGTAUCLUSTERCONTAINER_CNV_H

#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p5.h"

typedef TrigTauClusterContainer_p5   TrigTauClusterContainer_PERS;

typedef T_AthenaPoolCustomCnv<TrigTauClusterContainer, TrigTauClusterContainer_PERS> TrigTauClusterContainerCnvBase;

class TrigTauClusterContainerCnv : public TrigTauClusterContainerCnvBase
{
    friend class CnvFactory<TrigTauClusterContainerCnv>;

protected:
    TrigTauClusterContainerCnv(ISvcLocator *svcloc) : TrigTauClusterContainerCnvBase(svcloc){}

    virtual TrigTauClusterContainer_PERS* createPersistent(TrigTauClusterContainer *transObj);
    virtual TrigTauClusterContainer* createTransient();
};

#endif 
