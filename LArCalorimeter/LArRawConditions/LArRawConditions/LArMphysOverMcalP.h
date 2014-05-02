/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMPHYSOVERMCALP
#define LARRAWCONDITIONS_LARMPHYSOVERMCALP

#include <vector> 
#include "LArElecCalib/ILArMphysOverMcal.h"
/** c-struct reproducing the structure of the persistent data
 * @author S. Laplace
 * @version  0-0-1 , 01/07/05 */

/** MOdified for Pool persistency

*/ 


// persistent c-struct 
class  LArMphysOverMcalP{
public:

    LArMphysOverMcalP() : m_MphysOverMcal(float(ILArMphysOverMcal::ERRORCODE)) {} 
    LArMphysOverMcalP(float f) 
	    : m_MphysOverMcal(f) {} 
    bool isEmpty() const { return (m_MphysOverMcal == float(ILArMphysOverMcal::ERRORCODE)); };
    float         m_MphysOverMcal;
    
};
#endif 

