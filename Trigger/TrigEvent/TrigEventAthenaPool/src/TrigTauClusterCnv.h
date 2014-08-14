/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigTauClusterCnv
 *
 * @brief transient persistent converter for TrigTauCluster
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauClusterCnv.h,v 1.7 2009-03-06 18:08:44 masik Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGTAUCLUSTERCNV_H
#define TRIGEVENTATHENAPOOL_TRIGTAUCLUSTERCNV_H

#include "TrigCaloEvent/TrigTauCluster.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TrigTauCluster_tlp2;

// typedef to the latest persistent version
typedef TrigTauCluster_tlp2  TrigTauCluster_PERS;

class MsgStream;

class TrigTauClusterCnv_impl;
class TrigTauClusterCnv : public T_AthenaPoolCustomCnv<TrigTauCluster, TrigTauCluster_PERS > 
{

  friend class CnvFactory<TrigTauClusterCnv>;
  
 protected:
  TrigTauClusterCnv(ISvcLocator* svcloc);
  ~TrigTauClusterCnv();

  TrigTauCluster_PERS*  createPersistent(TrigTauCluster* transCont);

  TrigTauCluster*       createTransient ();

 private:
  TrigTauClusterCnv_impl* m_impl;
};


#endif
