/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @class  : TrigEMClusterCnv_p1
 *
 * @brief transient persistent converter for TrigEMCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterCnv_p2.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCNV_P2_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p1.h"
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "AthLinks/ElementLink.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

#include "TrigCaloEventTPCnv/TrigEMClusterConverterBase.h"

class MsgStream;

template <>
class TrigEMClusterConverterBase<TrigEMCluster_p2>  : public T_AthenaPoolTPCnvConstBase<TrigEMCluster, TrigEMCluster_p2>  {
    
  public:
    using base_class::transToPers;
    using base_class::persToTrans;

    TrigEMClusterConverterBase() {}

    virtual
    void persToTrans(const TrigEMCluster_p2* persObj,  TrigEMCluster* transObj,   MsgStream& log) const override;
    virtual
    void transToPers(const TrigEMCluster* transObj, TrigEMCluster_p2* persObj, MsgStream& log) const override;

 protected:
    
    ElementLinkCnv_p1< ElementLink<RingerRingsContainer> > m_ELinkRingerRingsCnv;
};

typedef TrigEMClusterConverterBase<TrigEMCluster_p2> TrigEMClusterCnv_p2;

#endif

