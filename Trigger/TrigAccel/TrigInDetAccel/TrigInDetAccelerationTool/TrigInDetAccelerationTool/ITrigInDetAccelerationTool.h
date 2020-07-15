/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETACCELERATIONTOOL_ITRIGINDETACCELERATIONTOOL_H
#define TRIGINDETACCELERATIONTOOL_ITRIGINDETACCELERATIONTOOL_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"
#include "TrigAccelEvent/DataExportBuffer.h"

static const InterfaceID IID_ITrigInDetAccelerationTool("ITrigInDetAccelerationTool", 1 , 0); 

class ITrigInDetAccelerationTool: virtual public IAlgTool 
{
 public:

  static const InterfaceID& interfaceID() {
    return IID_ITrigInDetAccelerationTool;
  }

  virtual size_t exportSeedMakingJob(const TrigCombinatorialSettings&, const IRoiDescriptor*, const std::vector<TrigSiSpacePointBase>&, TrigAccel::DATA_EXPORT_BUFFER&) const = 0;

};

#endif
