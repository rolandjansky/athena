/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BEAMEFFECTS_GENEVENTVALIDITYCHECK_H
#define BEAMEFFECTS_GENEVENTVALIDITYCHECK_H 1

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "HepMC_Interfaces/IGenEventManipulator.h"

namespace Simulation {

  /** @class GenEventValidityChecker

      Use the manipulate() method of this AthenaTool, to check a given GenEven for its validity.

      @author Elmar.Ritsch -at- cern.ch
     */
  class GenEventValidityChecker : public extends<AthAlgTool, IGenEventManipulator> {

    public:
      /** Constructor with parameters */
      GenEventValidityChecker( const std::string& t, const std::string& n, const IInterface* p );

      /** Athena algtool's Hooks */
      StatusCode  initialize() override final;
      StatusCode  finalize() override final;

      /** checks the the validity of the given GenEvent
          \return StatusCode::SUCCESS in case of a valid GenEvent, StatusCode::FAILURE otherwise */
      StatusCode  manipulate(HepMC::GenEvent& ge) const override final;

    private:
    Gaudi::Property<bool>        m_checkTime{this, "CheckTime", true};         //!< check the time dimension value for validity
  };

}

#endif //> !BEAMEFFECTS_GENEVENTVALIDITYCHECK_H
