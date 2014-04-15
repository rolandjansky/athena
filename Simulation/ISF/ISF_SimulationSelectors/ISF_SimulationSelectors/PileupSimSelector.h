/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PileupSimSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_PILEUPMUSIMFILTER_H
#define ISF_TOOLS_PILEUPMUSIMFILTER_H 1

// ISF includes
#include "ISF_Interfaces/ISimulationSelector.h"

// Barcode interpretation
#include "BarcodeServices/BitCalculator.h"

namespace ISF {

  /** @class PileupSimSelector
  
      This SimlationSelector implementation will select all particles that are handed to it.
      ( passFilter() always returns true )
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class PileupSimSelector : public ISimulationSelector {
      
    public: 
     /** Constructor with parameters */
     PileupSimSelector( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~PileupSimSelector();

     // Athena algtool's Hooks
     StatusCode  initialize();
     StatusCode  finalize();

     /** check whether given particle passes all cuts -> will be used for routing decision*/
     inline virtual bool passSelectorCuts(const ISFParticle& particle) const;

    private:
     mutable Barcode::BitCalculator m_bitcalculator;
  }; 
  
}


#endif //> !ISF_TOOLS_PILEUPMUSIMFILTER_H
