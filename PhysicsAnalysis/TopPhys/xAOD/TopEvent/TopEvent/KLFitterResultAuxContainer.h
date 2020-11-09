// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ANALYSISTOP_TOPEVENT_KLFITTERRESULTAUXCONTAINER_H
#define ANALYSISTOP_TOPEVENT_KLFITTERRESULTAUXCONTAINER_H

// EDM include(s).
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/CLASS_DEF.h"

// System include(s).
#include <vector>

namespace xAOD {

   /// Auxiliary container for @c xAOD::KLFitterResultContainer
   class KLFitterResultAuxContainer : public AuxContainerBase {

   public:
      /// Default constructor
      KLFitterResultAuxContainer();

   private:
      /// Name of selection run
      std::vector<std::size_t> selectionCode;

      /// Error flags
      std::vector<short> minuitDidNotConverge;
      std::vector<short> fitAbortedDueToNaN;
      std::vector<short> atLeastOneFitParameterAtItsLimit;
      std::vector<short> invalidTransferFunctionAtConvergence;

      /// Global result
      std::vector<unsigned int> bestPermutation;
      std::vector<float> logLikelihood;
      std::vector<float> eventProbability;
      std::vector<std::vector<double> > parameters;
      std::vector<std::vector<double> > parameterErrors;

      /// Model
      std::vector<float> model_bhad_pt;
      std::vector<float> model_bhad_eta;
      std::vector<float> model_bhad_phi;
      std::vector<float> model_bhad_E;
      std::vector<unsigned int>   model_bhad_jetIndex;

      std::vector<float> model_blep_pt;
      std::vector<float> model_blep_eta;
      std::vector<float> model_blep_phi;
      std::vector<float> model_blep_E;
      std::vector<unsigned int>   model_blep_jetIndex;

      std::vector<float> model_lq1_pt;
      std::vector<float> model_lq1_eta;
      std::vector<float> model_lq1_phi;
      std::vector<float> model_lq1_E;
      std::vector<unsigned int>   model_lq1_jetIndex;

      std::vector<float> model_lq2_pt;
      std::vector<float> model_lq2_eta;
      std::vector<float> model_lq2_phi;
      std::vector<float> model_lq2_E;
      std::vector<unsigned int>   model_lq2_jetIndex;

      // model particles in case of TTH topology
      std::vector<float> model_Higgs_b1_pt;
      std::vector<float> model_Higgs_b1_eta;
      std::vector<float> model_Higgs_b1_phi;
      std::vector<float> model_Higgs_b1_E;
      std::vector<unsigned int>   model_Higgs_b1_jetIndex;

      std::vector<float> model_Higgs_b2_pt;
      std::vector<float> model_Higgs_b2_eta;
      std::vector<float> model_Higgs_b2_phi;
      std::vector<float> model_Higgs_b2_E;
      std::vector<unsigned int>   model_Higgs_b2_jetIndex;

      std::vector<float> model_lep_pt;
      std::vector<float> model_lep_eta;
      std::vector<float> model_lep_phi;
      std::vector<float> model_lep_E;
      std::vector<unsigned int>   model_lep_index;

      // model particles for the TTZ trilepton channel
      std::vector<float> model_lepZ1_pt;
      std::vector<float> model_lepZ1_eta;
      std::vector<float> model_lepZ1_phi;
      std::vector<float> model_lepZ1_E;
      std::vector<unsigned int>   model_lepZ1_index;

      std::vector<float> model_lepZ2_pt;
      std::vector<float> model_lepZ2_eta;
      std::vector<float> model_lepZ2_phi;
      std::vector<float> model_lepZ2_E;
      std::vector<unsigned int>   model_lepZ2_index;

      std::vector<float> model_nu_pt;
      std::vector<float> model_nu_eta;
      std::vector<float> model_nu_phi;
      std::vector<float> model_nu_E;

      // model particles for ttbar allhadronic channel
      std::vector<float> model_b_from_top1_pt;
      std::vector<float> model_b_from_top1_eta;
      std::vector<float> model_b_from_top1_phi;
      std::vector<float> model_b_from_top1_E;
      std::vector<unsigned int>   model_b_from_top1_jetIndex;

      std::vector<float> model_b_from_top2_pt;
      std::vector<float> model_b_from_top2_eta;
      std::vector<float> model_b_from_top2_phi;
      std::vector<float> model_b_from_top2_E;
      std::vector<unsigned int>   model_b_from_top2_jetIndex;

      std::vector<float> model_lj1_from_top1_pt;
      std::vector<float> model_lj1_from_top1_eta;
      std::vector<float> model_lj1_from_top1_phi;
      std::vector<float> model_lj1_from_top1_E;
      std::vector<unsigned int>   model_lj1_from_top1_jetIndex;

      std::vector<float> model_lj2_from_top1_pt;
      std::vector<float> model_lj2_from_top1_eta;
      std::vector<float> model_lj2_from_top1_phi;
      std::vector<float> model_lj2_from_top1_E;
      std::vector<unsigned int>   model_lj2_from_top1_jetIndex;

      std::vector<float> model_lj1_from_top2_pt;
      std::vector<float> model_lj1_from_top2_eta;
      std::vector<float> model_lj1_from_top2_phi;
      std::vector<float> model_lj1_from_top2_E;
      std::vector<unsigned int>   model_lj1_from_top2_jetIndex;

      std::vector<float> model_lj2_from_top2_pt;
      std::vector<float> model_lj2_from_top2_eta;
      std::vector<float> model_lj2_from_top2_phi;
      std::vector<float> model_lj2_from_top2_E;
      std::vector<unsigned int>   model_lj2_from_top2_jetIndex;

   }; // class KLFitterResultAuxContainer

} // namespace xAOD

// Define a ClassID for the type.
CLASS_DEF( xAOD::KLFitterResultAuxContainer, 1292529835, 1 )

#endif // ANALYSISTOP_TOPEVENT_KLFITTERRESULTAUXCONTAINER_H
