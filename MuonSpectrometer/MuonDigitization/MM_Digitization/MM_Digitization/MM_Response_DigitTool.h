/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MM_RESPONSE_DIGITTOOL_H
#define MM_DIGITIZATION_MM_RESPONSE_DIGITTOOL_H

#include "MM_Digitization/IMM_DigitizationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// #include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MM_Digitization/MM_DigitToolOutput.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
/*-----------------------------------------------

Created March 2013 by Nektarios Chr. Benekos

Digitization tool which uses MM_Response to convert MM digitization
input quantities into the output

-----------------------------------------------*/
/*******************************************************************************/

namespace MuonGM{
  class MuonDetectorManager;
}

class MM_Response_DigitTool : public AthAlgTool, virtual public IMM_DigitizationTool {
 public:
  MM_Response_DigitTool( const std::string& type, const std::string& name, const IInterface* parent);

  MM_DigitToolOutput digitize(/* const MmDigitToolInput& input */ );
  StatusCode initialize();
  bool initializeStrip();

 private:

  // const MuonGM::MuonDetectorManager* m_muonGeoMgr;
  // ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

 protected:
  CLHEP::HepRandomEngine *m_rndmEngine;    // Random number engine used - not init in SiDigitization
  std::string m_rndmEngineName;// name of random engine
  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;      // Random number service

};

#endif
