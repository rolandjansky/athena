/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

  This initial version of TrigJetHypo is not yet production code.
  In particular, it can read in a jet collection at given (default or
  configured) store address, whereas different chains may require
  different jet collections (constructed with different jet reco paramters).

  Either there will need to be more than one instance of TrigJetRec,
  or an association between chain name and collection name will ned tobe set
  up.
*/
#ifndef TRIGHLTJETHYPO_TRIGJETHYPO_H
#define TRIGHLTJETHYPO_TRIGJETHYPO_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigHLTJetHypo/ITrigHLTJetHypoTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
/**
 * @class Implements jet selection for the new HLT framework
 * @brief 
 **/


class TrigJetHypo
  : public ::AthReentrantAlgorithm
{ 
 public: 

  TrigJetHypo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigJetHypo(); 

  StatusCode  initialize() override;
  StatusCode  execute_r( const EventContext& context ) const override;
  StatusCode  finalize() override;


 private: 

  ToolHandleArray<ITrigHLTJetHypoTool> m_hypoTools;

  SG::ReadHandleKey<xAOD::JetContainer> m_jetsKey{
    this, 
      "JetCollection",  // name of python instance attribute 
      "JetCollection",  // default store key name 
      "jet container name in store" // doc
      };

  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionsKey{
    this,
      "JetHypoDecisions", // name of python instance attribute
      "JetHypoDecisions", // default store key name 
      "name of jet hypo decisions object in store" // doc
      };
};


#endif
