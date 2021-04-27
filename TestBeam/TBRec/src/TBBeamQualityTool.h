/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#ifndef TBREC_TBBEAMQUALITYTOOL_H
#define TBREC_TBBEAMQUALITYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>
#include <vector>

static const InterfaceID IID_TBBeamQualityTool("TBBeamQualityTool", 1 , 0);

class TBBeamQualityTool : public AthAlgTool
{
 public:

  TBBeamQualityTool(const std::string& name,
		    const std::string& type,
		    const IInterface* parent);
  
  virtual ~TBBeamQualityTool();
  
  virtual StatusCode accept(const std::vector<std::string>&) = 0;
  virtual StatusCode reject(const std::vector<std::string>&);
  
  virtual StatusCode initialize() override;

  virtual StatusCode initializeTool();
  
  static const InterfaceID& interfaceID() { return IID_TBBeamQualityTool;}
};
#endif
