//  -*- c++ -*- 

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


This is a temporary file which holds the older version of JetSelectorAttribute used in RunII
monitoring. It's here to allow the RunII monitoring system to continue to run during the migration.
It's meant to be removed together with the runII code when migration is complete.

*/

#ifndef JETMONITORING_JETSELECTORATTRIBUTERUNII_H
#define JETMONITORING_JETSELECTORATTRIBUTERUNII_H

#include "JetInterface/IJetSelector.h"

#include "AsgTools/AsgTool.h"

class JetSelectorAttributeRunII : public asg::AsgTool , virtual public IJetSelector
 { 
   ASG_TOOL_CLASS(JetSelectorAttributeRunII,IJetSelector)
   public:

   /// \class SelValueRetriever
   /// SelValueRetriever retrieves the value on which this tool select jets.
   /// concrete classes are used internally to retrieve the attribute according to its type. 
   struct SelValueRetriever {
     virtual float value(const xAOD::Jet& j) const = 0;
     virtual ~SelValueRetriever(){};
   };
   
   JetSelectorAttributeRunII(const std::string &t);
   virtual ~JetSelectorAttributeRunII();

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

