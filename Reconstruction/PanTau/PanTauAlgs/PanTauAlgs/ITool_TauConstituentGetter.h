/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_TAUCONSTITUENTGETTER_H
#define PANTAUALGS_ITOOL_TAUCONSTITUENTGETTER_H

#include "AsgTools/IAsgTool.h"

#include <string>

namespace PanTau{
  class TauConstituent;
}

#include "xAODTau/TauJet.h"

namespace PanTau {


  /** @class ITool_TauConstituentGetter
      @brief Interface for Tool_TauConstituentGetter
      @author Christian Limbach (limbach@physik.uni-bonn.de)
  */


  class ITool_TauConstituentGetter : virtual public asg::IAsgTool {

    ASG_TOOL_INTERFACE(ITool_TauConstituentGetter)

      public:
            
    virtual bool isInitialized() = 0;
    virtual StatusCode GetTauConstituents(const xAOD::TauJet* tauJet,
					  std::vector<TauConstituent*>& outputList,
					  const std::string& algName) const = 0;
            
  };
    
}
#endif //PANTAUALGS_ITOOL_TAUCONSTITUENTGETTER_H 
