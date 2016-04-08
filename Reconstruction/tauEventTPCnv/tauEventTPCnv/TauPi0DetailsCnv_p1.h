/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Declaration of Transient-Persistent converters for Analysis::TauPi0Details
 *
 * @author Felix Friedrich
 * @date October 2012
 *
 */

#ifndef TAUEVENTTPCNV_TAUPI0DETAILSCNV_P1_H
#define TAUEVENTTPCNV_TAUPI0DETAILSCNV_P1_H

/// pool converter
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

/// transient and persistent versions
#include "tauEvent/TauPi0Details.h"
#include "tauEventTPCnv/TauPi0Details_p1.h"

#include "tauEvent/TauPi0Candidate.h"  //forward declartion possible?
#include "tauEvent/TauPi0Cluster.h"  //forward declartion possible?

/// forward declarations
class MsgStream;
//class TauPi0Candidate;

class TauPi0DetailsCnv_p1 : public T_AthenaPoolTPPolyCnvBase<Analysis::TauDetails, Analysis::TauPi0Details, TauPi0Details_p1 >
{
public:
  TauPi0DetailsCnv_p1() {};

  /// Fills a transient object from a persistent object
  ///
  /// @param persObj Persistent object
  /// @param transObj Transient object
  /// @param log Message stream
  virtual void persToTrans(const TauPi0Details_p1 *persObj, Analysis::TauPi0Details *transObj, MsgStream &log);

  /// Fills a persistent object from a transient object
  ///
  /// @param persObj Persistent object
  /// @param transObj Transient object
  /// @param log Message stream
  virtual void transToPers(const Analysis::TauPi0Details *transObj, TauPi0Details_p1 *persObj, MsgStream &log);
  
  /// make sure that the private converter is known
  virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) 
  {
         m_vecTauPi0CandidateCnv.setTopConverter( topCnv, 0 );
         m_vecTauPi0ClusterCnv.setTopConverter( topCnv, 0 );
  }
  
private:
  /// converter for the TauPi0Candidate
  T_AthenaPoolTPPtrVectorCnv< std::vector<Analysis::TauPi0Candidate*>, std::vector<TPObjRef>, ITPConverterFor<Analysis::TauPi0Candidate> > m_vecTauPi0CandidateCnv;
  T_AthenaPoolTPPtrVectorCnv< std::vector<Analysis::TauPi0Cluster*>, std::vector<TPObjRef>, ITPConverterFor<Analysis::TauPi0Cluster> > m_vecTauPi0ClusterCnv;
};
#endif
