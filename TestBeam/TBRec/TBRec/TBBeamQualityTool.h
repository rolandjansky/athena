/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityTool.h                      #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#          based on TBEventStreamerTool             #
//# Last Modified: Nov 18/2004                        #
//#####################################################

#ifndef TBBEAMQUALITYTOOL_H
#define TBBEAMQUALITYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>
#include <vector>

class StoreGateSvc;

static const InterfaceID IID_TBBeamQualityTool("TBBeamQualityTool", 1 , 0);

class TBBeamQualityTool : public AthAlgTool
{
 public:

  TBBeamQualityTool(const std::string& name,
		    const std::string& type,
		    const IInterface* parent);
  
  virtual ~TBBeamQualityTool();
  
  virtual StatusCode accept(std::vector<std::string>) = 0;
  virtual StatusCode reject(std::vector<std::string>);
  
  virtual StatusCode initialize();

  virtual StatusCode initializeTool();
  
  static const InterfaceID& interfaceID() { return IID_TBBeamQualityTool;}
};
#endif
