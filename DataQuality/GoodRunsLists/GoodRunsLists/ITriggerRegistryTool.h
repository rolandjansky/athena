/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GoodRunsLists_ITriggerRegistryTool_H
#define GoodRunsLists_ITriggerRegistryTool_H

#include "GaudiKernel/IAlgTool.h"
#include "TString.h"
#include <list>
#include <map>

typedef std::pair< TString, std::list<TString> > tvtPair;

class ITriggerRegistryTool : virtual public IAlgTool {
public:

   static const InterfaceID& interfaceID() {
     static const InterfaceID ITriggerRegistryToolID( "ITriggerRegistryTool", 1, 0 );
     return ITriggerRegistryToolID;
   }

   /// register grl/brl combination 
   virtual bool registerTriggerSelector(const TString& name, const TString& regexpr, const std::list<TString>& trigpar) = 0 ;
   /// get GRL selector registry
   virtual const std::map< TString, tvtPair >& getTriggerSelectorRegistry() = 0 ;
};

#endif // GoodRunsLists_ITriggerRegistryTool_H

