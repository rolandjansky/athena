/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARH6OSCILLATIONP
#define LARRAWCONDITIONS_LARH6OSCILLATIONP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author T. Barillari
 * @version  \$Id: LArH6OscillationP.h,v 1.4 2006-07-11 07:43:21 schaffer Exp $*/


class LArH6OscillationP
{
public: 

    LArH6OscillationP() : m_channelAmplitude(-99999.), m_channelPhase(-99999.) {} 
    bool isEmpty() const { return m_channelAmplitude < -9999 && m_channelPhase < -9999 ; } ;
    double         m_channelAmplitude;
    double         m_channelPhase;

};


     
#endif 
     
