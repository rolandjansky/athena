/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAREMEC_HVALPHAP
#define LARRAWCONDITIONS_LAREMEC_HVALPHAP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05 */

/** Modified for Pool persistency

*/ 


// persistent c-struct 
class  LArEMEC_HValphaP{
public:

    LArEMEC_HValphaP() : m_EMEC_HValpha(-9999999) {} 
    LArEMEC_HValphaP(float f) 
	    : m_EMEC_HValpha(f) {} 
    bool isEmpty() const { return m_EMEC_HValpha < -9999 ; } ;
    float         m_EMEC_HValpha;
};

#endif 

