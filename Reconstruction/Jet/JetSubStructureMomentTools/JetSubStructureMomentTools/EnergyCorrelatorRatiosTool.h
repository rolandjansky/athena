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

#ifndef jetsubstructuremomenttools_energycorrelatorratiostool_header
#define jetsubstructuremomenttools_energycorrelatorratiostool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/ECFHelper.h"

class EnergyCorrelatorRatiosTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorRatiosTool, IJetModifier)

    public:

      /// Constructor
      EnergyCorrelatorRatiosTool(std::string name);
     
      virtual StatusCode initialize() override;

      int modifyJet(xAOD::Jet &jet) const override;

    private:

      /// ECF ratio moments structure
      struct moments_t;

      /// Configurable as properties
      bool m_doC3;
      bool m_doC4;
      std::vector<float> m_rawBetaVals; /// Vector of input values before cleaning
      bool m_doDichroic;

      /// Map of moment accessors and decorators using beta as the key
      std::map< float, moments_t > m_moments;

  };

/**
 * --------------------------------------------------------------------------------
 * Structure to hold all of the necessary moment information for a single set of
 * EnergyCorrelator ratio calculations. This includes the prefix and suffix, beta,
 * and the necessary accessors and decorators.
 * --------------------------------------------------------------------------------
 **/

struct EnergyCorrelatorRatiosTool::moments_t {

  /// Prefix for decorations
  std::string prefix;

  /// Suffix for decorations
  std::string suffix;

  /// Beta value for calculations
  float beta;

  /// ECF accessors
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_ECF1;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_ECF2;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_ECF3;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_ECF4;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_ECF5;

  /// ECF ungroomed accessors
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_ECF1_ungroomed;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_ECF2_ungroomed;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_ECF3_ungroomed;

  /// C and D series decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_C1;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_C2;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_C3;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_C4;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_D2;

  /// Dichroic C and D series decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_D2_dichroic;

  moments_t (float Beta, std::string Prefix) {

    prefix = Prefix;
    beta = Beta;

    suffix = GetBetaSuffix(beta);

    acc_ECF1 = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"ECF1"+suffix);
    acc_ECF2 = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"ECF2"+suffix);
    acc_ECF3 = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"ECF3"+suffix);
    acc_ECF4 = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"ECF4"+suffix);
    acc_ECF5 = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"ECF5"+suffix);

    acc_ECF1_ungroomed = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"ECF1_ungroomed"+suffix);
    acc_ECF2_ungroomed = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"ECF2_ungroomed"+suffix);
    acc_ECF3_ungroomed = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"ECF3_ungroomed"+suffix);

    dec_C1 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"C1"+suffix);
    dec_C2 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"C2"+suffix);
    dec_C3 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"C3"+suffix);
    dec_C4 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"C4"+suffix);
    dec_D2 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"D2"+suffix);

    dec_D2_dichroic = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"D2_dichroic"+suffix);

  }

};

#endif
