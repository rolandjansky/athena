/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// simple AND
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_FILTERCOMBINATIONAND
#define DERIVATIONFRAMEWORK_FILTERCOMBINATIONAND

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  /** @class FilterCombinationAND
 
      the code used in this implementation is kindly stolen from:
      atlasoff:: ISF/ISF_Core/ISF_Tools

      @author James Catmore -at- cern.ch
     */
  class FilterCombinationAND : public AthAlgTool, public ISkimmingTool {
   
  public: 
    /** Constructor with parameters */
    FilterCombinationAND( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~FilterCombinationAND();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
   
    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const;
   
  private:
    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;
    ToolHandleArray<ISkimmingTool> m_filtersToCombine;
   

  }; 
 
}


#endif
