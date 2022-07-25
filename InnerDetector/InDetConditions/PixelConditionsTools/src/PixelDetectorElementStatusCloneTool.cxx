#include "PixelDetectorElementStatusCloneTool.h"

StatusCode PixelDetectorElementStatusCloneTool::initialize()
{
   return PixelDetectorElementStatusToolBase::initialize();
}

std::unique_ptr<InDet::SiDetectorElementStatus>
PixelDetectorElementStatusCloneTool::getDetectorElementStatus(const EventContext& ctx,
                                                              SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const  {
   std::unique_ptr<InDet::SiDetectorElementStatus> element_status( createDetectorElementStatus(ctx, whandle));
   std::vector<bool> &status=element_status->getElementStatus();
   if (status.empty()) {
      status.resize(m_pixelID->wafer_hash_max(),true );
   }
   std::vector<InDet::ChipFlags_t> &chip_status=element_status->getElementChipStatus();
   if (chip_status.empty()) {
      chip_status.resize(status.size(),0xffff);
   }

   if (whandle) {
     ATH_MSG_ERROR("PixelDetectorElementStatusCloneTool is not for use with conditions objects");
     whandle->addDependency (IDetectorElementStatusTool::getInvalidRange());
   }

   return element_status;
}
