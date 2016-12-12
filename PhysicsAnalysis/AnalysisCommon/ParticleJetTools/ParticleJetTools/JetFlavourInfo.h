/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJet/Jet.h"

namespace xAOD {

  // return a "basic" jet flavour label:
  //    5:             b jet
  //    4:             c jet
  //   15:           tau jet
  //    0: light-flavour jet
  //
  // The first three different methods all use ghost association and correspond to the
  // different labeling schemes:
  // using the weakly decaying hadrons, using the hadrons resulting from fragmentation,
  // or using the partons just before fragmentation

  int GAFinalHadronFlavourLabel (const xAOD::Jet* jet);
  int GAInitialHadronFlavourLabel (const xAOD::Jet* jet);
  int GAFinalPartonFlavourLabel (const xAOD::Jet* jet);

  // The following methods use the "traditional" (Run-1) cone-based labelling and a "safer" successor
  // (safer because it uses hadrons, and becauses it uses an exclusive labelling -- it will associate
  // a HF hadron only with a single jet)

  int ConeFinalPartonFlavourLabel (const xAOD::Jet* jet);
  int ExclusiveConeHadronFlavourLabel (const xAOD::Jet* jet);
  int ExclusiveConeDoubleHadronFlavourLabel (const xAOD::Jet* jet);

  enum JetFlavourLabelType { GAFinalHadron, GAInitialHadron, GAFinalParton, ConeFinalParton, ExclConeHadron, ExclConeDoubleHadron };

  // Provide a unified interface to all methods
  int jetFlavourLabel (const xAOD::Jet* jet, JetFlavourLabelType = ExclConeHadron);
  
}
