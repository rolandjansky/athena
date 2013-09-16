/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARRAMPPTMP
#define LARRAWCONDITIONS_LARRAMPPTMP

#include <vector> 

/** Persistent data for LArRamp 
    Copied from LAr
 * @author W. Lampl, S. Laplace, H. Ma
 * @version  0-0-1 , March 29,2005 */


/*  Modified for POOL persistency
	Hong Ma
	July 15, 2005
*/ 



class LArRampPTmp
{
public: 

    LArRampPTmp() : m_gain(0) {} 
    LArRampPTmp(HWIdentifier id,int g) : m_channelID(id), m_gain(g) {} 
    bool isEmpty() { return (m_vRamp.size() == 0); }
    HWIdentifier m_channelID;
    unsigned int m_gain;
    std::vector<float> m_vRamp;

};

#endif 

