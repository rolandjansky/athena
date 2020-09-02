/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARRINJP
#define LARRAWCONDITIONS_LARRINJP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05 */

/** Modified for Pool persistency

*/ 


// persistent c-struct 
class  LArRinjP{
public:

    LArRinjP() : m_Rinj(-9999999) {} 
    LArRinjP(float f) : m_Rinj(f) {} 
    bool isEmpty() const { return m_Rinj < -9999 ; } ;
    float         m_Rinj;
};

#endif 

