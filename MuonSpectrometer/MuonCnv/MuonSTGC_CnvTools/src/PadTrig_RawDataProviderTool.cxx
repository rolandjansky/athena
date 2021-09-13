#include "PadTrig_RawDataProviderTool.h"

namespace Muon {

//=====================================================================
PadTrig_RawDataProviderTool::PadTrig_RawDataProviderTool(const std::string& type, const std::string& name, const IInterface* parent)
: AthAlgTool(type, name, parent) 
{
  declareInterface<IMuonRawDataProviderTool>(this);
}

//=====================================================================
StatusCode PadTrig_RawDataProviderTool::convert(const ROBFragmentList&) const 
{
  ATH_MSG_ERROR(__PRETTY_FUNCTION__ << " not yet implemented!");
  return StatusCode::FAILURE;
}

StatusCode PadTrig_RawDataProviderTool::convert(const ROBFragmentList&, const std::vector<IdentifierHash>&) const 
{
  ATH_MSG_ERROR(__PRETTY_FUNCTION__ << " not yet implemented!");
  return StatusCode::FAILURE;
}

StatusCode PadTrig_RawDataProviderTool::convert() const 
{
  ATH_MSG_ERROR(__PRETTY_FUNCTION__ << " not yet implemented!");
  return StatusCode::FAILURE;
}

StatusCode PadTrig_RawDataProviderTool::convert(const std::vector<IdentifierHash>&) const 
{
  ATH_MSG_ERROR(__PRETTY_FUNCTION__ << " not yet implemented!");
  return StatusCode::FAILURE;
}

}
