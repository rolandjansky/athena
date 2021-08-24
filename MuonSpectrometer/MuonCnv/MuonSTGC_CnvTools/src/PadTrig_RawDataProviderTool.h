#ifndef MUONSTGC_CNVTOOLS_PADTRIG_RAWDATAPROVIDERTOOL_H
#define MUONSTGC_CNVTOOLS_PADTRIG_RAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

namespace Muon {

class PadTrig_RawDataProviderTool : virtual public IMuonRawDataProviderTool, public AthAlgTool 
{
 public:
  using IMuonRawDataProviderTool::convert;

  PadTrig_RawDataProviderTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~PadTrig_RawDataProviderTool() = default;

  //StatusCode initialize();
  //StatusCode finalize();

  // unimplemented methods
  StatusCode convert() const override;
  StatusCode convert(const ROBFragmentList&) const override;
  StatusCode convert(const std::vector<IdentifierHash>&) const override;
  StatusCode convert(const ROBFragmentList&, const std::vector<IdentifierHash>&) const override;
};

}

#endif
