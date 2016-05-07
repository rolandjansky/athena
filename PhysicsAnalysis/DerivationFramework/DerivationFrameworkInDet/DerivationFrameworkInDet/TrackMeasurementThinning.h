/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackMeasurementThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRACKMEASUREMENTTHINNING_H
#define DERIVATIONFRAMEWORK_TRACKMEASUREMENTTHINNING_H

#include<string>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  /** @class TrackMeasurementThinning
  
      @author David Salek -at- cern.ch
     */
  class TrackMeasurementThinning : public AthAlgTool, public IThinningTool {
    
  public: 
    /** Constructor with parameters */
    TrackMeasurementThinning( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    ~TrackMeasurementThinning();
    
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
    
    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const;
 
  private:
    ServiceHandle<IThinningSvc> m_thinningSvc;

    //Expression for object thinning selection
    std::string m_expression;
    ExpressionParsing::ExpressionParser *m_parser;
    std::string m_selectionString;

    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;
    std::string m_SGKey;
    bool m_and;
  }; 
  
}


#endif
