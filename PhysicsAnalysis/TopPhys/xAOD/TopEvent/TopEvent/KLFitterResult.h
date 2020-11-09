/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
#ifndef ANALYSISTOP_TOPEVENT_KLFITTERRESULT_H
#define ANALYSISTOP_TOPEVENT_KLFITTERRESULT_H

// EDM include(s).
#include "AthContainers/AuxElement.h"
#include "xAODCore/CLASS_DEF.h"

// System include(s).
#include <vector>

namespace xAOD {

  /**
   * @author John Morris <john.morris@cern.ch>
   *
   * @brief KLFitterResult
   *   A simple xAOD class which we can persist into a mini-xAOD
   *   The xAOD EDM is way too complex, so let's simplify it
   *   It's not like ROOT can do schema evolution......
   *
   *   This class contains the result of the KLFitter algorithm
   **/
  class KLFitterResult: public SG::AuxElement {
  public:
    /// Default constructor
    KLFitterResult() = default;

    /// get selectionName
    std::size_t selectionCode() const;
    /// set selectionName
    void setSelectionCode(std::size_t);

    /// get minuitDidNotConverge
    short minuitDidNotConverge() const;
    /// set minuitDidNotConverge
    void setMinuitDidNotConverge(short);

    /// get fitAbortedDueToNaN
    short fitAbortedDueToNaN() const;
    /// set fitAbortedDueToNaN
    void setFitAbortedDueToNaN(short);

    /// get atLeastOneFitParameterAtItsLimit
    short atLeastOneFitParameterAtItsLimit() const;
    /// set atLeastOneFitParameterAtItsLimit
    void setAtLeastOneFitParameterAtItsLimit(short);

    /// get invalidTransferFunctionAtConvergence
    short invalidTransferFunctionAtConvergence() const;
    /// set invalidTransferFunctionAtConvergence
    void setInvalidTransferFunctionAtConvergence(short);

    /// get bestPermutation
    unsigned int bestPermutation() const;
    /// set bestPermutation
    void setBestPermutation(unsigned int);

    /// get logLikelihood
    float logLikelihood() const;
    /// set logLikelihood
    void setLogLikelihood(float);

    /// get eventProbability
    float eventProbability() const;
    /// set eventProbability
    void setEventProbability(float);

    /// get parameters
    const std::vector<double>& parameters() const;
    /// set parameters
    void setParameters(const std::vector<double>&);

    /// get parameterErrors
    const std::vector<double>& parameterErrors() const;
    /// set parameterErrors
    void setParameterErrors(const std::vector<double>&);


    /// get model_bhad_pt
    float model_bhad_pt() const;
    /// set model_bhad_pt
    void setModel_bhad_pt(float);

    /// get model_bhad_eta
    float model_bhad_eta() const;
    /// set model_bhad_eta
    void setModel_bhad_eta(float);

    /// get model_bhad_phi
    float model_bhad_phi() const;
    /// set model_bhad_phi
    void setModel_bhad_phi(float);

    /// get model_bhad_E
    float model_bhad_E() const;
    /// set model_bhad_E
    void setModel_bhad_E(float);

    /// get model_bhad_jetIndex
    unsigned int model_bhad_jetIndex() const;
    /// set model_bhad_jetIndex
    void setModel_bhad_jetIndex(unsigned int);


    /// get model_blep_pt
    float model_blep_pt() const;
    /// set model_blep_pt
    void setModel_blep_pt(float);

    /// get model_blep_eta
    float model_blep_eta() const;
    /// set model_blep_eta
    void setModel_blep_eta(float);

    /// get model_blep_phi
    float model_blep_phi() const;
    /// set model_blep_phi
    void setModel_blep_phi(float);

    /// get model_blep_E
    float model_blep_E() const;
    /// set model_blep_E
    void setModel_blep_E(float);

    /// get model_blep_jetIndex
    unsigned int model_blep_jetIndex() const;
    /// set model_blep_jetIndex
    void setModel_blep_jetIndex(unsigned int);


