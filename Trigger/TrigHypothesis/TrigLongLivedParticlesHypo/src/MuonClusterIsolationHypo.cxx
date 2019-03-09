/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// MuonClusterIsolationHypo  
// (see header for history/etc..)
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigLongLivedParticlesHypo/MuonClusterIsolationHypo.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

MuonClusterIsolationHypo::MuonClusterIsolationHypo(const std::string & name, ISvcLocator* pSvcLocator):
                                  HLT::HypoAlgo(name, pSvcLocator){

    //default monitored values are unphysical
    //events failing cuts will show up in monitoring with these values
    //note that the "cut" for background trigger is nJet,nTrk ==-1, but
    //the actual number of jets and tracks in dR cone is monitored

    m_CluNum = 0;
    m_NumJet = -1;
    m_NumTrk = -1;

    declareMonitoredVariable("NumClu", m_CluNum);
    declareMonitoredVariable("NumJet", m_NumJet);
    declareMonitoredVariable("NumTrk", m_NumTrk);

    declareProperty("AcceptAll", m_acceptAll=false);
    declareProperty("doIsolation", m_doIsolation=false);
    declareProperty("nRoIEndCap", m_nRoIEndCap=4);   
    declareProperty("nRoIBarrel", m_nRoIBarrel=3);   
    declareProperty("midEta", m_etaMid=1.0);   
    declareProperty("maxEta", m_etaMax=2.5);   
    declareProperty("nJet", m_nJet=0);   
    declareProperty("nTrk", m_nTrk=0);   
}

MuonClusterIsolationHypo::~MuonClusterIsolationHypo(){
}

HLT::ErrorCode MuonClusterIsolationHypo::hltInitialize(){

    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "in initialize()" << endmsg;
    }

    if(m_acceptAll) {
        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG
                    << "Accepting all the events with no cuts!"
                    << endmsg;
        }
    } else {
        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG
                    << "Selecting muon RoI clusters with |eta|< " << m_etaMax << ", number of RoIs >= " << m_nRoIBarrel << " if in Barrel and >= " <<m_nRoIEndCap << " if in EndCap "  
                    << " and " << m_nJet << " Jets with Log(H/E)<=0.5 and " << m_nTrk << " Tracks in ID within isolation cone" 
                    << endmsg; 
        }
    }

    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG 
                << "Initialization completed successfully" 
                << endmsg;
    }

    return HLT::OK;
}

HLT::ErrorCode MuonClusterIsolationHypo::hltFinalize(){

    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "in finalize()" << endmsg;
    }

    return HLT::OK;

}

HLT::ErrorCode MuonClusterIsolationHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

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
        int numberJet = 0;
        int numberTrk = 0;
        float etaClust = -99;
        float phiClust = -99;

        bool foundMuonRoICluster = false;

        for(const xAOD::TrigComposite * compObj : *compCont) {
            if(msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG << "TrigComposite ptr = " << (const void*)compObj << endmsg;
            }
            for(const std::string & collName : compObj->linkColNames()) {
                if(msgLvl() <= MSG::DEBUG) {
                    msg() << MSG::DEBUG << "    link to collection " << collName << endmsg;
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
                if(!compObj->getDetail("nJets", numberJet)) { return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); }
                if(!compObj->getDetail("nTrks", numberTrk)) { return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); }
                if(!compObj->getDetail("ClusterEta", etaClust)) { return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); }
                if(!compObj->getDetail("ClusterPhi", phiClust)) { return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); }
            }
        }

        if( !foundMuonRoICluster ) { return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);}

        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG
                    << "Cluster has " << numberRoI << " muon RoIs, at eta=" << etaClust 
                    << " and phi=" << phiClust << " and matches " << numberJet << " Jets with Log(H/E) <= 0.5 " 
                    << " and " << numberTrk << " tracks in ID" << endmsg;
        }

        // Cut on number of RoIs in the cluster
        if(fabs(etaClust)<=m_etaMax) {

            if(fabs(etaClust)< m_etaMid && numberRoI >= m_nRoIBarrel) {

                //background trigger has doIsolation = False, so bypass jet and track cuts if background trigger
                if(m_doIsolation){
                    if( numberJet == m_nJet && numberTrk == m_nTrk ) {
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG
                                    << "Cluster passes barrel selection cuts of > " << m_nRoIBarrel << " RoIs and " << m_nJet << " Jets with Log(H/E)<=0.5 and " << m_nTrk << " tracks in ID... event accepted"
                                    << endmsg;
                        }
                        // monitored variables
                        m_CluNum = numberRoI;
                        m_NumJet = numberJet;
                        m_NumTrk = numberTrk; //set these equal to actual cluster parameters 
                        result = true;
                    } else {
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << "Cluster does not satisfy all the conditions... event not accepted" << endmsg;
                        }
                        result = false;
                    }
                } else {
                    if(msgLvl() <= MSG::DEBUG) {
                        msg() << MSG::DEBUG << "Cluster passes noiso barrel selection cuts of > " << m_nRoIBarrel << " RoIs... event accepted" << endmsg;
                    }
                    // monitored variables
                    m_CluNum = numberRoI;
                    m_NumJet = numberJet;
                    m_NumTrk = numberTrk; //set these equal to actual cluster parameters 
                    result = true;
                }
            }  else if((fabs(etaClust)>=m_etaMid && fabs(etaClust)<=m_etaMax) && numberRoI >= m_nRoIEndCap) {
                if(m_doIsolation){
                    if(numberJet == m_nJet && numberTrk == m_nTrk){
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG
                                    << "Cluster passes endcap selection cuts of > " << m_nRoIEndCap << " RoIs and " 
                                    << m_nJet << " Jets with Log(H/E)<=0.5 and " << m_nTrk << " tracks in ID... event accepted"
                                    << endmsg;
                        }

                        // monitored variables
                        m_CluNum = numberRoI;
                        m_NumJet = numberJet;
                        m_NumTrk = numberTrk; //set these equal to actual cluster parameters
                        result = true;
                    } else {
                        if(msgLvl() <= MSG::DEBUG) {
                            msg() << MSG::DEBUG << "Cluster does not satisfy all the conditions... event not accepted" << endmsg;
                        }
                        result = false;
                    }
                } else {
                    if(msgLvl() <= MSG::DEBUG) {
                        msg() << MSG::DEBUG << "Cluster passes noiso endcap selection cuts of > " << m_nRoIEndCap << " RoIs... event accepted" << endmsg;
                    }
                    // monitored variables
                    m_CluNum = numberRoI;
                    m_NumJet = numberJet;
                    m_NumTrk = numberTrk; //set these equal to actual cluster parameters 
                    result = true;
                }
            } else {
                if(msgLvl() <= MSG::DEBUG) {
                    msg() << MSG::DEBUG
                            << "Cluster does not satisfy all the conditions... event not accepted"
                            << endmsg;
                }
                result = false;		   
            }
        } else {
            if(msgLvl() <= MSG::DEBUG) {
                msg() << MSG::DEBUG
                        << "Cluster does not satisfy all the conditions... event not accepted"
                        << endmsg;
            }
            result = false;	    	  
        }

        //Store result
        pass = result;

        return HLT::OK;
    }
}
