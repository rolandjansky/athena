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
				      bool UseArgonStraws,   // added by Sasha for Argon
				      bool useConditionsHTStatus, // added by Sasha for Argon
				      ServiceHandle<ITRT_StrawStatusSummarySvc> sumSvc // added by Sasha for Argon
				    )
  : TRTDigCondBase(digset, detmgr, trt_id, UseArgonStraws, useConditionsHTStatus, sumSvc)
{
  m_deadlevel = m_settings->deadStrawFraction();
  m_average_noiselevel = m_settings->averageNoiseLevel();
  m_pHRengine = atRndmGenSvc->GetEngine("TRT_FakeConditions");
}


/// Sasha: first argument was changed: "const unsigned int& hitid" -> "Identifier& TRT_Identifier"
void TRTDigCondFakeMap::setStrawStateInfo(Identifier& TRT_Identifier,
                                          const double& strawlength,
					  bool& isdead, double& noiselevel,
					  double& relative_noiseamplitude ) {

  noiselevel = m_average_noiselevel; // Why is this an argument of setStrawStateInfo()?
                                     // It is not used below and is simply a copy of m_settings->averageNoiseLevel().

  if ( m_deadlevel > 0.0 ) { // 0.8% CPU saving :)
    if ( CLHEP::RandFlat::shoot(m_pHRengine, 0.0, 1.0) < m_deadlevel ) {
      isdead = true;
      return;
    }
  }
  isdead = false;

  const double relfluct(0.05); // Hard coded! FIXME
  double relnoiseamp = CLHEP::RandGaussZiggurat::shoot(m_pHRengine, 1.0, relfluct );
  while (relnoiseamp < 0.1) {
    relnoiseamp = CLHEP::RandGaussZiggurat::shoot(m_pHRengine, 1.0, relfluct );
  }

  double averagenoiseampforstrawlength = ( ( (800.)/(100*CLHEP::cm) ) * strawlength + 2100.0 ) / 2500.0;

  /// Sasha: according to Anatoli, we need to scale the noise amplitude for Argon straws by factor LT_argon/LT_xenon
  if ( IsArgonStraw(TRT_Identifier) ) { // Argon straw
    int barrel_ec = m_id_helper->barrel_ec(TRT_Identifier);
    if ( abs(barrel_ec) == 1 ) {
      averagenoiseampforstrawlength *= ( m_settings->lowThresholdBar(true) / m_settings->lowThresholdBar(false) );
    } else {
      averagenoiseampforstrawlength *= ( m_settings->lowThresholdEC(true) / m_settings->lowThresholdEC(false) );
    }
  }

  relative_noiseamplitude = relnoiseamp * averagenoiseampforstrawlength;

  return;
}
