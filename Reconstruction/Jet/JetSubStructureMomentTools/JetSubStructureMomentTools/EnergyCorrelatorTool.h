/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ----------------------------------------------------------------
 * The default behavior of this tool is to use beta = 1.0, but multiple
 * values of beta can be used simultaneously. The property BetaList
 * should be passed a list of floats. Values of < 0 or > 10 may result
 * in problematic output variable names and all values will be rounded
 * to the nearest 0.1. No suffix will be added to the outputs for beta = 1.0
 * and for other values a suffix of _BetaN will be added where N = int(10*beta).
 *
 * The DoDichroic option adds dichroic energy correlator ratios described
 * on page 120 in https://arxiv.org/abs/1803.07977
 * ----------------------------------------------------------------
 */

#ifndef jetsubstructuremomenttools_energycorrelatortool_header
#define jetsubstructuremomenttools_energycorrelatortool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/ECFHelper.h"

class EnergyCorrelatorTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorTool, IJetModifier)

    public:
      
      /// Constructor
      EnergyCorrelatorTool(std::string name);
     
      virtual StatusCode initialize() override;

      int modifyJet(xAOD::Jet &injet) const override;

    private:

      /// ECF moments structure
      struct moments_t;

      /// Configurable as properties
      float m_Beta;
      bool m_doC3;
      bool m_doC4;
      std::vector<float> m_rawBetaVals; /// Vector of input values before cleaning
      bool m_doDichroic;
      
      /// Map of moment calculators and decorators using beta as the key
      std::map< float, moments_t > m_moments;

  };

/**
 * --------------------------------------------------------------------------------
 * Structure to hold all of the necessary moment information for a single set of
 * EnergyCorrelator calculations. This includes the prefix and suffix, beta, and
 * the necessary decorators.
 * --------------------------------------------------------------------------------
 **/

struct EnergyCorrelatorTool::moments_t {

  /// Prefix for decorations
  std::string prefix;

  /// Suffix for decorations
  std::string suffix;

  /// Beta value for calculations
  float beta;

  /// ECF decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECF1;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECF2;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECF3;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECF4;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECF5;

  /// ECF ungroomed decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECF1_ungroomed;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECF2_ungroomed;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECF3_ungroomed;

  moments_t (float Beta, std::string Prefix) {

    prefix = Prefix;
    beta = Beta;

    suffix = GetBetaSuffix(beta);

    dec_ECF1 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECF1"+suffix);
    dec_ECF2 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECF2"+suffix);
    dec_ECF3 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECF3"+suffix);
    dec_ECF4 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECF4"+suffix);
    dec_ECF5 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECF5"+suffix);

    dec_ECF1_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECF1_ungroomed"+suffix);
    dec_ECF2_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECF2_ungroomed"+suffix);
    dec_ECF3_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECF3_ungroomed"+suffix);

  }

};

#endif
