/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMPHYSOVERMCALP1
#define LARRAWCONDITIONS_LARMPHYSOVERMCALP1

#include "LArElecCalib/ILArMphysOverMcal.h"
/** c-struct reproducing the structure of the persistent data
 * @author S. Laplace
 * @version  0-0-1 , 01/07/05 */

/** MOdified for Pool persistency

*/ 


// persistent c-struct 
class  LArMphysOverMcalP1{
public:

    LArMphysOverMcalP1() : m_MphysOverMcal(float(ILArMphysOverMcal::ERRORCODE)) {} 
    LArMphysOverMcalP1(float f) 
	    : m_MphysOverMcal(f) {} 
    bool isEmpty() const { return (m_MphysOverMcal == float(ILArMphysOverMcal::ERRORCODE)); };
    float         m_MphysOverMcal;
    
};
#endif 

