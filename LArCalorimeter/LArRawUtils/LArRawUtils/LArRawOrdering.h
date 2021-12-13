/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWUTILS_LARRAWORDERING_H
#define LARRAWUTILS_LARRAWORDERING_H


#include "LArRawEvent/LArRawChannel.h"

class LArRoIMap; 


class LArRawOrdering
{

  public: 
    /** Constructor 
	need LArRoIMap to do the job. 
    */ 
    LArRawOrdering ( const LArRoIMap* map ); 
    LArRawOrdering (  ); 
    void setMap(const LArRoIMap* map) ; 
    bool operator () (const LArRawChannel* ch1, 
		      const LArRawChannel* ch2);

 private: 
   const LArRoIMap* m_roiMap; 

};


#endif 
