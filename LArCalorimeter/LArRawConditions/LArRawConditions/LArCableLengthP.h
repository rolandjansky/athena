/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARCABLELENGTHP
#define LARRAWCONDITIONS_LARCABLELENGTHP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05 */

/** Modified for Pool persistency

*/ 


// persistent c-struct 
class  LArCableLengthP{
public:

    LArCableLengthP() : m_CableLength(-9999999) {} 
    LArCableLengthP(float f) 
	    : m_CableLength(f) {} 
    bool isEmpty() const { return m_CableLength < -9999 ; } ;
    float         m_CableLength;
};

#endif 

