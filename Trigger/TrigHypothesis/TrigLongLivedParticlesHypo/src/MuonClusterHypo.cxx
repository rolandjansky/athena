/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// MuonClusterHypo  
// (see header for history/etc..)
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigLongLivedParticlesHypo/MuonClusterHypo.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

MuonClusterHypo::MuonClusterHypo(const std::string & name, ISvcLocator* pSvcLocator):
                          HLT::HypoAlgo(name, pSvcLocator){

    //default monitored values are unphysical
    //events failing cuts will show up in monitoring with these values
    //note that the "cut" for background trigger is nJet,nTrk ==-1, but
    //the actual number of jets and tracks in dR cone is monitored

    m_CluNum = 3;

    declareMonitoredVariable("NumClu", m_CluNum);

    declareProperty("AcceptAll", m_acceptAll=false);
    declareProperty("nRoIEndCap", m_nRoIEndCap=4);   
    declareProperty("nRoIBarrel", m_nRoIBarrel=3);   
    declareProperty("maxEta", m_etaMax=2.5);   
    declareProperty("midEta", m_etaMid=1.0); 
    
}

MuonClusterHypo::~MuonClusterHypo(){
}

HLT::ErrorCode MuonClusterHypo::hltInitialize(){

    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "in initialize()" << endmsg;

        if(m_acceptAll) {
            msg() << MSG::DEBUG << "Accepting all the events with no cuts!" << endmsg;
        } else {
            msg() << MSG::DEBUG << "Selecting barrel muon RoI clusters with |eta|< " << m_etaMid << " and >= " << m_nRoIBarrel << " RoIs" << endmsg;
            msg() << MSG::DEBUG << "and endcap muon RoI clusters with |eta| > : " << m_etaMid << "and >= " <<m_nRoIEndCap << " RoIs" << endmsg; 
        }
        msg() << MSG::DEBUG << "Initialization completed successfully"  << endmsg;
    }

    return HLT::OK;
}

HLT::ErrorCode MuonClusterHypo::hltFinalize(){

    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "in finalize()" << endmsg;
    }

    return HLT::OK;

}

HLT::ErrorCode MuonClusterHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

    // Retrieve store.
    m_storeGate = store();

    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "in execute()" << endmsg;
    }

    m_cutCounter = -1;

    if(m_acceptAll) {
        pass = true;
        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG 
                    << "Accept property is set: taking all the events"
                    << endmsg;
        }
        return HLT::OK;

    } else {

        bool result = true; //set result to true in case of errors for safety

        // Some debug output:
        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endmsg;
        }

        // Get the TrigCompositeContainer linked to the outputTE
        // it contains the Muon RoI Cluster information

        const xAOD::TrigCompositeContainer *compCont(0);
        HLT::ErrorCode status = getFeature(outputTE, compCont);
        if(status != HLT::OK){
            if(msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << "no TrigCompositeContainer feature found" << endmsg;
            }
            return status;
        } else {
            if(msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << "TrigCompositeContainer feature retrieved" << endmsg;
            }
        }
        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << "Found  " << compCont->size() << " TrigComposite objects" << endmsg;
        }
        int numberRoI = 0;
        float etaClust = -99;
        float phiClust = -99;

        bool foundMuonRoICluster = false;

        for(const xAOD::TrigComposite * compObj : *compCont) {
            if(msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << "TrigComposite ptr = " << (const void*)compObj << endmsg;
            }
            for(const std::string & collName : compObj->linkColNames()) {
                if(msgLvl() <= MSG::DEBUG) {
                    msg() << MSG::DEBUG << " link to collection " << collName << endmsg;
                }
            }         
            if(msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << "Found object named Cluster : " << (compObj->name()== "Cluster" ? "yes":"no") << endmsg;
            }
            if(compObj->name() == "Cluster" ) {
                foundMuonRoICluster = true; 
                //if we can't access all the variables necessary for the trigger hypothesis, 
                //return MISSING_FEATURE, because it means the variables are missing
                if(!compObj->getDetail("nRoIs", numberRoI)) { return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); }
                if(!compObj->getDetail("ClusterEta", etaClust)) { return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); }
                if(!compObj->getDetail("ClusterPhi", phiClust)) { return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); }
            }
        }

        if( !foundMuonRoICluster ) { return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);}

        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << "Cluster has " << numberRoI << " muon RoIs, at (eta, phi) = (" << etaClust << ", " << phiClust << ")" << endmsg;
        }

        // Cut on number of RoIs in the cluster
        if(fabs(etaClust)<=m_etaMax) {

            if(fabs(etaClust)< m_etaMid && numberRoI >= m_nRoIBarrel) {

                if(msgLvl() <= MSG::DEBUG) {
                    msg() << MSG::DEBUG << "Cluster passes barrel selection > " << m_nRoIBarrel << " RoIs" << endmsg;
                }

                // monitored variables
                m_CluNum = numberRoI;
                result = true;

            } else if((fabs(etaClust)>= m_etaMid && fabs(etaClust)<= m_etaMax) && numberRoI >= m_nRoIEndCap) {

                if(msgLvl() <= MSG::DEBUG) {
                    msg() << MSG::DEBUG << "Cluster passes endcap selection > " << m_nRoIEndCap << " RoIs" << endmsg;
                }

                // monitored variables
                m_CluNum = numberRoI;
                result = true;

            } else {
                if(msgLvl() <= MSG::DEBUG) {
                    msg() << MSG::DEBUG << "Cluster does not satisfy all the conditions... event not accepted" << endmsg;
                }
                result = false;
            }
        } else {
            if(msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << "Cluster does not satisfy all the conditions... event not accepted" << endmsg;
            }
            result = false;		   
        }

        //Store result
        pass = result;

        return HLT::OK;
    }
}
