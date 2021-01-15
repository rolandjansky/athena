/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTT_IINPUTTOOL_H
#define HTT_IINPUTTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"

class HTTEventInputHeader;

static const InterfaceID IID_HTT_IInputTool("HTT_IInputTool",1,0);

class HTT_IInputTool : virtual public ::IAlgTool 
{      
 public:

  DeclareInterfaceID( HTT_IInputTool, 1, 0);
  virtual ~HTT_IInputTool(){}
   
  virtual StatusCode readData(HTTEventInputHeader* header) = 0;
  
 protected:
   HTTEventInputHeader*        m_eventHeader;
 
};


#endif // HTT_IINPUTTOOL_H
