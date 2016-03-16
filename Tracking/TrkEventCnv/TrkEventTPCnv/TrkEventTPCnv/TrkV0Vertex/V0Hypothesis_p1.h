/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef V0HYPOTHESIS_P1_H
#define V0HYPOTHESIS_P1_H

/**
 * Persistent representation of the TrkV0Vertex/V0Hypothesis class
 *
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch> 
 * 
 * March 2007
 */
 
 #include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{

 class V0Hypothesis_p1
 {
 
  public:
  
   V0Hypothesis_p1() : m_positiveTrackID(0), m_negativeTrackID(0), m_constraintID(0) {}
 
//extended VxCandidate itself
 
   TPObjRef m_extendedVxCandidate;
  
// Stored PID's
   int   m_positiveTrackID;
   int   m_negativeTrackID;
   int   m_constraintID;
 
 };//end of class definitions

}//end of namespace definitions

#endif //V0HYPOTHESIS_P1_H
