/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef V0CANDIDATE_P1_H
#define V0CANDIDATE_P1_H

/**
 * Persistent representation of the TrkV0Vertex/V0Candidate class
 *
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch> 
 * 
 * March 2007
 */
 
#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{

 class V0Candidate_p1
 {
 
  public:
  
   V0Candidate_p1(){}
   
//vector of V0Hypothesys   
   std::vector<TPObjRef> m_v0Hyp;
  
 };//end of class definitions

}//end of namespace definitions


#endif
