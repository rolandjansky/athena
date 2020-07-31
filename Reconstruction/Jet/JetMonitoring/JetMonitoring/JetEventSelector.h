//  -*- c++ -*- 

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JetEventSelector_H
#define JETMONITORING_JetEventSelector_H
#include "AsgTools/AsgTool.h"

#include "JetInterface/IJetEventSelector.h"
#include "JetMonitoring/EventHistoVarTool.h"


class JetEventSelector : public asg::AsgTool , virtual public IJetEventSelector
 { 
   ASG_TOOL_CLASS(JetEventSelector,IJetEventSelector)
   public:
   
   JetEventSelector(const std::string &t);
   virtual ~JetEventSelector();

   virtual StatusCode initialize() ;
     
   int keep(const xAOD::EventInfo& e, const xAOD::JetContainer & jets) const;

 protected:
   
   float m_min;
   float m_max;

   ToolHandle<IEventHistoVarTool> m_var;

};




#endif
