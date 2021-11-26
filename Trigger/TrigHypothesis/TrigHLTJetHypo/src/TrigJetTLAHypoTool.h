/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
   */

#ifndef TRIGJETTLAHYPOTOOL_H
#define TRIGJETTLAHYPOTOOL_H
/********************************************************************
 *
 * NAME:     TrigJetTLAHypoTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *  * @class TrigJetTLAHypoTool
 *  * @brief HypoTool for TLA algorithm
 *  * @details HypoTool needed to associate trigger navigation to jets seleected for writing out by TLA algorithm 
 *  * @author Antonio Boveia
 *  * @author Caterina Doglioni
 *  * @author Teng Jian Khoo
 *  * @author Tim Martin
 *  * @author Rafal Bielski
 *          
 *
 ********************************************************************/

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "TrigHLTJetHypo/ITrigJetHypoToolHelper.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

class TrigJetTLAHypoTool: public AthAlgTool{

  public:

    TrigJetTLAHypoTool(const std::string& type,
        const std::string& name,
        const IInterface* parent);

    ~TrigJetTLAHypoTool();
    StatusCode initialize();
    StatusCode finalize();

    StatusCode
      decide(TrigCompositeUtils::DecisionContainer* outputDecisions) const;

    const HLT::Identifier& getId() const; 

  private:

    // Identifier is used to keep track of which tool made which decision.
    // The information is stored in the event store.
    HLT::Identifier m_decisionId;  

    ToolHandle<ITrigJetHypoToolHelper> m_helper {
      this, "helper_tool", {}, "Jet hypo helper AlgTool"};

    Gaudi::Property<bool>
      m_visitDebug {this, "visit_debug", false, "debug flag"};

    Gaudi::Property<std::string> m_chainName {
    this, "chain_name", {}, "chain name"};
  



};
#endif
