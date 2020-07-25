/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TagAndProbeExAlg.cxx 785295 2016-11-19 03:18:52Z ssnyder $
// Updated to xAOD for Trigger Tutorial
//
// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/Units.h"

// Trigger EDM object(s):
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODBase/IParticle.h"
#include "FourMomUtils/xAODP4Helpers.h"
// Local include(s):
#include "TagAndProbeExAlg.h"

using Athena::Units::GeV;

TagAndProbeExAlg::TagAndProbeExAlg( const std::string& name, ISvcLocator *pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ),
    m_eventNr( 0 ),
    m_flavor(""),
    m_objType(xAOD::Type::Particle),
    m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
    m_matchTool( "Trig::MatchingTool/MatchingTool",this),
    m_tah( "Trig::TriggerAnalysisHelper/TriggerAnalysisHelper",this ),
    m_histSvc( "THistSvc", name ),
    m_numTaggedEvents( 0 ),
    m_h_triggerAccepts( nullptr ),
    m_h_triggerAcceptsRaw( nullptr ),
    m_h_triggerPrescaled( nullptr ),
    m_h_emulationAccepts( nullptr ),
    m_h_zMass( nullptr )
{
        // job option configurable properties
        declareProperty("Flavor",m_flavor);
        declareProperty( "L1TriggerList", m_l1chainList);
        declareProperty( "HLTTriggerList", m_hltchainList);
        declareProperty( "dRMax", m_dRMax=0.1);
        declareProperty( "PtCut", m_ptCut = 20. );
        declareProperty( "Chi2Cut", m_chi2Cut = 100. );
        declareProperty( "MassMin", m_massMin = 70. );
        declareProperty( "MassMax", m_massMax = 110. );
        declareProperty( "EtaMax", m_etaMax = 2.4 );
}

StatusCode TagAndProbeExAlg::initialize() {

   CHECK( m_trigDec.retrieve() );
   CHECK( m_matchTool.retrieve() );
   CHECK( m_tah.retrieve() );
   CHECK( m_histSvc.retrieve() );
   m_trigDec->ExperimentalAndExpertMethods()->enable();
   if(m_flavor=="Electron") m_objType = xAOD::Type::Electron;
   else if(m_flavor=="Muon") m_objType = xAOD::Type::Muon;
   else if(m_flavor=="Tau") m_objType = xAOD::Type::Tau;
   else {
       ATH_MSG_ERROR("Flavor not known, initialization failed");
       return StatusCode::FAILURE;
   }
   
   //Setup histograms for trigger decision and prescale
   const int nTrigger = (int) m_hltchainList.size();
   m_h_triggerAccepts = new TH1F( "TriggerAccepts", "TriggerAccepts", nTrigger, 0,  nTrigger);
   m_h_emulationAccepts = new TH1F( "EmulationAccepts", "EmulationAccepts", nTrigger, 0,  nTrigger);
   m_h_triggerAcceptsRaw = new TH1F( "TriggerAcceptsRaw", "TriggerAcceptsRaw", nTrigger, 0, nTrigger );
   m_h_triggerPrescaled = new TH1F( "TriggerPrescaled", "TriggerPrescaled", nTrigger, 0, nTrigger );
   if ( ! m_hltchainList.empty() ){
       for ( int i = 0; i < std::min( (int)m_hltchainList.size(), (int)m_h_triggerAccepts->GetNbinsX() ); ++i ) {
           int bin = i+1;
           m_h_triggerAccepts->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           m_h_emulationAccepts->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           m_h_triggerAcceptsRaw->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           m_h_triggerPrescaled->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           ATH_MSG_INFO("setting label X" <<  m_hltchainList[i] << " for bin " << bin);
       }
   }
   m_h_zMass = new TH1F( "ZMass", "m(Z)", 30, 70, 110 );
   CHECK( m_histSvc->regHist( "/Trigger/TagAndProbe/TriggerAccepts", m_h_triggerAccepts ) );
   CHECK( m_histSvc->regHist( "/Trigger/TagAndProbe/EmulationAccepts", m_h_emulationAccepts ) );
   CHECK( m_histSvc->regHist( "/Trigger/TagAndProbe/TriggerAcceptsRaw", m_h_triggerAcceptsRaw ) );
   CHECK( m_histSvc->regHist( "/Trigger/TagAndProbe/TriggerPrescaled", m_h_triggerPrescaled ) );
   CHECK( m_histSvc->regHist( "/Trigger/TagAndProbe/ZMass", m_h_zMass ) );
   for(const std::string& chain:m_hltchainList){
       m_numSelectedEvents[chain]=0;
       m_numHLTPassedEvents[chain]=0;
       std::string histName="Eff_et_"+chain;
       m_h_eff_et[chain] = new TProfile( histName.c_str(), "#epsilon(Et)", 20, 0., 100. );
       CHECK( m_histSvc->regHist( "/Trigger/TagAndProbe/"+histName, m_h_eff_et[chain] ) );
       histName="Eff_eta_"+chain;
       m_h_eff_eta[chain] = new TProfile( histName.c_str(), "#epsilon(Et)", 20, (-1.*m_etaMax), m_etaMax );
       CHECK( m_histSvc->regHist( "/Trigger/TagAndProbe/"+histName, m_h_eff_eta[chain] ) );
   }
   for(const std::string& chain:m_hltchainList){
       m_numSelectedEvents[chain]=0;
       m_numL1PassedEvents[chain]=0;
   }
   
   return StatusCode::SUCCESS;
}

