/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATECAVERNBKG_H
#define GENERATECAVERNBKG_H

// This code is used to generate cavern background.

#include "GeneratorModules/GenModule.h"
#include "GaudiKernel/ServiceHandle.h"
#include "HepPDT/ParticleDataTable.hh"

#include "CavernBkgGenerator/BkgOut.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <string>

// Forward declarations
class IPartPropSvc;
class McEventCollection;
namespace CLHEP {
  class HepRandomEngine;
}


class GenerateCavernBkg : public GenModule {
public:

  /// Constructor
  GenerateCavernBkg(const std::string& name, ISvcLocator* pSvcLocator);

  /// @name Loop methods
  //@{
  StatusCode genInitialize();
  StatusCode callGenerator();
  StatusCode fillEvt(HepMC::GenEvent* evt);
  StatusCode genFinalize();
  //@}

  /// Static pointer to random number generator for use by
  static CLHEP::HepRandomEngine* CAVBKG_RANDOM_ENGINE;


private:

  /// @name Settable properties
  //@{
  std::string            m_infile;
  //@}

  std::vector<long int>  m_seeds;

  // Accessors to the event
  BkgOut m_bkgout;
  //@}

};


#endif
