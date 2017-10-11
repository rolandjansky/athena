  
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
 
#ifndef EVENTUTILS_TRIGGERSELECTIONALG_H
#define EVENTUTILS_TRIGGERSELECTIONALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// EDM includes
#include "AthContainers/AuxElement.h"

// Forward declarations
namespace Trig{
  class TrigDecisionTool;
}


class TriggerSelectionAlg
  : public ::AthFilterAlgorithm
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
  public:

    /// Constructor with parameters:
    TriggerSelectionAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /// Destructor:
    virtual ~TriggerSelectionAlg();

    /// Athena algorithm's initalize hook
    virtual StatusCode  initialize();

    /// Athena algorithm's execute hook
    virtual StatusCode  execute();

    /// Athena algorithm's finalize hook
    virtual StatusCode  finalize();


  private:

    /// @name The properties that can be defined via the python job options
    /// @{

    /// The ToolHandle for the TrigDecisionTool
    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;

    /// The list of triggers to cut on
    StringArrayProperty m_triggerList;

    /// Decide if we also want to decorate the xAOD::EventInfo object with the pass/fail information
    BooleanProperty m_decoEvtInfo;

    /// Name of the xAOD::EventInfo object that we want to decorate
    StringProperty m_evtInfoName;

    /// Prefix used for the decoration variables
    StringProperty m_varPrefix;

    /// Decide if we also want to decorate the xAOD::EventInfo object with the (full-chain) prescale information
    BooleanProperty m_storePrescaleInfo;

    /// @}

    /// @name Other private members
    /// @{

    /// The list of all variables names
    std::vector<std::string> m_varNameList;

    /// @}

};



#endif //> !EVENTUTILS_TRIGGERSELECTIONALG_H
