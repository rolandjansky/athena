/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackMeasurementThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRACKMEASUREMENTTHINNING_H
#define DERIVATIONFRAMEWORK_TRACKMEASUREMENTTHINNING_H

#include <string>
#include <atomic>

// Gaudi & Athena basics
#include "StoreGate/ThinningHandleKey.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// DerivationFramework includes
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"

#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  /** @class TrackMeasurementThinning
  
      @author David Salek -at- cern.ch
     */
  class TrackMeasurementThinning : public extends<ExpressionParserUser<AthAlgTool>, IThinningTool> {
    
  public: 
    /** Constructor with parameters */
    TrackMeasurementThinning( const std::string& t, const std::string& n, const IInterface* p );
    
    /** Destructor */
    virtual ~TrackMeasurementThinning();
    
    // Athena algtool's Hooks
    virtual StatusCode  initialize() override;
    virtual StatusCode  finalize() override;
    
    /** Check that the current event passes this filter */
    virtual StatusCode doThinning() const override;
 
  private:
    Gaudi::Property<std::string> m_selectionString
      { this, "SelectionString", "", ""};

    mutable std::atomic<unsigned int> m_ntot {};
    mutable std::atomic<unsigned int> m_npass {};
    StringProperty m_streamName
      { this, "StreamName", "", "Name of the stream being thinned" };

    SG::ThinningHandleKey<xAOD::TrackMeasurementValidationContainer> m_SGKey
    { this, "TrackMeasurementValidationKey", "PixelClusters", "" };
  }; 
  
}


#endif
