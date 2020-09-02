/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAREMEC_HVBETAP
#define LARRAWCONDITIONS_LAREMEC_HVBETAP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05 */

/** Modified for Pool persistency

*/ 


// persistent c-struct 
class  LArEMEC_HVbetaP{
public:

    LArEMEC_HVbetaP() : m_EMEC_HVbeta(-9999999) {} 
    LArEMEC_HVbetaP(float f) 
	    : m_EMEC_HVbeta(f) {} 
    bool isEmpty() const { return m_EMEC_HVbeta < -9999 ; } ;
    float         m_EMEC_HVbeta;
};

#endif 