    /// get model_lq1_pt
    float model_lq1_pt() const;
    /// set model_lq1_pt
    void setModel_lq1_pt(float);

    /// get model_lq1_eta
    float model_lq1_eta() const;
    /// set model_lq1_eta
    void setModel_lq1_eta(float);

    /// get model_lq1_phi
    float model_lq1_phi() const;
    /// set model_lq1_phi
    void setModel_lq1_phi(float);

    /// get model_lq1_E
    float model_lq1_E() const;
    /// set model_lq1_E
    void setModel_lq1_E(float);

    /// get model_lq1_jetIndex
    unsigned int model_lq1_jetIndex() const;
    /// set model_lq1_jetIndex
    void setModel_lq1_jetIndex(unsigned int);


    /// get model_lq2_pt
    float model_lq2_pt() const;
    /// set model_lq2_pt
    void setModel_lq2_pt(float);

    /// get model_lq2_eta
    float model_lq2_eta() const;
    /// set model_lq2_eta
    void setModel_lq2_eta(float);

    /// get model_lq2_phi
    float model_lq2_phi() const;
    /// set model_lq2_phi
    void setModel_lq2_phi(float);

    /// get model_lq2_E
    float model_lq2_E() const;
    /// set model_lq2_E
    void setModel_lq2_E(float);

    /// get model_lq2_jetIndex
    unsigned int model_lq2_jetIndex() const;
    /// set model_lq2_jetIndex
    void setModel_lq2_jetIndex(unsigned int);

    /// get model_Higgs_b1_pt
    float model_Higgs_b1_pt() const;
    /// set model_Higgs_b1_pt
    void setModel_Higgs_b1_pt(float);

    /// get model_Higgs_b1_eta
    float model_Higgs_b1_eta() const;
    /// set model_Higgs_b1_eta
    void setModel_Higgs_b1_eta(float);

    /// get model_Higgs_b1_phi
    float model_Higgs_b1_phi() const;
    /// set model_Higgs_b1_phi
    void setModel_Higgs_b1_phi(float);

    /// get model_Higgs_b1_E
    float model_Higgs_b1_E() const;
    /// set model_Higgs_b1_E
    void setModel_Higgs_b1_E(float);

    /// get model_Higgs_b1_jetIndex
    unsigned int model_Higgs_b1_jetIndex() const;
    /// set model_Higgs_b1_jetIndex
    void setModel_Higgs_b1_jetIndex(unsigned int);


    /// get model_Higgs_b2_pt
    float model_Higgs_b2_pt() const;
    /// set model_Higgs_b2_pt
    void setModel_Higgs_b2_pt(float);

    /// get model_Higgs_b2_eta
    float model_Higgs_b2_eta() const;
    /// set model_Higgs_b2_eta
    void setModel_Higgs_b2_eta(float);

    /// get model_Higgs_b2_phi
    float model_Higgs_b2_phi() const;
    /// set model_Higgs_b2_phi
    void setModel_Higgs_b2_phi(float);

    /// get model_Higgs_b2_E
    float model_Higgs_b2_E() const;
    /// set model_Higgs_b2_E
    void setModel_Higgs_b2_E(float);

    /// get model_Higgs_b2_jetIndex
    unsigned int model_Higgs_b2_jetIndex() const;
    /// set model_Higgs_b2_jetIndex
    void setModel_Higgs_b2_jetIndex(unsigned int);

    /// get model_lep_pt
    float model_lep_pt() const;
    /// set model_lep_pt
    void setModel_lep_pt(float);

    /// get model_lep_eta
    float model_lep_eta() const;
    /// set model_lep_eta
    void setModel_lep_eta(float);

    /// get model_lep_phi
    float model_lep_phi() const;
    /// set model_lep_phi
    void setModel_lep_phi(float);

    /// get model_lep_E
    float model_lep_E() const;
    /// set model_lep_E
    void setModel_lep_E(float);

    /// get model_lep_index
    unsigned int model_lep_index() const;
    /// set model_lep_index
    void setModel_lep_index(unsigned int);

