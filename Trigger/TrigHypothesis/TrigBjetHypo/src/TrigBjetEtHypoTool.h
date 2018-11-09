/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetEtHypoTool.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetEtHypo
//
// AUTHOR:   Carlo Varni
// EMAIL:    Carlo.Varni@ge.infn.it
// 
// ************************************************

#ifndef TRIGBJETHYPO_TRIGBJETETHYPOTOOL_H
#define TRIGBJETHYPO_TRIGBJETETHYPOTOOL_H 1

#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgTool.h" 
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

static const InterfaceID IID_TrigBjetEtHypoTool("TrigBjetEtHypoTool",1,0);


class TrigBjetEtHypoTool : virtual public ::AthAlgTool {
  
 public:

  /** @brief Constructor. */
  TrigBjetEtHypoTool (const std::string& type,
		      const std::string& name,
		      const IInterface* parent );
  /** @brief Destructor. */
  virtual ~TrigBjetEtHypoTool ();

  StatusCode initialize() override;
  StatusCode finalize() override;

  static const InterfaceID& interfaceID();

  TrigCompositeUtils::DecisionID decisionId() const {
    return m_id.numeric();
  }

  const HLT::Identifier getId() const {
    return m_id;
  }

  StatusCode decide(  const xAOD::JetContainer*,bool& ) const;

 private:
  HLT::Identifier m_id;

  /** @brief DeclareProperty: if acceptAll flag is set to true, every event is taken. */ 
  Gaudi::Property< bool > m_acceptAll {this,"AcceptAll",false,"if acceptAll flag is set to true, every event is taken"};
  /** @brief DeclareProperty: Multeplicity requirement. */
  Gaudi::Property< int > m_multeplicity {this,"Multeplicity",1,"Multeplicity requirement"};
  /** @brief DeclareProperty: Et threshold cut. */
  Gaudi::Property< float > m_etThreshold {this,"EtThreshold",0.0,"Et threshold cut"};
};

inline const InterfaceID& TrigBjetEtHypoTool::interfaceID()
{
  return IID_TrigBjetEtHypoTool;
}


#endif  // !TRIGBJETHYPO_TRIGBJETETHYPOTOOL_H   