StatusCode TagAndProbeExAlg::execute() {

    ++m_eventNr;
   ATH_MSG_INFO( "In TagAndProbeExAlg::Execute()");

   CHECK(collectTriggerStatistics());
   // collect the first two muons which pass our muon selection
   if(m_objType==xAOD::Type::Muon){
       const xAOD::MuonContainer *muonContainer = 0;
       CHECK( evtStore()->retrieve( muonContainer, "Muons" ) );
       ATH_MSG_INFO("Select Z events");
       CHECK( TriggerAnalysis( muonContainer ) );
   }
  
   if(m_objType==xAOD::Type::Electron){
       const xAOD::ElectronContainer *electronContainer = 0;
       CHECK( evtStore()->retrieve( electronContainer, "Electrons" ) );    
       CHECK( TriggerAnalysis( electronContainer ) );
   }
  
   CHECK(EmulationAnalysis());
   return StatusCode::SUCCESS;
}


StatusCode TagAndProbeExAlg::finalize() {

   // Here we can print out all the information we've learned
   // We need to be a little clever in how we interpret the information
   // presented here.  In particular, remember that each muon has a chance
   // to act as the tag muon, so for chains with very high efficiency
   // its not unreasonable to get a number of tag muons greater than the
   // number of selected events.
   REPORT_MESSAGE( MSG::DEBUG ) << "TagAndProbeExAlg::finalize()";

   ATH_MSG_INFO( "" );
   ATH_MSG_INFO( "TagAndProbeExAlg results" );
   ATH_MSG_INFO( "L1 Chain name: " << m_l1chainList );
   ATH_MSG_INFO( "HLT Chain name: " << m_hltchainList );
   ATH_MSG_INFO( "Selection cuts: " );
   ATH_MSG_INFO( "lepton pT > " << m_ptCut << " GeV" );
   ATH_MSG_INFO( "lepton combined chi2 < " << m_chi2Cut );
   ATH_MSG_INFO( "lepton |eta| < " << m_etaMax );
   ATH_MSG_INFO( m_massMin << " GeV < mZ < " << m_massMax << " GeV" );
   ATH_MSG_INFO( "Number of events with a tag muon: " << m_numTaggedEvents );
   for(const std::string& chain:m_hltchainList){
       ATH_MSG_INFO( " Chain " << chain);
       ATH_MSG_INFO( "Number of selected events: " << m_numSelectedEvents[chain] );
       //ATH_MSG_INFO( "Number of events with a L1 passed matched probe: " << m_numL1PassedEvents[chain] );
       ATH_MSG_INFO( "Number of events with a HLT passed matched probe: " << m_numHLTPassedEvents[chain] );
       ATH_MSG_INFO( " HLT trigger efficiency (relative to Offline): " 
               << (float)m_numHLTPassedEvents[chain]/(float)m_numSelectedEvents[chain] * 100. << "%" );
       //ATH_MSG_INFO( " HLT trigger efficiency (relative to Level 1): " 
       //        << (float)m_numHLTPassedEvents[chain]/(float)m_numL1PassedEvents[chain] * 100. << "%" );
       ATH_MSG_INFO( "" );
   }
   for(const std::string& chain:m_l1chainList){
       ATH_MSG_INFO( " Chain " << chain);
       ATH_MSG_INFO( "Number of events with a L1 passed matched probe: " << m_numL1PassedEvents[chain] );
       ATH_MSG_INFO(" L1 Trigger efficiency (relative to offline): " 
               << (float)m_numL1PassedEvents[chain]/(float)m_numSelectedEvents[chain] * 100. << "%" );
   }

   

   return StatusCode::SUCCESS;
}

