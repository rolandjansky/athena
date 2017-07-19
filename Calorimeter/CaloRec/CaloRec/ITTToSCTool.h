/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class ITTToSCTool
 * @author Walter Hopkins
 * @brief Interface definition for tools to build LArRawChannels
 * 
 */

#ifndef CALOREC_ITTTOSCTOOL_H
#define CALOREC_ITTTOSCTOOL_H
class CaloCellContainer;
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IAlgTool.h"
static const InterfaceID IID_ITTToSCTool("ITTToSCTool", 1 , 0);
class ITTToSCTool: virtual public IAlgTool
{
 public:
  
  virtual ~ITTToSCTool() { };
  virtual StatusCode buildTileSCs(CaloCellContainer* caloCell)=0;
	static const InterfaceID& interfaceID() { return IID_ITTToSCTool;}
};
#endif

