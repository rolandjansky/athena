/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTXAODBphysMonTool.cxx
 *
 *
 *
 *
 *    @author name (uid@cern.ch)
 *    Based on HLTMonTool example by Christiane Risler and Martin zur Nedden
 */

// Gaudi and Athena
#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "TrkParametersBase/ParametersBase.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"

// ROOT
#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TVector3.h"

// C++
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <map>
#include <cmath>
#include <algorithm>
#include <iomanip>

// Jpsi's
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODMuon/Muon.h"

// TrigBphysMonitoring
#include "TrigBphysMonitoring/HLTXAODBphysMonTool.h"

// TrigBphysContainers
#include "TrigParticle/TrigL2BphysContainer.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"
#include "TrigParticle/TrigEFBphys.h"

#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphys.h"
// Muon Containers
#include "TrigMuonEvent/TrigMuonEF.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"

// Vertex
#include "VxVertex/VxContainer.h"

// Tracks
#include "TrkParameters/TrackParameters.h"

// For GetKalmanUpdator
#include "GaudiKernel/ListItem.h"

// TriggerDecisionTool
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"

// For JpsiFinder
//#include "TrigBphysMonitoring/BCompositeParticleBuilder.h"
//#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"

using namespace std;

/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection
 *  of the filepath for histograms, the first and second plane
 *  numbers to be used, and the timebin.
 */
HLTXAODBphysMonTool::HLTXAODBphysMonTool(const std::string & type,
                                         const std::string & name,
                                         const IInterface* parent)
:IHLTMonTool(type, name, parent),
m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
m_muonMass(105.66)
{
    
   // m_bphysItems.push_back("HLT_2mu6_bDimu");
    declareProperty("muonMass"          , m_muonMass);
    declareProperty("TrigNames"         , m_TrigNames);
    declareProperty("TrigNames1D"       , m_TrigNames1D);
    declareProperty("TrigDecisionTool"  , m_trigDec, "The tool to access TrigDecision");
    declareProperty("BphysicsItems"     , m_bphysItems, "Bphysics triggers to test");
    
    declareProperty("JpsiCandidates"    , m_JpsiCandidatesKey = "JpsiCandidates","Offline di-muon candidates");

}



