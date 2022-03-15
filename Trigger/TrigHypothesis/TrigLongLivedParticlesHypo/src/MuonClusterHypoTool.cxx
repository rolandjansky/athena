/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

  * Trigger Hypo Tool, that is aimed at triggering clusters of muon RoIs
  * author Michael Schott <michael.louis.schott@cern.ch>
*/

#include <cmath>
#include <algorithm>
#include <sstream>
#include <string>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigLongLivedParticlesHypo/MuonClusterHypoAlg.h"
#include "TrigLongLivedParticlesHypo/MuonClusterHypoTool.h"

using namespace TrigCompositeUtils;

MuonClusterHypoTool::MuonClusterHypoTool(   const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode MuonClusterHypoTool::initialize()
{
   if ( !m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );

   ATH_MSG_INFO("Initialization completed successfully:");
   ATH_MSG_INFO("Tool configured for chain/id: " << m_decisionId);

   return StatusCode::SUCCESS;
}

MuonClusterHypoTool::~MuonClusterHypoTool() {}

StatusCode  MuonClusterHypoTool::decide( DecisionInfo& input ) const
{
    auto monNumberRoI    = Monitored::Scalar<int>( "numberRoI",  -99 );

    auto monEtaClust     = Monitored::Scalar<double>( "etaClust", -99 );
    auto monPhiClust     = Monitored::Scalar<double>( "phiClust", -99 );
    auto monEtaClustPass = Monitored::Scalar<double>( "etaClustPass", -99 );
    auto monPhiClustPass = Monitored::Scalar<double>( "phiClustPass", -99 );

    auto monNumRoIB      = Monitored::Scalar<int>( "nRoIBarrel",  -99 );
    auto monNumRoIE      = Monitored::Scalar<int>( "nRoIEndcap",  -99 );
    auto monNumRoIBPass  = Monitored::Scalar<int>( "nRoIBarrelPass",  -99 );
    auto monNumRoIEPass  = Monitored::Scalar<int>( "nRoIEndcapPass",  -99 );

    auto result          = Monitored::Scalar<Bool_t>( "result", false );

    auto t1              = Monitored::Timer("TIME_HypoTool"); // microseconds
    auto t2              = Monitored::Timer("TIME_HypoTool_GetCluster");
    auto t3              = Monitored::Timer("TIME_HypoTool_Selection");

    auto monitorIt = Monitored::Group( m_monTool, monNumberRoI, monEtaClust, monPhiClust,
                                       monNumRoIB, monNumRoIE, monEtaClustPass, monPhiClustPass,
                                       result, monNumRoIBPass, monNumRoIEPass,
                                       t1, t2, t3 );

    const xAOD::TrigCompositeContainer *compCont = input.Composites;
    TrigCompositeUtils::DecisionIDContainer prev = input.activeChainIDs;

    // Exctracting MuonClusterRoI info
    ATH_MSG_DEBUG("Found  " << compCont->size() << " TrigComposite objects");

    int numberRoI = 0;
    double etaClust = -99;
    double phiClust = -99;

    bool foundMuonRoICluster = false;

    t2.start();
    for(const xAOD::TrigComposite *compObj : *compCont) {
        ATH_MSG_DEBUG("TrigComposite ptr = " << compObj);
        ATH_MSG_DEBUG("Found object named Cluster : " << (compObj->name()== "Cluster" ? "yes":"no"));
        if(compObj->name() == "Cluster" ) {
            foundMuonRoICluster = true;
            //if we can't access all the variables necessary for the trigger hypothesis,
            //return FAILURE, because it means the variables are missing
            if(!compObj->getDetail("nRoIs", numberRoI)) { return StatusCode::FAILURE; }
            if(!compObj->getDetail("ClusterEta", etaClust)) { return StatusCode::FAILURE; }
            if(!compObj->getDetail("ClusterPhi", phiClust)) { return StatusCode::FAILURE; }
        }
    }
    t2.stop();
    if( !foundMuonRoICluster ) { return StatusCode::FAILURE;}

    // Updating Monitoring values
    monNumberRoI = numberRoI;
    monEtaClust  = etaClust;
    monPhiClust  = phiClust;

    // Monitor ALL muonRoICluster multiplicities
    if(fabs(etaClust)< m_etaMid){
        monNumRoIB = numberRoI;
    } else if((fabs(etaClust)>= m_etaMid && fabs(etaClust)<= m_etaMax)){
        monNumRoIE = numberRoI;
    }

    // Bool as to whether we passed.
    result = false;

    // acceptAll can be enabled for testing
    if(m_acceptAll) {
        t3.start();
        result = true;
        ATH_MSG_DEBUG("Accept property is set: taking all the events");
        t3.stop();

    } else {

        t3.start();
        result = true; //set result to true in case of errors for safety

        ATH_MSG_DEBUG("Cluster has " << numberRoI << " muon RoIs, at (eta, phi) = (" << etaClust << ", " << phiClust << ")");

        // Cut on number of RoIs in the cluster
        if(fabs(etaClust)<=m_etaMax) {

            if(fabs(etaClust)< m_etaMid && numberRoI >= m_nRoIBarrel) {

                    ATH_MSG_DEBUG("Cluster passes barrel selection > " << m_nRoIBarrel << " RoIs");

                    // monitored variables
                    monNumRoIBPass = numberRoI;
                    monEtaClustPass = etaClust;
                    monPhiClustPass = phiClust;
                    result = true;

            } else if((fabs(etaClust)>= m_etaMid && fabs(etaClust)<= m_etaMax) && numberRoI >= m_nRoIEndCap) {

                    ATH_MSG_DEBUG("Cluster passes endcap selection > " << m_nRoIEndCap << " RoIs");

                    // monitored variables
                    monNumRoIEPass = numberRoI;
                    monEtaClustPass = etaClust;
                    monPhiClustPass = phiClust;
                    result = true;

            } else {
                ATH_MSG_DEBUG("Cluster does not satisfy all the conditions... event not accepted");
                result = false;
            }

        } else {
            ATH_MSG_DEBUG("Cluster does not satisfy all the conditions... event not accepted");
            result = false;
        }

        t3.stop();

    }

    // Checking if this HypoTool's chain is active in this event...
    if( (prev.count(m_decisionId.numeric() ) > 0 ) )
    {
        // If it's active, then check selection. ie nothing should pass if its not active.
        if( result ) {
            addDecisionID( m_decisionId.numeric(), input.decision );
            ATH_MSG_DEBUG("event accepted");

            return StatusCode::SUCCESS;
        }
        else {
            return StatusCode::SUCCESS;
        }
    } else {
        return StatusCode::SUCCESS;
    }
}
