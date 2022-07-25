#include "PixelActiveDetectorElementStatusTool.h"
#include <unordered_map>

StatusCode PixelActiveDetectorElementStatusTool::initialize()
{
   ATH_CHECK( PixelDetectorElementStatusToolBase::initialize() );
   ATH_CHECK(m_condDCSStatusKey.initialize());

   m_activeStatusMask=0;
   for (unsigned int istatus=0; istatus<m_isActiveStatus.size(); istatus++) {
      unsigned int active_status=0;
      if      (m_isActiveStatus[istatus]=="OK")       { active_status = PixelDCSStatusData::DCSModuleStatus::OK; }
      else if (m_isActiveStatus[istatus]=="WARNING")  { active_status = PixelDCSStatusData::DCSModuleStatus::WARNING; }
      else if (m_isActiveStatus[istatus]=="ERROR")    { active_status = PixelDCSStatusData::DCSModuleStatus::ERROR; }
      else if (m_isActiveStatus[istatus]=="FATAL")    { active_status = PixelDCSStatusData::DCSModuleStatus::FATAL; }
      else if (m_isActiveStatus[istatus]=="NOSTATUS") { active_status = PixelDCSStatusData::DCSModuleStatus::NOSTATUS; }
      else {
         ATH_MSG_ERROR("No matching DCS status " << m_isActiveStatus[istatus] << " in DCSModuleStatus");
         return StatusCode::FAILURE;
      }
      if (active_status>31) {
         ATH_MSG_FATAL("Logic error: status id too large. Cannot be represented by a bit");
         return StatusCode::FAILURE;
      }
      m_activeStatusMask |= (1u<<active_status);
   }
   return StatusCode::SUCCESS;
}

namespace {
   inline void andStatus(const std::unordered_map<int, int> &status_map, unsigned int status_mask, std::vector<bool> &module_status) {
      for (const std::pair<const int, int> &elm : status_map ) {
         // set modules good if the module status passes the mask.
         module_status.at(elm.first) = module_status.at(elm.first) && (status_mask & (1<<elm.second));
      }
   }
}

std::unique_ptr<InDet::SiDetectorElementStatus>
PixelActiveDetectorElementStatusTool::getDetectorElementStatus(const EventContext& ctx,
                                                               SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const  {
   std::unique_ptr<InDet::SiDetectorElementStatus> element_status( createDetectorElementStatus(ctx, whandle));
   std::vector<bool> &status=element_status->getElementStatus();
   if (status.empty()) {
      status.resize(m_pixelID->wafer_hash_max(),
                    true      // default value of PixelDCSStateData  is 0
                    );
   }
   std::vector<InDet::ChipFlags_t> &chip_status=element_status->getElementChipStatus();
   if (chip_status.empty()) {
      chip_status.resize(status.size(),0xffff); // Should set this to the correct number of chips
   }

   SG::ReadCondHandle<PixelDCSStatusData> dcs_status_handle(m_condDCSStatusKey, ctx);
   andStatus(dcs_status_handle->moduleStatusMap(), m_activeStatusMask, status);
   if (whandle) {
     whandle->addDependency (dcs_status_handle);
   }

   return element_status;
}