    /// get model_lep_pt
    float model_lepZ1_pt() const;
    /// set model_lepZ1_pt
    void setModel_lepZ1_pt(float);

    /// get model_lepZ1_eta
    float model_lepZ1_eta() const;
    /// set model_lepZ1_eta
    void setModel_lepZ1_eta(float);

    /// get model_lepZ1_phi
    float model_lepZ1_phi() const;
    /// set model_lepZ1_phi
    void setModel_lepZ1_phi(float);

    /// get model_lepZ1_E
    float model_lepZ1_E() const;
    /// set model_lepZ1_E
    void setModel_lepZ1_E(float);

    /// get model_lepZ1_index
    unsigned int model_lepZ1_index() const;
    /// set model_lepZ1_index
    void setModel_lepZ1_index(unsigned int);

    /// get model_lepZ2_pt
    float model_lepZ2_pt() const;
    /// set model_lepZ2_pt
    void setModel_lepZ2_pt(float);

    /// get model_lepZ2_eta
    float model_lepZ2_eta() const;
    /// set model_lepZ2_eta
    void setModel_lepZ2_eta(float);

    /// get model_lepZ2_phi
    float model_lepZ2_phi() const;
    /// set model_lepZ2_phi
    void setModel_lepZ2_phi(float);

    /// get model_lepZ2_E
    float model_lepZ2_E() const;
    /// set model_lepZ2_E
    void setModel_lepZ2_E(float);

    /// get model_lepZ2_index
    unsigned int model_lepZ2_index() const;
    /// set model_lepZ2_index
    void setModel_lepZ2_index(unsigned int);

    /// get model_nu_pt
    float model_nu_pt() const;
    /// set model_nu_pt
    void setModel_nu_pt(float);

    /// get model_nu_eta
    float model_nu_eta() const;
    /// set model_nu_eta
    void setModel_nu_eta(float);

    /// get model_nu_phi
    float model_nu_phi() const;
    /// set model_nu_phi
    void setModel_nu_phi(float);

    /// get model_nu_E
    float model_nu_E() const;
    /// set model_nu_E
    void setModel_nu_E(float);

    /// get model_b_from_top1_pt
    float model_b_from_top1_pt() const;
    /// set model_b_from_top1_pt
    void setModel_b_from_top1_pt(float);

    /// get model_b_from_top1_eta
    float model_b_from_top1_eta() const;
    /// set model_b_from_top1_eta
    void setModel_b_from_top1_eta(float);

    /// get model_b_from_top1_phi
    float model_b_from_top1_phi() const;
    /// set model_b_from_top1_phi
    void setModel_b_from_top1_phi(float);

    /// get model_b_from_top1_E
    float model_b_from_top1_E() const;
    /// set model_b_from_top1_E
    void setModel_b_from_top1_E(float);

    /// get model_b_from_top1_jetIndex
    unsigned int model_b_from_top1_jetIndex() const;
    /// set model_b_from_top1_jetIndex
    void setModel_b_from_top1_jetIndex(unsigned int);

    /// get model_b_from_top2_pt
    float model_b_from_top2_pt() const;
    /// set model_b_from_top2_pt
    void setModel_b_from_top2_pt(float);

    /// get model_b_from_top2_eta
    float model_b_from_top2_eta() const;
    /// set model_b_from_top2_eta
    void setModel_b_from_top2_eta(float);

    /// get model_b_from_top2_phi
    float model_b_from_top2_phi() const;
    /// set model_b_from_top2_phi
    void setModel_b_from_top2_phi(float);

    /// get model_b_from_top2_E
    float model_b_from_top2_E() const;
    /// set model_b_from_top2_E
    void setModel_b_from_top2_E(float);

    /// get model_b_from_top2_jetIndex
    unsigned int model_b_from_top2_jetIndex() const;
    /// set model_b_from_top2_jetIndex
    void setModel_b_from_top2_jetIndex(unsigned int);

    /// get model_lj1_from_top1_pt
    float model_lj1_from_top1_pt() const;
    /// set model_lj1_from_top1_pt
    void setModel_lj1_from_top1_pt(float);

