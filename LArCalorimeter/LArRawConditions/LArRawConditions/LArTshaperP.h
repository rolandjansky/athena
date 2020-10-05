/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARTSHAPERP
#define LARRAWCONDITIONS_LARTSHAPERP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05 */

/** Modified for Pool persistency

*/ 


// persistent c-struct 
class  LArTshaperP{
public:

    LArTshaperP() : m_Tshaper(-9999999) {} 
    LArTshaperP(float f) : m_Tshaper(f) {} 
    bool isEmpty() const { return m_Tshaper < -9999 ; } ;
    float         m_Tshaper;
};

#endif 

