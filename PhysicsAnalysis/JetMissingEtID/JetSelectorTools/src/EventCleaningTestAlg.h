/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_OVERLAPREMOVALTESTALG_H
#define ASSOCIATIONUTILS_OVERLAPREMOVALTESTALG_H

// Framework includes
#include "GaudiKernel/ToolHandle.h"
#include "AsgTools/AnaToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// EDM includes
#include "xAODBase/IParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"

// Local includes
#include "JetSelectorTools/IEventCleaningTool.h"

/// A testing algorithm for the dual-use event cleaning tool in Athena
///
/// @author Julia Gonski <j.gonski@cern.ch>
///
class EventCleaningTestAlg : public AthAlgorithm
{

  public:

    /// Standard algorithm constructor
    EventCleaningTestAlg(const std::string& name, ISvcLocator* svcLoc);

    /// Initialize the algorithm
    virtual StatusCode initialize() override;

    /// Finalize the algorithm
    virtual StatusCode finalize() override;

    /// Execute the algorithm
    virtual StatusCode execute() override;

  private:

    /// Handle to the tool
    asg::AnaToolHandle<ECUtils::IEventCleaningTool> m_ecTool;

    /// Configuration
    std::string m_prefix;
    std::string m_cleaningLevel;
    std::string m_collection;
    bool m_doEvent; //boolean to save event-level decoration
    std::unique_ptr<SG::AuxElement::Decorator<char>> m_dec_eventClean;
};

#endif