StatusCode TagAndProbeExAlg::collectTriggerStatistics() {

   // Now we'd like to collect some trigger statistics for the chains specified 
    for(const auto& chain : m_hltchainList){
        // Get the bits, this tell us more 
        const unsigned int bits = m_trigDec->isPassedBits(chain);
        bool efprescale=bits & TrigDefs::EF_prescaled;
        // bool rerun=bits&TrigDefs::EF_resurrected;
        // bool passThrough=bits&TrigDefs::EF_passThrough;
        bool passedRaw=bits&TrigDefs::EF_passedRaw;

        // What about the L1?
        bool tbp = bits&TrigDefs::L1_isPassedBeforePrescale;
        bool tap = bits&TrigDefs::L1_isPassedAfterPrescale;
        // bool tav = bits&TrigDefs::L1_isPassedAfterVeto;

        // What is the combined prescale result
        bool l1prescale=tbp && !tap;
        bool prescale=efprescale || l1prescale;
        if( m_trigDec->isPassed( chain ) )
            m_h_triggerAccepts->Fill( chain.c_str(), 1 );
        if(prescale)
            m_h_triggerPrescaled->Fill( chain.c_str(), 1 );
        if(passedRaw)
            m_h_triggerAcceptsRaw->Fill( chain.c_str(), 1);
    }

   return StatusCode::SUCCESS;
}

/* ******************************************************************************************
 * TriggerAnalysis for TagAndProbe
 * Does the event fire one of the triggers?
 * Selects pairs of leptons which fall into the Z mass window
 * Input: Offline container of electrons or muons
 * Ouput: Vector of pairs of objects, tag is first, probe is second
 * passHLT: The tag lepton must be matched to a triggered object which fired, see passHLT
 * passObjectSelection: The tag and the probe leptons are required to pass object selection
 * passEvent: Tag and Probe are same lepton flavor, opposite charge, fall in the Z mass window 
 * *******************************************************************************************/
