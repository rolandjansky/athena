/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTDigCondFakeMap.h"
#include "TRTDigSettings.h"
#include "TRTDigiHelper.h"

// For the Athena-based random numbers.
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"

// Units
#include "CLHEP/Units/SystemOfUnits.h"

//helpers for identifiers and hitids:
#include "InDetIdentifier/TRT_ID.h"

#include <cmath>
#include <cstdlib>
#include <utility>

//________________________________________________________________________________
TRTDigCondFakeMap::TRTDigCondFakeMap( const TRTDigSettings* digset,
                                      const InDetDD::TRT_DetectorManager* detmgr,
                                      const TRT_ID* trt_id,
                                      int UseGasMix,
                                      ToolHandle<ITRT_StrawStatusSummaryTool> sumTool
                                      )
: TRTDigCondBase(digset, detmgr, trt_id, UseGasMix, std::move(sumTool))
{
  m_average_noiselevel = m_settings->averageNoiseLevel();
}


//________________________________________________________________________________
void TRTDigCondFakeMap::setStrawStateInfo(Identifier& TRT_Identifier,
                                          const double& strawlength,
                                          double& noiselevel,
                                          double& relative_noiseamplitude,
                                          CLHEP::HepRandomEngine* rndmEngine) {

  noiselevel = m_average_noiselevel; // Not used here, but returned to caller

  // 5% relative fluctuation is hard-coded here
  double                       relnoiseamp = CLHEP::RandGaussZiggurat::shoot(rndmEngine, 1.00, 0.05 );
  while (relnoiseamp < 0.10) { relnoiseamp = CLHEP::RandGaussZiggurat::shoot(rndmEngine, 1.00, 0.05 ); }

  // Anatoli says we need to scale the noise amplitude of Kr,Ar according to LT_(Kr,Ar)/LT_Xe
  MsgStream* amsg = &(msg());
  const int statusHT = m_sumTool->getStatusHT(TRT_Identifier);
  const int strawGasType = TRTDigiHelper::StrawGasType(statusHT,m_UseGasMix, amsg);
  bool isBar = abs(m_id_helper->barrel_ec(TRT_Identifier))==1;
  double averagenoiseampforstrawlength = ( ( (800.)/(100*CLHEP::cm) ) * strawlength + 2100.0 ) / 2500.0;

  if ( isBar ) {
    averagenoiseampforstrawlength *= ( m_settings->lowThresholdBar(strawGasType)/m_settings->lowThresholdBar(0) );
  } else {
    averagenoiseampforstrawlength *= ( m_settings->lowThresholdEC(strawGasType)/m_settings->lowThresholdEC(0) );
  }

  relative_noiseamplitude = relnoiseamp * averagenoiseampforstrawlength;

  return;
}