//-----------------------------------------------------------
HLTXAODBphysMonTool::~HLTXAODBphysMonTool()
//-----------------------------------------------------------
{
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::init()
//-----------------------------------------------------------
{
    
    ATH_MSG_INFO ("Initializing... ");
    
    *m_log << MSG::INFO << "init being called" << endreq;
    
    m_all   = getTDT()->getChainGroup(".*");
    
    
    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------
void HLTXAODBphysMonTool::buildHist( const std::string & trigname,const std::string & prefix) // suppress 'unused' compiler warning
//-----------------------------------------------------------
{
    
   
        // build...(bphysItem.c_str(), "TrigBphys");
        addHistogram( new TH1F(Form("%s_%s_eta",prefix.c_str(),trigname.c_str()) ,    Form("%s_%s_eta;#eta(#mu#mu);Candidates",prefix.c_str(),trigname.c_str()) ,     60, -3.,3.) );
        addHistogram( new TH1F(Form("%s_%s_phi",prefix.c_str(),trigname.c_str()) ,     Form("%s_%s_phi;#phi(#mu#mu);Candidates",prefix.c_str(),trigname.c_str()) ,     70, -3.5,3.5) );
        //addHistogram( new TH1F(Form("TrigBphys_%s_pTmu1",trigname()) ,  Form("Bphys %s pTmu1",bphysItem.c_str()) ,    50, 0.,50.) );
        //addHistogram( new TH1F(Form("TrigBphys_%s_pTmu2",trigname()) ,  Form("Bphys %s pTmu2",bphysItem.c_str()) ,    50, 0.,50.) );
        addHistogram( new TH1F(Form("%s_%s_mass",prefix.c_str(),trigname.c_str()) ,   Form("%s_%s_mass;mass(#mu#mu)[GeV];Candidates",prefix.c_str(),trigname.c_str()) ,    100, 0.,13000.) );
        addHistogram( new TH1F(Form("%s_%s_fitmass",prefix.c_str(),trigname.c_str()) , Form("%s_%s_fitmass;fitmass(#mu#mu)[GeV];Candidates",prefix.c_str(),trigname.c_str()) ,    100, 0.,13000.) );
        addHistogram( new TH1F(Form("%s_%s_fitchi2",prefix.c_str(),trigname.c_str()) , Form("%s_%s_fitchi2;#chi^{2}(#mu#mu);Candidates",prefix.c_str(),trigname.c_str()) ,    50, 0.,10.) );
        //addHistogram( new TH1F(Form("TrigBphys_%s_dphi",trigname()) ,    Form("Bphys %s dphi",bphysItem.c_str()) ,     64, 0.,3.2) );
        //addHistogram( new TH1F(Form("TrigBphys_%s_deta",trigname()) ,    Form("Bphys %s deta",bphysItem.c_str()) ,     60, 0.,3.) );
        //addHistogram( new TH1F(Form("TrigBphys_%s_pTsum",trigname()) ,   Form("Bphys %s pTsum",bphysItem.c_str()) ,    50, 0.,50.) );

    
   // return StatusCode::SUCCESS;
}


void HLTXAODBphysMonTool::fillHist(const xAOD::TrigBphys *bphysItem, const std::string & trigItem,const std::string & prefix) // suppress 'unused' compiler warning
//-----------------------------------------------------------
{
    
    
    float eta = bphysItem->eta();
    float phi = bphysItem->phi();
    float mass = bphysItem->mass();
    float fitmass = bphysItem->fitmass();
    float fitchi2 = bphysItem->fitchi2();
    
    
    // Dimu
    hist(Form("%s_%s_eta",prefix.c_str(),trigItem.c_str()))->Fill(eta);
    hist(Form("%s_%s_phi",prefix.c_str(),trigItem.c_str()))->Fill(phi);
    hist(Form("%s_%s_mass",prefix.c_str(),trigItem.c_str()))->Fill(mass);
    hist(Form("%s_%s_fitmass",prefix.c_str(),trigItem.c_str()))->Fill(fitmass);
    if (fitchi2 != -99) hist(Form("%s_%s_fitchi2",prefix.c_str(),trigItem.c_str()))->Fill(fitchi2);
    
    
    
    const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = bphysItem->trackParticleLinks();
    if (trackVector.size() != 0) {
        ATH_MSG_INFO(" REGTEST got track vector size: " << trackVector.size() );
    } else {
        ATH_MSG_INFO(" REGTEST no track vector!!! " );
    }
    
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt=trackVector.begin();
    for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
        if (!(trkIt->isValid())) {
            ATH_MSG_WARNING("TrackParticleContainer::Invalid ElementLink to track ");
            continue;
        }
        //const Trk::Perigee* trackPerigee=(*(*trkIt))->measuredPerigee();
        const Trk::Perigee* trackPerigee=&((*(*trkIt))->perigeeParameters());
        
        //      msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endreq;
        double phiTrk = trackPerigee->parameters()[Trk::phi];
        double thetaTrk = trackPerigee->parameters()[Trk::theta];
        double pxTrk = trackPerigee->momentum()[Trk::px];
        double pyTrk = trackPerigee->momentum()[Trk::py];
        double ptTrk = sqrt(pxTrk*pxTrk + pyTrk*pyTrk);
        double etaTrk = -std::log(tan(thetaTrk/2));
        
        ATH_MSG_INFO("track " << itrk << " pt phi eta " << ptTrk << " " <<
                     phiTrk << " " << etaTrk);
    }

}


//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::book() // suppress 'unused' compiler warning
//-----------------------------------------------------------
{
    
    ATH_MSG_INFO ("Booking... ");
    
    
    // 2mu6_DiMu algorithms
    addMonGroup(new MonGroup(this,"HLT/BphysMon/DiMu",run));

    for(auto bphysItem : m_bphysItems) {
     
        buildHist(bphysItem);
    }
    
     addMonGroup(new MonGroup(this,"HLT/BphysMon/offlinemumu",run));
    // build...(bphysItem.c_str(), "TrigBphys");
    addHistogram( new TH1F(Form("mumu_offline_eta") ,    Form("mumu_offline_eta;#eta(#mu#mu);Candidates") ,     60, -3.,3.) );
    addHistogram( new TH1F(Form("mumu_offline_phi") ,     Form("mumu_offline_phi;#phi(#mu#mu);Candidates") ,     70, -3.5,3.5) );
    //addHistogram( new TH1F(Form("TrigBphys_%s_pTmu1",trigname()) ,  Form("Bphys %s pTmu1",bphysItem.c_str()) ,    50, 0.,50.) );
    //addHistogram( new TH1F(Form("TrigBphys_%s_pTmu2",trigname()) ,  Form("Bphys %s pTmu2",bphysItem.c_str()) ,    50, 0.,50.) );
    addHistogram( new TH1F(Form("mumu_offline_mass") ,   Form("mumu_offline_mass;mass(#mu#mu)[GeV];Candidates") ,    100, 0.0,9000.0) );
    addHistogram( new TH1F(Form("mumu_offline_fitmass") , Form("mumu_offline_fitmass;fitmass(#mu#mu)[GeV];Candidates") ,    100, 0.0,9000.0) );
    
    //addHistogram( new TH1F(Form("TrigBphys_%s_dphi",trigname()) ,    Form("Bphys %s dphi",bphysItem.c_str()) ,     64, 0.,3.2) );
    //addHistogram( new TH1F(Form("TrigBphys_%s_deta",trigname()) ,    Form("Bphys %s deta",bphysItem.c_str()) ,     60, 0.,3.) );
    //addHistogram( new TH1F(Form("TrigBphys_%s_pTsum",trigname()) ,   Form("Bphys %s pTsum",bphysItem.c_str()) ,    50, 0.,50.) );
    

    
    
    
    
    
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::fill()
//-----------------------------------------------------------
{
    ATH_MSG_INFO ("Filling... ");
    
    *m_log << MSG::DEBUG << " ====== Begin fillHists() ====== " << endreq;
    
    StatusCode sc;
    
    sc = test();
    if( sc.isFailure() ) *m_log << MSG::ERROR << "Method test failed " << endreq;
    
    
    for(auto bphysItem : m_bphysItems) {
        sc = testTrig(bphysItem);
        if ( sc.isFailure() ) {
            ATH_MSG_ERROR ("Could not finish testTrig test for chain " << bphysItem);
            // return sc; try not to return for other tests to run
        }
    }
    
    sc = JpsiFinder();
    if( sc.isFailure() )  ATH_MSG_ERROR ("Method JpsiFinder failed ");
    
    
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::test()
//-----------------------------------------------------------
{
    ATH_MSG_INFO ("Testing... ");
    
    setCurrentMonGroup("HLT/BphysMon/offlinemumu");
    
    const xAOD::TrigBphysContainer*  trigEFBphys;
    StatusCode sc = m_storeGate->retrieve(trigEFBphys,"HLT_xAOD__TrigBphysContainer_EFBMuMuFex");
    
    if (sc.isFailure()) {
        ATH_MSG_INFO ("  No TrigEFBphysContainer found with tag: HLT_xAOD__TrigBphysContainer_EFBMuMuFex");
        
    } else{
        
        xAOD::TrigBphysContainer::const_iterator EFBphysItr  = trigEFBphys->begin();
        xAOD::TrigBphysContainer::const_iterator EFBphysItrE = trigEFBphys->end();
        
        
        for (int j=0; EFBphysItr != EFBphysItrE; ++EFBphysItr, ++j ) {
            
            ATH_MSG_INFO ("  Looking at TrigEFBphys " << j );
            
            ATH_MSG_INFO (Form("  TrigEFBphys->eta() returns %g",(*EFBphysItr)->eta() ));
            ATH_MSG_INFO (Form("  TrigEFBphys->phi() returns %g",(*EFBphysItr)->phi() ));
            ATH_MSG_INFO (Form("  TrigEFBphys->mass() returns %g",(*EFBphysItr)->mass() ));
            ATH_MSG_INFO (Form("  TrigEFBphys->fitmass() returns %g",(*EFBphysItr)->fitmass() ));
            //      mLog <<MSG::INFO << "  TrigEFBphys->isValid() returns " << (*EFBphysItr)->isValid() << endreq;
            ATH_MSG_INFO (Form("  TrigEFBphys->roiId() returns %d",(*EFBphysItr)->roiId() ));
            ATH_MSG_INFO (Form("  TrigEFBphys->particleType() returns %d",(*EFBphysItr)->particleType() ));
            
            
            // Dimu
            hist(Form("mumu_offline_eta"))->Fill((*EFBphysItr)->eta());
            hist(Form("mumu_offline_phi"))->Fill((*EFBphysItr)->phi());
            hist(Form("mumu_offline_mass"))->Fill((*EFBphysItr)->mass());
            hist(Form("mumu_offline_fitmass"))->Fill((*EFBphysItr)->fitmass());
            
    
            
            ////////
            /*
             if( (*EFBphysItr)->secondaryDecay() != NULL){
             const xAOD::TrigBphys * psecond =(*EFBphysItr)->secondaryDecay();
             mLog <<MSG::INFO << "  Secondary decay info: " << endreq;
             mLog <<MSG::INFO << "  pSecondDecay->eta() returns " << psecond->eta() << endreq;
             mLog <<MSG::INFO << "  pSecondDecay->phi() returns " << psecond->phi() << endreq;
             mLog <<MSG::INFO << "  pSecondDecay->mass() returns " << psecond->mass() << endreq;
             mLog <<MSG::INFO << "  pSecondDecay->fitmass() returns " << psecond->fitmass() << endreq;
             //	mLog <<MSG::INFO << "  pSecondDecay->isValid() returns " << (*EFBphysItr)->secondaryDecayLink()->isValid() << endreq;
             mLog <<MSG::INFO << "  pSecondDecay->roiId() returns " << psecond->roiId() << endreq;
             mLog <<MSG::INFO << "  pSecondDecay->particleType() returns " << psecond->particleType() << endreq;
             
             } // end if secondary exists
             
             
             
             const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = (*EFBphysItr)->trackParticleLinks();
             if (trackVector.size() != 0) {
             mLog << MSG::INFO << "   got track vector size: " << trackVector.size() << endreq;
             } else {
             mLog << MSG::INFO << "   no track vector!!! "  << endreq;
             }
             std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt=trackVector.begin();
             for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
             if (!(trkIt->isValid())) {
             mLog << MSG::WARNING << "TrackParticleContainer::Invalid ElementLink to track " << endreq;
             continue;
             }
             //const Trk::Perigee* trackPerigee=(*(*trkIt))->measuredPerigee();
             const Trk::Perigee* trackPerigee=&((*(*trkIt))->perigeeParameters());
             
             //      msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endreq;
             double phi = trackPerigee->parameters()[Trk::phi];
             double theta = trackPerigee->parameters()[Trk::theta];
             double px = trackPerigee->momentum()[Trk::px];
             double py = trackPerigee->momentum()[Trk::py];
             double pt = sqrt(px*px + py*py);
             double eta = -std::log(tan(theta/2));
             
             mLog << MSG::INFO << "track " << itrk << " pt phi eta " << pt << " " <<
             phi << " " << eta << endreq;
             }
             */
            ////////
        }
    }
    
    
    
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::testTrig(const std::string& trigItem)
//-----------------------------------------------------------
{
    msg(MSG::INFO) << "REGTEST ==========START of Bphysics EDM/Navigation check for chain " << trigItem << " ===========" << endreq;
    
    setCurrentMonGroup("HLT/BphysMon/DiMu");
    
    ATH_MSG_INFO("Chain passed = " << m_trigDec->isPassed(trigItem));
    
    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    
    const std::vector< Trig::Feature<xAOD::TrigBphysContainer> > fc_bphys = fc.get<xAOD::TrigBphysContainer>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::TrigBphysContainer> > = " << fc_bphys.size());
    
    for( auto cont_bphys : fc_bphys ) {
        ATH_MSG_INFO("REGTEST Got Bphysics container, size = " << cont_bphys.cptr()->size());
        for ( auto bphys:  *(cont_bphys.cptr()) )  {
            fillHist(bphys,trigItem);
        }
    }
    
    
    // loop over bphys features
    
    //    const std::vector< Trig::Feature<CombinedMuonFeature> > vec_cbmufeats = fc.get<CombinedMuonFeature>();
    //    ATH_MSG_INFO("Size of vector< Trig::Feature<CombinedMuonFeature> > = " << vec_cbmufeats.size());
    //
    //    for( auto cbmufeat : vec_cbmufeats) {
    //        ATH_MSG_INFO("REGTEST CombinedMuonFeature with pt, eta, phi = " << cbmufeat.cptr()->pt() << ", " << cbmufeat.cptr()->eta() << ", " << cbmufeat.cptr()->phi());
    //    }
    //
    msg(MSG::INFO) << "REGTEST ==========END of Bphysics EDM/Navigation check for chain " << trigItem << " ===========" << endreq;
    
    
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::proc()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------
StatusCode HLTXAODBphysMonTool::JpsiFinder()
//---------------------------------------------------------
{
    ATH_MSG_INFO("JpsiFinder run here when completed" );
    /*
    ATH_MSG_DEBUG("in JpsiFinder()");
    const xAOD::VertexContainer*    jpsiContainer(0);
    const xAOD::VertexAuxContainer* jpsiAuxContainer(0);
    StatusCode sc  = evtStore()->retrieve(jpsiContainer   , m_JpsiCandidatesKey);
    StatusCode sc2 = evtStore()->retrieve(jpsiAuxContainer, m_JpsiCandidatesKey+"Aux.");
    if (sc.isFailure() || !jpsiContainer || sc2.isFailure() || !jpsiAuxContainer) {
        ATH_MSG_ERROR("No Jpsi Container Found, skipping event");
        return StatusCode::RECOVERABLE;
    } else {
        ATH_MSG_DEBUG("Obtained jpsiContainer");
    }
    ATH_MSG_DEBUG("number of jpsi candidates " << jpsiContainer->size());

    // Extract information from the Jpsi candidates
    for ( xAOD::VertexContainer::const_iterator vxcItr = jpsiContainer->begin() ; vxcItr != jpsiContainer->end() ; vxcItr++ ) {
        const xAOD::Vertex* jpsiCandidate = (*vxcItr);
        if (!jpsiCandidate) continue;
        // to see how to access variables; refer to:
        // https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/JpsiUpsilonTools/trunk/src/JpsiExample.cxx
        
        
        // refitted track parameters
        TLorentzVector refTrk1 = track4Momentum(jpsiCandidate, 0, m_muonMass);
        TLorentzVector refTrk2 = track4Momentum(jpsiCandidate, 1, m_muonMass);

        TLorentzVector origTrk1 = origTrack4Momentum(jpsiCandidate, 0, m_muonMass);
        TLorentzVector origTrk2 = origTrack4Momentum(jpsiCandidate, 1, m_muonMass);

        
        // Calculate the invariant masses and their errors
        double orig_mass = (origTrk1+origTrk2).M();                      // mass from original tracks
        double mass      = (refTrk1+refTrk2).M();                        // mass from refitted tracks
        double error     = invariantMassError(jpsiCandidate, std::vector<double>(2, m_muonMass));  // invariant mass error
        
        ATH_MSG_INFO("JpsiFinder: Original mass: " << orig_mass << ", refitted mass: " << mass << ", error: " <<
                     error);
        
        
    } // loop over candidates
    
    */
    return StatusCode::SUCCESS;
     
     
}



//---------------------------------------------------------
StatusCode HLTXAODBphysMonTool::Triggers()
//---------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2DsPhiPiMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2TopoDiMuMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2TopoDiMuXMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2DiMuMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2DiMuXMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2BJpsieeMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::EFDsPhiPiMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::EFTopoDiMuMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::EFDiMuMon()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2Tracks()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::EFTracks()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::L2BJpsimumu()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::EFBJpsimumu()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::CompareDiMuL2EF()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------
StatusCode HLTXAODBphysMonTool::CompareTopoDiMuL2EF()
//-----------------------------------------------------------
{
    return StatusCode::SUCCESS;
}

void HLTXAODBphysMonTool::divide(TH1 *num, TH1 *den, TH1 *quo) {
    
    quo->Divide(num, den, 1., 1., "b");
}



// *********************************************************************************
// ---------------------------------------------------------------------------------
// trackMomentum: returns refitted track momentum
// ---------------------------------------------------------------------------------

TVector3 HLTXAODBphysMonTool::trackMomentum(const xAOD::Vertex * vxCandidate, uint trkIndex) const
{
    float px = 0., py = 0., pz = 0.;
    //  assert(vxCandidate!=0);
    //  assert(i<vxCandidate->vxTrackAtVertex().size());
    
    //  if (0 != vxCandidate) {
    //    const Trk::TrackParameters* aPerigee = vxCandidate->vxTrackAtVertex()[trkIndex].perigeeAtVertex();
    //    px = aPerigee->momentum()[Trk::px];
    //    py = aPerigee->momentum()[Trk::py];
    //    pz = aPerigee->momentum()[Trk::pz];
    //  }
    
    static SG::AuxElement::Accessor< std::vector<float> > refTrackPxAcc("RefTrackPx");
    static SG::AuxElement::Accessor< std::vector<float> > refTrackPyAcc("RefTrackPy");
    static SG::AuxElement::Accessor< std::vector<float> > refTrackPzAcc("RefTrackPz");
    const std::vector<float>& refTrackPx = refTrackPxAcc(*vxCandidate);
    const std::vector<float>& refTrackPy = refTrackPyAcc(*vxCandidate);
    const std::vector<float>& refTrackPz = refTrackPzAcc(*vxCandidate);
    
    if(trkIndex < refTrackPx.size() && refTrackPx.size() == refTrackPy.size() && refTrackPz.size()) {
        px = refTrackPx[trkIndex];
        py = refTrackPy[trkIndex];
        pz = refTrackPz[trkIndex];
    }
    
    TVector3 mom(px,py,pz);
    return mom;
}

// ---------------------------------------------------------------------------------
// track4Momentum: returns refitted track 4-momentum given mass hypothesis
// ---------------------------------------------------------------------------------

TLorentzVector HLTXAODBphysMonTool::track4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const
{
    TVector3 mom = trackMomentum(vxCandidate, trkIndex);
    TLorentzVector lorentz;
    lorentz.SetVectM(mom, mass);
    return lorentz;
}


// ---------------------------------------------------------------------------------
// origTrackMomentum: returns original track momentum
// ---------------------------------------------------------------------------------

TVector3 HLTXAODBphysMonTool::origTrackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const
{
    TVector3 mom;
    const xAOD::TrackParticle* origTrack = vxCandidate->trackParticle(trkIndex);
    if(origTrack==NULL) {
        return mom;
    }
    
    mom.SetPtEtaPhi(origTrack->pt(), origTrack->eta(), origTrack->phi());
    return mom;
}

// ---------------------------------------------------------------------------------
// origTrack4Momentum: returns original track 4-momentum given mass hypothesis
// ---------------------------------------------------------------------------------

TLorentzVector HLTXAODBphysMonTool::origTrack4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const
{
    TVector3 mom = origTrackMomentum(vxCandidate, trkIndex);
    TLorentzVector lorentz;
    lorentz.SetVectM(mom, mass);
    return lorentz;
}

// ---------------------------------------------------------------------------------
// invariantMassError: returns invariant mass error
// ---------------------------------------------------------------------------------

double HLTXAODBphysMonTool::invariantMassError(const xAOD::Vertex* vxCandidate, std::vector<double> masses) const
{
    uint NTrk = vxCandidate->vxTrackAtVertex().size();
    if (masses.size() != NTrk) {
        ATH_MSG_WARNING("The provided number of masses does not match the number of tracks in the vertex");
        return -999999.;
    }
    
    uint ndim    = vxCandidate->covariance().size();
    uint ndimExp = (3*NTrk+3)*(3*NTrk+3+1)/2;
    if (ndim == ndimExp) {
        return massErrorVKalVrt(vxCandidate,masses);
    } else {
        ATH_MSG_WARNING("Unknown covariance matrix dimension: " << ndim << ", expected: " << ndimExp);
        return -999999.;
    }
}

// ---------------------------------------------------------------------------------
// massErrorVKalVrt: returns invariant mass error for vertex created by VKalVrtFitter
// ---------------------------------------------------------------------------------

double HLTXAODBphysMonTool::massErrorVKalVrt(const xAOD::Vertex * vxCandidate, std::vector<double> masses) const
{
    unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
    
    // construct the full covariance matrix
    Amg::MatrixX* fullCov = convertVKalCovMatrix(NTrk, vxCandidate->covariance());
    
    std::vector<CLHEP::HepLorentzVector> particleMom(NTrk);
    std::vector<CLHEP::HepMatrix> particleDeriv(NTrk);
    CLHEP::HepLorentzVector totalMom;
    CLHEP::HepMatrix tmpDeriv(3,3);
    
    double phi=0.,theta=0.,invP=0.;
    for( unsigned int it=0; it<NTrk; it++){
        const Trk::TrackParameters*  bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
        phi   =  bPer->parameters()[Trk::phi];
        theta =  bPer->parameters()[Trk::theta];
        invP  =  bPer->parameters()[Trk::qOverP];
        CLHEP::HepLorentzVector tmp( cos(phi)*sin(theta)/fabs(invP),
                                    sin(phi)*sin(theta)/fabs(invP),
                                    cos(theta)/fabs(invP) );
        double esq = tmp.px()*tmp.px() + tmp.py()*tmp.py() + tmp.pz()*tmp.pz() + masses[it]*masses[it];
        double e = (esq>0.) ? sqrt(esq) : 0.;
        tmp.setE(e);
        particleMom[it] = tmp;
        totalMom += tmp;
        
        //  d(Px,Py,Pz)/d(Phi,Theta,InvP)
        tmpDeriv(1,1) = - tmp.py();
        tmpDeriv(2,1) =   tmp.px();
        tmpDeriv(3,1) =   0.;
        tmpDeriv(1,2) =   cos(phi) * tmp.pz();
        tmpDeriv(2,2) =   sin(phi) * tmp.pz();
        tmpDeriv(3,2) = - sin(theta)/fabs(invP);
        tmpDeriv(1,3) = - tmp.px()/invP;
        tmpDeriv(2,3) = - tmp.py()/invP;
        tmpDeriv(3,3) = - tmp.pz()/invP;
        particleDeriv[it] = tmpDeriv;
    }
    
    double dMdPx=0., dMdPy=0., dMdPz=0., dMdPhi=0., dMdTheta=0., dMdInvP=0.;
    std::vector<double> Deriv(3*NTrk+3, 0.);
    for(unsigned int it=0; it<NTrk; it++){
        dMdPx = ( totalMom.e() * particleMom[it].px()/particleMom[it].e() - totalMom.px() ) / totalMom.m();
        dMdPy = ( totalMom.e() * particleMom[it].py()/particleMom[it].e() - totalMom.py() ) / totalMom.m();
        dMdPz = ( totalMom.e() * particleMom[it].pz()/particleMom[it].e() - totalMom.pz() ) / totalMom.m();
        
        dMdPhi   = dMdPx*particleDeriv[it](1,1) + dMdPy*particleDeriv[it](2,1) + dMdPz*particleDeriv[it](3,1);
        dMdTheta = dMdPx*particleDeriv[it](1,2) + dMdPy*particleDeriv[it](2,2) + dMdPz*particleDeriv[it](3,2);
        dMdInvP  = dMdPx*particleDeriv[it](1,3) + dMdPy*particleDeriv[it](2,3) + dMdPz*particleDeriv[it](3,3);
        
        Deriv[3*it + 3 + 0] = dMdPhi;    Deriv[3*it + 3 + 1] = dMdTheta; Deriv[3*it + 3 + 2] = dMdInvP;
    }
    
    double err = 0;
    for(unsigned int i=0; i<3*NTrk+3; i++){
        for(unsigned int j=0; j<3*NTrk+3; j++){
            err += Deriv[i]*( (*fullCov)(i,j))*Deriv[j];
        }
    }
    
    // cleanup
    delete fullCov;
    
    return (err>0.) ? sqrt(err) : 0.;
}

// ---------------------------------------------------------------------------------
// trackCharge: returns charge of the i-th track
// ---------------------------------------------------------------------------------

double HLTXAODBphysMonTool::trackCharge(const xAOD::Vertex * vxCandidate, int i) const
{
    if(vxCandidate!=NULL && vxCandidate->trackParticle(i)!=NULL) {
        return vxCandidate->trackParticle(i)->charge();
    }
    
    return -999999;
}

// ---------------------------------------------------------------------------------
// convertVKalCovMatrix: converts xAOD covariance matrix (i.e. std::vector<float>)
//                       into an actual matrix
// ---------------------------------------------------------------------------------

Amg::MatrixX * HLTXAODBphysMonTool::convertVKalCovMatrix(int NTrk, const std::vector<float> & Matrix) const
{
    // stolen from VKalVrtFitter    
    Amg::MatrixX * mtx = new Amg::MatrixX(3+3*NTrk,3+3*NTrk);
    long int ij=0;
    for(int i=1; i<=(3+3*NTrk); i++){
        for(int j=1; j<=i; j++){
            if(i==j) { 
                (*mtx)(i-1,j-1)=Matrix[ij];
            }else{ 
                (*mtx).fillSymmetric(i-1,j-1,Matrix[ij]);
            }
            ij++;
        }
    }
    
    return mtx;
}



