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

namespace ISF
{

  /** @class PileupSimSelector

      This SimlationSelector implementation will select all particles that are handed to it.
      ( passFilter() always returns true )

      @author Elmar.Ritsch -at- cern.ch
  */
  class PileupSimSelector : public ISimulationSelector
  {

  public:
    /** Constructor with parameters */
    PileupSimSelector( const std::string& t, const std::string& n, const IInterface* p );

    /** Destructor */
    ~PileupSimSelector();

    // Athena algtool's Hooks
    StatusCode  initialize() override final;
    StatusCode  finalize() override final;

    /** check whether given particle passes all cuts -> will be used for routing decision*/
    inline virtual bool passSelectorCuts(const ISFParticle& particle) const override final;

  private:
    mutable Barcode::BitCalculator m_bitcalculator;

    std::vector<int> m_pileupbcid; // vector of BCIDs to select

    mutable int m_npass; // number of particles filter accepts
    mutable int m_nfail; // number of particles filter rejects

  };

}


#endif //> !ISF_TOOLS_PILEUPMUSIMFILTER_H
