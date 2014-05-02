/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARCABLEATTENUATIONP
#define LARRAWCONDITIONS_LARCABLEATTENUATIONP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05 */

/** Modified for Pool persistency

*/ 


// persistent c-struct 
class  LArCableAttenuationP{
public:

    LArCableAttenuationP() : m_CableAttenuation(-999999999) {} 
    LArCableAttenuationP(float f) 
	    : m_CableAttenuation(f) {} 
    bool isEmpty() const { return m_CableAttenuation < -9999 ; } ;
    float         m_CableAttenuation;
};

#endif 

