/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TauTrackEfficiencyAlg
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Alg to calc tracking efficiencies for tracks from tau decays
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "TauTrackTools/TauTrackEfficiencyAlg.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include <map>

#include "GaudiKernel/ITHistSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TH3.h"
#include "TH2.h"
//#include "TH1.h"
#include "TauTrackEvent/TruthTau.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkParametersBase/ParametersBase.h"
// Trk
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "VxVertex/VxContainer.h"

#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthKey.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"


#include "GeneratorObjects/HepMcParticleLink.h"

#include <TGraphAsymmErrors.h>
//#include "GeneratorObjects/McEventCollection.h"

// #include "HepMC/GenEvent.h"
// #include "HepMC/GenVertex.h"
//#include "DataModel/ElementLink.h"


//================ Constructor =================================================

TauID::TauTrackEfficiencyAlg::TauTrackEfficiencyAlg(const std::string& name, ISvcLocator* pSvcLocator)
        :
        AthAlgorithm(name,pSvcLocator),
        m_inputTruthTauContainerName("TruthTaus"),
        m_inputTrackParticleTruthCollectionName("TrackParticleTruthCollection"),
        m_inputTrackParticleContainerName("TrackParticleCandidate"),
        m_inputPrimaryVertexCollection("VxPrimaryCandidate"),
        m_outputRootTreeName("/AANT/TauTrackEff"),
        m_trackSelectorTool("Fatras::DummyTrackSelectorTool/DummyTrackSelectorTool"),
        m_trackToVertexIPEstimator("Trk::TrackToVertexIPEstimator/TrackToVertexIPEstimator"),
        m_minTrackMatchProbability(0.8),
        m_ignorePionsWithHadronicInteractions(false),
        m_truthPionEtaPtHist(0),
        m_matchedPionEtaPtHist(0),
        m_recoPionEtaPtHist(0),
        m_truthPionEtaPtVisHist(0),
        m_matchedPionEtaPtVisHist(0),
        m_recoPionEtaPtVisHist(0),
        m_ptBins(5),
        m_ptMin(0.),
        m_ptMax(50.),
        m_ptVisBins(5),
        m_ptVisMin(20.),
        m_ptVisMax(170.),
        m_etaBins(5),
        m_etaMin(0.),
        m_etaMax(2.5),
        m_truthPionDRHist(0),
        m_matchedPionDRHist(0),
        m_dRBins(20),
        m_dRMin(0.),
        m_dRMax(0.2),
        m_recoD0Hist(0),
        m_d0Bins(100),
        m_d0Min(0.),
        m_d0Max(10.),
        m_recoD0SignfHist(0),
        m_d0SignfBins(100),
        m_d0SignfMin(0.),
        m_d0SignfMax(10.),
        m_trackNumberMigrationHist(0),
        m_trackNumberMax(8),
        m_tauConeSize(0.4)
 {
    declareProperty("TauTruthParticleContainer",    m_inputTruthTauContainerName,   "SG key of input TruthParticle container holding TruthTau objects");
    declareProperty("TrackParticleTruthCollection", m_inputTrackParticleTruthCollectionName,    "SG key of input TrackParticle truth collection");
    declareProperty("TrackParticleContainer",       m_inputTrackParticleContainerName,          "SG key of input TrackParticle container");
    declareProperty("PrimaryVertexCollection",      m_inputPrimaryVertexCollection,             "SG key of input PrimaryVertex collection");
    declareProperty("RootTreeName",                 m_outputRootTreeName,           "tree and dir name of the output ROOT file");
    declareProperty("TrackSelectorTool",            m_trackSelectorTool,            "Tool handle to TrackSelectorTool");
    declareProperty("TrackToVertexIPEstimator",     m_trackToVertexIPEstimator,     "Tool handle to TrackToVertexIPEstimator");
    declareProperty("MinTrackMatchProbability",     m_minTrackMatchProbability,     "min. probability to accept a track to truth match");
    declareProperty("IgnorePionsWithHadronicInteractions",  m_ignorePionsWithHadronicInteractions,  "ignore pions which had hadronic interactions in the Inner Detector");
    // histogram properties
    declareProperty("PtBins",                       m_ptBins,                       "number of pT bins");
    declareProperty("PtMin",                        m_ptMin,                        "min pT");
    declareProperty("PtMax",                        m_ptMax,                        "max pT");
    declareProperty("PtVisBins",                    m_ptVisBins,                    "number of visual pT bins");
    declareProperty("PtVisMin",                     m_ptVisMin,                     "min visual pT");
    declareProperty("PtVisMax",                     m_ptVisMax,                     "max visual pT");
    declareProperty("EtaBins",                      m_etaBins,                      "number of eta bins");
    declareProperty("EtaMin",                       m_etaMin,                       "min eta");
    declareProperty("EtaMax",                       m_etaMax,                       "max eta");
    declareProperty("DRBins",                       m_dRBins,                       "number of bins in delta R (pi, pi)");
    declareProperty("DRMin",                        m_dRMin,                        "min bin delta R");
    declareProperty("DRMax",                        m_dRMax,                        "max bin delta R");
    declareProperty("D0Bins",                       m_d0Bins,                       "number of bins in d0 (transverse impact parameter)");
    declareProperty("D0Min",                        m_d0Min,                        "min bin d0");
    declareProperty("D0Max",                        m_d0Max,                        "max bin d0");
    declareProperty("D0SignfBins",                  m_d0SignfBins,                  "number of bins in significance of d0 (transverse impact parameter)");
    declareProperty("D0SignfMin",                   m_d0SignfMin,                   "min bin d0 significance");
    declareProperty("D0SignfMax",                   m_d0SignfMax,                   "max bin d0 significance");
    declareProperty("TauConeSize",                  m_tauConeSize,                  "cone size of tau seeds used for track number migration hist");
    //char histName[80],histTitle[80];
}

