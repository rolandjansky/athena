/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISTRIPTDSTOOL_H
#define ISTRIPTDSTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "TrigT1NSWSimTools/StripData.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include <vector>

namespace NSWL1 {

class StripOfflineData;

  /**
   *
   *   @short interface for the StripTDS tools
   *
   * This class implements the public interface of the TDS Tools.
   * It returns a vector of StripData to input the StripTrigger simulation.
   *
   *
   *
   */


  class IStripTdsTool: public virtual IAlgTool {

  public:
    virtual ~IStripTdsTool() {}

    virtual StatusCode gather_strip_data(std::vector<std::unique_ptr<StripData>>& strips, const std::vector<std::unique_ptr<PadTrigger>>& padTriggers) = 0;

    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IStripTdsTool("NSWL1::IStripTdsTool", 1 ,0);
        return IID_IStripTdsTool;
    }

  };

}

#endif
