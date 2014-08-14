/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigEMClusterCnv
 *
 * @brief transient persistent converter for TrigEMCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterCnv.h,v 1.7 2009-03-06 18:08:44 masik Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGEMCLUSTERCNV_H
#define TRIGEVENTATHENAPOOL_TRIGEMCLUSTERCNV_H

#include "TrigCaloEvent/TrigEMCluster.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// typedef to the latest persistent version
class TrigEMCluster_tlp2;
typedef TrigEMCluster_tlp2  TrigEMCluster_PERS;

class MsgStream;

struct TrigEMClusterCnv_impl;
class TrigEMClusterCnv  : public T_AthenaPoolCustomCnv<TrigEMCluster, TrigEMCluster_PERS > 
{

  friend class CnvFactory<TrigEMClusterCnv>;

 protected:
  
  TrigEMClusterCnv(ISvcLocator* svcloc);
  ~TrigEMClusterCnv();

  TrigEMCluster_PERS*  createPersistent(TrigEMCluster* transCont);

  TrigEMCluster*       createTransient ();

 private:
  TrigEMClusterCnv_impl* m_impl;
};


#endif
