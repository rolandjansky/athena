/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigJetSplitterMT.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Carlo Varni
// EMAIL:    carlo.varni@ge.infn.it
//
// ************************************************

#ifndef TRIGJETSPLITTER_MT_H
#define TRIGJETSPLITTER_MT_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

class TrigJetSplitterMT : public AthAlgorithm {
 public:
  /** @brief Constructor */
  TrigJetSplitterMT(const std::string&, ISvcLocator*);

  StatusCode initialize();
  StatusCode execute();

 private:
  StatusCode shortListJets( const xAOD::JetContainer*,
                            std::unique_ptr< xAOD::JetContainer >&,
                            std::unique_ptr< TrigRoiDescriptorCollection >&,
			    const xAOD::Vertex* ) const;

 private:
  Gaudi::Property< bool > m_imposeZconstraint {this,"ImposeZconstraint",false,"Impose Constraint on PV z, thus selecting Jets pointing to PV"};
  Gaudi::Property< float > m_etaHalfWidth {this,"EtaHalfWidth",0.4,"Eta Half Width"};
  Gaudi::Property< float > m_phiHalfWidth {this,"PhiHalfWidth",0.4,"Phi Half Width"};
  Gaudi::Property< float > m_zHalfWidth {this,"ZHalfWidth",10.0,"Z Half Width in mm"};
  Gaudi::Property< float > m_minJetEt {this,"JetMinEt",15000,"Minimum of Output Jet Et in GeV"};
  Gaudi::Property< float > m_maxJetEta {this,"JetMaxEta",3.2,"Maximum eta acceptance of output Jet"};
  //=========== Handles                                                                                                                                                                      
  SG::ReadHandleKey< xAOD::JetContainer > m_inputJetsKey {this,"Jets","Jets","Input Jet Container Key"};
  SG::ReadHandleKey< xAOD::VertexContainer > m_inputVertexKey {this,"InputVertex","Undefined","Output Vertex Key"};

  SG::WriteHandleKey< xAOD::JetContainer > m_outputJetsKey {this,"OutputJets","SplitJets","Output Jet Container Key"};
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_outputRoiKey {this,"OutputRoi","SplitJet","Output RoI Container Key -- Same as OutputJets"};

  // Tmp Part for creating custom jet collection. These Jets will be used for creating the output Jet collection
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_inputRoIKey {this,"RoIs","FSJETRoI",""};
};

#endif
