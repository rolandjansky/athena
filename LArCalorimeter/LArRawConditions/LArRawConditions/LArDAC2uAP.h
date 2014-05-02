/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARDAC2UAP
#define LARRAWCONDITIONS_LARDAC2UAP

#include <vector> 
#include "LArElecCalib/ILArDAC2uA.h"
/** c-struct reproducing the structure of the persistent data
 * @author S. Laplace
 * @version  0-0-1 , 16/01/04 */

/*  Modified for POOL persistency
	Hong Ma
	July 15, 2005
*/ 

class LArDAC2uAP
{
public: 

    LArDAC2uAP() : m_DAC2uA(float(ILArDAC2uA::ERRORCODE)) {}
    LArDAC2uAP(float DAC2uA) : m_DAC2uA(DAC2uA) {}
    bool isEmpty() const { return (m_DAC2uA == float(ILArDAC2uA::ERRORCODE));} ;
    float         m_DAC2uA;
};

     
#endif 

