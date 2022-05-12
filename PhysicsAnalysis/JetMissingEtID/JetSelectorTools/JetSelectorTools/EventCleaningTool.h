/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"

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
    Gaudi::Property<double>      m_pt               {this, "PtCut" ,                  20000.,     "Jet pt cut"};
    Gaudi::Property<double>      m_eta              {this, "EtaCut" ,                 4.5,        "Jet eta cut"};
    Gaudi::Property<std::string> m_prefix           {this, "JetCleanPrefix",          "",         "Jet cleaning decoration prefix"};
    Gaudi::Property<bool>        m_decorate         {this, "DoDecorations",           true,       "Decorate jets with cleaning decision?"};
    Gaudi::Property<bool>        m_useDecorations   {this, "UseDecorations",          false,      "Use decorations in cleaning tool?"};
    Gaudi::Property<std::string> m_cleaningLevel    {this, "CleaningLevel",           "LooseBad", "Jet cleaning level"};
    Gaudi::Property<bool>        m_suppressInputDeps{this, "SuppressInputDependence", false,      "Will passOR be created in the same algorithm that uses this tool?"};
    asg::AnaToolHandle<IJetSelector> m_jetCleaningTool; //!

    Gaudi::Property<std::string> m_jetContainerName          {this, "JetContainer", "",        "Input jet container key"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_passJvtKey  {this, "JvtDecorator", "passJvt", "JVT decision decoration key"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_passORKey   {this, "OrDecorator" , "passOR",  "Overlap removal decision decoration key"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_jetCleanKey{this, "JetCleanKey" , "",        "Jet-level cleaning decoration key (autoconfigured)"};

}; // End: class definition
}//ECUtils

#endif
