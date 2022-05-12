/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
   */

#ifndef TRIGJETCRHYPOTOOL_H
#define TRIGJETCRHYPOTOOL_H
/********************************************************************
 *
 * NAME:     TrigJetCRHypoTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *  * @class TrigJetCRHypoTool
 *  * @brief HypoTool for 
 *  * @details HypoTool needed to decide on exotic jet selections
 *  * @author
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
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include <CaloEvent/CaloConstCellContainer.h>

class TrigJetCRHypoTool: public AthAlgTool{

 public:

  TrigJetCRHypoTool(const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  ~TrigJetCRHypoTool();
  StatusCode initialize();
  StatusCode finalize();

  // typedef for passing jet - decision object assocation
  using JetDecision = std::pair<const xAOD::Jet*,TrigCompositeUtils::Decision*>;

  struct JetInfo {
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    const xAOD::Jet* jet;
    const xAOD::TrackParticleContainer* allTracks;
    const CaloConstCellContainer* cells;
    TrigCompositeUtils::Decision* decision;
  };


  /**
   * @brief decides upon a collection of jets
   **/
  StatusCode decide( std::vector<JetInfo>& decisions )  const;


  /**
   * @brief Auxiluary method, single jet selection
   **/
  bool decide_on_single_jet( JetInfo& input ) const;


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

  // minjetlogR may be read from trigger chain name -- to be updated
  Gaudi::Property< float > m_jetlogRCut{ this, "MinjetlogR",  { float(1.2 ) }, "Jet logR requirement" };

  Gaudi::Property< float > m_trackPtCut{ this, "MintrackPt",  { float( 2.0*Gaudi::Units::GeV ) }, "Track pT requirement" };
  Gaudi::Property< float > m_deltaR{ this, "MindeltaR",  { float( 0.2 ) }, "deltaR(jet, track) requirement" };

  Gaudi::Property< bool > m_doBIBrm{ this, "doBIBremoval",  { int(0) }, "option to include BIB removal" };

  Gaudi::Property< float > m_dEta{ this, "BibMinDeltaEta",  { float( 0.4 ) }, "BIB deltaEta requirement" };
  Gaudi::Property< float > m_dPhi{ this, "BibMinDeltaPhi",  { float( 0.2 ) }, "BIB deltaPhi requirement" };
  Gaudi::Property< float > m_minCellEt{ this, "BibMinCellEt",  { float( 240.0 ) }, "BIB cell Et requirement" };
  Gaudi::Property< float > m_celltime{ this, "BibCelltiming",  { float( -2.0 ) }, "BIB cell timing requirement" };
  Gaudi::Property< float > m_dBIBtime{ this, "BibDeltatiming",  { float( 5.0 ) }, "BIB delta timing requirement" };
  Gaudi::Property< int > m_countCaloCell{ this, "countBIBcells",  { int( 4 ) }, "requirement on number of BIB cells" };


};
#endif
