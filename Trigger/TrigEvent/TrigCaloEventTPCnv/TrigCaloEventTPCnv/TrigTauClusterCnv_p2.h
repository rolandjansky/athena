/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @class  : TrigTauClusterCnv_p2
 *
 * @brief transient persistent converter for TrigTauCluster
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona 
**********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCNV_P2_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCNV_P2_H

// transient class
#include "TrigCaloEvent/TrigTauCluster.h"
// persistent class
#include "TrigCaloEventTPCnv/TrigTauCluster_p2.h"
// base-class converter
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p1.h"
// converter for ElementLinks
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
// converter for track and cluster objects (pointed to through ElementLinks)
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_p1.h"
// T/P converter base class
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigTauClusterCnv_p2 : public T_AthenaPoolTPCnvConstBase<TrigTauCluster, TrigTauCluster_p2>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigTauClusterCnv_p2() {}
  
  virtual void persToTrans( const TrigTauCluster_p2 *persObj,
			    TrigTauCluster    *transObj,
			    MsgStream            &log ) const override;
  virtual void transToPers( const TrigTauCluster    *transObj,
			    TrigTauCluster_p2 *persObj,
			    MsgStream            &log ) const override;

 protected:

  // to persistify ElementLinks to cluster and track 
  ElementLinkCnv_p1< ElementLink<TrigTauClusterDetailsContainer> >   m_ELinkTauClusterDetailsCnv ;
};


#endif
