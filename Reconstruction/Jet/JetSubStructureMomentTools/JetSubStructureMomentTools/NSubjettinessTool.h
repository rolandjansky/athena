/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ----------------------------------------------------------------
 * The default behavior of this tool is to use alpha = 1.0, but multiple
 * values of alpha can be used simultaneously. The property AlphaList
 * should be passed a list of floats. Values of < 0 or > 10 may result
 * in problematic output variable names and all values will be rounded
 * to the nearest 0.1. No suffix will be added to the outputs for alpha = 1.0
 * and for other values a suffix of _AlphaN will be added where N = int(10*alpha).
 *
 * The DoDichroic option adds dichroic N-subjettiness ratios described in
 * https://arxiv.org/abs/1612.03917
 * ----------------------------------------------------------------
 */

#ifndef jetsubstructuremomenttools_nsubjetinesstool_header
#define jetsubstructuremomenttools_nsubjetinesstool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/NSubjettinessHelper.h"

class NSubjettinessTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(NSubjettinessTool, IJetModifier)

    public:
      // Constructor and destructor
      NSubjettinessTool(std::string name);

      StatusCode initialize();

      int modifyJet(xAOD::Jet &injet) const;

    private:

      /// N-subjettiness moments structure
      struct moments_t;

      /// Configurable as properties
      float m_Alpha;
      std::vector<float> m_rawAlphaVals; /// Vector of input values before cleaning
      bool m_doDichroic;

      /// Map of decorators using alpha as the key
      std::map< float, moments_t > m_moments;

  };

/**
 * --------------------------------------------------------------------------------
 * Structure to hold all of the necessary moment information for a single set of
 * NSubjettiness calculations. This includes the prefix and suffix, alpha, and the
 * necessary decorators.
 * --------------------------------------------------------------------------------
 **/

struct NSubjettinessTool::moments_t {

  /// Prefix for decorations
  std::string prefix;

  /// Alpha value for calculations
  float alpha;

  /// NSubjettiness decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau1;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau2;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau3;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau4;

  /// NSubjettiness ungroomed decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau2_ungroomed;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau3_ungroomed;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau4_ungroomed;

  /// WTA NSubjettiness decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau1_wta;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau2_wta;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau3_wta;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau4_wta;

  /// WTA NSubjettiness ungroomed decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau2_wta_ungroomed;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau3_wta_ungroomed;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau4_wta_ungroomed;

  moments_t (float Alpha, std::string Prefix) {

    prefix = Prefix;
    alpha = Alpha;

    std::string suffix = GetAlphaSuffix(alpha);

    dec_Tau1 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau1"+suffix);
    dec_Tau2 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau2"+suffix);
    dec_Tau3 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau3"+suffix);
    dec_Tau4 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau4"+suffix);

    dec_Tau2_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau2_ungroomed"+suffix);
    dec_Tau3_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau3_ungroomed"+suffix);
    dec_Tau4_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau4_ungroomed"+suffix);

    dec_Tau1_wta = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau1_wta"+suffix);
    dec_Tau2_wta = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau2_wta"+suffix);
    dec_Tau3_wta = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau3_wta"+suffix);
    dec_Tau4_wta = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau4_wta"+suffix);

    dec_Tau2_wta_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau2_wta_ungroomed"+suffix);
    dec_Tau3_wta_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau3_wta_ungroomed"+suffix);
    dec_Tau4_wta_ungroomed = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau4_wta_ungroomed"+suffix);

  }

};

#endif
