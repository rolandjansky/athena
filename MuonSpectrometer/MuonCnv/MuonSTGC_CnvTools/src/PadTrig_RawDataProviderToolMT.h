#ifndef MUONSTGC_CNVTOOLS_PADTRIG_RAWDATAPROVIDERTOOLMT_H
#define MUONSTGC_CNVTOOLS_PADTRIG_RAWDATAPROVIDERTOOLMT_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonRDO/NSW_PadTriggerDataContainer.h"
#include "MuonSTGC_CnvTools/IPadTrig_ROD_Decoder.h"

namespace Muon {

class PadTrig_RawDataProviderToolMT : virtual public IMuonRawDataProviderTool, public AthAlgTool 
{
 public:
  using IMuonRawDataProviderTool::convert;
  
  PadTrig_RawDataProviderToolMT(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~PadTrig_RawDataProviderToolMT() = default;

  StatusCode initialize() override;

  // unimplemented
  StatusCode convert() const override;
  StatusCode convert(const ROBFragmentList&) const override;
  StatusCode convert(const std::vector<IdentifierHash>&) const override;
  StatusCode convert(const ROBFragmentList&, const std::vector<IdentifierHash>&) const override;

  // implemented
  StatusCode convert(const ROBFragmentList& fragments, const EventContext& ctx) const override;
  StatusCode convert(const EventContext& ctx) const override;

 private:
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ToolHandle<IPadTrig_ROD_Decoder>      m_decoder{this, "Decoder", "Muon::PadTrig_ROD_Decoder/PadTrig_ROD_Decoder"};
  ServiceHandle<IROBDataProviderSvc>    m_robDataProvider;
  SG::WriteHandleKey<NSW_PadTriggerDataContainer> m_rdoContainerKey{this, "RdoLocation", "PADTRGRDO", "Name of of the RDO container to write to"};
  
  unsigned int m_maxhashtoUse{0};
};

}  // namespace Muon

#endif  // MUONSTGC_CNVTOOLS_PADTRIG_RAWDATAPROVIDERTOOLMT_H
