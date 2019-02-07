/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_CalibModuleListTool.h
* Header file for the SCT_CalibModuleListTool class
* @author Shaun Roe
**/

#ifndef SCT_CalibModuleListTool_h
#define SCT_CalibModuleListTool_h

// STL and boost headers
#include <string>
#include <vector>
#include <set>
#include <map>

#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "SCT_CalibAlgs/ISCT_CalibModuleListTool.h"
#include "SCT_ConditionsTools/ISCT_MonitorConditionsTool.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"

//fwd declarations
class StatusCode;

class SCT_CalibModuleListTool : public extends<AthAlgTool, ISCT_CalibModuleListTool>
{

   public:
      //@name Tool methods, reimplemented
      //@{
      SCT_CalibModuleListTool(const std::string&, const std::string&, const IInterface*);
      virtual ~SCT_CalibModuleListTool() = default;
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
      //@}
      virtual StatusCode readModuleList( std::map< Identifier, std::set<Identifier> >& moduleList );

   private:
      const SCT_ID* m_pSCTHelper;
      ServiceHandle<StoreGateSvc> m_detStore;
      ToolHandle<ISCT_MonitorConditionsTool> m_MonitorConditionsTool{this, "SCT_MonitorConditionsTool", "SCT_MonitorConditionsTool/InDetSCT_MonitorConditionsTool", "Tool to retrieve noisy strip information"};
      ServiceHandle<IIOVDbSvc> m_IOVDbSvc;

};
#endif
