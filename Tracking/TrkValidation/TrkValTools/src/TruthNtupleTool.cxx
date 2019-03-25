/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TruthNtupleTool.cxx
//   Source file for class TruthNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////


//Gaudi
#include "GaudiKernel/ITHistSvc.h"

// Trk
#include "TrkValTools/TruthNtupleTool.h"
#include "TrkParameters/TrackParameters.h"
// Truth
#include "TrkTruthData/TrackTruth.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// root
#include "TTree.h"
#include <TH1D.h>
#include <TGraphAsymmErrors.h>

// constructor
Trk::TruthNtupleTool::TruthNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_fillJets(false),
        m_nt(nullptr),
        m_numberOfTreeEntries(0),
        m_runNumber{},
        m_eventNumber{},        
        m_TrackLinkIndex{},
        m_mc_prob{},
        m_classifications{},
         m_mc_d0{},           
         m_mc_z0{},           
         m_mc_phi0{},         
         m_mc_theta{},        
         m_mc_qOverP{},       
         m_mc_qOverPt{},      
         m_mc_eta{},          

         m_mc_particleID{},   
         m_mc_barcode{},      
         m_mc_energy{},       
         m_mc_jetLinkIndex{}, 
         m_mc_prodR{}, 
         m_mc_prodz{}
{
    declareInterface<ITruthNtupleTool>(this);

    m_etaBins.push_back(0.0);
    m_etaBins.push_back(0.8);
    m_etaBins.push_back(1.6);
    m_etaBins.push_back(2.5);
    // Declare the properties
    declareProperty("NtupleFileName",               m_ntupleFileName = "TRKVAL/Validation",    "Ntuple file handle");
    declareProperty("NtupleTreeName",               m_ntupleTreeName = "Truth",        "Name of the ntuple tree");
}

// destructor
Trk::TruthNtupleTool::~TruthNtupleTool() {}




// initialize
StatusCode Trk::TruthNtupleTool::initialize() {

  ATH_CHECK( m_evt.initialize() );

    if (m_etaBins.size()<2) {
      ATH_MSG_ERROR ("Vector of eta bins too small");
      return StatusCode::FAILURE;
    }

    // ---------------------------
    // retrive pointer to THistSvc
    ITHistSvc *tHistSvc;
    StatusCode sc =  service("THistSvc", tHistSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR ("Unable to retrieve pointer to THistSvc");
        return sc;
    }
    // ---------------------------
    // create tree and register it to THistSvc
    m_nt = new TTree(TString(m_ntupleTreeName), "Track Validation Truth");
    // NB: we must not delete the tree, this is done by THistSvc
    std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleTreeName;
    sc = tHistSvc->regTree(fullNtupleName, m_nt);
    if (sc.isFailure()) {
      ATH_MSG_ERROR ("Unable to register TTree : " << m_ntupleTreeName);
      return sc;
    }

    //-----------------
    // add items
    // event info:
    m_nt->Branch( "RunNumber",          &m_runNumber       );
    m_nt->Branch( "EventNumber",        &m_eventNumber     );
    m_nt->Branch( "truth_d0",           &m_mc_d0           );
    m_nt->Branch( "truth_z0",           &m_mc_z0           );
    m_nt->Branch( "truth_phi0",         &m_mc_phi0         );
    m_nt->Branch( "truth_theta",        &m_mc_theta        );
    m_nt->Branch( "truth_qOverP",       &m_mc_qOverP       );
    m_nt->Branch( "truth_qOverPt",      &m_mc_qOverPt      );
    m_nt->Branch( "truth_eta",          &m_mc_eta          );
    m_nt->Branch( "truth_particleID",   &m_mc_particleID   );
    m_nt->Branch( "truth_barcode",      &m_mc_barcode      );
    m_nt->Branch( "truth_energy",       &m_mc_energy       );
    m_nt->Branch( "truth_prod_R",       &m_mc_prodR        );
    m_nt->Branch( "truth_prod_z",       &m_mc_prodz        );

    ATH_MSG_VERBOSE ("added items to ntuple");

    return StatusCode::SUCCESS;

}