StatusCode TagAndProbeExAlg::TriggerAnalysis (const xAOD::IParticleContainer *cont){
    bool isTriggered=false; 
    for(const auto& chain : m_hltchainList)
        if( m_trigDec->isPassed( chain ) ) isTriggered=true;


    //Event does not trigger, continue to next
    if(!isTriggered) return StatusCode::SUCCESS;
    
    // Select Z candidates
    // Note that we need to be careful in the case that
   // both electrons/muons pass the tag requirements.  In that case
   // we need to consider tag = m1, probe = m2 and
   // tag = m2, probe = m1, or we will bias our measurement
    std::vector<std::pair<const xAOD::IParticle *,const xAOD::IParticle *> > Zcandidates;
    // Loop over container to find the tag particles 
    for(const xAOD::IParticle *tag:*cont){
        // Find suitable tag leptons
        if(!passObjectSelection(*tag)) continue;

        //Tag the event w/ a match to any tag trigger
        if(!passHLT(*tag,m_hltchainList)) continue;
        ++m_numTaggedEvents;

        // Loop over container to find probes
        for(const xAOD::IParticle *probe:*cont){
            // Cannot be a tag
            if(tag==probe)
                continue;
            // Ensure probe passes selection and has opposite charge w/ tag
            if(!passObjectSelection(*probe)) continue;
            // Make a tag and probe pair
            // Select events in Z mass window
            std::pair<const xAOD::IParticle *,const xAOD::IParticle *> Zcand = std::make_pair(tag,probe);
            if(!passEvent(Zcand)) continue;
            Zcandidates.push_back(Zcand);
        } // Find Probes
    } // Find tags

    // Did we find any suitable probes?
    if(Zcandidates.empty()) return StatusCode::SUCCESS;

    // Now perform trigger analysis
    ATH_MSG_INFO("Lepton Trigger efficiency with " << Zcandidates.size() << " Z candidates");
    float mass=0.;
    float probeEt=0.;
    float probeEta=0.;
    for(std::pair<const xAOD::IParticle *,const xAOD::IParticle *> z:Zcandidates){
        mass=DiobjectMass(z);
        probeEta=z.second->eta();
        probeEt=(z.second->e()/cosh(z.second->eta()))/GeV;
        ATH_MSG_INFO("Z Mass " << mass << " Probe et: " << probeEt << " eta: " << probeEta);
        m_h_zMass->Fill(mass);
        for(const std::string& chain:m_l1chainList){
            m_numSelectedEvents[chain]+=1;
            if(passL1(*z.second,chain))
                m_numL1PassedEvents[chain]+=1;
        }
        for(const std::string& chain:m_hltchainList){
            ATH_MSG_INFO("Mathcing to chain " << chain);
            m_numSelectedEvents[chain]+=1;
            // L1 measurement
            if(passHLT(*z.second,chain)){ 
                m_numHLTPassedEvents[chain]+=1;
                m_h_eff_et[chain]->Fill(probeEt,1);
                m_h_eff_eta[chain]->Fill(probeEta,1);
            } 
            else {
                m_h_eff_et[chain]->Fill(probeEt,0);
                m_h_eff_eta[chain]->Fill(probeEta,0);
                // Determine why trigger fails
                if(m_objType == xAOD::Type::Electron){
                    Trig::FeatureContainer f = m_trigDec->features( chain, TrigDefs::alsoDeactivateTEs );
                    m_tah->getFeatureAndTEInfo<xAOD::ElectronContainer>(chain, TrigDefs::alsoDeactivateTEs);
                    m_tah->getSelectedObjects<xAOD::ElectronContainer>(chain, TrigDefs::alsoDeactivateTEs);
                    // Retrieve a vector of Trigger Element (TE) features, i.e. collect all the TEs that contain ElectronContainers
                    std::vector<Trig::Feature<xAOD::ElectronContainer> > vec = f.get<xAOD::ElectronContainer>("egamma_Electrons");
                    // Loop over the features of type ElectornContainer
                    for( const Trig::Feature<xAOD::ElectronContainer>& feat : vec ) {
                        // Use the helper util to retrieve trigger decision at each step in electron trigger sequence 
                        ATH_MSG_INFO( "TagAndProbe L2Calo Active " << m_tah->ancestorPassed<xAOD::TrigEMCluster>(feat.te())
                                << " L2Electron Active " << m_tah->ancestorPassed<xAOD::TrigElectronContainer>(feat.te())
                                << " EFCalo Active " << m_tah->ancestorPassed<xAOD::CaloClusterContainer>(feat.te())
                                << " L1 Active " << m_tah->ancestorPassed<TrigRoiDescriptor>(feat.te()));
                    } // Finish loop over all Electron features
                } // Finish Electron trigger analysis
            } // Finish trigger analysis 
        } // Complete chain analysis
        // Count how many probes pass the trigger
    } // Loop over tags

    return StatusCode::SUCCESS;
}


