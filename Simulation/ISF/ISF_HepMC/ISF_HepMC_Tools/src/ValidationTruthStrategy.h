/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ValidationTruthStrategy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_VALIDATIONTRUTHSTRATEGY_H
#define ISF_TOOLS_VALIDATIONTRUTHSTRATEGY_H 1

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// ISF includes
#include "ISF_HepMC_Interfaces/ITruthStrategy.h"

namespace ISF {
  /** @class ValidationTruthStrategy
  
      This truth strategy is used to record a high number of interaction
      processes. In the end the MC truth output will be used for the
      validation of various fast simulators.
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class ValidationTruthStrategy : public extends<AthAlgTool, ITruthStrategy> {
      
    public: 
     /** Constructor with parameters */
     ValidationTruthStrategy( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~ValidationTruthStrategy();

     // Athena algtool's Hooks
     StatusCode  initialize();
     StatusCode  finalize();

     /** true if the ITruthStrategy implementation applies to the given ITruthIncident */
     bool pass( ITruthIncident& incident) const;

	  private:
     /** cuts on the parent particle */
     double                          m_minParentP2;  //!< minimum parent particle momentum ^ 2

   }; 
  
}


#endif //> !ISF_TOOLS_VALIDATIONTRUTHSTRATEGY_H