//================ Destructor =================================================
TauID::TauTrackEfficiencyAlg::~TauTrackEfficiencyAlg() {}


//================ Initialisation =================================================

StatusCode TauID::TauTrackEfficiencyAlg::initialize() {

    ATH_MSG_INFO( name() << " initialize()" );

    //ntuple + histogramming service
    StatusCode sc = service("THistSvc", m_thistSvc);
    if(sc.isFailure()){
        ATH_MSG_ERROR( "Unable to retrieve pointer to THistSvc" );
        return sc;
    }

    m_truthPionEtaPtHist    = new  TH3I( "truthPionEtaPt", "eta, pT of truth pions",
                                NumberOfTruthTauHistTypes, 0, NumberOfTruthTauHistTypes,
                                m_etaBins,  m_etaMin,   m_etaMax,
                                m_ptBins,   m_ptMin,    m_ptMax );
    m_matchedPionEtaPtHist  = new  TH3I( "matchedPionEtaPt", "eta, pT of truth pions with matched reconstructed track",
                                NumberOfTruthTauHistTypes, 0, NumberOfTruthTauHistTypes,
                                m_etaBins,  m_etaMin,   m_etaMax,
                                m_ptBins,   m_ptMin,    m_ptMax );
    m_recoPionEtaPtHist     = new  TH3I( "recoPionEtaPt", "eta, pT of reconstructed tracks matched with truth pions",
                                NumberOfTruthTauHistTypes, 0, NumberOfTruthTauHistTypes,
                                m_etaBins,  m_etaMin,   m_etaMax,
                                m_ptBins,   m_ptMin,    m_ptMax );
    m_truthPionEtaPtVisHist = new  TH3I( "truthPionEtaPtVis", "eta, pT vis of truth pions",
                                NumberOfTruthTauHistTypes, 0, NumberOfTruthTauHistTypes,
                                m_etaBins,  m_etaMin,   m_etaMax,
                                m_ptVisBins,   m_ptVisMin,    m_ptVisMax );
    m_matchedPionEtaPtVisHist= new  TH3I( "matchedPionEtaPtVis", "eta, pT vis of truth pions with matched reconstructed track",
                                NumberOfTruthTauHistTypes, 0, NumberOfTruthTauHistTypes,
                                m_etaBins,  m_etaMin,   m_etaMax,
                                m_ptVisBins,   m_ptVisMin,    m_ptVisMax );
    m_recoPionEtaPtVisHist  = new  TH3I( "recoPionEtaPtVis", "eta, pT vis of reconstructed tracks matched with truth pions",
                                NumberOfTruthTauHistTypes, 0, NumberOfTruthTauHistTypes,
                                m_etaBins,  m_etaMin,   m_etaMax,
                                m_ptVisBins,   m_ptVisMin,    m_ptVisMax );

    m_truthPionDRHist       = new  TH3I( "truthPionDR", "Delta R (pi, pi) of truth pions",
                                NumberOfTruthTauHistTypes, 0, NumberOfTruthTauHistTypes,
                                m_etaBins,  m_etaMin,   m_etaMax,
                                m_dRBins,   m_dRMin,    m_dRMax );
    m_matchedPionDRHist     = new  TH3I( "matchedPionDR", "Delta R (pi, pi) of truth pions with matched reconstructed track",
                                NumberOfTruthTauHistTypes, 0, NumberOfTruthTauHistTypes,
                                m_etaBins,  m_etaMin,   m_etaMax,
                                m_dRBins,   m_dRMin,    m_dRMax );

    m_recoD0Hist            = new  TH2I( "recoPionD0", "d0 (transverse impact parameter) of reconstructed tracks matched with truth pions",
                                NumberOfTruthTauHistTypes, 0, NumberOfTruthTauHistTypes,
                                m_d0Bins,   m_d0Min,    m_d0Max );
    m_recoD0SignfHist       = new  TH2I( "recoPionD0Signf", "d0 (transverse impact parameter) significance of reconstructed tracks matched with truth pions",
                                NumberOfTruthTauHistTypes, 0, NumberOfTruthTauHistTypes,
                                m_d0SignfBins,   m_d0SignfMin,    m_d0SignfMax );

    m_trackNumberMigrationHist  = new  TH2I( "trackNumberMigration", "number of reconstructed (and selected) tracks vs. true tau type",
                                NumberOfTruthTauHistTypes, 0, NumberOfTruthTauHistTypes,
                                m_trackNumberMax,          0, m_trackNumberMax );
    m_trackNumberMigrationHist->GetXaxis()->SetBinLabel(1, "1 Prong");
    m_trackNumberMigrationHist->GetXaxis()->SetBinLabel(2, "1 Prong + conv");
    m_trackNumberMigrationHist->GetXaxis()->SetBinLabel(3, "3 Prong");
    m_trackNumberMigrationHist->GetXaxis()->SetBinLabel(4, "3 Prong + conv");
    m_trackNumberMigrationHist->GetXaxis()->SetBinLabel(5, "Kaon");
    m_trackNumberMigrationHist->GetXaxis()->SetBinLabel(6, "Kaon + conv");
    m_trackNumberMigrationHist->SetOption("BOX TEXT");

    // register histograms
    sc = m_thistSvc->regHist(m_outputRootTreeName+"/truthPionEtaPt",    m_truthPionEtaPtHist);
    sc = m_thistSvc->regHist(m_outputRootTreeName+"/matchedPionEtaPt",  m_matchedPionEtaPtHist);
    sc = m_thistSvc->regHist(m_outputRootTreeName+"/recoPionEtaPt",     m_recoPionEtaPtHist);
    sc = m_thistSvc->regHist(m_outputRootTreeName+"/truthPionEtaPtVis",     m_truthPionEtaPtVisHist);
    sc = m_thistSvc->regHist(m_outputRootTreeName+"/matchedPionEtaPtVis",   m_matchedPionEtaPtVisHist);
    sc = m_thistSvc->regHist(m_outputRootTreeName+"/recoPionEtaPtVis",      m_recoPionEtaPtVisHist);
    sc = m_thistSvc->regHist(m_outputRootTreeName+"/truthPionDR",       m_truthPionDRHist);
    sc = m_thistSvc->regHist(m_outputRootTreeName+"/matchedPionDR",     m_matchedPionDRHist);
    sc = m_thistSvc->regHist(m_outputRootTreeName+"/recoPionD0",        m_recoD0Hist);
    sc = m_thistSvc->regHist(m_outputRootTreeName+"/recoPionD0Signf",   m_recoD0SignfHist);
    sc = m_thistSvc->regHist(m_outputRootTreeName+"/trackNumberMigrationHist",  m_trackNumberMigrationHist);
    if(sc.isFailure()){
        ATH_MSG_ERROR( "ROOT Hist registration failed" );
        return sc;
    }

    ATH_MSG_INFO( "initialize() successful in " << name() );
    return StatusCode::SUCCESS;
}

