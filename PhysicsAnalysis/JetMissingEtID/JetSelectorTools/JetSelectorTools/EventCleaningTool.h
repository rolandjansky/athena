/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __EVENTCLEANINGTOOL__
#define __EVENTCLEANINGTOOL__

/**
   @class EventCleaningTool
   @brief Class for selecting jets that pass cleaning cuts

   @author Julia Gonski
   @date   May 2017
*/

// Stdlib includes
#include <string>
#include <vector>
#include <unordered_map>

// Base classes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// Local includes
#include "PATCore/TAccept.h"
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

     /** Initialize method */
    virtual StatusCode finalize();

    virtual bool acceptEvent(const xAOD::JetContainer* jets) const override;

    virtual int keepJet(const xAOD::Jet& jet) const override; 
    
  private:
    double m_pt; 
    double m_eta; 
    std::string m_jvt; 
    std::string m_or; 
    std::string m_prefix; 
    std::string m_cleaningLevel; 
    ToolHandle<IJetSelector> m_jetCleaningTool;
    SG::AuxElement::Decorator<char>* m_dec_jetClean;
    SG::AuxElement::Accessor<char>* m_acc_passJvt;
    SG::AuxElement::Accessor<char>* m_acc_passOR;


}; // End: class definition
}//ECUtils

#endif
