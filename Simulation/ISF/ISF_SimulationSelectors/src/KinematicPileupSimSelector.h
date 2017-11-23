/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// KinematicPileupSimSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_KINEMATICPILEUPSIMSELECTOR_H
#define ISF_TOOLS_KINEMATICPILEUPSIMSELECTOR_H 1

// ISF includes
#include "ISF_Event/KinematicParticleCuts.h"
#include "BaseSimulationSelector.h"

// Barcode interpretation
#include "BarcodeServices/BitCalculator.h"

namespace ISF {

  /** @class KinematicPileupSimSelector

      Simplistic pileup filter with cuts on energy and pseudorapidity.

      @author Elmar.Ritsch -at- cern.ch
      @author Artem.Basalaev -at- cern.ch
     */
  class KinematicPileupSimSelector : public BaseSimulationSelector, public KinematicParticleCuts {

    public:
     /** Constructor with parameters */
     KinematicPileupSimSelector( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~KinematicPileupSimSelector();


    // Athena algtool's Hooks
    StatusCode  initialize() override final;
    StatusCode  finalize() override final;

    /** check whether given particle passes all cuts -> will be used for routing decision*/
    inline virtual bool passSelectorCuts(const ISFParticle& particle) const override final;

  private:
    mutable Barcode::BitCalculator m_bitcalculator;

    std::vector<int> m_pileupbcid; // vector of BCIDs to select

  };

}

#endif //> !ISF_TOOLS_KINEMATICPILEUPSIMSELECTOR_H
