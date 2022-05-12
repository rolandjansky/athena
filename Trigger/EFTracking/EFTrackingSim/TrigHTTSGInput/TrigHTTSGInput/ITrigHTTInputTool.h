/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigHTTSGInput_ITrigHTTInputTool_h
#define TrigHTTSGInput_ITrigHTTInputTool_h

#include "GaudiKernel/IAlgTool.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"

class HTTEventInputHeader;
/**
 * @brief Interface of tools responsible for generation of the HTT sim wrapper files
 * 
 */
class ITrigHTTInputTool : virtual public ::IAlgTool 
{      
 public:

  DeclareInterfaceID( ITrigHTTInputTool, 1, 0);
  virtual ~ITrigHTTInputTool(){}
  /**
   * @brief Reads the data from outer word (Athena  StoreGate) and fills the output object called header
   * 
   * @param header object to update
   * @return StatusCode 
   */
  virtual StatusCode readData(HTTEventInputHeader* header, const EventContext& eventContext) = 0;
   
};


#endif // TrigHTTSGInput_ITrigHTTInputTool_h
