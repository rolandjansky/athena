/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  MdtDigiToolOutput digitize(const MdtDigiToolInput& input);

  StatusCode initialize();

  bool initializeTube();

  // accessors
    ServiceHandle<IAtRndmGenSvc> getRndmSvc() const { return m_rndmSvc; }    // Random number service
    CLHEP::HepRandomEngine  *getRndmEngine() const { return m_rndmEngine; } // Random number engine used 

 private:
  MDT_Response m_tube;

  double m_clusterDensity;
  double m_threshold;
  double m_attenuationLength;

  const MuonGM::MuonDetectorManager* m_muonGeoMgr;
  const MdtIdHelper*         m_idHelper;
  bool m_DoQballGamma;
//    CLHEP::HepRandomEngine* p_engine;

 protected:
    CLHEP::HepRandomEngine *m_rndmEngine;    // Random number engine used - not init in SiDigitization
    std::string m_rndmEngineName;// name of random engine
    ServiceHandle <IAtRndmGenSvc> m_rndmSvc;      // Random number service

};

#endif
