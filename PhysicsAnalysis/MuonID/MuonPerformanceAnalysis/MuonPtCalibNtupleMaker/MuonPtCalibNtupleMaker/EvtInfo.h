/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __EvtInfo_h__
#define __EvtInfo_h__

//::: STL includes
#include <string>

//::: ROOT includes
#include <TTree.h>

//::: EDM includes
#include "xAODEventInfo/EventInfo.h"

class EvtInfo { 
  
private:
  UInt_t    m_RunNumber, m_LumiBlock, m_ChannelNumber;
  ULong64_t m_EvtNumber;
  Float_t   m_EventWeight;

public:
  EvtInfo();
  ~EvtInfo() {}

  void Reset();
  void Register( TTree* t );
  void Fill( const xAOD::EventInfo* ei );

};

#endif
