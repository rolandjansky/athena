/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for Missing ET objects.
 */


#include "MissingETD3PDMaker/MissingETRegionFillerTool.h"
#include "MissingETEvent/MissingET.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MissingETRegionFillerTool::MissingETRegionFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : BlockFillerTool<MissingET> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode MissingETRegionFillerTool::book()
{
  CHECK( addVariable("etx_CentralReg", m_exCentralReg));
  CHECK( addVariable("ety_CentralReg", m_eyCentralReg));
  CHECK( addVariable("sumet_CentralReg", m_etSumCentralReg));
  CHECK( addVariable("phi_CentralReg", m_ePhiCentralReg));
    
  CHECK( addVariable("etx_EndcapRegion", m_exEndcapReg));
  CHECK( addVariable("ety_EndcapRegion", m_eyEndcapReg));
  CHECK( addVariable("sumet_EndcapRegion", m_etSumEndcapReg));
  CHECK( addVariable("phi_EndcapRegion", m_ePhiEndcapReg));
    
  CHECK( addVariable("etx_ForwardReg", m_exForwardReg));
  CHECK( addVariable("ety_ForwardReg", m_eyForwardReg));
  CHECK( addVariable("sumet_ForwardReg", m_etSumForwardReg));
  CHECK( addVariable("phi_ForwardReg", m_ePhiForwardReg));
  
  return StatusCode::SUCCESS;
}


StatusCode MissingETRegionFillerTool::fill (const MissingET& p)
{
  const MissingEtRegions *p_reg = p.getRegions();
  
  if (p_reg != 0 ) {
    *m_etSumCentralReg = p_reg->etSumReg(MissingEtRegions::Central);
    *m_exCentralReg = p_reg->exReg(MissingEtRegions::Central);
    *m_eyCentralReg = p_reg->eyReg(MissingEtRegions::Central);
    *m_ePhiCentralReg = atan2(p_reg->eyReg(MissingEtRegions::Central), p_reg->exReg(MissingEtRegions::Central));
  
    *m_etSumEndcapReg = p_reg->etSumReg(MissingEtRegions::EndCap);
    *m_exEndcapReg = p_reg->exReg(MissingEtRegions::EndCap);
    *m_eyEndcapReg = p_reg->eyReg(MissingEtRegions::EndCap);
    *m_ePhiEndcapReg = atan2(p_reg->eyReg(MissingEtRegions::EndCap), p_reg->exReg(MissingEtRegions::EndCap));
    
    *m_etSumForwardReg = p_reg->etSumReg(MissingEtRegions::Forward);
    *m_exForwardReg = p_reg->exReg(MissingEtRegions::Forward);
    *m_eyForwardReg = p_reg->eyReg(MissingEtRegions::Forward);
    *m_ePhiForwardReg = atan2(p_reg->eyReg(MissingEtRegions::Forward), p_reg->exReg(MissingEtRegions::Forward));
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
