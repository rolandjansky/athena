/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
* @Project: Trigger
* @Package: TrigEventAthenaPool
* @class  : TrigTauClusterContainerCnv
*
* @brief transient persistent converter for TrigTauClusterContainer
*
* @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
* @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
*
**********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGTAUCLUSTERCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGTAUCLUSTERCONTAINER_CNV_H

#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p5.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_p3.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_p4.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_p5.h"

typedef TrigTauClusterContainer_p5   TrigTauClusterContainer_PERS;

typedef T_AthenaPoolCustomCnv<TrigTauClusterContainer, TrigTauClusterContainer_PERS> TrigTauClusterContainerCnvBase;

class TrigTauClusterContainerCnv : public TrigTauClusterContainerCnvBase
{
    friend class CnvFactory<TrigTauClusterContainerCnv>;

protected:
public:
    TrigTauClusterContainerCnv(ISvcLocator *svcloc) : TrigTauClusterContainerCnvBase(svcloc){}
protected:

    virtual TrigTauClusterContainer_PERS* createPersistent(TrigTauClusterContainer *transObj);
    virtual TrigTauClusterContainer* createTransient();

private:
    TrigTauClusterContainerCnv_tlp1   m_converter1;
    TrigTauClusterContainerCnv_p3     m_converter3;
    TrigTauClusterContainerCnv_p4     m_converter4;
    TrigTauClusterContainerCnv_p5     m_converter5;

};

#endif 
