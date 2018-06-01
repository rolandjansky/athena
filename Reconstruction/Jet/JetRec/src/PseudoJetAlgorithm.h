/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetAlgorithm.h 

#ifndef PseudoJetAlgorithm_H
#define PseudoJetAlgorithm_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPseudoJetGetter;

class PseudoJetAlgorithm : public AthAlgorithm { 

public: 

  /// Constructor with parameters: 
  PseudoJetAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  ~PseudoJetAlgorithm(); 

  /// Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override;
  virtual StatusCode  finalize() override;

private: 

  /// Default constructor: 
  PseudoJetAlgorithm();

private:

  /// Athena configured tools
  ToolHandle<IPseudoJetGetter> m_pjg;

}; 

#endif
