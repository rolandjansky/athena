/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDetectorElementStatusToolBase.h"
#include "PixelDetectorElementStatus.h"

PixelDetectorElementStatusToolBase::PixelDetectorElementStatusToolBase(const std::string& type, const std::string& name, const IInterface* parent)
  :base_class(type, name, parent)
{
}

PixelDetectorElementStatusToolBase::~PixelDetectorElementStatusToolBase(){}

StatusCode PixelDetectorElementStatusToolBase::initialize(){
  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));
  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  ATH_CHECK(m_pixelDetElStatusCondKey.initialize( !m_pixelDetElStatusCondKey.empty()));

  return StatusCode::SUCCESS;
}

namespace {
   const InDet::PixelDetectorElementStatus *castToDerived(const InDet::SiDetectorElementStatus *input) {
      const InDet::PixelDetectorElementStatus *ret = dynamic_cast<const InDet::PixelDetectorElementStatus *>(input);
      if (!ret) {
         throw std::runtime_error("Object is not of expected type InDet::PixelDetectorElementStatus");
      }
      return ret;
   }
}

std::tuple<std::unique_ptr<InDet::SiDetectorElementStatus>, EventIDRange> PixelDetectorElementStatusToolBase::createDetectorElementStatus(const EventContext& ctx) const  {
   if (!m_pixelDetElStatusCondKey.empty()) {
      SG::ReadCondHandle<InDet::SiDetectorElementStatus> input_element_status{m_pixelDetElStatusCondKey, ctx};
      return std::make_tuple(std::unique_ptr<InDet::SiDetectorElementStatus>(new InDet::PixelDetectorElementStatus(*castToDerived(input_element_status.cptr()))),
                             input_element_status.getRange() );
   }
   else {
      SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey, ctx);
      if (not pixelDetEleHandle.isValid() ) {
         std::stringstream msg;
         msg << m_pixelDetEleCollKey.fullKey() << " is not available.";
         throw std::runtime_error(msg.str());
      }
      const InDetDD::SiDetectorElementCollection* elements(*pixelDetEleHandle);
      return std::make_tuple(std::unique_ptr<InDet::SiDetectorElementStatus>(new InDet::PixelDetectorElementStatus(*elements)),
                             pixelDetEleHandle.getRange() );
   }
}


