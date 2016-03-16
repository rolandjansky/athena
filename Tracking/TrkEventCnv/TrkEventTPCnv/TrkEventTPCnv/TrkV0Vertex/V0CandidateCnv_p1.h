/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef V0CANDIDATE_CNV_P1_H
#define V0CANDIDATE_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkV0Vertex/V0Candidate.h"
#include "TrkEventTPCnv/TrkV0Vertex/V0Candidate_p1.h"
#include "TrkEventTPCnv/TrkV0Vertex/V0HypothesisCnv_p1.h"
#include "VxVertex/VxCandidate.h"

  
 class MsgStream;

 class V0CandidateCnv_p1 : public T_AthenaPoolTPCnvBase< Trk::V0Candidate, Trk::V0Candidate_p1> {
  public:
   
   V0CandidateCnv_p1() {}

   void  persToTrans(const Trk::V0Candidate_p1 * persObj, Trk::V0Candidate * transObj, MsgStream &log); 
   void  transToPers(const Trk::V0Candidate * transObj, Trk::V0Candidate_p1 * persObj, MsgStream &log);     

   virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) {
       m_v0HypothesisVectorCnv.setTopConverter( topCnv, 0 );
   }   
     
  private:
   
   //V0HypothesisCnv_p1 * m_v0HypothesisConverter;
   
  // typedef T_AthenaPoolTPPtrVectorCnv< std::vector<Trk::V0Hypothesis *>,
  //                                      std::vector<TPObjRef>,
  //                                      ITPConverterFor<Trk::V0Hypothesis>  >   V0HypothesisVectorCnv_p1;

   typedef T_AthenaPoolTPPtrVectorCnv< std::vector<Trk::VxCandidate *>, std::vector<TPObjRef>, ITPConverterFor<Trk::VxCandidate>  >   V0HypothesisVectorCnv_p1;
  
   V0HypothesisVectorCnv_p1 m_v0HypothesisVectorCnv;
 };

#endif
