/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GoodRunsLists_TriggerRegistryTool_H
#define GoodRunsLists_TriggerRegistryTool_H

/** @file TriggerRegistryTool.h
 *  @brief This file contains the class definition for the TriggerRegistryTool class.
 *  @author Max Baak <mbaak@cern.ch>
 **/

#include "GoodRunsLists/ITriggerRegistryTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

//typedef std::pair< std::vector<std::string>, std::vector<std::string> > vvPair; 

/** @class TriggerRegistryTool
 *  @brief This file contains the class definition for the TriggerRegistryTool class.
 **/

class TriggerRegistryTool : virtual public AthAlgTool, virtual public ITriggerRegistryTool
{
 public:    
  TriggerRegistryTool( const std::string&, const std::string&, const IInterface* );
  virtual ~TriggerRegistryTool();

  /// To allow access to the ITriggerRegistryTool interface
  StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

  /// register trigger
  bool registerTriggerSelector(const TString& name, const TString& regexpr, const std::list<TString>& trigpar);
  /// get selector registry
  const std::map< TString, tvtPair >& getTriggerSelectorRegistry() { return m_registry; }

 protected:

  std::map< TString, tvtPair > m_registry;
};

#endif

