/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_OVERLAPREMOVALTESTALG_H
#define ASSOCIATIONUTILS_OVERLAPREMOVALTESTALG_H

// Framework includes
#include "GaudiKernel/ToolHandle.h"
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
    virtual StatusCode initialize();
    
    /// Finalize the algorithm
    virtual StatusCode finalize();

    /// Execute the algorithm
    virtual StatusCode execute();

  private:

    /// Handle to the tool
    ToolHandle<ECUtils::IEventCleaningTool> m_ecTool;
    
    /// Configuration
    double m_pt;
    double m_eta;
    std::string m_or;
    std::string m_jvt;
    std::string m_prefix;
    std::string m_cleaningLevel;
    std::string m_collection;
    bool m_doEvent;
    SG::AuxElement::Decorator<char>* m_dec_eventClean; 
};

#endif
