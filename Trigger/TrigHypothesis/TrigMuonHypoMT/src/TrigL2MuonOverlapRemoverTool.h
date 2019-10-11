/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONOVERLAPREMOVER_TRIGL2MUONOVERLAPREMOVERTOOL_H 
#define TRIGL2MUONOVERLAPREMOVER_TRIGL2MUONOVERLAPREMOVERTOOL_H 1

#include "DecisionHandling/HLTIdentifier.h"

#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "DecisionHandling/TrigCompositeUtils.h" 
#include "AthenaMonitoring/GenericMonitoringTool.h"

class StoreGateSvc;
class TriggerElement;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonOverlapRemoverToolConsts {
const int  errorCode_cannot_get_EL         = 1;
const int  errorCode_EL_not_valid          = 2;
const int  errorCode_inconsistent_overlap1 = 3;
const int  errorCode_inconsistent_overlap2 = 4;
const int  errorCode_cannot_get_RoI        = 5;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigL2MuonOverlapRemoverTool: public ::AthAlgTool {

  public:

    TrigL2MuonOverlapRemoverTool(const std::string& type, 
		                 const std::string & name,
		                 const IInterface* parent );

    virtual ~TrigL2MuonOverlapRemoverTool();

    struct L2SAMuonOverlapInfo {
    L2SAMuonOverlapInfo( TrigCompositeUtils::Decision* d, 
                      const LVL1::RecMuonRoI* r,
                      const xAOD::L2StandAloneMuon* o,
                      const TrigCompositeUtils::Decision* previousDecision )
    : decision( d ),
      RecRoI( r ),
      overlap( o ),
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(),
                           TrigCompositeUtils::decisionIDs( previousDecision ).end() )
      {}


      TrigCompositeUtils::Decision* decision;
      const LVL1::RecMuonRoI* RecRoI;
      const xAOD::L2StandAloneMuon* overlap;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };
 
    struct L2CBMuonOverlapInfo {
    L2CBMuonOverlapInfo( TrigCompositeUtils::Decision* d, 
                      const LVL1::RecMuonRoI* r,
                      const xAOD::L2CombinedMuon* o,
                      const TrigCompositeUtils::Decision* previousDecision )
    : decision( d ),
      RecRoI( r ),
      overlap( o ),
      previousDecisionIDs( TrigCompositeUtils::decisionIDs( previousDecision ).begin(),
                           TrigCompositeUtils::decisionIDs( previousDecision ).end() )
      {}


      TrigCompositeUtils::Decision* decision;
      const LVL1::RecMuonRoI* RecRoI;
      const xAOD::L2CombinedMuon* overlap;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };
 

    virtual StatusCode initialize() override;

    virtual StatusCode decide(std::vector<TrigL2MuonOverlapRemoverTool::L2SAMuonOverlapInfo>& toolInput) const;
    virtual StatusCode decide(std::vector<TrigL2MuonOverlapRemoverTool::L2CBMuonOverlapInfo>& toolInput) const;

    virtual StatusCode checkOverlap(std::vector<L2SAMuonOverlapInfo>& toolInput) const;
    virtual StatusCode checkOverlap(std::vector<L2CBMuonOverlapInfo>& toolInput) const;

    bool isOverlap(const xAOD::L2StandAloneMuon *mf1, const xAOD::L2StandAloneMuon *mf2) const;
    bool isOverlap(const xAOD::L2CombinedMuon *mf1, const xAOD::L2CombinedMuon *mf2) const;

    double dR(double eta1, double phi1, double eta2, double phi2) const;

    double invMass(double m1, double pt1, double eta1, double phi1,
                   double m2, double pt2, double eta2, double phi2) const;

    virtual StatusCode chooseBestMuon(std::vector<L2SAMuonOverlapInfo>& toolInput, std::vector<L2SAMuonOverlapInfo>& uniqueMuon, std::vector<unsigned int> mufastResult) const;
    virtual StatusCode chooseBestMuon(std::vector<L2CBMuonOverlapInfo>& toolInput, std::vector<L2CBMuonOverlapInfo>& uniqueMuon, std::vector<unsigned int> mucombResult) const;