    /// get model_lj1_from_top1_eta
    float model_lj1_from_top1_eta() const;
    /// set model_lj1_from_top1_eta
    void setModel_lj1_from_top1_eta(float);

    /// get model_lj1_from_top1_phi
    float model_lj1_from_top1_phi() const;
    /// set model_lj1_from_top1_phi
    void setModel_lj1_from_top1_phi(float);

    /// get model_lj1_from_top1_E
    float model_lj1_from_top1_E() const;
    /// set model_lj1_from_top1_E
    void setModel_lj1_from_top1_E(float);

    /// get model_lj1_from_top1_jetIndex
    unsigned int model_lj1_from_top1_jetIndex() const;
    /// set model_lj1_from_top1_jetIndex
    void setModel_lj1_from_top1_jetIndex(unsigned int);

    /// get model_lj2_from_top1_pt
    float model_lj2_from_top1_pt() const;
    /// set model_lj2_from_top1_pt
    void setModel_lj2_from_top1_pt(float);

    /// get model_lj2_from_top1_eta
    float model_lj2_from_top1_eta() const;
    /// set model_lj2_from_top1_eta
    void setModel_lj2_from_top1_eta(float);

    /// get model_lj2_from_top1_phi
    float model_lj2_from_top1_phi() const;
    /// set model_lj2_from_top1_phi
    void setModel_lj2_from_top1_phi(float);

    /// get model_lj2_from_top1_E
    float model_lj2_from_top1_E() const;
    /// set model_lj2_from_top1_E
    void setModel_lj2_from_top1_E(float);

    /// get model_lj2_from_top1_jetIndex
    unsigned int model_lj2_from_top1_jetIndex() const;
    /// set model_lj2_from_top1_jetIndex
    void setModel_lj2_from_top1_jetIndex(unsigned int);

    /// get model_lj1_from_top2_pt
    float model_lj1_from_top2_pt() const;
    /// set model_lj1_from_top2_pt
    void setModel_lj1_from_top2_pt(float);

    /// get model_lj1_from_top2_eta
    float model_lj1_from_top2_eta() const;
    /// set model_lj1_from_top2_eta
    void setModel_lj1_from_top2_eta(float);

    /// get model_lj1_from_top2_phi
    float model_lj1_from_top2_phi() const;
    /// set model_lj1_from_top2_phi
    void setModel_lj1_from_top2_phi(float);

    /// get model_lj1_from_top2_E
    float model_lj1_from_top2_E() const;
    /// set model_lj1_from_top2_E
    void setModel_lj1_from_top2_E(float);

    /// get model_lj1_from_top2_jetIndex
    unsigned int model_lj1_from_top2_jetIndex() const;
    /// set model_lj1_from_top2_jetIndex
    void setModel_lj1_from_top2_jetIndex(unsigned int);

    /// get model_lj2_from_top2_pt
    float model_lj2_from_top2_pt() const;
    /// set model_lj2_from_top2_pt
    void setModel_lj2_from_top2_pt(float);

    /// get model_lj2_from_top2_eta
    float model_lj2_from_top2_eta() const;
    /// set model_lj2_from_top2_eta
    void setModel_lj2_from_top2_eta(float);

    /// get model_lj2_from_top2_phi
    float model_lj2_from_top2_phi() const;
    /// set model_lj2_from_top2_phi
    void setModel_lj2_from_top2_phi(float);

    /// get model_lj2_from_top2_E
    float model_lj2_from_top2_E() const;
    /// set model_lj2_from_top2_E
    void setModel_lj2_from_top2_E(float);

    /// get model_lj2_from_top2_jetIndex
    unsigned int model_lj2_from_top2_jetIndex() const;
    /// set model_lj2_from_top2_jetIndex
    void setModel_lj2_from_top2_jetIndex(unsigned int);

  }; // class KLFitterResult

} // namespace xAOD

// Define a ClassID for the type.
CLASS_DEF(xAOD::KLFitterResult, 103465656, 1)

#endif // not ANALYSISTOP_TOPEVENT_KLFITTERRESULT_H
