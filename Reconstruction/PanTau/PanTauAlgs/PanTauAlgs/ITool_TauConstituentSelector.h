/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_TAUCONSTITUENTSELECTOR_H
#define PANTAUALGS_ITOOL_TAUCONSTITUENTSELECTOR_H

#include "AsgTools/IAsgTool.h"

#include <vector>

#include "PanTauAlgs/TauConstituent.h"

namespace PanTau {

  /** @class ITool_TauConstituentSelector
      @brief Interface for Tool_TauConstituentSelector
      @author Christian Limbach (limbach@physik.uni-bonn.de)
  */


  class ITool_TauConstituentSelector : virtual public asg::IAsgTool {

    ASG_TOOL_INTERFACE(ITool_TauConstituentSelector)

      public:
    virtual bool isInitialized() = 0;
    virtual StatusCode SelectTauConstituents(const std::vector<TauConstituent*>& inputList,
					     std::vector<TauConstituent*>& outputList) const = 0;
  };
    
}
#endif //PANTAUALGS_ITOOL_TAUCONSTITUENTSELECTOR_H 
