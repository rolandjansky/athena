/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef V0HYPOTHESIS_CNV_P1_H
#define V0HYPOTHESIS_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkV0Vertex/V0Hypothesis.h"
#include "TrkEventTPCnv/TrkV0Vertex/V0Hypothesis_p1.h"
#include "VxVertex/VxCandidate.h"

/**
 * Converter for Transient/Persistent separation
 * of the TrkV0Vertex/V0Hypothesis class.
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *
 * March 2007
 */

 class MsgStream;
 
 class V0HypothesisCnv_p1 : public T_AthenaPoolTPPolyCnvBase< Trk::VxCandidate, Trk::V0Hypothesis, Trk::V0Hypothesis_p1>
// class V0HypothesisCnv_p1 : public T_AthenaPoolTPCnvBase< Trk::V0Hypothesis, Trk::V0Hypothesis_p1>
 {

  public:

/** 
 * Default constructor   
 */
//   V0HypothesisCnv_p1(): m_extendedVxCandidateConverter(0){} 
   
   V0HypothesisCnv_p1(): m_vxCandidateConverter(0){} 
  
  
/**
 * Conversion methods
 */ 
   void  persToTrans(const Trk::V0Hypothesis_p1 * persObj, Trk::V0Hypothesis * transObj, MsgStream &log);

   void  transToPers(const Trk::V0Hypothesis * transObj, Trk::V0Hypothesis_p1 * persObj, MsgStream &log);

  private:
  
//   ITPConverterFor<Trk::ExtendedVxCandidate> * m_extendedVxCandidateConverter;

   ITPConverterFor<Trk::VxCandidate> * m_vxCandidateConverter;

 
 };//end of class definitions

#endif
