//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "TopEvent/KLFitterResultAuxContainer.h"

// EDM includes(s).
#include "xAODCore/AuxStoreAccessorMacros.h"

namespace xAOD {

   KLFitterResultAuxContainer::KLFitterResultAuxContainer()
   : AuxContainerBase() {

      AUX_VARIABLE(selectionCode);

      AUX_VARIABLE(minuitDidNotConverge);
      AUX_VARIABLE(fitAbortedDueToNaN);
      AUX_VARIABLE(atLeastOneFitParameterAtItsLimit);
      AUX_VARIABLE(invalidTransferFunctionAtConvergence);

      AUX_VARIABLE(bestPermutation);
      AUX_VARIABLE(logLikelihood);
      AUX_VARIABLE(eventProbability);
      AUX_VARIABLE(parameters);
      AUX_VARIABLE(parameterErrors);

      AUX_VARIABLE(model_bhad_pt);
      AUX_VARIABLE(model_bhad_eta);
      AUX_VARIABLE(model_bhad_phi);
      AUX_VARIABLE(model_bhad_E);
      AUX_VARIABLE(model_bhad_jetIndex);

      AUX_VARIABLE(model_blep_pt);
      AUX_VARIABLE(model_blep_eta);
      AUX_VARIABLE(model_blep_phi);
      AUX_VARIABLE(model_blep_E);
      AUX_VARIABLE(model_blep_jetIndex);

      AUX_VARIABLE(model_lq1_pt);
      AUX_VARIABLE(model_lq1_eta);
      AUX_VARIABLE(model_lq1_phi);
      AUX_VARIABLE(model_lq1_E);
      AUX_VARIABLE(model_lq1_jetIndex);

      AUX_VARIABLE(model_lq2_pt);
      AUX_VARIABLE(model_lq2_eta);
      AUX_VARIABLE(model_lq2_phi);
      AUX_VARIABLE(model_lq2_E);
      AUX_VARIABLE(model_lq2_jetIndex);

      AUX_VARIABLE(model_Higgs_b1_pt);
      AUX_VARIABLE(model_Higgs_b1_eta);
      AUX_VARIABLE(model_Higgs_b1_phi);
      AUX_VARIABLE(model_Higgs_b1_E);
      AUX_VARIABLE(model_Higgs_b1_jetIndex);

      AUX_VARIABLE(model_Higgs_b2_pt);
      AUX_VARIABLE(model_Higgs_b2_eta);
      AUX_VARIABLE(model_Higgs_b2_phi);
      AUX_VARIABLE(model_Higgs_b2_E);
      AUX_VARIABLE(model_Higgs_b2_jetIndex);

      AUX_VARIABLE(model_lep_pt);
      AUX_VARIABLE(model_lep_eta);
      AUX_VARIABLE(model_lep_phi);
      AUX_VARIABLE(model_lep_E);
      AUX_VARIABLE(model_lep_index);

      AUX_VARIABLE(model_lepZ1_pt);
      AUX_VARIABLE(model_lepZ1_eta);
      AUX_VARIABLE(model_lepZ1_phi);
      AUX_VARIABLE(model_lepZ1_E);
      AUX_VARIABLE(model_lepZ1_index);

      AUX_VARIABLE(model_lepZ2_pt);
      AUX_VARIABLE(model_lepZ2_eta);
      AUX_VARIABLE(model_lepZ2_phi);
      AUX_VARIABLE(model_lepZ2_E);
      AUX_VARIABLE(model_lepZ2_index);

      AUX_VARIABLE(model_nu_pt);
      AUX_VARIABLE(model_nu_eta);
      AUX_VARIABLE(model_nu_phi);
      AUX_VARIABLE(model_nu_E);

      AUX_VARIABLE(model_b_from_top1_pt);
      AUX_VARIABLE(model_b_from_top1_eta);
      AUX_VARIABLE(model_b_from_top1_phi);
      AUX_VARIABLE(model_b_from_top1_E);
      AUX_VARIABLE(model_b_from_top1_jetIndex);

      AUX_VARIABLE(model_b_from_top2_pt);
      AUX_VARIABLE(model_b_from_top2_eta);
      AUX_VARIABLE(model_b_from_top2_phi);
      AUX_VARIABLE(model_b_from_top2_E);
      AUX_VARIABLE(model_b_from_top2_jetIndex);

      AUX_VARIABLE(model_lj1_from_top1_pt);
      AUX_VARIABLE(model_lj1_from_top1_eta);
      AUX_VARIABLE(model_lj1_from_top1_phi);
      AUX_VARIABLE(model_lj1_from_top1_E);
      AUX_VARIABLE(model_lj1_from_top1_jetIndex);

      AUX_VARIABLE(model_lj2_from_top1_pt);
      AUX_VARIABLE(model_lj2_from_top1_eta);
      AUX_VARIABLE(model_lj2_from_top1_phi);
      AUX_VARIABLE(model_lj2_from_top1_E);
      AUX_VARIABLE(model_lj2_from_top1_jetIndex);

      AUX_VARIABLE(model_lj1_from_top2_pt);
      AUX_VARIABLE(model_lj1_from_top2_eta);
      AUX_VARIABLE(model_lj1_from_top2_phi);
      AUX_VARIABLE(model_lj1_from_top2_E);
      AUX_VARIABLE(model_lj1_from_top2_jetIndex);

      AUX_VARIABLE(model_lj2_from_top2_pt);
      AUX_VARIABLE(model_lj2_from_top2_eta);
      AUX_VARIABLE(model_lj2_from_top2_phi);
      AUX_VARIABLE(model_lj2_from_top2_E);
      AUX_VARIABLE(model_lj2_from_top2_jetIndex);
   }

} // namespace xAOD
