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
		     bool UseArgonStraws,   // added by Sasha for Argon
		     bool useConditionsHTStatus, // added by Sasha for Argon
		     ServiceHandle<ITRT_StrawStatusSummarySvc> sumSvc // added by Sasha for Argon
 		   );

protected:

  /// Sasha: first argument was changed: "const unsigned int& hitid" -> "Identifier& TRT_Identifier"
  void setStrawStateInfo(Identifier& TRT_Identifier,
			  const double& strawlength,
			  bool& isdead,
			  double& noiselevel,
			  double& relative_noiseamplitude );

private:

  CLHEP::HepRandomEngine* m_pHRengine;

  double m_deadlevel;         /**< Fraction of dead straws */
  float m_average_noiselevel; /**< Average noise level     */

};

#endif
