/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWUTILS_LARRAWORDERING_H
#define LARRAWUTILS_LARRAWORDERING_H


#include "LArRawEvent/LArRawChannel.h" 

class LArRoI_Map; 


class LArRawOrdering
{

  public: 
    /** Constructor 
	need LArRoI_Map to do the job. 
    */ 
    LArRawOrdering ( LArRoI_Map* map ); 
    LArRawOrdering (  ); 
    void setMap(LArRoI_Map* map) ; 
    bool operator () (const LArRawChannel* ch1, 
		      const LArRawChannel* ch2);

 private: 
   LArRoI_Map* m_roiMap; 

};


#endif 
