/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAREMEC_CPHIP
#define LARRAWCONDITIONS_LAREMEC_CPHIP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05 */

/** Modified for Pool persistency

*/ 


// persistent c-struct 
class  LArEMEC_CphiP{
public:

    LArEMEC_CphiP() : m_EMEC_Cphi(-9999999) {} 
    LArEMEC_CphiP(float f) 
	    : m_EMEC_Cphi(f) {} 
    float         m_EMEC_Cphi;
    bool isEmpty() const { return m_EMEC_Cphi < -9999 ; } ;
};

#endif 

