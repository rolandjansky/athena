/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCEvent.cc
                                      QCJP, 1999
*/
// ====================================================================
#include <iostream> 

#include "TrigT1TGC/TGCEvent.h"
#include "TrigT1TGC/TGCASDOut.h"

namespace LVL1TGCTrigger {

// ====================================================================
//
// class description
//
// ====================================================================

////////////////////
TGCEvent::TGCEvent()
  : m_eventNumber(0)
////////////////////
{
}


/////////////////////
TGCEvent::~TGCEvent()
/////////////////////  
{
}



/////////////////////////////////////////////////////////////////////////
TGCASDOut* TGCEvent::NewASDOut(TGCIndex tgcindex, int ilyr, 
			       TGCSignalType sigtype, int id, double tof)
/////////////////////////////////////////////////////////////////////////
{
  TGCASDOut* asdout= new TGCASDOut(tgcindex, ilyr, sigtype, id, tof);
  m_vecASDOut.push_back(asdout);
  return asdout;
}


/////////////////////////////////////////////////////////////////////////
TGCASDOut* TGCEvent::NewASDOut(TGCReadoutIndex tgcrindex, 
			       TGCSignalType sigtype, int id, double tof)
/////////////////////////////////////////////////////////////////////////
{
  TGCASDOut* asdout= new TGCASDOut(tgcrindex, sigtype, id, tof);
  m_vecASDOut.push_back(asdout);
  return asdout;
}

//////////////////////
void TGCEvent::Clear()
//////////////////////
{

  // delete ASDOut
  std::vector<TGCASDOut*>::iterator it_vecASDOut;
  for (it_vecASDOut= m_vecASDOut.begin(); 
       it_vecASDOut != m_vecASDOut.end(); it_vecASDOut++) {

    if(*it_vecASDOut) delete *it_vecASDOut;
  }
  m_vecASDOut.clear();
}


////////////////////////////
void TGCEvent::Print() const
////////////////////////////
{
  std::cout << " Event#= " << m_eventNumber << std::endl;
}


} //end of namespace bracket