// Try some simple trigger emulation 
StatusCode TagAndProbeExAlg::EmulationAnalysis(){

    return StatusCode::SUCCESS;
}
// The jist of the algorithm is asking the three following questions:
// (1) Does a given reco'd lepton pass our selection cuts?
//     This is answered in passObjectSelection.
//     For muon object in passMuon.
//     For electron object in passElectron
//
// (2) Given our selected leptons, does the event pass our cuts?
//     This is answered in passEvent
// (3) Does a given reco'd object pass our trigger?
//     This is answered in passL1, passL2, and passEF
// (4) Given our selection of probes, what is the efficiency
//     This is answered in triggerEfficiency

//
// Trigger - Reco matching
//
// The basic question here is, does a given reco'd muon pass a specified
// trigger?
// We answer this by asking, does the muon match to a trigger object
// in the chain of interest, and if so, did that trigger object cause
// the chain to be marked as passed?
// 
// There are a few subtleties in this process:
// (a) What does it mean for a trigger and reco object to match?  If we
//     don't specify anything, this is done by deltaR matching.  However,
//     we could be more creative if we like.  See DistanceFunctor.h for
//     examples of how to write a custom distance measure
// (b) What objects do we match to?  The best way to figure this out is
//     to look at the typedefs in TrigMatchDefs.h in Trigger/TrigAnalysis/
//     TrigObjectMatching package

// Does the reco'd muon pass L1 of the chain we're interested in?
bool TagAndProbeExAlg::passL1( const xAOD::IParticle &recoObj, const std::string chain ) {
    ATH_MSG_INFO("L1Matching: Type " << recoObj.type() << " Chain " << chain);
    //L1 matching, requires custom matching
    double deltaR=999.;
    double deta=0.;
    double dphi=0.;

    // Need to retrieve the L1 TrigRoiDescriptor
    // RoiDescriptor provides the HLT with the initial TriggerElement
    // Each RoIDescriptor has 1 L1 object associated to it
    // Since we retrieve the features w/ TrigRoiDescriptor starting from an HLT chain (not the L1 item)
    // Important to ask for allDeactivateTEs or else we only get L1 information
    // for active HLT chains
    // then, of course, we only want the active L1 elements
    // Due to less granularity of L1 eta, phi need to enlarge the dR conesize
    
    auto fc = (m_trigDec->features(chain,TrigDefs::alsoDeactivateTEs));
    auto initRois = fc.get<TrigRoiDescriptor>();
    if ( initRois.size() < 1 ) {
        ATH_MSG_WARNING("L1Matching: No Rois!");
        return false;
    }

    for(const auto &initRoi:initRois){
        // This is a feature, get TE and check state
        if(!initRoi.te()->getActiveState()){
            ATH_MSG_WARNING("L1Matching: inactive!");
            continue;
        }

        if(recoObj.type()==xAOD::Type::Muon){
            auto itMu = m_trigDec->ancestor<xAOD::MuonRoI>(initRoi);
            const auto *l1 = itMu.cptr();

            deta = fabs(recoObj.eta() - l1->eta());
            dphi = xAOD::P4Helpers::deltaPhi(recoObj,l1->phi());
            deltaR = sqrt(deta*deta + dphi*dphi); 
        }
        else if(recoObj.type()==xAOD::Type::Electron || recoObj.type()==xAOD::Type::Tau){
            auto itEmTau = m_trigDec->ancestor<xAOD::EmTauRoI>(initRoi);
            const auto *l1 = itEmTau.cptr();

            deta = fabs(recoObj.eta() - l1->eta());
            dphi = xAOD::P4Helpers::deltaPhi(recoObj,l1->phi());
            deltaR = sqrt(deta*deta + dphi*dphi); 
        }
        else if(recoObj.type()==xAOD::Type::Jet){
            auto itJet = m_trigDec->ancestor<xAOD::JetRoI>(initRoi);
            const auto *l1 = itJet.cptr();

            deta = fabs(recoObj.eta() - l1->eta());
            dphi = xAOD::P4Helpers::deltaPhi(recoObj,l1->phi());
            deltaR = sqrt(deta*deta + dphi*dphi); 
        }
        else {
            ATH_MSG_WARNING("L1Matching: No L1 object for Reco type");
            return false;
        }
        
        // Due to L1 granularity, need to enlarge dR window
        if(deltaR < (2*m_dRMax))
            return true;
    }

    
    return false;  
}

