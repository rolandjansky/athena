/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARRAMPP
#define LARRAWCONDITIONS_LARRAMPP

#include <vector> 

/** Persistent data for LArRamp 
    Copied from LAr
 * @author W. Lampl, S. Laplace, H. Ma
 * @version  0-0-1 , March 29,2005 */


/*  Modified for POOL persistency
	Hong Ma
	July 15, 2005
*/ 



class LArRampP
{
public: 

    LArRampP() {} 
    bool isEmpty() const { return (m_vRamp.size() == 0); }
    std::vector<float> m_vRamp;

};

#endif 

