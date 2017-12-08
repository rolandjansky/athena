/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTESTS_SIMTESTTOOLBASE_H
#define G4ATLASTESTS_SIMTESTTOOLBASE_H
/** @file SimTestToolBase.h
 * @author John Chapman - ATLAS Collaboration
 */
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasTests/ISimTestTool.h"

#include "SimTestHisto.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ITHistSvc.h"
// #include "GaudiKernel/ISvcLocator.h"
// #include "GaudiKernel/Bootstrap.h"
// #include "GaudiKernel/ServiceHandle.h"

namespace HepMC {
  class GenParticle;
}

class TProfile;
class TH1;
class TH2;

class SimTestToolBase :  virtual public ISimTestTool, public SimTestHisto, public AthAlgTool {
public:
 /// \name structors and AlgTool implementation
 //@{
  SimTestToolBase(const std::string& type, const std::string& name, const IInterface* parent);
  virtual StatusCode queryInterface(const InterfaceID&, void**);
  virtual StatusCode initialize() {
    return StatusCode::SUCCESS;
  }
  //@}

 protected:
  const HepMC::GenParticle * getPrimary();

  // general prefix (e.g. "/truth/")
  std::string m_path;

  /// The MC truth key
  std::string m_key;
};

#endif //G4ATLASTESTS_SIMTESTTOOLBASE_H
