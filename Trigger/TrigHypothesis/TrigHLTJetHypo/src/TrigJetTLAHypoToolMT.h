/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

#ifndef TRIGJETTLAHYPOTOOLMT_H
#define TRIGJETTLAHYPOTOOLMT_H
/********************************************************************
 *
 * NAME:     TrigJetTLAHypoToolMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *  * @class TrigJetTLAHypoToolMT
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

#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

class TrigJetTLAHypoToolMT: public AthAlgTool{

  public:

    TrigJetTLAHypoToolMT(const std::string& type,
        const std::string& name,
        const IInterface* parent);

    ~TrigJetTLAHypoToolMT();
    StatusCode initialize();
    StatusCode finalize();

    // typedef for passing jet - decision object assocation
    using JetDecision = std::pair<const xAOD::Jet*,TrigCompositeUtils::Decision*>;

    StatusCode
      decide(std::vector<JetDecision>& jetHypoInputs) const;

    const HLT::Identifier& getId() const; 

  private:

    // Identifier is used to keep track of which tool made which decision.
    // The information is stored in the event store.
    HLT::Identifier m_decisionId;  

    ToolHandle<ITrigJetHypoToolHelperMT> m_helper {
      this, "helper_tool", {}, "Jet hypo helper AlgTool"};

    Gaudi::Property<bool>
      m_visitDebug {this, "visit_debug", false, "debug flag"};



};
#endif
