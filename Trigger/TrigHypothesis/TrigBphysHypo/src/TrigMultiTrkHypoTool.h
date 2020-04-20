/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigMultiTrkHypoTool.h
 **
 **   Description: multi track hypothesis tool
 **
 **   Author: H. Russell
 **
 **************************************************************************/

#ifndef TRIG_TrigMultiTrkHypoTool_H
#define TRIG_TrigMultiTrkHypoTool_H

// standard stuff
#include <string>
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "Constants.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "TrigCompositeUtils/Combinators.h"


class TrigMultiTrkHypoTool: public ::AthAlgTool  {
  enum { MaxNumberTools = 20 };
  public:
    TrigMultiTrkHypoTool(const std::string& type,
                       const std::string & name,
                       const IInterface* parent );

    virtual ~TrigMultiTrkHypoTool();

    struct TrigMultiTrkInfo {
    TrigMultiTrkInfo( TrigCompositeUtils::Decision* d, const xAOD::TrigBphys* c,
                     const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs0,
                     const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs1 )
    : decision( d ),
      trigBphys( c ),
      previousDecisionIDs0 ( previousDecisionIDs0 ),
      previousDecisionIDs1( previousDecisionIDs1 )

      {}

      TrigCompositeUtils::Decision* decision;
      const xAOD::TrigBphys* trigBphys;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs0;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs1;

    };

    virtual StatusCode initialize() override;
    virtual StatusCode decide(std::vector<TrigMultiTrkHypoTool::TrigMultiTrkInfo>& toolInput) const;
    bool decideOnSingleObject(const xAOD::TrigBphys* trigBphys, size_t cutIndex) const;
    StatusCode inclusiveSelection(std::vector<TrigMultiTrkHypoTool::TrigMultiTrkInfo>& toolInput) const;

  private:

    HLT::Identifier m_decisionId;


    // Mass window cuts
    Gaudi::Property<int> m_nTrk { this, "nTrk",2,"Number of tracks in the vertex"};
    Gaudi::Property< int > m_TotChargeCut{this, "TotChargeCut", 0, "The Magnitude of the total charge to accept, negative is none" };
    Gaudi::Property< float > m_lowerMassCut{this, "LowerMassCut", -99. , "Lower mass cut for vertex " };
    Gaudi::Property< float > m_upperMassCut{this, "UpperMassCut", -99. , "Upper mass cut for vertex" };
    Gaudi::Property< bool >m_applyUpperMassCut{this, "ApplyUpperMassCut", false, "Apply the upper mass cut" };
    Gaudi::Property< bool > m_applyChi2Cut{this, "ApplyChi2Cut", false, "Apply a chi2 cut to vertex" };
    Gaudi::Property< float > m_chi2VtxCut{this, "Chi2VtxCut", -99. , "Chi2 cut for vertex (0 < chi2 < cut)" };
    Gaudi::Property< int > m_nBphysObjects {this, "nBphysObjects", 1, "Number of good b-phys objects required per event" };
    Gaudi::Property<std::vector<float>> m_ptTrkMin { this, "trkPtThresholds", {3650, 3650} ,"minimum track pTs (one per track, sorted descending!)"};

    // to set Accept-All mode: should be done with force-accept when possible
    Gaudi::Property< bool > m_acceptAll {this, "AcceptAll", false, "Ignore selection" };

    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

};

#endif  // TRIG_TrigMultiTrkHypoTool_H
