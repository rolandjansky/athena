/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "L1NonlinearLutDatabaseOverrideTool.h"

#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloDisabledTowersContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannelsContainer.h"

namespace LVL1 {

L1NonlinearLutDatabaseOverrideTool::L1NonlinearLutDatabaseOverrideTool(
                          const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :  AthAlgTool(t,n,p)
{
  declareInterface<IL1DatabaseOverrideTool>(this);
  declareProperty("JepOffset", m_offset);
  declareProperty("JepSlope", m_slope);
  declareProperty("JepAmplitude", m_amplitude);
  declareProperty("JepExponent", m_exponent);
  declareProperty("JepNoiseFraction", m_noiseFraction);
  declareProperty("JepStrategy", m_strategy);
}

L1NonlinearLutDatabaseOverrideTool::~L1NonlinearLutDatabaseOverrideTool()
{
}

// this tool only modifies ChanCalib, the rest is a straight-copy of the input
StatusCode L1NonlinearLutDatabaseOverrideTool::modifyChanCalib(L1CaloPprChanCalibContainer& c)
{
  for(auto& chan : c) {
    // database fields are unsigned shorts, values requrired are floating point
    // store the actual value multiplied by 1000 and undo the transfromation on usage
    chan.setLutJepPar1(static_cast<unsigned short>(1000. * m_offset));
    chan.setLutJepPar2(static_cast<unsigned short>(1000. * m_amplitude));
    chan.setLutJepPar3(static_cast<unsigned short>(1000. * m_exponent));
    chan.setLutJepPar4(static_cast<unsigned short>(1000. * m_noiseFraction));
    chan.setLutJepScale(static_cast<unsigned short>(1000. * m_slope));
    chan.setLutJepStrategy(m_strategy);
  }
  return StatusCode::SUCCESS;
}
StatusCode L1NonlinearLutDatabaseOverrideTool::modifyDisabledTowers(L1CaloDisabledTowersContainer&)
{
  return StatusCode::SUCCESS;
}
StatusCode L1NonlinearLutDatabaseOverrideTool::modifyDeadChannels(L1CaloPpmDeadChannelsContainer&)
{
  return StatusCode::SUCCESS;
}

StatusCode L1NonlinearLutDatabaseOverrideTool::modifyChanDefaults(L1CaloPprChanDefaultsContainer&)
{
  return StatusCode::SUCCESS;
}

} // end of namespace LVL1
