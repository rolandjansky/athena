/* Dear emacs, this is -*-c++-*- */
#ifndef PIXELACTIVEDETECTORELEMENTSTATUSTOOL_H
#define PIXELACTIVEDETECTORELEMENTSTATUSTOOL_H

#include "PixelDetectorElementStatusToolBase.h"
#include "PixelConditionsData/PixelDCSStatusData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "Gaudi/Property.h"
#include <memory>
#include <tuple>
#include <vector>
#include <string>

namespace InDet{
  class SiDetectorElementStatus;
}

class EventContext;

class PixelActiveDetectorElementStatusTool : public PixelDetectorElementStatusToolBase
{
public:
   PixelActiveDetectorElementStatusTool(const std::string& type, const std::string& name, const IInterface* parent)
      :  PixelDetectorElementStatusToolBase(type,name, parent)
   {}
   static InterfaceID& interfaceID();

   virtual StatusCode initialize() override;

   virtual std::unique_ptr<InDet::SiDetectorElementStatus>
   getDetectorElementStatus(const EventContext& ctx,
                            SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const override;
private:
   SG::ReadCondHandleKey<PixelDCSStatusData> m_condDCSStatusKey
      {this, "PixelDCSStatusCondData", "PixelDCSStatusCondData", "Pixel FSM status key"};
   Gaudi::Property< std::vector<std::string> > m_isActiveStatus
      {this,  "IsActiveStatus", {},"Pixel module status for which the module is considered active." };

   unsigned int m_activeStatusMask{0}; ///< mask in which each status is represented by a bit and for status values which are cnsidered active the corresponding bit is set;
};

inline InterfaceID& PixelActiveDetectorElementStatusTool::interfaceID(){
  static InterfaceID IID_PixelActiveDetectorElementStatusTool("PixelActiveDetectorElementStatusTool", 1, 0);
  return IID_PixelActiveDetectorElementStatusTool;
}

#endif
