/* Dear emacs, this is -*-c++-*- */
#ifndef PIXELDETECTORELEMENTSTATUSCLONETOOL_H
#define PIXELDETECTORELEMENTSTATUSCLONETOOL_H

#include "PixelDetectorElementStatusToolBase.h"

/** Tool to extend pixel detector element status data by the status extracted from the bytestream error container.
 * The input to the tool can only be conditions data, the output will have an invalid IOV. Thus, can only be
 * event data.
 */
class PixelDetectorElementStatusCloneTool : public PixelDetectorElementStatusToolBase
{
public:
   PixelDetectorElementStatusCloneTool(const std::string& type, const std::string& name, const IInterface* parent)
      :  PixelDetectorElementStatusToolBase(type,name, parent)
   {}
   static InterfaceID& interfaceID();

   virtual StatusCode initialize() override;

   virtual std::unique_ptr<InDet::SiDetectorElementStatus>
   getDetectorElementStatus(const EventContext& ctx,
                            SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const override;
};

inline InterfaceID& PixelDetectorElementStatusCloneTool::interfaceID(){
  static InterfaceID IID_PixelDetectorElementStatusCloneTool("PixelDetectorElementStatusCloneTool", 1, 0);
  return IID_PixelDetectorElementStatusCloneTool;
}

#endif
