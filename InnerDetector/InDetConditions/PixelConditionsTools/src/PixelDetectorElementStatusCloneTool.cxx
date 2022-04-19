#include "PixelDetectorElementStatusCloneTool.h"

StatusCode PixelDetectorElementStatusCloneTool::initialize()
{
   return PixelDetectorElementStatusToolBase::initialize();
}

std::tuple<std::unique_ptr<InDet::SiDetectorElementStatus>, EventIDRange> PixelDetectorElementStatusCloneTool::getDetectorElementStatus(const EventContext& ctx) const  {
   std::tuple< std::unique_ptr<InDet::SiDetectorElementStatus>,EventIDRange> element_status_and_range( createDetectorElementStatus(ctx));
   EventIDRange &the_range = std::get<1>(element_status_and_range);
   InDet::SiDetectorElementStatus *element_status = std::get<0>(element_status_and_range).get();
   std::vector<bool> &status=element_status->getElementStatus();
   if (status.empty()) {
      status.resize(m_pixelID->wafer_hash_max(),true );
   }
   std::vector<InDet::ChipFlags_t> &chip_status=element_status->getElementChipStatus();
   if (chip_status.empty()) {
      chip_status.resize(status.size(),0xffff);
   }

   the_range = IDetectorElementStatusTool::getInvalidRange();


   return element_status_and_range;
}
