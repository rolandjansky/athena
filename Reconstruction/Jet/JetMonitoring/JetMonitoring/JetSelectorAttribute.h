//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETSELECTORATTRIBUTE_H
#define JETMONITORING_JETSELECTORATTRIBUTE_H
#include "AsgTools/AsgTool.h"

#include "JetInterface/IJetSelector.h"
#include "JetMonitoring/JetHistoVarTool.h"


namespace JetVar {
  class Variable;
}

class JetSelectorAttribute : public asg::AsgTool , virtual public IJetSelector
 { 
   ASG_TOOL_CLASS(JetSelectorAttribute,IJetSelector)
   public:
   
   JetSelectorAttribute(const std::string &t);
   virtual ~JetSelectorAttribute();

   virtual StatusCode initialize() ;
     
   int keep(const xAOD::Jet& jet) const;

 protected:
   
   float m_min;
   float m_max;

   ToolHandle<IJetHistoVarTool> m_var;

};




#endif