  private:

    HLT::Identifier m_decisionId;

    Gaudi::Property< unsigned int > m_multiplicity {
        this, "Multiplicity", 1, "multiplicity" };

    // Mufast based cuts
    Gaudi::Property<bool> m_doMufastBasedRemoval {
        this, "DoMufastBasedRemoval", false, "execute or not mufast overlap removal"};

    Gaudi::Property<bool> m_muFastRequireDR{
        this, "MufastRequireDR", true, "require or not DR cut for mufast overlap removal"};

    Gaudi::Property<bool> m_muFastRequireMass{
        this, "MufastRequireMass", true, "require or not mass cut for mufast overlap removal"};

    Gaudi::Property<bool> m_muFastRequireSameSign{
        this, "MufastRequireSameSign", true, "require or not charge cut for mufast overlap removal"};

    Gaudi::Property< float > m_muFastDRThresBB {
        this, "MufastDRThresBB", 0.05, "DR threshold for mufast in barel and barel region"};

    Gaudi::Property< float > m_muFastMassThresBB {
        this, "MufastMassThresBB", 0.20, "mass threshold for mufast in barel and barel region"};

    Gaudi::Property< float > m_muFastDRThresBE {
        this, "MufastDRThresBE", 0.05, "DR threshold for mufast in barel and barel region"};

    Gaudi::Property< float > m_muFastMassThresBE {
        this, "MufastMassThresBE", 0.20, "mass threshold for mufast in barel and endcap region"};

    Gaudi::Property< std::vector<float> > m_muFastEtaBinsEC {
        this, "MufastEtaBinsEC", {0, 1.9, 2.1, 9.9}, "eta bins of DR and mass thresholds for mufast in endcap and barel region"};

    Gaudi::Property< std::vector<float> > m_muFastDRThresEC {
        this, "MufastDRThresEC", {0.06, 0.05, 0.05}, "DR threshold for mufast in endcap and barel region"};

    Gaudi::Property< std::vector<float> > m_muFastMassThresEC {
        this, "MufastMassThresEC", {0.20, 0.15, 0.10}, "mass threshold for mufast in endcap and endcap region"};


    // Mucomb based cuts
    Gaudi::Property<bool> m_doMucombBasedRemoval {
        this, "DoMucombBasedRemoval", false, "execute or not mucomb overlap removal"};

    Gaudi::Property<bool> m_muCombRequireDR{
        this, "MucombRequireDR", true, "require or not DR cut for mucomb overlap removal"};

    Gaudi::Property<bool> m_muCombRequireMufastDR{
        this, "MucombRequireMufastDR", true, "require or not mufast DR cut for mucomb overlap removal"};

    Gaudi::Property<bool> m_muCombRequireMass{
        this, "MucombRequireMass", true, "require or not mass cut for mucomb overlap removal"};

    Gaudi::Property<bool> m_muCombRequireSameSign{
        this, "MucombRequireSameSign", true, "require or not charge cut for mucomb overlap removal"};

    Gaudi::Property< std::vector<float> > m_muCombEtaBins {
        this, "MucombEtaBins", {0, 1.9, 2.1, 9.9}, "eta bins of DR and mass thresholds for mucomb"};

    Gaudi::Property< std::vector<float> > m_muCombDRThres {
        this, "MucombDRThres", {0.06, 0.05, 0.05}, "DR threshold for mucomb"};

    Gaudi::Property< std::vector<float> > m_muCombMufastDRThres {
        this, "MucombMufastDRThres", {0.06, 0.05, 0.05}, "mufast DR threshold for mucomb"};

    Gaudi::Property< std::vector<float> > m_muCombMassThres {
        this, "MucombMassThres", {0.20, 0.15, 0.10}, "mass threshold for mucomb"};


    // Other members:   
    std::vector<size_t> m_bins = {0};
    
    ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };   

};

#endif