// Does the reco'd object pass HLT of the chain we're interested in?
// And the reco'd object is matched to a triggered object
bool TagAndProbeExAlg::passHLT( const xAOD::IParticle &recoObj, const std::string chain) {
    return m_matchTool->match(recoObj,chain,m_dRMax);
}

//Just find a match to any trigger in a list or group
bool TagAndProbeExAlg::passHLT( const xAOD::IParticle &recoObj, std::vector<std::string> chainList) {
    bool match=false;
    for(const std::string& chain:chainList){
        if(m_matchTool->match(recoObj,chain,m_dRMax))
            match=true;
    }
    return match;
}
// Determines lepton object type and calls appropiate selection (passElectron, passMuon)
bool TagAndProbeExAlg::passObjectSelection(const xAOD::IParticle &obj){
    
    // Determine the object type: Muon or Electron
    // Apply selecton
    if(obj.type()==xAOD::Type::Muon)
        if(!this->passMuon(static_cast<const xAOD::Muon&>(obj))) return false;
    if(obj.type()==xAOD::Type::Electron)
        if(!this->passElectron(static_cast<const xAOD::Electron&>(obj))) return false;

    return true;
}
// Does the muon pass the reconstruction selection cuts?
// This is just a simple example of selction based on pT,
// match X^2 and eta
bool TagAndProbeExAlg::passMuon( const xAOD::Muon &muon ) {
    ATH_MSG_INFO("Selecting Tag Object");
    float matchChi2=0;
    if(muon.parameter(matchChi2, xAOD::Muon::msInnerMatchChi2)){
        if( muon.pt() / GeV > m_ptCut && muon.muonType()==xAOD::Muon::Combined  && matchChi2 < m_chi2Cut &&
                fabs( muon.eta() ) < m_etaMax )
            return true;
    }

   return false;
}

// Apply electron tag selection
bool TagAndProbeExAlg::passElectron( const xAOD::Electron &electron){
    ATH_MSG_INFO("Selecting Tag Electron");
    //Check constituents
    if(!electron.trackParticle()){
        ATH_MSG_DEBUG("No track Particle");
        return false;
    }
    if(!electron.caloCluster()){
        ATH_MSG_DEBUG("No caloCluster");
        return false;
    }
    ATH_MSG_DEBUG("Selecting Tag Electron PID");
    if (!electron.passSelection("LHTight")) return false;
    //Require Et > 25 GeV
    if( !(electron.e()/cosh(electron.trackParticle()->eta())  > 25.*GeV) ){
        return false;
    }
    //fiducial detector acceptance region
    float absEta = fabs(electron.caloCluster()->etaBE(2));
    if ((absEta > 1.37 && absEta < 1.52) || absEta > 2.47) {
        return false;
    }
    if (!electron.isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) return false;
    ATH_MSG_INFO("Found a tag electron");
    return true;
}

float TagAndProbeExAlg::DiobjectMass(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* > reco ) {
    return (reco.first->p4() + reco.second->p4() ).M() / GeV;
}

// Does the event pass the reconstruction selection cuts?
// This is just a simple example of reconstructing the Z
// boson invariant mass.
bool TagAndProbeExAlg::passEvent( const std::pair< const xAOD::IParticle*, const xAOD::IParticle* > reco ) {

    // Check that leptons are same flavor
    if(reco.first->type()!=reco.second->type())
        return false;
    // Leptons have opposite charge
    if(reco.first->type()==xAOD::Type::Muon){
        if((static_cast<const xAOD::Muon*>(reco.first))->charge()==(static_cast<const xAOD::Muon*>(reco.second))->charge())
            return false;
    }
    if(reco.first->type()==xAOD::Type::Electron){
        if((static_cast<const xAOD::Electron*>(reco.first))->charge()==(static_cast<const xAOD::Electron*>(reco.second))->charge())
            return false;
    }
    // Tag and Probe mass in Z window
   float mass=DiobjectMass(reco);
   if( mass > m_massMin && mass < m_massMax )
      return true;

   return false;
}

