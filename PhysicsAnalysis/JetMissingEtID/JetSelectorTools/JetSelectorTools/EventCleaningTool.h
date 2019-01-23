/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETSELECTORTOOLS_EVENTCLEANINGTOOL_H
#define JETSELECTORTOOLS_EVENTCLEANINGTOOL_H

/**
   @class EventCleaningTool
   @brief Class for selecting jets that pass cleaning cuts

   @author Julia Gonski
   @date   Nov 2016
*/

// Stdlib includes
#include <string>
#include <vector>
#include <unordered_map>

// Base classes
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

// Local includes
#include "IEventCleaningTool.h"
#include "xAODJet/Jet.h"
#include "xAODEventInfo/EventInfo.h"
#include "JetSelectorTools/JetCleaningTool.h"
#include "JetInterface/IJetSelector.h"

namespace ECUtils 
{ 

class EventCleaningTool : public virtual IEventCleaningTool, 
			  public asg::AsgTool
{

    /// Create a proper constructor for Athena
  ASG_TOOL_CLASS(EventCleaningTool,IEventCleaningTool)

  public: 

    /** Standard constructor */
    EventCleaningTool(const std::string& name="EventCleaningTool");

    /** Standard destructor */
    virtual ~EventCleaningTool();   
    
     /** Initialize method */
    virtual StatusCode initialize() override;

    virtual bool acceptEvent(const xAOD::JetContainer* jets) const override;

    virtual int keepJet(const xAOD::Jet& jet) const override; 
    
  private:
    double m_pt; 
    double m_eta; 
    std::string m_jvt; 
    std::string m_or; 
    std::string m_prefix; 
    bool m_decorate; 
    bool m_useDecorations;
    std::string m_cleaningLevel; 
    asg::AnaToolHandle<IJetSelector> m_jetCleaningTool; //!

    std::unique_ptr<SG::AuxElement::Decorator<char>> m_dec_jetClean;
    std::unique_ptr<SG::AuxElement::Accessor<char>> m_acc_passJvt;
    std::unique_ptr<SG::AuxElement::Accessor<char>> m_acc_passOR;


}; // End: class definition
}//ECUtils

#endif
