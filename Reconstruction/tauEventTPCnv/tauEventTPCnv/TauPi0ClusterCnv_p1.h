/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Declaration of Transient-Persistent converters for Analysis::TauPi0Cluster
 *
 * @author Felix Friedrich
 * @date November 2012
 *
 */

#ifndef TAUEVENTTPCNV_TAUPI0CLUSTERCNV_P1_H
#define TAUEVENTTPCNV_TAUPI0CLUSTERCNV_P1_H

/// pool converter
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
//#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

/// transient and persistent versions
#include "tauEvent/TauPi0Cluster.h"
#include "tauEventTPCnv/TauPi0Cluster_p1.h"

/// forward declarations
class MsgStream;

/// Transient Persistent converter version 1 for TauPi0Cluster
class TauPi0ClusterCnv_p1 : public T_AthenaPoolTPCnvBase<Analysis::TauPi0Cluster, TauPi0Cluster_p1> 
{
public:
  /// constructor
  TauPi0ClusterCnv_p1() {};
  
  /// Fills a transient object from a persistent object
  ///
  /// @param persObj Persistent object
  /// @param transObj Transient object
  /// @param log Message stream
  virtual void persToTrans(const TauPi0Cluster_p1 *persObj, Analysis::TauPi0Cluster *transObj, MsgStream &log);

  /// Fills a persistent object from a transient object
  ///
  /// @param persObj Persistent object
  /// @param transObj Transient object
  /// @param log Message stream
  virtual void transToPers(const Analysis::TauPi0Cluster *transObj, TauPi0Cluster_p1 *persObj, MsgStream &log);

  /// make sure that the private converter is known
  virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv )
  {
      m_vecTauShotCnv.setTopConverter( topCnv, 0 );
  }

private:
  //ElementLinkVectorCnv_p1<ElementLinkVector<CaloClusterContainer> > m_clusterVectCnv;
  ElementLinkCnv_p3<ElementLink<CaloClusterContainer> > m_clusterCnv;
  
  /// converter for the TauShot
  T_AthenaPoolTPPtrVectorCnv< std::vector<Analysis::TauShot*>, std::vector<TPObjRef>, ITPConverterFor<Analysis::TauShot> > m_vecTauShotCnv;

};

#endif

