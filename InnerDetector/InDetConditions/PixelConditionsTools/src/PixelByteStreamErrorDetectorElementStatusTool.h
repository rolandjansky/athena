/* Dear emacs, this is -*-c++-*- */
#ifndef PIXELBYTESTREAMERRORDETECTORELEMENTSTATUSTOOL_H
#define PIXELBYTESTREAMERRORDETECTORELEMENTSTATUSTOOL_H

#include "PixelDetectorElementStatusToolBase.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "Gaudi/Property.h"
#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"

/** Tool to extend pixel detector element status data by the status extracted from the bytestream error container.
 * The input to the tool can only be conditions data, the output will have an invalid IOV. Thus, can only be
 * event data.
 */
class PixelByteStreamErrorDetectorElementStatusTool : public PixelDetectorElementStatusToolBase
{
public:
   PixelByteStreamErrorDetectorElementStatusTool(const std::string& type, const std::string& name, const IInterface* parent)
      :  PixelDetectorElementStatusToolBase(type,name, parent)
   {}
   static InterfaceID& interfaceID();

   virtual StatusCode initialize() override;

   virtual std::unique_ptr<InDet::SiDetectorElementStatus>
   getDetectorElementStatus(const EventContext& ctx,
                            SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const override;
private:
   SG::ReadHandleKey<IDCInDetBSErrContainer>  m_BSErrContReadKey
      {this, "PixelByteStreamErrs", "PixelByteStreamErrs", "PixelByteStreamErrs container key"};

   Gaudi::Property<bool> m_useByteStreamFEI4
      {this, "UseByteStreamFEI4", false, "Switch of the ByteStream error for FEI4"};

   Gaudi::Property<bool> m_useByteStreamFEI3
      {this, "UseByteStreamFEI3", false, "Switch of the ByteStream error for FEI3"};

   Gaudi::Property<bool> m_useByteStreamRD53
      {this, "UseByteStreamRD53", false, "Switch of the ByteStream error for RD53"};

   Gaudi::Property< bool> m_activeOnly
      {this, "ActiveOnly", false, "Module and chip status will only reflect whether the modules or chips are active not necessarily whether the signals are good."};

   unsigned int m_readoutTechnologyMask {}; ///< Mask where each bit represents a readout technology;
};

inline InterfaceID& PixelByteStreamErrorDetectorElementStatusTool::interfaceID(){
  static InterfaceID IID_PixelByteStreamErrorDetectorElementStatusTool("PixelByteStreamErrorDetectorElementStatusTool", 1, 0);
  return IID_PixelByteStreamErrorDetectorElementStatusTool;
}

#endif
