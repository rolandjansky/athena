/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ILINEAR_FAKE_BKG_TOOL_H
#define ILINEAR_FAKE_BKG_TOOL_H

#include "AsgAnalysisInterfaces/IFakeBkgTool.h"

#include <string>

namespace CP
{

/// \brief Interface for the tools providing an estimate of the fake lepton background, which are able to provide an event-by-event weight
class ILinearFakeBkgTool : virtual public IFakeBkgTool
{
  public:
  
    /// \brief returns an event weight
    /// addEvent() must have been called before hand. 
    /// Can be called several times for the same event, possibly with different arguments. 
    /// The 'selection' argument indicates how the analysis selects leptons in the signal region
    /// The 'process' argument indicates what should be treated as fake lepton background. 
    /// The syntax supported for these two arguments is described in the package documentation.
    /// The extraWeight argument specified to addEvent() is NOT included in the returned weight. 
    virtual StatusCode getEventWeight(float& weight, const std::string& selection = defaultSelection(), const std::string& process = defaultProcess()) = 0;
  
};

}

#endif
