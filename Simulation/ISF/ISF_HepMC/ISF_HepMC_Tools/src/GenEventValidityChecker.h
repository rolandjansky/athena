/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenEventValidityChecker.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_GENEVENTVALIDITYCHECK_H
#define ISF_HEPMC_GENEVENTVALIDITYCHECK_H 1

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_HepMC_Interfaces/IGenEventManipulator.h"

namespace ISF {

  /** @class GenEventValidityChecker
  
      Use the manipulate() method of this AthenaTool, to check a given GenEven for its validity.
      
      @author Elmar.Ritsch -at- cern.ch
     */
  class GenEventValidityChecker : public AthAlgTool, virtual public IGenEventManipulator { 
      
    public: 
      /** Constructor with parameters */
      GenEventValidityChecker( const std::string& t, const std::string& n, const IInterface* p );

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();
	  
      /** checks the the validity of the given GenEvent
          \return StatusCode::SUCCESS in case of a valid GenEvent, StatusCode::FAILURE otherwise */
      StatusCode  manipulate(HepMC::GenEvent& ge);
	  
	private:
      bool        m_checkTime;         //!< check the time dimension value for validity
  }; 
  
}


#endif //> !ISF_HEPMC_GENEVENTVALIDITYCHECK_H
