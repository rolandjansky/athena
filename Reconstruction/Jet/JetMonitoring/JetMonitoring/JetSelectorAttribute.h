//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETSELECTORATTRIBUTE_H
#define JETMONITORING_JETSELECTORATTRIBUTE_H

#include "JetInterface/IJetSelector.h"

#include "AsgTools/AsgTool.h"

class JetSelectorAttribute : public asg::AsgTool , virtual public IJetSelector
 { 
   ASG_TOOL_CLASS(JetSelectorAttribute,IJetSelector)
   public:

   /// \class SelValueRetriever retrieves the value on which this tool select jets.
   /// concrete classes are used internally to retrieve the attribute according to its type. 
   struct SelValueRetriever {
     virtual float value(const xAOD::Jet& j) = 0;
     virtual ~SelValueRetriever(){};
   };
   
   JetSelectorAttribute(const std::string &t);
   virtual ~JetSelectorAttribute();

   virtual StatusCode initialize() ;
     
   int keep(const xAOD::Jet& jet) const;

 protected:
   
   float m_min;
   float m_max;

   std::string m_attName;
   std::string m_attType;
   
   int m_vectorAttIndex; /// if the attribute is a vector we'll use the value at this index. else it is ignored.
   SelValueRetriever *m_vretriever;
   
};




#endif
