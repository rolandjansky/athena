/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTDIGCONDFAKEMAP_H
#define TRTDIGCONDFAKEMAP_H

#include "TRTDigCondBase.h"
#include "CLHEP/Random/RandomEngine.h"
#include "GaudiKernel/ServiceHandle.h"
class IAtRndmGenSvc;

/**
 * "Fake" straw map until "real" map is known.
 */
class TRTDigCondFakeMap : public TRTDigCondBase {

public:
  /** Constructor */
  TRTDigCondFakeMap( const TRTDigSettings*,
		     const InDetDD::TRT_DetectorManager*,
		     ServiceHandle <IAtRndmGenSvc> atRndmGenSvc,
		     const TRT_ID* trt_id,
		     int UseGasMix,
		     ServiceHandle<ITRT_StrawStatusSummarySvc> sumSvc
 		   );

protected:

  void setStrawStateInfo(Identifier& TRT_Identifier,
			  const double& strawlength,
			  double& noiselevel,
			  double& relative_noiseamplitude );

private:

  CLHEP::HepRandomEngine* m_pHRengine;

  float m_average_noiselevel; /**< Average noise level     */

};

#endif
