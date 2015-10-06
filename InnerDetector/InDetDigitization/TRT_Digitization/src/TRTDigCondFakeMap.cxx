/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTDigCondFakeMap.h"
#include "TRTDigSettings.h"

// For the Athena-based random numbers.
#include "CLHEP/Random/RandFlat.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// Units
#include "CLHEP/Units/SystemOfUnits.h"

//helpers for identifiers and hitids:
#include "InDetIdentifier/TRT_ID.h"

#include <cmath>
#include <cstdlib> //Always include this when including cmath!

//________________________________________________________________________________
TRTDigCondFakeMap::TRTDigCondFakeMap( const TRTDigSettings* digset,
				      const InDetDD::TRT_DetectorManager* detmgr,
				      ServiceHandle <IAtRndmGenSvc> atRndmGenSvc,
				      const TRT_ID* trt_id,
				      int UseGasMix,
				      ServiceHandle<ITRT_StrawStatusSummarySvc> sumSvc // added by Sasha for Argon
				    )
  : TRTDigCondBase(digset, detmgr, trt_id, UseGasMix, sumSvc)
{
  m_average_noiselevel = m_settings->averageNoiseLevel();
  m_pHRengine = atRndmGenSvc->GetEngine("TRT_FakeConditions");
}


//________________________________________________________________________________
void TRTDigCondFakeMap::setStrawStateInfo(Identifier& TRT_Identifier,
                                          const double& strawlength,
					  double& noiselevel,
					  double& relative_noiseamplitude ) {

  noiselevel = m_average_noiselevel; // Not used here, but returned to caller

  const double relfluct(0.05); // Hard coded! FIXME
  double                      relnoiseamp = CLHEP::RandGaussZiggurat::shoot(m_pHRengine, 1.0, relfluct );
  while (relnoiseamp < 0.1) { relnoiseamp = CLHEP::RandGaussZiggurat::shoot(m_pHRengine, 1.0, relfluct ); }

  // Anatoli says we need to scale the noise amplitude of Kr,Ar according to LT_(Kr,Ar)/LT_Xe
  int strawGasType = StrawGasType(TRT_Identifier);
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
