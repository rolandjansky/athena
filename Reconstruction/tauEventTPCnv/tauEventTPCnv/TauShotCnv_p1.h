/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Declaration of Transient-Persistent converters for Analysis::TauShot
 *
 * @author Felix Friedrich
 * @date November 2012
 *
 */

#ifndef TAUEVENTTPCNV_TAUSHOTCNV_P1_H
#define TAUEVENTTPCNV_TAUSHOTCNV_P1_H

/// pool converter
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
//#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "EventCommonTPCnv/P4EEtaPhiMCnv_p2.h"

/// transient and persistent versions
#include "tauEvent/TauShot.h"
#include "tauEventTPCnv/TauShot_p1.h"

/// forward declarations
class MsgStream;

/// Transient Persistent converter version 1 for TauShot
class TauShotCnv_p1 : public T_AthenaPoolTPCnvBase<Analysis::TauShot, TauShot_p1> 
{
public:
  /// constructor
  TauShotCnv_p1() {};
  
  /// Fills a transient object from a persistent object
  ///
  /// @param persObj Persistent object
  /// @param transObj Transient object
  /// @param log Message stream
  virtual void persToTrans(const TauShot_p1 *persObj, Analysis::TauShot *transObj, MsgStream &log);

  /// Fills a persistent object from a transient object
  ///
  /// @param persObj Persistent object
  /// @param transObj Transient object
  /// @param log Message stream
  virtual void transToPers(const Analysis::TauShot *transObj, TauShot_p1 *persObj, MsgStream &log);

private:
  //ElementLinkVectorCnv_p1<ElementLinkVector<CaloClusterContainer> > m_clusterVectCnv;
  ElementLinkCnv_p3<ElementLink<CaloClusterContainer> > m_clusterCnv;
  ElementLinkCnv_p3<ElementLink<CaloCellContainer> >    m_seedCellCnv;
  P4EEtaPhiMCnv_p2                                      m_P4EEtaPhiMCnv;
  
};

#endif

