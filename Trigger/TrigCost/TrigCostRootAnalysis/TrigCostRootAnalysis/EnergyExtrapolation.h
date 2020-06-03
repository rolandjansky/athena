// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_EnergyExtrapolation_H
#define TrigCostRootAnalysis_EnergyExtrapolation_H

// ROOT include(s):
#include <Rtypes.h>

//Local include(s):
#include "Utility.h"


namespace TrigCostRootAnalysis {
  /**
   * @class EnergyExtrapolation
   */
  class EnergyExtrapolation {
  public:
    static EnergyExtrapolation& energyExtrapolation(); //!< Use this method to get the singleton
    Float_t getEventWeight(const TrigCostData* costData);
  private:
    void load8To13();
    void loadMenuV5();
    Float_t eval(Float_t x);

    /**
     * Private constructor.
     */
    EnergyExtrapolation();

    /**
     * Private destructor. Empty
     */
    ~EnergyExtrapolation() {
    };

    /**
     * Private unimplemented copy constructor.
     */
    EnergyExtrapolation(EnergyExtrapolation const&);

    /**
     * Private unimplemented assignment constructor.
     */
    void operator = (EnergyExtrapolation const&);

    Float_t m_param[9]; //<! Hold parameters of polynominal approximation of parton evolution
    Bool_t m_enabled;  //<! If this class is being used
    Bool_t m_13To5;    //<! If downscaling


    StringFloatMap_t m_jettyItems;   //<! Energy of jet/EM
    StringFloatMap_t m_muonItems;    //<! Energy of muons
    StringFloatMap_t m_missingItems; //<! Missing energy of nu
  }; //class EnergyExtrapolation
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_EnergyExtrapolation_H