StatusCode TauID::TauTrackEfficiencyAlg::finalize() {

    // create efficiency plots from the counted number of true pions and reconstructed tracks
    std::vector<std::string> tauTypeNames(0);
    tauTypeNames.push_back("1P");
    tauTypeNames.push_back("1Pconv");
    tauTypeNames.push_back("3P");
    tauTypeNames.push_back("3Pconv");
    tauTypeNames.push_back("Kaon");
    tauTypeNames.push_back("KaonConv");
    char graphName[80];
    for (int truthTauType = 0; truthTauType < NumberOfTruthTauHistTypes; truthTauType++) {
        for (int etaBin = 1; etaBin < m_etaBins+1; etaBin++) {
            // get number of true pions:
            sprintf(graphName, "truePions_%s_eta%d", tauTypeNames[truthTauType].c_str(), etaBin);
            TH1 *histProjTruth = m_truthPionEtaPtHist->ProjectionZ(graphName, truthTauType+1, truthTauType+1, etaBin, etaBin, "e");
            // number of matched pions:
            sprintf(graphName, "matchedPions_%s_eta%d", tauTypeNames[truthTauType].c_str(), etaBin);
            TH1 *histProjMatched = m_matchedPionEtaPtHist->ProjectionZ(graphName, truthTauType+1, truthTauType+1, etaBin, etaBin, "e");
            // calc efficiency:
            TGraphAsymmErrors* effPlot = new TGraphAsymmErrors(histProjMatched, histProjTruth, "");
            ATH_MSG_DEBUG( "mean Eff for type " << tauTypeNames[truthTauType] << " eta bin " << etaBin << ": " << effPlot->GetMean(2) );
            // add graph to ROOT file
            sprintf(graphName, "/trackPionEff_%s_eta%d", tauTypeNames[truthTauType].c_str(), etaBin);
            effPlot->SetNameTitle(graphName, graphName);
            StatusCode sc = m_thistSvc->regGraph(m_outputRootTreeName+graphName,    effPlot);
            if(sc.isFailure()){
                ATH_MSG_ERROR( "ROOT Graph registration failed" );
                return sc;
            }
            delete histProjTruth;
            delete histProjMatched;

            // same for pT vis:
            // get number of true pions:
            sprintf(graphName, "truePions_%s_eta%d", tauTypeNames[truthTauType].c_str(), etaBin);
            histProjTruth = m_truthPionEtaPtVisHist->ProjectionZ(graphName, truthTauType+1, truthTauType+1, etaBin, etaBin, "e");
            // number of matched pions:
            sprintf(graphName, "matchedPions_%s_eta%d", tauTypeNames[truthTauType].c_str(), etaBin);
            histProjMatched = m_matchedPionEtaPtVisHist->ProjectionZ(graphName, truthTauType+1, truthTauType+1, etaBin, etaBin, "e");
            // calc efficiency:
            effPlot = new TGraphAsymmErrors(histProjMatched, histProjTruth, "");
            //ATH_MSG_DEBUG( "mean Eff for type " << tauTypeNames[truthTauType] << " eta bin " << etaBin << ": " << effPlot->GetMean(2) );
            // add graph to ROOT file
            sprintf(graphName, "/trackPionEffVsPtVis_%s_eta%d", tauTypeNames[truthTauType].c_str(), etaBin);
            effPlot->SetNameTitle(graphName, graphName);
            sc = m_thistSvc->regGraph(m_outputRootTreeName+graphName,    effPlot);
            if(sc.isFailure()){
                ATH_MSG_ERROR( "ROOT Graph registration failed" );
                return sc;
            }
            delete histProjTruth;
            delete histProjMatched;

            // and again for dR:
            // ignore 1Prongs
            if (!((truthTauType == TruthOneProngConversions) || (truthTauType == TruthOneProng))) {
                // get number of true pions:
                sprintf(graphName, "truePions_%s_eta%d", tauTypeNames[truthTauType].c_str(), etaBin);
                TH1 *histProjTruth = m_truthPionDRHist->ProjectionZ(graphName, truthTauType+1, truthTauType+1, etaBin, etaBin, "e");
                // number of matched pions:
                sprintf(graphName, "matchedPions_%s_eta%d", tauTypeNames[truthTauType].c_str(), etaBin);
                TH1 *histProjMatched = m_matchedPionDRHist->ProjectionZ(graphName, truthTauType+1, truthTauType+1, etaBin, etaBin, "e");
                // calc efficiency:
                TGraphAsymmErrors* effPlot = new TGraphAsymmErrors(histProjMatched, histProjTruth, "");
                //ATH_MSG_DEBUG( "mean Eff for type " << tauTypeNames[truthTauType] << " eta bin " << etaBin << ": " << effPlot->GetMean(2) );
                // add graph to ROOT file
                sprintf(graphName, "/trackPionEffVsDR_%s_eta%d", tauTypeNames[truthTauType].c_str(), etaBin);
                effPlot->SetNameTitle(graphName, graphName);
                StatusCode sc = m_thistSvc->regGraph(m_outputRootTreeName+graphName,    effPlot);
                if(sc.isFailure()){
                    ATH_MSG_ERROR( "ROOT Graph registration failed" );
                    return sc;
                }
                delete histProjTruth;
                delete histProjMatched;
            }
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode TauID::TauTrackEfficiencyAlg::execute() {
    /** get the MC truth particle container from StoreGate */
    const TruthParticleContainer*  truthTauCont = 0;
    StatusCode sc = evtStore()->retrieve(truthTauCont, m_inputTruthTauContainerName);
    if ( !truthTauCont || sc.isFailure() ) {
        ATH_MSG_ERROR( " Truth particle container (" << m_inputTruthTauContainerName << ") not found in StoreGate" );
        return StatusCode::FAILURE;
    }

    /** get the track particle truth container (for track to truth link) from StoreGate */
    const TrackParticleTruthCollection* trackTruthParticles = 0;
    sc = evtStore()->retrieve(trackTruthParticles, m_inputTrackParticleTruthCollectionName);
    if ( !trackTruthParticles || sc.isFailure() ) {
        ATH_MSG_ERROR( " Track Truth particle container (" << m_inputTrackParticleTruthCollectionName << ") not found in StoreGate" );
        return StatusCode::FAILURE;
    }

    /** get the track particle container from StoreGate */
    const Rec::TrackParticleContainer* trackPartCont = 0;
    sc = evtStore()->retrieve(trackPartCont, m_inputTrackParticleContainerName);
    if ( !trackPartCont || sc.isFailure() ) {
        ATH_MSG_ERROR( " Track particle container (" << m_inputTrackParticleContainerName << ") not found in StoreGate" );
        return StatusCode::FAILURE;
    }

//    /** get the GenEvent collection from StoreGate */
//     const McEventCollection* truthEvent = 0;
//     sc = evtStore()->retrieve(truthEvent, "GEN_AOD");
//     if ( !truthEvent || sc.isFailure() ) {
//         ATH_MSG_ERROR( " GenEvent collection (" << m_inputTruthTauContainerName << ") not found in StoreGate" );
//         return StatusCode::FAILURE;
//     }
    /** get the primary vertex container from StoreGate */
    const VxContainer* primaryVertexContainer = 0;
    sc = evtStore()->retrieve(primaryVertexContainer, m_inputPrimaryVertexCollection);
    if ( !primaryVertexContainer || sc.isFailure() ) {
        ATH_MSG_ERROR( " Primary Vertex container (" << m_inputPrimaryVertexCollection << ") not found in StoreGate" );
        return StatusCode::FAILURE;
    }

    // loop over truth taus to get particle barcodes
    std::map<int, const TauID::TruthTau*> tauTrackBarcodeMap;
    std::map<int, const TruthParticle*> pionTrackBarcodeMap;
    TruthParticleContainer::const_iterator mcpartItr  = truthTauCont->begin();
    TruthParticleContainer::const_iterator mcpartItrE = truthTauCont->end();
    for (; mcpartItr != mcpartItrE; ++mcpartItr) {
        // use TruthTaus only
        const TauID::TruthTau* truthTau = dynamic_cast<const TauID::TruthTau*>(*mcpartItr);
        if (!truthTau) continue;
        if (!truthTau->type(TruthTau::Hadronic)) continue;
        ATH_MSG_DEBUG( "TruthTau contains " << truthTau->chargedProducts().size() << " charged products" );
        // loop over charged decay products to get barcodes
        unsigned int productIndex = 0;
        std::vector<const TruthParticle*>::const_iterator tauChargedProductsIter = truthTau->chargedProducts().begin();
        for (; tauChargedProductsIter != truthTau->chargedProducts().end(); tauChargedProductsIter++) {
            productIndex++;
            if (!(*tauChargedProductsIter)) continue;
            ATH_MSG_VERBOSE( "-- BC:" << (*tauChargedProductsIter)->barcode() << " PDG:" << (*tauChargedProductsIter)->pdgId() << " eta=" << (*tauChargedProductsIter)->eta() << " phi=" << (*tauChargedProductsIter)->phi() << " pT=" << (*tauChargedProductsIter)->pt() );
            // take pions only
            if (abs((*tauChargedProductsIter)->pdgId()) != PDG::pi_plus) continue;
            if (m_ignorePionsWithHadronicInteractions && truthTau->type(TruthTau::HadronicInteractions)) {
                if (truthTau->chargedProductHadHadronicInteraction(productIndex-1)) {
                    ATH_MSG_VERBOSE( "---- pion had hadronic interaction: ignore!" );
                    continue;
                }
            }
            tauTrackBarcodeMap[(*tauChargedProductsIter)->barcode()] = truthTau;
            pionTrackBarcodeMap[(*tauChargedProductsIter)->barcode()] = (*tauChargedProductsIter);
            if ( (fillTruthChargedPionData(*truthTau, *(*tauChargedProductsIter))).isFailure() ) {
                ATH_MSG_WARNING( "could not add truth data" );
            }
        } // end for (tauBarcodesIter)
    } // end for (mcpartItr)

    std::map<int, const Rec::TrackParticle*> barcodeTrackMap;
    std::vector<const Rec::TrackParticle*>  selectedTracks;
    // loop over all TrackParticles
    Rec::TrackParticleContainer::const_iterator trackPartIter = trackPartCont->begin();
    for (; trackPartIter != trackPartCont->end(); trackPartIter++) {
        if (!(*trackPartIter)) continue;
        // do track selection
        const Trk::Vertex* vertex = 0;
        if (primaryVertexContainer->size() > 0) vertex = &((*primaryVertexContainer)[0]->recVertex());
        if (!m_trackSelectorTool->decision(**trackPartIter, vertex)) {
            // track out of selection cuts
            continue;
        }
        selectedTracks.push_back(*trackPartIter);
        ATH_MSG_DEBUG( "TrackParticle: pT=" << (*trackPartIter)->pt() << "\t eta=" << (*trackPartIter)->eta() << "\t phi=" << (*trackPartIter)->phi() );
        // TrackParticleTruthKey (the index of TrackParticleTruthCollection) is just the index of a TrackParticle in the
        // TrackParticleContainer, no real ElementLink!
	//        TrackParticleTruthCollection::const_iterator tpTruthIter = trackTruthParticles->find(trackPartIter - trackPartCont->begin());  
	ElementLink<Rec::TrackParticleContainer> mctruthLink; 
	mctruthLink.setElement(const_cast<Rec::TrackParticle*>(*trackPartIter)); 
	mctruthLink.setStorableObject(*trackPartCont); 
	TrackParticleTruthCollection::const_iterator tpTruthIter = trackTruthParticles->find(Rec::TrackParticleTruthKey(mctruthLink));  
        if(tpTruthIter == trackTruthParticles->end()) {
            // no truth link found, ignore!
            continue;
        }
        // check whether we got a good link
        if (!((*tpTruthIter).second.particleLink().isValid()) || (*tpTruthIter).second.probability() < m_minTrackMatchProbability) {
            continue;
        }
        const int barcode = (*tpTruthIter).second.particleLink()->barcode();
        ATH_MSG_DEBUG( "Barcode assigned to TrackParticle: " << barcode << "\t" << (*tpTruthIter).second.probability() );
        std::map<int, const TauID::TruthTau*>::iterator matchedTauIter = tauTrackBarcodeMap.find(barcode);
        if (matchedTauIter != tauTrackBarcodeMap.end()) {
            // found matching truth tau
            ATH_MSG_DEBUG( "Found matching truth tau: " << (*matchedTauIter).second->barcode() << "\t" << (*matchedTauIter).second->dumpType() );
            if (barcodeTrackMap.count(barcode) > 0 ) {
                ATH_MSG_DEBUG( "Barcode already used!" );
            } else if ( (fillMatchedTrackData(*((*matchedTauIter).second), *(pionTrackBarcodeMap[barcode]), **trackPartIter)).isFailure() ) {
                ATH_MSG_WARNING( "could not add matched track data" );
            }
            //trackTauTruthMap.insert(std::make_pair( (*trackPartIter), (*matchedTauIter) ));
            barcodeTrackMap.insert(std::make_pair(barcode, (*trackPartIter) ));
        }
    } // end for (trackPartIter)

    ///////////////////////
    //// migration hist
    //std::vector<const Rec::TrackParticle*>  tracksInSeedCone;
    // loop over truth taus to get "tau seeds"
    mcpartItr  = truthTauCont->begin();
    mcpartItrE = truthTauCont->end();
    for (; mcpartItr != mcpartItrE; ++mcpartItr) {
        // use TruthTaus only
        const TauID::TruthTau* truthTau = dynamic_cast<const TauID::TruthTau*>(*mcpartItr);
        if (!truthTau) continue;
        // use TruthTaus in acceptance range only:
        if (fabs(truthTau->hlvVis().eta()) > m_etaMax || fabs(truthTau->hlvVis().eta()) < m_etaMin) continue;
        // get truth tau type:
        TruthTauHistType truthTauType = getTruthTauType(*truthTau);
        // ignore types we are not interested in
        if (truthTauType == NumberOfTruthTauHistTypes) continue;
        ATH_MSG_DEBUG( "TruthTau contains " << truthTau->chargedProducts().size() << " charged products" );
        //tracksInSeedCone.clear();
        double nTracksInSeedCone = 0;
        // TODO: search for leading track in cone and use it as seed direction
        // find reconstructed tracks around seed direction
        std::vector<const Rec::TrackParticle*>::const_iterator trackIter = selectedTracks.begin();
        for ( ; trackIter != selectedTracks.end(); trackIter++) {
            double dR = (*trackIter)->hlv().deltaR( truthTau->hlvVis() );
            if (dR > m_tauConeSize) continue;
            ATH_MSG_DEBUG( "Found track in cone: dR=" << dR );
            //tracksInSeedCone.push_back(*trackIter);
            nTracksInSeedCone++;
        } // end for (trackIter)
        //ATH_MSG_DEBUG( "Found " << tracksInSeedCone.size() << " tracks in cone around seed" );
        ATH_MSG_DEBUG( "Found " << nTracksInSeedCone << " tracks in cone around seed" );
        //if ((truthTau->hlvVis().perp()/1000. < m_ptVisMax) && (fabs(truthTau->hlvVis().eta()) < m_etaMax)) {
        m_trackNumberMigrationHist->Fill(truthTauType, nTracksInSeedCone );
        //}
    } // end for (mcpartItr)
    return StatusCode::SUCCESS;
}

StatusCode TauID::TauTrackEfficiencyAlg::fillTruthChargedPionData(
                                            const TauID::TruthTau& truthTau,
                                            const TruthParticle& truthChargedPion) {
    // get truth tau type:
    TruthTauHistType truthTauType = getTruthTauType(truthTau);
    // other decay mode, we are not interested in
    if (truthTauType == NumberOfTruthTauHistTypes) return StatusCode::SUCCESS;

    m_truthPionEtaPtHist->Fill( truthTauType, fabs(truthChargedPion.eta()), truthChargedPion.pt()/1000. );
    m_truthPionEtaPtVisHist->Fill( truthTauType, fabs(truthChargedPion.eta()), truthTau.hlvVis().perp()/1000. );

    ATH_MSG_DEBUG( "true pion: tau type=" << truthTauType << " eta=" << truthChargedPion.eta() << " phi=" << truthChargedPion.phi() << " pT=" << truthChargedPion.pt() );

    // calculate R distance to next charged pion from tau decay (does not make sense for 1prong!):
    if (truthTau.chargedProducts().size()>1) {
        // pions without a partner end up in the overflow bin...
        double minDR = getMinDRPiPi(truthTau, truthChargedPion);
        ATH_MSG_DEBUG( "min delta R (pi, pi)=" << minDR );
        m_truthPionDRHist->Fill( truthTauType, fabs(truthTau.hlvVis().eta()), minDR );
    }
    return StatusCode::SUCCESS;
}

StatusCode TauID::TauTrackEfficiencyAlg::fillMatchedTrackData(
                                            const TauID::TruthTau& truthTau,
                                            const TruthParticle& truthChargedPion,
                                            const Rec::TrackParticle& track) {
    // get truth tau type:
    TruthTauHistType truthTauType = getTruthTauType(truthTau);
    // other decay mode, we are not interested in
    if (truthTauType == NumberOfTruthTauHistTypes) return StatusCode::SUCCESS;

    m_matchedPionEtaPtHist->Fill( truthTauType, fabs(truthChargedPion.eta()), truthChargedPion.pt()/1000. );
    m_recoPionEtaPtHist->Fill( truthTauType, fabs(track.eta()), track.pt()/1000. );
    m_matchedPionEtaPtVisHist->Fill( truthTauType, fabs(truthChargedPion.eta()), truthTau.hlvVis().perp()/1000. );
    m_recoPionEtaPtVisHist->Fill( truthTauType, fabs(track.eta()), truthTau.hlvVis().perp()/1000. );

    ATH_MSG_DEBUG( "matched pion: tau type=" << truthTauType << " eta=" << truthChargedPion.eta() << " phi=" << truthChargedPion.phi() << " pT=" << truthChargedPion.pt() << " track eta=" << track.eta() << " track pT=" << track.pt() );


    // calculate R distance to next charged pion from tau decay (does not make sense for 1prong!):
    if (truthTau.chargedProducts().size()>1) {
        // pions without a partner end up in the overflow bin...
        double minDR = getMinDRPiPi(truthTau, truthChargedPion);
        ATH_MSG_DEBUG( "min delta R (pi, pi)=" << minDR );
        m_matchedPionDRHist->Fill( truthTauType, fabs(truthTau.hlvVis().eta()), minDR );
    }
    // calculate the transverse impact parameter
    const Trk::VxCandidate* vertex = track.reconstructedVertex();
    const Trk::ImpactParametersAndSigma* impactParameter = m_trackToVertexIPEstimator->estimate(&track, vertex);
    if (impactParameter) {
        double recoD0 = impactParameter->IPd0;
        double signfD0 = (impactParameter->sigmad0 != 0. ? recoD0/impactParameter->sigmad0 : -1. );
        // calculate true impact parameter
//         //ThreeVector GenVertex * truthTau.genParticle ()->production_vertex()->point3d();
//         const Trk::RecVertex* tauProdVertex = truthTau.origin();
//         const Hep3Vector&   tauProdPos = tauProdVertex->position();
// 
//         GlobalPosition tauGloPos(tauProdPos.x(), tauProdPos.y(), tauProdPos.z());
//         PerigeeSurface perigeeSurface(tauGloPos);
//         // direct extrapolation, no material effects:
//         const Trk::Perigee* extrapolatedParameters = m_extrapolator->extrapolateDirectly(truePionTrackPar, perigeeSurface);
//         if (extrapolatedParameters) {
//             double trueD0 = extrapolatedParameters->parameters()[Trk::d0];
//             delete extrapolatedParameters;
//             //const Trk::RecVertex* pionProdVertex = truthChargedPion.origin();
//             //const Hep3Vector&   pionProdPos = pionProdVertex->position();
//             double residualD0 = recoD0 - trueD0;
//             double pullD0 = (impactParameter->second != 0. ? residualD0/impactParameter->second : 10. );
//        }
        m_recoD0Hist->Fill( truthTauType, recoD0 );
        m_recoD0SignfHist->Fill( truthTauType, signfD0 );
        delete impactParameter;
        impactParameter=0;
    } else {
        ATH_MSG_DEBUG( "Impact parameter could not be estimated" );
    }

    return StatusCode::SUCCESS;
}

double TauID::TauTrackEfficiencyAlg::getMinDRPiPi(
                    const TauID::TruthTau& truthTau,
                    const TruthParticle& truthChargedPion) {
    double minDR = 99999.;
    std::vector<const TruthParticle*>::const_iterator chargedProductsIter = truthTau.chargedProducts().begin();
    for (; chargedProductsIter != truthTau.chargedProducts().end(); chargedProductsIter++) {
        const TruthParticle* part = *chargedProductsIter;
        if (!part) continue;
        // TODO: decide: take only pions into account, or other charged particles as well?????
        if (abs(part->pdgId()) != PDG::pi_plus) continue;
        // ignore the particle itself
        if (part->barcode() == truthChargedPion.barcode()) continue;
        double dR = truthChargedPion.hlv().deltaR( part->hlv() );
        if (dR < minDR) minDR = dR;
    }
    return minDR;
}

TauID::TauTrackEfficiencyAlg::TruthTauHistType TauID::TauTrackEfficiencyAlg::getTruthTauType(const TauID::TruthTau& truthTau) {
    TruthTauHistType truthTauType = NumberOfTruthTauHistTypes;
    // For the moment do not add decays with Kaons to the 1-prong or 3-prong class, but put them in an own class:
    // ignore 5-prongs!
    if (truthTau.type(TruthTau::Kaon)) {
        truthTauType = truthTau.type(TruthTau::Conversions) ? TruthKaonConversions : TruthKaon;
    } else if (truthTau.type(TruthTau::OneProng)) {
        truthTauType = truthTau.type(TruthTau::Conversions) ? TruthOneProngConversions : TruthOneProng;
    } else if (truthTau.type(TruthTau::ThreeProng)) {
        truthTauType = truthTau.type(TruthTau::Conversions) ? TruthThreeProngConversions : TruthThreeProng;
    }
    return truthTauType;
}
