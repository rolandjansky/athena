// this is a -*- C++ -*- file
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////
/// \class JetViewAlg 
///
/// Algorithm that selects a subset of jets from
/// the input container, places them in a view
/// JetContainer 
/// (i.e. one that does not own its contents)
/// and then writes this container to StoreGate.
///
///

#ifndef JetViewAlg_H
#define JetViewAlg_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "AthContainers/ConstDataVector.h"

class JetViewAlg : public AthReentrantAlgorithm { 

public: 

  //Delegate to base-class constructor
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  /// Athena algorithm's Hooks
  StatusCode initialize() override;
  StatusCode execute(const EventContext&) const override;
  StatusCode finalize() override {return StatusCode::SUCCESS;};

private: 

  /// Method to select jets
  bool selected( const xAOD::Jet& ) const;

private:

  Gaudi::Property<float> m_ptmin         {this, "PtMin",     0.0,  "Min pT in MeV"};
  Gaudi::Property<float> m_absetamax     {this, "AbsEtaMax", 10.0, "Max absolute eta"};

  SG::ReadHandleKey<xAOD::JetContainer>  m_input  = {this, "InputContainer",  "", "The input jet container name"};
  SG::WriteHandleKey<ConstDataVector<xAOD::JetContainer> > m_output = {this, "OutputContainer", "", "The output jet container name"};
  
}; 

#endif

