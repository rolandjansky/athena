/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARFSAMPLP
#define LARRAWCONDITIONS_LARFSAMPLP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author S. Laplace
 * @version  0-0-1 , 21/01/04 */

/*  Modified for POOL persistency
	Hong Ma
	July 15, 2005
*/ 

class LArfSamplP
{
public: 

    LArfSamplP() : m_fSampl(-9999999) {}
    LArfSamplP(float f) : m_fSampl(f) {}
    bool isEmpty() const { return m_fSampl < -9999 ; } ;
    float         m_fSampl;
};

     
#endif 

