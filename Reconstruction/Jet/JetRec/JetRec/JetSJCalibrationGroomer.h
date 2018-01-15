/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JetSJCalibrationGroomer_H
#define JetSJCalibrationGroomer_H

// Tool to groom jets by trimming.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "JetCalibTools/IJetCalibrationTool.h"
#include "AsgTools/ToolHandle.h"

class JetSJCalibrationGroomer
: public asg::AsgTool,
  virtual public IJetGroomer {
ASG_TOOL_CLASS(JetSJCalibrationGroomer, IJetGroomer)

public:

  JetSJCalibrationGroomer(std::string name);

  ~JetSJCalibrationGroomer();

  StatusCode initialize() override ;

  // Groom a jet (trimming and filtering supported), calibrate subjets, and add result to a container.
  int groom(const xAOD::Jet& jin, xAOD::JetContainer& jcout) const override;

  // print config
  void print() const override;

  private: 
  double m_rClus;                        // R for reclustering (0 for none)
  double m_pTFrac;                       // pT fraction for retaining subjets
  int    m_nFiltJets;                    // n jets kept in filtering (0 -> trimming)
  int    m_reclustAlgo;                  // algorithm used for subjet reclustering - 0: kt, 1: CA. 2: AntiKt
  ToolHandle<IJetFromPseudojet>   m_jetFromPJTool;  // tool to build jets
  ToolHandle<IJetCalibrationTool> m_subjetCalTool; // tool to calibrate jets

  enum GroomingMethod { trimming, filtering, invalid };
  GroomingMethod m_groomingMethod;

  // helper functions
  void fillJetContainer( 
      const std::vector<fastjet::PseudoJet>& inPjVec, 
      xAOD::JetContainer& outJetCont) const;


};

#endif
