/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_CalibModuleListTool.h"

SCT_CalibModuleListTool::SCT_CalibModuleListTool(const std::string& type, const std::string& name, const IInterface* parent):
   base_class(type, name, parent)
{
}

StatusCode SCT_CalibModuleListTool::initialize() {
   ATH_MSG_DEBUG("Initializing SCT_CalibModuleListTool");

   ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));
   ATH_CHECK(m_MonitorConditionsTool.retrieve());
   ATH_MSG_DEBUG("in SCT_CalibModuleListTool::initialize after m_MonitorConditionsTool retrieved");
   ATH_CHECK(m_IOVDbSvc.retrieve());
   ATH_MSG_DEBUG("in SCT_CalibModuleListTool::initialize after m_IOVDbSvc retrieved");

   return StatusCode::SUCCESS;
}

StatusCode SCT_CalibModuleListTool::finalize() {
   return StatusCode::SUCCESS;
}

StatusCode SCT_CalibModuleListTool::readModuleList(std::map<Identifier, std::set<Identifier>>& moduleList) const {
   //--- Read strips using SCT_MonitorConditionsSvc
   SCT_ID::const_id_iterator waferIdItr{m_pSCTHelper->wafer_begin()};
   SCT_ID::const_id_iterator waferIdItrE{m_pSCTHelper->wafer_end()};
   for (; waferIdItr != waferIdItrE; ++waferIdItr) {
      if (m_pSCTHelper->side( *waferIdItr ) != 0) continue;
      Identifier waferId{*waferIdItr};
      Identifier moduleId{m_pSCTHelper->module_id(waferId)};

      std::set<Identifier> stripIdList;
      //ATH_MSG_DEBUG("in SCT_CalibModuleListTool::readModuleList: before accessing badStrips");
      m_MonitorConditionsTool->badStrips(moduleId, stripIdList);
      //ATH_MSG_DEBUG("in SCT_CalibModuleListTool::readModuleList: after accessing badStrips");
      if (!stripIdList.empty()) moduleList.insert(std::map<Identifier, std::set<Identifier>>::value_type(moduleId, stripIdList));
   }

   //--- Drop the folder to avoid conflict with SCTCalibWriteSvc
   if (!(m_IOVDbSvc->dropObject("/SCT/Derived/Monitoring", true))) ATH_MSG_ERROR("Unable to drop /SCT/Derived/Monitoring");

   return StatusCode::SUCCESS;
}
