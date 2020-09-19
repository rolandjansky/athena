// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "HepMC/GenEvent.h"

namespace Rivet {

  /// @brief Analysis for testing trivial reweighting
  class SherpaTest : public Analysis {
  public:

    /// @name Constructors etc.
    //@{

    /// Constructor
    SherpaTest()
      : Analysis("SherpaTest")
    {    }

    //@}


  public:

    /// @name Analysis methods
    //@{

    /// Book histograms before the run
    void init() {
      _h_enW  = bookHisto1D("NomEqualsTrivWeightVariationMUR1_MUF1_PDF261000", 2, -0.5, 1.5);
      _h_enW2  = bookHisto1D("LessThan1e-14RelativeDiffNomTrivWeightVariationMUR1_MUF1_PDF261000", 2, -0.5, 1.5);
      _h_enW3  = bookHisto1D("LessThan1e-12RelativeDiffNomTrivWeightVariationMUR1_MUF1_PDF261000", 2, -0.5, 1.5);
    }


    /// Perform the per-event analysis
    void analyze(const Event& event) {
      double trivWeight = event.genEvent()->weights()["MUR1_MUF1_PDF261000"];
      double Weight     = event.weight();//= event.genEvent()->weights()["Weight"];
      _h_enW ->fill((trivWeight==Weight ? 1. : 0.), 1.);
      double presicion  = 1e-14;
      double presicion3 = 1e-12;      
      _h_enW2 ->fill((abs(trivWeight/Weight-1) < presicion ? 1. : 0.), 1.);      
      _h_enW3 ->fill((abs(trivWeight/Weight-1) < presicion3 ? 1. : 0.), 1.);      
      /*if (abs(trivWeight/Weight-1) > 1e-16) {
        std::cout << "Weight:              " << Weight <<std::endl;
        std::cout << "trivWeight:          " << trivWeight <<std::endl;
        std::cout << "trivWeight-Weight:   " << trivWeight-Weight <<std::endl;
        std::cout << "trivWeight/Weight-1: " << trivWeight/Weight-1 <<std::endl;
        std::cout << "abs(trivWeight/Weight-1): " << abs(trivWeight/Weight-1) <<std::endl;
        }*/
    }


    /// Normalise histograms etc., after the run
    void finalize() {
    }

    //@}


  private:

    /// @name Histograms
    Histo1DPtr _h_enW;
    Histo1DPtr _h_enW2;
    Histo1DPtr _h_enW3;
  };


  // The hook for the plugin system
  DECLARE_RIVET_PLUGIN(SherpaTest);

}
