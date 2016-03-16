/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkV0Vertex/V0Candidate.h"
#include "TrkEventTPCnv/TrkV0Vertex/V0CandidateCnv_p1.h"

void  V0CandidateCnv_p1::persToTrans(const Trk::V0Candidate_p1 * persObj, Trk::V0Candidate * transObj, MsgStream &log)
{ 
 
// A simplest possible approcah to work with the base class:
// dynamic casting each element of the vector 
// allows to work with vectors and containers of
// all derived types of VxCandidate. I.e. poly converters
// are now defined for each of derived types. A VxContainer
// of any of them can now be stored and read. But objects
// like V0Candidate are now forced to work through a base
// type. Dynamic_cast is the simplest solution. In principle, 
// the template should have been re-written 
 
  std::vector<Trk::VxCandidate *> v; 
  m_v0HypothesisVectorCnv.persToTrans( &persObj->m_v0Hyp, &v,log); 
  std::vector<Trk::V0Hypothesis *> vh; 
  vh.reserve (v.size());
  for(unsigned int i= 0; i<v.size();++i)
    vh.push_back( dynamic_cast<Trk::V0Hypothesis *>(v[i])); 
  *transObj = Trk::V0Candidate (std::move (vh));
}

void  V0CandidateCnv_p1::transToPers(const Trk::V0Candidate * transObj, Trk::V0Candidate_p1 * persObj, MsgStream &log){
      
    std::vector<Trk::VxCandidate *> lVx;
//    std::cout<<"Ilija writing v0candidate. size: "<<transObj->m_v0Hyp.size()<<std::endl;
    const std::vector<Trk::V0Hypothesis *>& v0Hyp = *transObj->v0Hypothesis();
    for(unsigned int i= 0; i < v0Hyp.size();++i)
        lVx.push_back( v0Hyp[i]); 
    m_v0HypothesisVectorCnv.transToPers( &lVx, &persObj->m_v0Hyp, log);

}
