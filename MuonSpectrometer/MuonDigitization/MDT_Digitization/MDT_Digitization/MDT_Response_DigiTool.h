/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DIGITIZATION_MDT_RESPONSE_DIGITOOL_H
#define MDT_DIGITIZATION_MDT_RESPONSE_DIGITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MDT_Digitization/MdtDigiToolOutput.h"
#include "MDT_Digitization/IMDT_DigitizationTool.h"
#include "MDT_Response/MDT_Response.h"

#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
/*-----------------------------------------------

   Created 7-5-2004 by Niels van Eldik

 Digitization tool which uses MDT_Response to convert MDT digitization
 input quantities into the output

-----------------------------------------------*/

namespace MuonGM{
  class MuonDetectorManager;
}
class MdtIdHelper;
class IAtRndmGenSvc;

class MDT_Response_DigiTool : public AthAlgTool,
			      virtual public IMDT_DigitizationTool {
 public:
  MDT_Response_DigiTool( const std::string& type,
			 const std::string& name,
			 const IInterface* parent );

  MdtDigiToolOutput digitize(const MdtDigiToolInput& input, CLHEP::HepRandomEngine *rndmEngine);

  StatusCode initialize();

  bool initializeTube();

 private:
  MDT_Response m_tube;

  double m_clusterDensity;
  double m_threshold;
  double m_attenuationLength;

  const MuonGM::MuonDetectorManager* m_muonGeoMgr;
  const MdtIdHelper*         m_idHelper;
  bool m_DoQballGamma;
};

#endif
