/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterCnv_p2
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


class TrigTauClusterCnv_p2 : public T_AthenaPoolTPCnvBase<TrigTauCluster, TrigTauCluster_p2>
{
 public:
  
  TrigTauClusterCnv_p2() : m_trigCaloClusterCnv(0) {}
  
  virtual void persToTrans( const TrigTauCluster_p2 *persObj,
			    TrigTauCluster    *transObj,
			    MsgStream            &log );
  virtual void transToPers( const TrigTauCluster    *transObj,
			    TrigTauCluster_p2 *persObj,
			    MsgStream            &log );

 protected:

  // to persistify ElementLinks to cluster and track 
  ElementLinkCnv_p1< ElementLink<TrigTauClusterDetailsContainer> >   m_ELinkTauClusterDetailsCnv ;
  ITPConverterFor<TrigCaloCluster>*    m_trigCaloClusterCnv;

};


#endif
