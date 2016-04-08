/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Declaration of Transient-Persistent converters for Analysis::TauPi0Candidate
 *
 * @author Felix Friedrich
 * @date October 2012
 *
 */

#ifndef TAUEVENTTPCNV_TAUPI0CANDIDATECNV_P1_H
#define TAUEVENTTPCNV_TAUPI0CANDIDATECNV_P1_H

/// pool converter
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

/// transient and persistent versions
#include "tauEvent/TauPi0Candidate.h"
#include "tauEventTPCnv/TauPi0Candidate_p1.h"

/// forward declarations
class MsgStream;

/// Transient Persistent converter version 1 for TauPi0Candidate
class TauPi0CandidateCnv_p1 : public T_AthenaPoolTPCnvBase<Analysis::TauPi0Candidate, TauPi0Candidate_p1> 
{
public:
  /// constructor
  TauPi0CandidateCnv_p1() {};
  
  /// Fills a transient object from a persistent object
  ///
  /// @param persObj Persistent object
  /// @param transObj Transient object
  /// @param log Message stream
  virtual void persToTrans(const TauPi0Candidate_p1 *persObj, Analysis::TauPi0Candidate *transObj, MsgStream &log);

  /// Fills a persistent object from a transient object
  ///
  /// @param persObj Persistent object
  /// @param transObj Transient object
  /// @param log Message stream
  virtual void transToPers(const Analysis::TauPi0Candidate *transObj, TauPi0Candidate_p1 *persObj, MsgStream &log);

  /// make sure that the private converter is known
  virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv )
  {
         m_vecTauPi0ClusterCnv.setTopConverter( topCnv, 0 );
  }

private:
  /// converter for the TauPi0Cluster
  T_AthenaPoolTPPtrVectorCnv< std::vector<Analysis::TauPi0Cluster*>, std::vector<TPObjRef>, ITPConverterFor<Analysis::TauPi0Cluster> > m_vecTauPi0ClusterCnv;
  
};

#endif

