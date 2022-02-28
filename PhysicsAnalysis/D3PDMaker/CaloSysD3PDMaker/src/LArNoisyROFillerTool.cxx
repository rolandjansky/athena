/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "LArNoisyROFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "Identifier/HWIdentifier.h"


namespace D3PD{

LArNoisyROFillerTool::LArNoisyROFillerTool(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  : D3PD::BlockFillerTool<LArNoisyROSummary> (type, name, parent)
{
  declareProperty("SaveNB",m_SaveNB = true);
  declareProperty("SaveFEBID",m_SaveFEBIDs = true);
  declareProperty("SavePAID",m_SavePAIDs = true);

  LArNoisyROFillerTool::book().ignore(); // Avoid coverity warnings
}


StatusCode LArNoisyROFillerTool:: book()
{
  if (m_SaveNB) CHECK( addVariable("nNoisyFEB", m_nNoisyFEB, "Number of LAr FEB declared noisy"));

  if (m_SaveFEBIDs) CHECK( addVariable("NoisyFEB", m_NoisyFEBIDs, "List of FEB ids for FEB declared noisy"));

  if (m_SaveNB) CHECK( addVariable("nNoisyPA", m_nNoisyPA, "Number of LAr preamp declared noisy"));

  if (m_SavePAIDs) CHECK( addVariable("NoisyPA", m_NoisyPAIDs, "List of preamp ids for preamp declared noisy"));

  return StatusCode::SUCCESS;
}

StatusCode LArNoisyROFillerTool::fill (const LArNoisyROSummary& c)
{
  const std::vector<HWIdentifier>& noisyfebs = c.get_noisy_febs();
  if (m_SaveNB) *m_nNoisyFEB = noisyfebs.size();

  if (m_SaveFEBIDs) 
  {
    m_NoisyFEBIDs->clear();
    for (HWIdentifier hwid : noisyfebs) {
      m_NoisyFEBIDs->push_back(hwid.get_identifier32().get_compact());
    }
  }


  const std::vector< std::pair<HWIdentifier, std::vector<int> > >& noisypas = c.get_noisy_preamps();
  if (m_SaveNB) *m_nNoisyPA = 0;

  if (m_SavePAIDs) m_NoisyPAIDs->clear();
  for ( size_t i = 0; i < noisypas.size(); i++ )
  {
    if (m_SaveNB) *m_nNoisyPA  += noisypas[i].second.size();
    if (m_SavePAIDs) 
      m_NoisyPAIDs->push_back( std::make_pair( noisypas[i].first.get_identifier32().get_compact(),noisypas[i].second));
  }

  return StatusCode::SUCCESS;
}

}


