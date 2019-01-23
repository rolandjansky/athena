/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMUONROISELECTIONTOOL_MUONROISELECTIONTOOL_H
#define IMUONROISELECTIONTOOL_MUONROISELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

#include "TrigTauEmulation/ILevel1SelectionTool.h"

//EDM include
#include "xAODTrigger/MuonRoI.h"

class IMuonRoISelectionTool : public virtual ILevel1SelectionTool
{
   ASG_TOOL_INTERFACE(IMuonRoISelectionTool)
    
 public:

  virtual const asg::AcceptInfo& getAcceptInfo() const = 0;
  virtual asg::AcceptData accept(const xAOD::MuonRoI& l1jet) const = 0;
  virtual ~IMuonRoISelectionTool() {};


};

#endif
