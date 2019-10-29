/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @class  : TrigEMClusterCnv_p4
 *
 * @brief transient persistent converter for TrigEMCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterCnv_p4.h,v 1.2 2011-05-31 22:01:37 damazio Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCNV_P4_H
#define TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p4.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p2.h"
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "AthLinks/ElementLink.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

//#include "TrigCaloEventTPCnv/TrigEMClusterConverterBase.h"

class MsgStream;

class TrigEMClusterCnv_p4 : public T_AthenaPoolTPCnvConstBase<TrigEMCluster, TrigEMCluster_p4>  {
    
  public:
    using base_class::transToPers;
    using base_class::persToTrans;


    TrigEMClusterCnv_p4() { }

    virtual void persToTrans(const TrigEMCluster_p4* persObj,  TrigEMCluster* transObj,   MsgStream& log) const override;
    virtual void transToPers(const TrigEMCluster* transObj, TrigEMCluster_p4* persObj, MsgStream& log) const override;

 protected:
    
    ElementLinkCnv_p3< ElementLink<RingerRingsContainer> > ELinkRingerRingsCnv;

//    ITPConverterFor<TrigCaloCluster>*   m_trigCaloClusterCnv;
};

//typedef TrigEMClusterConverterBase<TrigEMCluster_p2> TrigEMClusterCnv_p2;

#endif

