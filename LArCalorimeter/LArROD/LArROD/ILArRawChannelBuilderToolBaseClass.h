/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderToolBaseClass
 * @author Rolf Seuster
 * @brief base Definition of all tools called by LArRawChannelBuilderDriver
 * 
 */

#ifndef LARROD_ILARRAWCHANNELBUILDERTOOLBASECLASS_H
#define LARROD_ILARRAWCHANNELBUILDERTOOLBASECLASS_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"

#include "LArROD/LArRawChannelBuilderParams.h"

class ILArRawChannelBuilderToolBaseClass : public virtual IAlgTool
{
 public:
  
  ~ILArRawChannelBuilderToolBaseClass() { };
  
  virtual StatusCode initToolHidden(LArRawChannelBuilderParams *theParams) =0;
  
  virtual void initEventHidden() =0;
  
  virtual void finalEventHidden() =0;
  
  virtual void printSummary() =0;  
  
};

#endif