StatusCode Trk::TruthNtupleTool::initBranches(const std::vector<const Trk::ITrackTruthClassifier*>& classifiers, bool fillJets,  const std::vector<std::string> trackCollectionNames) const {
    m_trackTruthClassifiers = classifiers;
    m_classifications.resize(classifiers.size(), 0);
    m_recoTrackCounts.resize(classifiers.size());
    m_truthTrackCounts.resize(classifiers.size());
    m_fillJets = fillJets;

    if (m_fillJets) 
      m_nt->Branch("JetLinkIndex", &m_mc_jetLinkIndex );// link to MC jets

    double etaBins[100];
    for (unsigned int bin = 0; bin < m_etaBins.size(); ++bin ) {
        etaBins[bin] = m_etaBins[bin];
    }
    for (unsigned int classifierIndex = 0; classifierIndex < classifiers.size(); ++classifierIndex ) {
        if (!(classifiers[classifierIndex])) {
          ATH_MSG_ERROR ("Got NULL pointer to truth classifier tool!");
          return StatusCode::FAILURE;
        }
        std::string classifierName = classifiers[classifierIndex]->nameOfClassifier();
        // add branch for classifier
        ATH_MSG_INFO ("using classifier " << classifierName);
        std::string branchName = "Class_" + classifierName;
        std::string branchDescription = "Class_" + classifierName + "/I";
        m_nt->Branch(branchName.c_str(), &(m_classifications[classifierIndex]), branchDescription.c_str());
        // init statistic counters
        for (unsigned int clIndex = 0; clIndex < classifiers[classifierIndex]->numberOfClassifiers(); clIndex++) {
            m_truthTrackCounts[classifierIndex].push_back( new TH1D(
                ("true"+classifiers[classifierIndex]->classificationAsString(clIndex)).c_str(),
                ("true"+classifiers[classifierIndex]->classificationAsString(clIndex)).c_str(),
                m_etaBins.size()-1,
                etaBins
            ) );
            // TODO: do statistics for each input track collection
            m_recoTrackCounts[classifierIndex].push_back( new TH1D(
                ("reco"+classifiers[classifierIndex]->classificationAsString(clIndex)).c_str(),
                ("reco"+classifiers[classifierIndex]->classificationAsString(clIndex)).c_str(),
                m_etaBins.size()-1,
                etaBins
            ) );
        }
    }

    // add branches with links to the reconstructed tracks:
    if (m_TrackLinkIndex.size()>0) {
      ATH_MSG_ERROR ("Track links seem to be already initialized! Do NOT call initBranches() twice!");
      return StatusCode::FAILURE;
    }
    m_TrackLinkIndex.resize(trackCollectionNames.size(), 0);
    m_mc_prob.resize(trackCollectionNames.size(), 0);
    for (unsigned int trackColIndex = 0; trackColIndex < trackCollectionNames.size(); ++trackColIndex ) {
        m_TrackLinkIndex[trackColIndex] = new std::vector<unsigned int>();
        std::string branchName = "TrackLinkIndex_" + trackCollectionNames[trackColIndex];
        m_nt->Branch(branchName.c_str(),  &(m_TrackLinkIndex[trackColIndex]));       // link to tracks
        m_mc_prob[trackColIndex]        = new std::vector<float>();
        branchName = "truth_prob_" + trackCollectionNames[trackColIndex];
        m_nt->Branch(branchName.c_str(), &(m_mc_prob[trackColIndex]));              // matching probability
    }
    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::TruthNtupleTool::finalize() {

  ATH_MSG_INFO ("start finalize() in " << name());

    // retrive pointer to THistSvc
    ITHistSvc *tHistSvc;
    StatusCode sc =  service("THistSvc", tHistSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR ("Unable to retrieve pointer to THistSvc");
      return sc;
    }

    ATH_MSG_INFO ( "Efficiencies:" );

    //Double_t* efficiencyValues;
    for (unsigned int classifierIndex = 0; classifierIndex < m_trackTruthClassifiers.size(); ++classifierIndex ) {
        for (unsigned int clIndex = 0; clIndex < m_trackTruthClassifiers[classifierIndex]->numberOfClassifiers(); clIndex++) {
            // output:

          msg() << std::resetiosflags(std::ios::right) << std::setiosflags(std::ios::left) << std::setw(15) << m_trackTruthClassifiers[classifierIndex]->classificationAsString(clIndex);
          msg() << std::resetiosflags(std::ios::left)  << std::setiosflags(std::ios::right);
          for (int etaBin = 0; etaBin < m_recoTrackCounts[classifierIndex][clIndex]->GetNbinsX(); etaBin++) {
            double truth = m_truthTrackCounts[classifierIndex][clIndex]->GetBinContent(etaBin+1);
            double reco = m_recoTrackCounts[classifierIndex][clIndex]->GetBinContent(etaBin+1);
            msg() << std::resetiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(8) << std::setw(8) << reco << "/" << std::setw(8) << truth;
            msg() << std::setiosflags(std::ios::fixed | std::ios::showpoint | std::ios::right ) << std::setw(7)
                  << " (" << std::setprecision(4) << (truth>0. ? reco/truth : 0.) << ")";
            }
            // calc efficiency:
            TGraphAsymmErrors* effPlot = new TGraphAsymmErrors(m_recoTrackCounts[classifierIndex][clIndex], m_truthTrackCounts[classifierIndex][clIndex], "");
            // add graph to ROOT file
            std::string graphName = "trackEff_" + m_trackTruthClassifiers[classifierIndex]->classificationAsString(clIndex);
            effPlot->SetNameTitle(graphName.c_str(), graphName.c_str());
            StatusCode sc = tHistSvc->regGraph("/"+m_ntupleFileName+"/"+graphName,    effPlot);
            if(sc.isFailure()){
              ATH_MSG_ERROR ("ROOT Graph registration failed");
              return sc;
            }
            msg() << "\n";
            delete m_recoTrackCounts[classifierIndex][clIndex];
            m_recoTrackCounts[classifierIndex][clIndex] = 0;
            delete m_truthTrackCounts[classifierIndex][clIndex];
            m_truthTrackCounts[classifierIndex][clIndex] = 0;
        }
    }
    msg() << endmsg;

    // delete track links and probs
    for (unsigned int trackColIndex = 0; trackColIndex < m_TrackLinkIndex.size(); ++trackColIndex ) {
      delete m_TrackLinkIndex[trackColIndex];
      delete m_mc_prob[trackColIndex];
    }
    return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// write track data to ntuple
//////////////////////////////////////
StatusCode Trk::TruthNtupleTool::writeTruthData (
               const std::vector< Trk::ValidationTrackTruthData >& truthData) const {

    ATH_MSG_DEBUG ("in writeTruthData(...) with ValTrackTruthData size = "<<truthData.size());
    // ---------------------------------------
    // fill event data
    SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
    if (!evt.isValid()) {
      ATH_MSG_WARNING ("Could not retrieve event info");
      return StatusCode::FAILURE;
    }

    m_runNumber=evt->runNumber();
    m_eventNumber=evt->eventNumber();

    const HepMC::GenParticle*   genParticle = 0;
    const Trk::TrackParameters* truePerigee = 0;
    for (unsigned int index = 0; index < truthData.size(); index++) {
        genParticle = truthData[index].genParticle;
        truePerigee = truthData[index].truthPerigee;
        if (genParticle==NULL || truePerigee==NULL) {
          if (genParticle==NULL) ATH_MSG_WARNING ("NULL pointer to gen particle at index "<<index<<
                                                  ", problem with truth selection logic?");
          else ATH_MSG_DEBUG ("NULL pointer perigee from TruthToTrack. Index is "<<index);
            m_mc_barcode = 0;
            for (unsigned int trackColIndex = 0; trackColIndex < m_mc_prob.size(); ++trackColIndex ) {
                (m_mc_prob[trackColIndex])->clear();
                (m_TrackLinkIndex[trackColIndex])->clear();
            }
            //m_classifications->clear(); do not clear! vector needs to be the same
            m_mc_particleID = 0;
            m_mc_energy     = 0.;

            m_mc_d0      = 0.;
            m_mc_z0      = 0.;
            m_mc_phi0    = 0.;
            m_mc_theta   = 0.;
            m_mc_qOverP  = 0.;
            m_mc_qOverPt = 0.;
            m_mc_eta     = 0.;
            m_mc_prodR   = 0.;
            m_mc_prodz   = 0.;
        } else {
            m_mc_barcode = genParticle->barcode();
            m_mc_particleID = genParticle->pdg_id();
            m_mc_energy     = genParticle->momentum().e();

            m_mc_d0      = truePerigee->parameters()[Trk::d0];
            m_mc_z0      = truePerigee->parameters()[Trk::z0];
            m_mc_phi0    = truePerigee->parameters()[Trk::phi0];
            m_mc_theta   = truePerigee->parameters()[Trk::theta];
            m_mc_qOverP  = truePerigee->parameters()[Trk::qOverP];
            m_mc_qOverPt = (sin( m_mc_theta ) != 0.) ? m_mc_qOverP / sin( m_mc_theta ) : 0.;
            m_mc_eta     = truePerigee->eta();
            m_mc_prodR   = fabs(genParticle->production_vertex()->point3d().perp());
            m_mc_prodz   = fabs(genParticle->production_vertex()->point3d().z());

            for (unsigned int trackColIndex = 0; trackColIndex < truthData[index].truthToTrackIndices.size(); ++trackColIndex ) {
                (*(m_TrackLinkIndex[trackColIndex])) = truthData[index].truthToTrackIndices[trackColIndex];
                (*(m_mc_prob[trackColIndex]))        = truthData[index].truthToTrackMatchingProbabilities[trackColIndex];
            }
            // get classification
            m_classifications = truthData[index].classifications;

            if (m_fillJets)
              m_mc_jetLinkIndex = truthData[index].truthToJetIndex;

            // do statistics:
            for (unsigned int classIndex = 0; classIndex < m_classifications.size(); ++classIndex ) {
                m_truthTrackCounts[classIndex][m_classifications[classIndex]]->Fill(fabs(m_mc_eta));
                if (truthData[index].truthToTrackIndices[0].size() > 0) {
                    // TODO: do statistics for each input track collection
                    m_recoTrackCounts[classIndex][m_classifications[classIndex]]->Fill(fabs(m_mc_eta));
                }
               
            }

        }
        m_nt->Fill();
        m_numberOfTreeEntries++;
    }
    ATH_MSG_DEBUG ("filled " << m_numberOfTreeEntries << " entries, returning...");
    return StatusCode::SUCCESS;
}


unsigned int Trk::TruthNtupleTool::getNumberOfTreeRecords() const {
    return m_numberOfTreeEntries;
}
