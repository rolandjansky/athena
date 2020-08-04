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

#ifndef jetsubstructuremomenttools_energycorrelatorgeneralizedtool_header
#define jetsubstructuremomenttools_energycorrelatorgeneralizedtool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/ECFHelper.h"

class EnergyCorrelatorGeneralizedTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorGeneralizedTool, IJetModifier)

    public:

      /// Constructor
      EnergyCorrelatorGeneralizedTool(std::string name);

      virtual StatusCode initialize() override;

      int modifyJet(xAOD::Jet &injet) const override;

    private:

      /// ECFG moments structure
      struct moments_t;

      /// Configurable as properties
      float m_Beta;
      bool m_doM3;
      bool m_doN3;
      bool m_doLSeries;
      std::vector<float> m_rawBetaVals; /// Vector of input values before cleaning
      bool m_doDichroic;

      /// Map of moment decorators using beta as the key
      std::map< float, moments_t > m_moments;

      /// Decorators for L-series ECFs
      std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_ECFG_2_1_2;
      std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_ECFG_3_1_1;
      std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_ECFG_3_2_1;
      std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_ECFG_3_2_2;
      std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_ECFG_3_3_1;
      std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_ECFG_4_2_2;
      std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_ECFG_4_4_1;

  };

/**
 * --------------------------------------------------------------------------------
 * Structure to hold all of the necessary moment information for a single set of 
 * EnergyCorrelatorGeneralized calculations. This includes the prefix and suffix, 
 * beta, and the necessary decorators.
 * --------------------------------------------------------------------------------
 **/

struct EnergyCorrelatorGeneralizedTool::moments_t {

  /// Prefix for decorations
  std::string prefix;

  /// Suffix for decorations
  std::string suffix;

  /// Beta value for calculations
  float beta;

  /// ECFG decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECFG_2_1;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECFG_3_1;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECFG_3_2;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECFG_4_1;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECFG_4_2;
  
  /// ECFG ungroomed decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECFG_2_1_ungroomed;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECFG_3_1_ungroomed;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_ECFG_3_2_ungroomed;
  
  moments_t (float Beta, std::string Prefix) {

    prefix = Prefix;
    beta = Beta;

    suffix = GetBetaSuffix(beta);

    dec_ECFG_2_1 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECFG_2_1"+suffix);
    dec_ECFG_3_1 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECFG_3_1"+suffix);
    dec_ECFG_3_2 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECFG_3_2"+suffix);
    dec_ECFG_4_1 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECFG_4_1"+suffix);
    dec_ECFG_4_2 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECFG_4_2"+suffix);
    
    dec_ECFG_2_1_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECFG_2_1_ungroomed"+suffix);
    dec_ECFG_3_1_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECFG_3_1_ungroomed"+suffix);
    dec_ECFG_3_2_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"ECFG_3_2_ungroomed"+suffix);

  }

};

#endif
