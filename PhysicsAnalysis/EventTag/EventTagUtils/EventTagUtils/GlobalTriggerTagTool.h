/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GLOBALTRIGGERTAGTOOL_H 
#define GLOBALTRIGGERTAGTOOL_H 

/*****************************************************************************
Name    : GlobalTriggerTagTool.h
Package : offline/PhysicsAnalysis/EventTag/EventTagUtils
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : Tool to buid the Global Trigegr Tags
*****************************************************************************/

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

//#include "CoolKernel/IDatabase.h"

#include <map>
#include <vector>
#include <string>

//using namespace Trig;
namespace Trig {
  class ChainGroup;
}

/** Interface ID for GlobalTriggerTagTool */  
static const InterfaceID IID_GlobalTriggerTagTool("GlobalTriggerTagTool", 1, 0);

class GlobalTriggerTagTool : public AthAlgTool, virtual public IIncidentListener {

public:
  
  /** Standard Constructor */
  GlobalTriggerTagTool(const std::string& type, const std::string& name, const IInterface* 
parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_GlobalTriggerTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual void handle(const Incident&);
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const std::vector<int>& max);
  virtual StatusCode execute(TagFragmentCollection& eventTagCol, const std::vector<int>& max); 
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~GlobalTriggerTagTool( );

private:

  inline bool isPassed(std::string, unsigned int);
  inline bool isPassedBits(std::string, unsigned int);

  std::map<int,std::string> getChainCounter(std::string level);

  ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  //const ChainGroup* m_level;
  std::map<int, std::string> m_ccnameL1;
  std::map<int, std::string> m_ccnameHLT;

  /** the attribute names */
  std::vector<std::string> m_lv1StrType;
  std::vector<std::string> m_lv1Str;
  std::vector<std::string> m_hltStr;

   /** stream info */
   std::map<std::string,unsigned int> m_TDTstreamMap;

  ServiceHandle<StoreGateSvc> m_pMetaDataStore;

 };

#endif // GLOBALTRIGGERTAGTOOL_H



