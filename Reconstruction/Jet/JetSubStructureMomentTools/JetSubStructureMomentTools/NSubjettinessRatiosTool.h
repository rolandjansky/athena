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

#ifndef jetsubstructuremomenttools_nsubjetinessratiostool_header
#define jetsubstructuremomenttools_nsubjetinessratiostool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/NSubjettinessHelper.h"

class NSubjettinessRatiosTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(NSubjettinessRatiosTool, IJetModifier)

    public:

      /// Constructor
      NSubjettinessRatiosTool(std::string name);

      StatusCode initialize();

      int modifyJet(xAOD::Jet &jet) const;

    private:

      /// N-subjettiness ratio moments structure
      struct moments_t;

      /// Configurable as properties
      std::vector<float> m_rawAlphaVals; /// Vector of input values before cleaning
      bool m_doDichroic;
  
      /// Map of moment accessors and decorators using alpha as the key
      std::map< float, moments_t > m_moments;

  };

/**
 * --------------------------------------------------------------------------------
 * Structure to hold all of the necessary moment information for a single set of
 * NSubjettiness ratio calculations. This includes the prefix and suffix, alpha, 
 * and the necessary accessors and decorators.
 * --------------------------------------------------------------------------------
 **/

struct NSubjettinessRatiosTool::moments_t {

  /// Prefix for decorations
  std::string prefix;

  /// Suffix for decorations
  std::string suffix;

  /// Beta value for calculations
  float alpha;

  /// NSubjettiness accessors
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau1;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau2;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau3;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau4;

  /// NSubjettiness ungroomed accessors
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau2_ungroomed;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau3_ungroomed;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau4_ungroomed;

  /// WTA NSubjettiness accessors
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau1_wta;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau2_wta;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau3_wta;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau4_wta;

  /// WTA NSubjettiness ungroomed accessors
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau2_wta_ungroomed;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau3_wta_ungroomed;
  std::unique_ptr< SG::AuxElement::ConstAccessor<float> > acc_Tau4_wta_ungroomed;

  /// NSubjettiness Ratios decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau21;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau32;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau42;

  /// Dichroic NSubjettiness Ratios decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau21_dichroic;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau32_dichroic;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau42_dichroic;

  /// WTA NSubjettiness Ratios decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau21_wta;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau32_wta;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau42_wta;

  /// WTA Dichroic NSubjettiness Ratios decorators
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau21_wta_dichroic;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau32_wta_dichroic;
  std::unique_ptr< SG::AuxElement::Decorator<float> > dec_Tau42_wta_dichroic;

  moments_t (float Alpha, std::string Prefix) {

    prefix = Prefix;
    alpha = Alpha;

    suffix = GetAlphaSuffix(alpha);

    acc_Tau1 = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau1"+suffix);
    acc_Tau2 = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau2"+suffix);
    acc_Tau3 = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau3"+suffix);
    acc_Tau4 = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau4"+suffix);

    acc_Tau2_ungroomed = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau2_ungroomed"+suffix);
    acc_Tau3_ungroomed = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau3_ungroomed"+suffix);
    acc_Tau4_ungroomed = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau4_ungroomed"+suffix);

    acc_Tau1_wta = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau1_wta"+suffix);
    acc_Tau2_wta = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau2_wta"+suffix);
    acc_Tau3_wta = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau3_wta"+suffix);
    acc_Tau4_wta = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau4_wta"+suffix);

    acc_Tau2_wta_ungroomed = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau2_wta_ungroomed"+suffix);
    acc_Tau3_wta_ungroomed = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau3_wta_ungroomed"+suffix);
    acc_Tau4_wta_ungroomed = std::make_unique< SG::AuxElement::ConstAccessor<float> >(prefix+"Tau4_wta_ungroomed"+suffix);

    dec_Tau21 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau21"+suffix);
    dec_Tau32 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau32"+suffix);
    dec_Tau42 = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau42"+suffix);

    dec_Tau21_dichroic = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau21_dichroic"+suffix);
    dec_Tau32_dichroic = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau32_dichroic"+suffix);
    dec_Tau42_dichroic = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau42_dichroic"+suffix);

    dec_Tau21_wta = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau21_wta"+suffix);
    dec_Tau32_wta = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau32_wta"+suffix);
    dec_Tau42_wta = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau42_wta"+suffix);

    dec_Tau21_wta_dichroic = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau21_wta_dichroic"+suffix);
    dec_Tau32_wta_dichroic = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau32_wta_dichroic"+suffix);
    dec_Tau42_wta_dichroic = std::make_unique< SG::AuxElement::Decorator<float> >(prefix+"Tau42_wta_dichroic"+suffix);

  }

};

#endif
