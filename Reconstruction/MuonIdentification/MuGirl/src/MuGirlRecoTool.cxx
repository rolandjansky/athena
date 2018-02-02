/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"

#include "MuGirlRecoTool.h"

#include "MuGirlInterfaces/ICandidateTool.h"
#include "MuGirlInterfaces/IPerformanceTruthTool.h"
#include "MuGirlInterfaces/IGlobalFitTool.h"
#include "MuGirlInterfaces/IANNSelectionTool.h"
#include "MuGirlInterfaces/IMuGirlParticleCreatorTool.h"
#include "MuGirlInterfaces/IStauTool.h"
#include "MuGirlInterfaces/SegmentManager.h"
#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"

#include "ICaloTrkMuIdTools/ICaloMuonLikelihoodTool.h"
#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "MuonSegment/MuonSegment.h"

#include "MuonHoughPatternTools/MuonLayerHoughTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkCaloExtension/CaloExtension.h"

#include "MuonIdHelpers/MuonStationIndex.h"

#include "MuonCombinedEvent/MuGirlTag.h"
#include "MuonCombinedEvent/MuGirlLowBetaTag.h"

using namespace MuGirlNS;
using namespace std;

MuGirlRecoTool::MuGirlRecoTool(const std::string& type, const std::string& name, const IInterface* parent) :
AthAlgTool(type, name, parent),
m_applyLHR("On"),
m_pSegmentManager(0),
m_pClusCollection(0),
m_pCandNTuple(0),
m_doLHR(true),
m_doANNSelection(true),
m_doGlobalFit(true),
m_doStau(true),
m_doRH(false),
m_doSAFit(true),
m_doHoughTransform(false),
m_doNTuple(false),
m_doTruth(false),
m_ptLowerLimit(3000.0),
m_stauPtCut(30000.0),
m_defaultLHR(0.85),
m_ntupleName("/NTUPLES/MUGIRL/MUGIRL"),
m_ntupleTitle("Muon Candidates"),
m_CaloCollection("CaloCalTopoCluster"),
m_pCandidate("MuGirlNS::CandidateTool"),
m_pTruthTool("MuGirlNS::PerformanceTruthTool", 0), // this is a public tool
m_pGlobalFitTool("MuGirlNS::GlobalFitTool", 0), // this is a public tool
m_pANNSelectionTool("MuGirlNS::ANNSelectionTool", 0), // this is a public tool
m_pParticleCreatorTool("MuGirlNS::MuGirlParticleCreatorTool", 0), // this is a public tool // use this ?? 
m_pStauTool("MuGirlNS::StauTool", 0), // this is a public tool
m_pMuLHR("CaloMuonLikelihoodTool"),
m_pMuonLayerHoughTool("Muon::MuonLayerHoughTool/MuonLayerHoughTool"),
m_MuonIdHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
m_magFieldSvc("MagField::AtlasFieldSvc/AtlasFieldSvc", name)
{

    declareInterface<MuGirlNS::IMuGirlRecoTool>(this);

    // Steering properties
    declareProperty("doLHR", m_doLHR);
    declareProperty("doANNSelection", m_doANNSelection);
    declareProperty("doGlobalFit", m_doGlobalFit);
    declareProperty("doNTuple", m_doNTuple);
    declareProperty("doStau", m_doStau);
    declareProperty("doRH", m_doRH);
    declareProperty("doSAFit", m_doSAFit);
    declareProperty("doTruth", m_doTruth);
    declareProperty("doHoughTransform", m_doHoughTransform);

    // Variables and threshold cuts
    declareProperty("PtLowerLimit", m_ptLowerLimit);
    declareProperty("StauPtCut", m_stauPtCut);
    declareProperty("DefaultLHR", m_defaultLHR);

    // Container and collection names
    declareProperty("NtupleName", m_ntupleName);
    declareProperty("NtupleTitle", m_ntupleTitle);
    declareProperty("InCaloCollection", m_CaloCollection);

    // Tools
    declareProperty("MuGirlCandidate", m_pCandidate);
    declareProperty("PerformanceTruthTool", m_pTruthTool);
    declareProperty("MuGirlGlobalFitTool", m_pGlobalFitTool);
    declareProperty("ANNSelectionTool", m_pANNSelectionTool);
    declareProperty("MuGirlParticleCreatorTool", m_pParticleCreatorTool);
    declareProperty("MuGirlStauTool", m_pStauTool);
    declareProperty("CaloMuonLikelihoodTool", m_pMuLHR);
    declareProperty("MuonLayerHoughTool", m_pMuonLayerHoughTool);
    declareProperty("ParticleCaloExtensionTool", m_caloExtensionTool);
}

MuGirlRecoTool::~MuGirlRecoTool()
{
    delete m_pSegmentManager;
}

void
MuGirlRecoTool::print_configuration() const
{
    ATH_MSG_INFO("MuGirlReco Steering configuration:");
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doLHR.name() << m_doLHR.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doANNSelection.name() << m_doANNSelection.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doGlobalFit.name() << m_doGlobalFit.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doNTuple.name() << m_doNTuple.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doStau.name() << m_doStau.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doRH.name() << m_doRH.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doSAFit.name() << m_doSAFit.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doTruth.name() << m_doTruth.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_doHoughTransform.name() << m_doHoughTransform.value());
    ATH_MSG_INFO("Thresholds and cuts configuration:");
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_ptLowerLimit.name() << m_ptLowerLimit.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_stauPtCut.name() << m_stauPtCut.value());
    ATH_MSG_INFO("Containers and Collections name:");
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_ntupleName.name() << m_ntupleName.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_ntupleTitle.name() << m_ntupleTitle.value());
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_CaloCollection.name() << m_CaloCollection.value());
    ATH_MSG_INFO("Tools:");
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pCandidate.name() << m_pCandidate);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pTruthTool.name() << m_pTruthTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pGlobalFitTool.name() << m_pGlobalFitTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pANNSelectionTool.name() << m_pANNSelectionTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pParticleCreatorTool.name() << m_pParticleCreatorTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pStauTool.name() << m_pStauTool);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pMuLHR.name() << m_pMuLHR);
    ATH_MSG_INFO(setw(32) << setfill('.') << setiosflags(ios::left) << m_pMuonLayerHoughTool.name() << m_pMuonLayerHoughTool);
}

StatusCode MuGirlRecoTool::initialize()
{

    ATH_MSG_INFO("Initializing MuGirlRecoTool - package version " << PACKAGE_VERSION);
    print_configuration();

    m_pSegmentManager = new MuGirlNS::SegmentManager;

    // Initialize tools
    ATH_CHECK(m_pTruthTool.retrieve());
    if( m_doLHR ) ATH_CHECK(m_pMuLHR.retrieve());
    ATH_CHECK(m_pGlobalFitTool.retrieve());
    ATH_CHECK(m_pANNSelectionTool.retrieve());
    ATH_CHECK(m_pParticleCreatorTool.retrieve());

    if (m_doStau) {
        ATH_CHECK(m_pStauTool.retrieve());
        m_pStauTool->setSegmentManager(m_pSegmentManager);
        m_pStauTool->initTool(m_doGlobalFit);
        if (m_doSAFit) m_pStauTool->initTool(m_doSAFit);
    }

    if (m_pCandidate.retrieve().isFailure())
        return StatusCode::RECOVERABLE;
    m_pCandidate->setSegmentManager(m_pSegmentManager);

    // Initialize NTuple
    if (m_doNTuple) {
        ServiceHandle<INTupleSvc> ntupleSvc("NTupleSvc/NTupleSvc", this->name());
        if (ntupleSvc.retrieve().isFailure()) {
            ATH_MSG_WARNING("initialize: cannot retrieve the NTuple Svc. MUgirlNTuple will not be initialized!");
        } else {
            m_pCandNTuple = ntupleSvc->book(m_ntupleName, CLID_ColumnWiseTuple, m_ntupleTitle);
            if (m_pCandNTuple == NULL) {
                ATH_MSG_ERROR("initialize: could not create NTuple " << m_ntupleName);
                return StatusCode::RECOVERABLE;
            }
            if (m_pCandidate->bookNTuple(m_pCandNTuple).isFailure()) {
                ATH_MSG_ERROR("initialize: could not book Candidate NTuple " << m_ntupleName);
                return StatusCode::RECOVERABLE;
            }

            if (m_doTruth && m_pTruthTool->bookNTuple(m_pCandNTuple).isFailure()) {
                ATH_MSG_ERROR("initialize: could not book Truth NTuple " << m_ntupleName);
                return StatusCode::RECOVERABLE;
            }
            if (m_doGlobalFit && m_pGlobalFitTool->bookNTuple(m_pCandNTuple, "").isFailure()) {
                ATH_MSG_ERROR("initialize: could not book GlobalFit NTuple " << m_ntupleName);
                return StatusCode::RECOVERABLE;
            }
            if (m_doStau) {
                if (m_pStauTool->bookNTuple(m_pCandNTuple).isFailure()) {
                    ATH_MSG_ERROR("initialize: could not book Stau NTuple " << m_ntupleName);
                    return StatusCode::RECOVERABLE;
                }
            }
        } // end of ntuple booking
    } // end of NTuple initialization

    if (m_magFieldSvc.retrieve().isFailure()) {
        ATH_MSG_ERROR("initialize: could not retrieve magFieldSvc");
        return StatusCode::RECOVERABLE;
    }

    if (m_doHoughTransform) {
        if (m_pMuonLayerHoughTool.retrieve().isFailure()) {
            ATH_MSG_WARNING("initialize: could not retrieve the MuonLayerHoughTool");
            ATH_MSG_WARNING("initialize: info from the Hough transform will not be in the MuGirl ntuple");
            return StatusCode::RECOVERABLE;
        } else {
            ATH_MSG_INFO("MuonLayerHoughTool successfully retrieved");
        }
    }

    if (m_MuonIdHelperTool.retrieve().isFailure()) {
        ATH_MSG_WARNING("initialize: failed to retrieve tool " << m_MuonIdHelperTool);
        return StatusCode::RECOVERABLE;
    }
    ATH_MSG_INFO("Retrieved tool " << m_MuonIdHelperTool);

    InterfaceID id = IMuGirlRecoTool::interfaceID();

    ATH_MSG_INFO("Interface id=" << id.id() << ",  majorVersion=" << id.majorVersion() << ",  minorVersion=" << id.minorVersion());

    this->clear_hough_data();

    if (!m_caloExtensionTool.empty()) ATH_CHECK(m_caloExtensionTool.retrieve());

    ATH_MSG_INFO("MuGirlrecoTool initialized successfully");

    return StatusCode::SUCCESS;
}

StatusCode MuGirlRecoTool::MuGirlReco(const InDetCandidateCollection& InDetSeeds)
{

    ATH_MSG_DEBUG("MuGirlReco( .. ) executing.");

    /** Terminate the processing if the Solenoid is off */
    if (!m_magFieldSvc->solenoidOn()) {
        ATH_MSG_DEBUG("Solenoid is off: Execute done");
        return StatusCode::SUCCESS;
    }

    /** Hough Data processing */
    if (m_doHoughTransform && this->processHoughData().isFailure()) {
        ATH_MSG_INFO("Hough data cannot be processed successfully, cleaning Hough data structure ....");
        this->clear_hough_data();
    }

    /** Retrieve the CaloCluster container */
    if (evtStore()->contains<xAOD::CaloClusterContainer>(m_CaloCollection)) {
      if (evtStore()->retrieve(m_pClusCollection, m_CaloCollection).isFailure() || m_pClusCollection == NULL) {
        ATH_MSG_WARNING("MuGirlReco: cannot retrieve ClusterContainer");
      } else
        ATH_MSG_DEBUG("Calo Collection Size: " << m_pClusCollection->size());
    }


    /** Create the CaloParticles */
    if (msgLvl(MSG::DEBUG)) listTrackParticles(InDetSeeds);

    Amg::Vector3D pt_calo_local(0, 0, 0);
    int iPart = 0;
    for (InDetCandidateCollection::const_iterator ipIt = InDetSeeds.begin();
            ipIt != InDetSeeds.end();
            ++ipIt, iPart++) {
        const xAOD::TrackParticle& tp_id = (*ipIt)->indetTrackParticle();
        const Trk::Perigee& pPerigee = tp_id.perigeeParameters();

        /**
         * Check also the track pt as the pt cut of the TrackSelectorTool may be different.
         */
        double pt = pPerigee.pT();
        if (pt < m_ptLowerLimit) {
          ATH_MSG_DEBUG("pT=" << pt << " is lower than limit = " << m_ptLowerLimit << ". Skipping track.");
          continue;
        }


        const Trk::TrackParameters* extrParameters = NULL;
        /**
           get the extrapolation to the MS entry
        */
        const Trk::CaloExtension* extension = 0;

	if (m_caloExtensionTool.empty()) continue;
	if (!m_caloExtensionTool->caloExtension(tp_id, extension)) continue;
	if (!extension) continue; 
	if (!extension->muonEntryLayerIntersection() && extension->caloLayerIntersections().empty()) continue;
	
	ATH_MSG_DEBUG("Seen by calo");

        // if available use muon entry intersection
        if (extension->muonEntryLayerIntersection()) {
            extrParameters = extension->muonEntryLayerIntersection();
            pt_calo_local = extrParameters->position();
        } else {
            // use last calo layer
            pt_calo_local = extension->caloLayerIntersections().back()->position();
        }

        ATH_MSG_DEBUG("CaloExtensionTool: pt_calo_local " << pt_calo_local.x() << ", z=" << pt_calo_local.z());

        // FIXME: const_cast
        CaloParticle* pParticle = new CaloParticle(const_cast<MuonCombined::InDetCandidate*>(*ipIt), &tp_id, extrParameters, pt_calo_local.eta(), pt_calo_local.phi());
        m_caloParticles.push_back(pParticle);
    }
    ATH_MSG_DEBUG("CaloExtensionTool: size of CaloParticles " << m_caloParticles.size());


    /** Run the MuGirl reconstruction */
    MuGirlNS::CandidateSummaryList summaryList;
    this->RunFromID(summaryList);
    ATH_MSG_DEBUG("MuGirlReco: number of summaries=" << summaryList.size());


    /** in order to fill the ntuple storing the MuGirl results */
    if( m_doNTuple && m_pParticleCreatorTool->fillContainer(&summaryList, true).isFailure()) {
      ATH_MSG_DEBUG("MuGirlReco: m_pParticleCreatorTool->fillContainer failed.");
    }

    /** empty caloParticles */
    for (unsigned i = 0; i < m_caloParticles.size(); i++) {
        delete m_caloParticles[i];
    }
    m_caloParticles.clear();

    /** Clear the Summary List */
    summaryList.clear();

    ATH_MSG_DEBUG("summaryList cleared");

    /** Clear the temporary data after the reconstruction */
    m_pCandidate->candidateClear();
    m_pSegmentManager->clear();

    /** clear Hough data */
    if (m_doHoughTransform) {
        ATH_MSG_DEBUG("After MuGirlReco n. " << this->hough_data_size() << " Hough maxima are unassociated");

        if (this->hough_data_size() && m_doNTuple) {
            /** put dummy candidate in the ntuple in order to deliver the Hough maxima which has been left unassociated */
            if (m_pCandidate->fillNTuple(m_hough_data).isFailure())
                ATH_MSG_WARNING("execute: cannot fill the unassociated maxima in the NTuple");
        }

        this->clear_hough_data();
    }


    ATH_MSG_DEBUG("MuGirlReco( .. ) done.");

    return StatusCode::SUCCESS;
}

bool MuGirlRecoTool::RunFromID(MuGirlNS::CandidateSummaryList& summaryList)
{

    int iTrack = 0; /** counts the number of track processed */

    /** start looping on calo particles */
    for (unsigned int i = 0; i < m_caloParticles.size(); i++) {
        std::unique_ptr<MuGirlNS::CandidateSummary> summary(new MuGirlNS::CandidateSummary);

        CaloParticle* pParticle = m_caloParticles[i];

        ATH_MSG_DEBUG("RunFromID: start processing CaloParticle seed at eta=" << pParticle->eta << " phi=" << pParticle->phi);

        const xAOD::TrackParticle* pTrackParticle = pParticle->pTrackParticle;
        const Trk::TrackParameters* extrMuonEntry = pParticle->pTrackParameters;

        bool doStau = m_doStau && pTrackParticle->perigeeParameters().pT() >= m_stauPtCut;
        bool doRH = m_doRH && doStau;

        if (!m_pCandidate->fill(iTrack, pTrackParticle, extrMuonEntry, doStau).isSuccess()) continue;

        if (m_pClusCollection && m_doLHR && m_pMuLHR && pParticle->isolated && m_pClusCollection->size() > 0)
          calculateLHR(pParticle);
        else{
          // in case we are running without LHR, set dummy values 
          m_pCandidate->setLHR(m_defaultLHR);
        }
        bool ANNaccept = false;

        if (m_pCandidate->fillSummary(summary.get()).isFailure()) // what is the point of this?
            ATH_MSG_DEBUG("RunFromID: cannot fill summary");
        else
            if (m_doANNSelection) ANNaccept = generateMuGirl(*summary);

	if(pParticle->pInDetCandidate->isSiliconAssociated()) summary->isSiliconAssociated=true;

        const Trk::Track* RefittedTrack = NULL;

        if ((m_doANNSelection && ANNaccept) || !(m_doANNSelection)) {
            summary->saveAnn = 1;

            if (m_doGlobalFit && (pTrackParticle->trackLink().isValid())&&
                    (m_pCandidate->vectorMuonSegments()).size() > 0 && summary->numMdtHitsInSeg > 0 ) {
                ATH_MSG_DEBUG("RunFromID: performing the global track fit.  muonSegemnts.size=" << (m_pCandidate->vectorMuonSegments()).size()
                              << " nmdts " << summary->numMdtHits << " nmdtSegs " << summary->numMdtSegs << " nmdtHitsInSeg " << summary->numMdtHitsInSeg);
                const Trk::Track* originalTrack = *pTrackParticle->trackLink();

                //// does it call backextrap ??
                RefittedTrack = m_pGlobalFitTool->globalFit(originalTrack, m_pCandidate->vectorMuonSegments(), m_doNTuple);


                if (RefittedTrack != NULL) {
                    ATH_MSG_DEBUG("Set a NULL Refitted xAOD::TrackParticle into the CandidateTool");
                    m_pCandidate-> fillRefittedTrack(NULL, RefittedTrack, summary.get());


                    if (m_doSAFit) doSAFit(RefittedTrack, *summary);
                } else ATH_MSG_DEBUG("RunFromID: Global Fit produced a NULL Refitted Track");
            }
        } else {
            ATH_MSG_DEBUG("RunFromID: Candidate is not a muon track, filling a NULL summary");
            m_pCandidate-> fillRefittedTrack(NULL, NULL, summary.get());
        }

        if (m_doNTuple) doHoughTransformForNtuple(pTrackParticle, *summary);

        /**
         * Stau processing
         */
        bool passedStau = false;
        if (doStau && summary->numMdtHits > 4 && (summary->numTgcEtaHits > 2 || summary->numTgcPhiHits > 2 || 
                                                  summary->numRpcEtaHits > 2 || summary->numRpcPhiHits > 2) ) {
            ATH_MSG_DEBUG("RunFromID: looking for staus ...");
            m_pCandidate->setPassdoStau(true);
            //process stau
            const Trk::Track* pMuonRefittedTrack = RefittedTrack; //NULL;

            if (m_pStauTool->processStau(pTrackParticle,
                                         m_pCandidate->vectorMuonSegments(),
                                         m_pCandidate->getMdtSegmentMakerInfo(),
                                         m_pCandidate->getRpcHitsInSegments(),
                                         m_pCandidate->getTgcHitsInSegments(),
                                         pMuonRefittedTrack, NULL).isFailure()) continue;

            double Beta = m_pStauTool->beta();
            double stauMass = m_pStauTool->mass();
            summary->beta = Beta;

            if (Beta > 0.2) {

                ATH_MSG_DEBUG("stau Beta: " << Beta << ", mass: " << stauMass);
                //	     MuGirlNS::CandidateSummary* stauSummary = new MuGirlNS::CandidateSummary();

                std::unique_ptr<MuGirlNS::CandidateSummary> stauSummary(new MuGirlNS::CandidateSummary);
                std::unique_ptr<MuGirlNS::StauExtras> stauExtras(new MuGirlNS::StauExtras);

                if (m_pStauTool->fillStauSummary(summary.get(), stauSummary.get()).isFailure())
                    ATH_MSG_DEBUG("RunFromID: Cannot fill stauSummary");
                bool ANNstau = generateMuGirl(*stauSummary);
                ATH_MSG_DEBUG("RunFromID: stau passANN " << ANNstau);
                if (stauSummary->nnBarrel > 0.2 || stauSummary->nnEndCap > 0.2) ANNstau = true;
                if (m_pStauTool->fillStauExtras(stauSummary.get(), stauExtras.get()).isFailure())
                    ATH_MSG_DEBUG("RunFromID: Cannot fill stauExtras");

                //Store in the low beta container either if pass stau ann 
                if (ANNstau) {
                    const Trk::Track* stauRefittedTrack = m_pStauTool->refittedTrack();
                    if (NULL != stauRefittedTrack) {
                        stauSummary->pTrkRefitted = stauRefittedTrack;

                        //stau
                        ATH_MSG_DEBUG("MuGirlRecoTool: stauRefittedTrack " << (m_pCandidate->vectorMuonSegments()).size());

                        std::vector<const Muon::MuonSegment*> stauSegmentList = *m_pStauTool->newMdtSegments();
                        m_pSegmentManager->setInStore(stauSegmentList, true);

                        MuonCombined::MuGirlLowBetaTag* lowbetatag = new MuonCombined::MuGirlLowBetaTag(stauRefittedTrack,stauSegmentList);

                        //dress tag
                        stauSummary->saveStau = 1;
                        lowbetatag->setMuBeta(Beta);
                        lowbetatag->setStauSummary(stauSummary.release());
                        lowbetatag->setStauExtras(stauExtras.release());

                        pParticle->pInDetCandidate->addTag(*lowbetatag);

                    } else {
                        ATH_MSG_DEBUG("stauRefittedTrack is NULL");
                        stauSummary->pRefittedTrack = NULL;
                        stauSummary->pTrkRefitted = NULL;
                    }
                    passedStau = true;
                }// ANNstau

            }// beta>0.2

            if (m_doNTuple) {
                if (m_pStauTool->fillNTuple().isFailure())
                    ATH_MSG_DEBUG("RunFromID: stauTool->fillNtuple failed ");
            }

            if (!passedStau) {
                ATH_MSG_DEBUG("RunFromID: passedStau == false");
                auto newMdtSegments = m_pStauTool->newMdtSegments();
                if (newMdtSegments != nullptr) {
                    for (auto pMuonSeg : *newMdtSegments)
                        m_pSegmentManager->addSegment(pMuonSeg, MuGirlNS::SegmentManager::MDT_STAU);
                }
                if (doRH) {
                    ATH_MSG_DEBUG("RunFromID: doRH == true");
                    std::unique_ptr<MuGirlNS::RHExtras> rhExtras(new MuGirlNS::RHExtras);
                    if (m_pStauTool->fillRHExtras(rhExtras.get()).isFailure())
                        ATH_MSG_DEBUG("RunFromID: Cannot fill rhExtras");
                    if (rhExtras->numCaloCells > 0) {
                        MuonCombined::MuGirlLowBetaTag* lowbetatag
                                = new MuonCombined::MuGirlLowBetaTag(pTrackParticle->track(), std::vector<const Muon::MuonSegment*>());
                        lowbetatag->setRHExtras(rhExtras.release());
                        pParticle->pInDetCandidate->addTag(*lowbetatag);
                    }
                    ATH_MSG_DEBUG("RunFromID: doRH == true - done");
                }
                ATH_MSG_DEBUG("RunFromID: passedStau == false - done");
            }

        } //stau processing

        if (summary->saveAnn == 1) {
            ATH_MSG_DEBUG("RunFromID: delivering the muon summary to the summaryList");

            //Generate the MuGirlTag according to the new running schema: MuGirlTag will delete the segments 
            //Otherwise put the list of MuGirl Segments into Storegate: Storegate will delete the segments   


            ATH_MSG_DEBUG("RunFromID: delivering the muon pTrkRefitted track to the MuGirl tag");
            std::vector<const Muon::MuonSegment*> muonSegmentList = summary->muonSegmentList;
            MuonCombined::MuGirlTag* tag = 0;
            if (summary->pTrkRefitted == NULL){
	      if(summary->isSiliconAssociated) continue;
	      tag = new MuonCombined::MuGirlTag(muonSegmentList);
	    }
            else tag = new MuonCombined::MuGirlTag(summary->pTrkRefitted, muonSegmentList);
	    tag->setUpdatedExtrapolatedTrack(std::move(summary->pTrkMSRefitted));
            pParticle->pInDetCandidate->addTag(*tag);
            //set the segment into SegmentManager undeletable 
            m_pSegmentManager->setInStore(muonSegmentList, true);
            summary->saveStau = 0;
            //m_pCandidate->releaseMuonSegments(false);   //the segments released from Candidate ownership
            //summaryList.push_back(summary.release());
        } else {
            if (!passedStau) continue;

            //  m_pCandidate->releaseMuonSegments(true);		   
            //Generate the MuGirlTag according to the new running schema: MuGirlTag will delete the segments 

            ATH_MSG_DEBUG("RunFromID: delivering the pTrkLowBeta to the MuGirl tag");
            std::vector<const Muon::MuonSegment*> muonSegmentList = summary->muonSegmentList;
            MuonCombined::MuGirlTag* tag = new MuonCombined::MuGirlTag(summary->pTrkLowBeta, muonSegmentList);
	    tag->setUpdatedExtrapolatedTrack(nullptr);
            pParticle->pInDetCandidate->addTag(*tag);
            //set the segment into SegmentManager undeletable 
            m_pSegmentManager->setInStore(muonSegmentList, true);

        }

        if (m_doNTuple) {
            //if (m_pCandNTuple->writeRecord().isFailure())
            //      ATH_MSG_WARNING("RunFromID: Cannot write NTuple");
        }
        ATH_MSG_DEBUG("Done processing track " << iTrack << " number of cand: " << summaryList.size());
        iTrack++;
    }

    return true;
}

StatusCode MuGirlRecoTool::execute()
{
    ATH_MSG_DEBUG("execute() not called .");

    return StatusCode::SUCCESS;
}

bool MuGirlRecoTool::generateMuGirl(MuGirlNS::CandidateSummary& summary)
{
    ATH_MSG_DEBUG("generateMuGirl");

    double eta = 999.;
    double pt = 0.;
    if (summary.startFromEF && summary.pMuonEFTrack != NULL) {
      //eta = (summary.pMuonEFTrack)->eta();
      //pt = (summary.pMuonEFTrack)->pt();
      ATH_MSG_WARNING("Starting from EF not supported");
    } else {
        eta = (summary.pTrackParticle)->perigeeParameters().eta();
        pt = (summary.pTrackParticle)->perigeeParameters().pT();
    }

    ATH_MSG_DEBUG("generateMuGirl:  seed eta=" << eta << " and seed pT=" << pt);

    if (fabs(eta) > 2.0 && (summary.numMdtHits <= 2 && summary.numCscSegs == 0 && summary.numTgcEtaHits <= 2 && summary.numTgcPhiHits <= 2))
        return false;
    if (fabs(eta) < 2.0 && summary.numMdtHits <= 2)
        return false;

    ATH_MSG_DEBUG("generateMuGirl:  eta/hits topology satisfied");

    if ((summary.innAngle == 999.) && (summary.midAngle == 999.))
        return false;
    //     double nsegs = summary.numMdtSegs + summary.numCscSegs + summary.numRpcSegs + summary.numTgcSegs;
    //     if (nsegs == 0)
    //         return false;

    double value[8];
    double ANN[2]{0, 0};
    m_pANNSelectionTool->evaluateANN(&summary, value, ANN);
    m_pCandidate->setAnn(ANN[0], ANN[1]);
    summary.nnBarrel = ANN[0];
    summary.nnEndCap = ANN[1];

    ATH_MSG_DEBUG(" ANN [0] " << value[0] << " ANN [1] " << value[1] << " ANN [2] " << value[2] << " ANN [3] " << value[3] << " ANN [4] " << value[4] << " ANN [5] " << value[5]);
    /*
        if (value[0]>0.3 ||    // BARREL GOOD  PT<10GeV
                value[1]>0.2  ||    // BARREL BAD
                value[2]>0.3 ||    // REGION OVERLAP
                value[3]>0.25 ||     // MAGNET OVERLAP
                value[4]>0.45 ||    // ENDCAP
                value[5]>0.4 )      // FORWARD
             //   value[6] > m_barrelNNCut || //TEST BARREL PT>10GeV
             //   value[7] > m_endcapNNCut)   // TEST ENDCAP PT>10GeV
     */

    if (pt > 5000) {
        /*
         if (value[0]>0.18 ||    // BARREL GOOD  PT>5GeV
             value[1]>0.2  ||    // BARREL BAD
             value[2]>0.2 ||    // REGION OVERLAP
             value[3]>0.18 ||     // MAGNET OVERLAP
         */
        if (value[0] > 0.55 || // BARREL GOOD  PT>5GeV
                value[1] > 0.55 || // BARREL BAD
                value[2] > 0.55 || // REGION OVERLAP
                value[3] > 0.55 || // MAGNET OVERLAP
                value[4] > 0.2 || // ENDCAP
                value[5] > 0.3) // FORWARD
        {

            summary.PassNNsel = true;
            m_pCandidate->setPassANN(true);
            return true;
        } else {
            summary.PassNNsel = false;
            m_pCandidate->setPassANN(false);
            return false;
        }
    } else {
        if (value[0] > 0.55 || // BARREL GOOD  PT<5GeV
                value[1] > 0.55 || // BARREL BAD
                value[2] > 0.55 || // REGION OVERLAP
                value[3] > 0.55 || // MAGNET OVERLAP
                value[4] > 0.2 || // ENDCAP
                value[5] > 0.3) // FORWARD
        {

            summary.PassNNsel = true;
            m_pCandidate->setPassANN(true);
            return true;
        } else {
            summary.PassNNsel = false;
            m_pCandidate->setPassANN(false);
            return false;
        }
    }

}


void MuGirlRecoTool::calculateLHR(CaloParticle* pParticle)
{
  if( !m_pClusCollection || !m_doLHR || !m_pMuLHR || !pParticle->isolated || m_pClusCollection->empty() ){
    // in case we are running without LHR, set dummy values 
    m_pCandidate->setLHR(m_defaultLHR);
    return;
  }

    const xAOD::TrackParticle* pTrackParticle = pParticle->pTrackParticle;
    const Trk::Perigee pPerigee = pTrackParticle->perigeeParameters();
    double charge_id = pTrackParticle->charge();
    double qOverP_id = pPerigee.parameters()[Trk::qOverP];
    double p_id = fabs(1.0 / qOverP_id);
    double phi_id = pPerigee.parameters()[Trk::phi];
    double eta_id = asinh(1. / tan(pPerigee.parameters()[Trk::theta]));
    double pt_id = pPerigee.pT();

    ATH_MSG_DEBUG("Parameters of isolated tracks seen by calo: " <<
            " pt =  " << pt_id <<
            " eta = " << eta_id <<
            " phi = " << phi_id <<
            " charge = " << charge_id);

    double theLHR = m_pMuLHR->getLHR(m_pClusCollection, eta_id, p_id, pParticle->eta, pParticle->phi);
    ATH_MSG_DEBUG("theLHR = " << theLHR);

    m_pCandidate->setIsolation(1);
    m_pCandidate->setLHR(theLHR);

    ATH_MSG_DEBUG("calculate LHR done = " << theLHR);
}

StatusCode MuGirlRecoTool::processHoughData()
{
    int NumMaxima = 0;

    if (m_pMuonLayerHoughTool) {
        Muon::MuonLayerHoughTool::HoughDataPerSectorVec data = m_pMuonLayerHoughTool->houghData();
        ATH_MSG_DEBUG(data.size() << " sector are present in the HoughData");
        for (unsigned int sector = 0; sector < data.size(); sector++) {
            int sector_id = -99;
            bool hits_in_same_sector = true;
            Muon::MuonLayerHoughTool::HoughDataPerSector sector_data = data.at(sector);
            ATH_MSG_DEBUG("----------------------------- Sector " << sector
                    << " -----------------------------");
            for (unsigned int region = 0; region < sector_data.maxVec.size(); region++) {
                std::pair<Muon::MuonStationIndex::DetectorRegionIndex, Muon::MuonStationIndex::LayerIndex> coordinate =
                        Muon::MuonStationIndex::decomposeSectorLayerHash(region);
                Muon::MuonStationIndex::DetectorRegionIndex det_reg_id = coordinate.first;
                Muon::MuonStationIndex::LayerIndex det_lay_id = coordinate.second;
                std::string det_reg_str = Muon::MuonStationIndex::regionName(det_reg_id);
                std::string det_lay_str = Muon::MuonStationIndex::layerName(det_lay_id);

                int region_id = -99;
                bool hits_in_same_region = true;
                /**  Old code used when the region was not handled directly within the MuonIdHelper package.
                std::string tag;
                switch (region) {
                  case 0 : tag="BI station        "; break;
                  case 1 : tag="BM station        "; break;
                  case 2 : tag="BO station        "; break;
                  case 3 : tag="BE station, side C"; break;
                  case 4 : tag="EI station, side C"; break;
                  case 5 : tag="EM station, side C"; break;
                  case 6 : tag="EO station, side C"; break;
                  case 7 : tag="EE station, side C"; break;
                  case 8 : tag="BE station, side A"; break;
                  case 9 : tag="EI station, side A"; break;
                  case 10: tag="EM station, side A"; break;
                  case 11: tag="EO station, side A"; break;
                  case 12: tag="EE station, side A"; break;
                  default: tag=""; break;
                }
                 */
                ATH_MSG_DEBUG("------------------- RegLay "
                        << setw(14) << setfill(' ') << setiosflags(ios::left) << det_lay_str
                        << setw(7) << setfill(' ') << setiosflags(ios::left) << det_reg_str
                        << " ------------------");
                std::vector<MuonHough::MuonLayerHough::Maximum*> region_data = sector_data.maxVec.at(region);
                for (unsigned int maximum = 0; maximum < region_data.size(); maximum++) {
                    MuonHough::MuonLayerHough::Maximum* max = region_data.at(maximum);
                    std::vector<MuonHough::Hit*>::iterator it = max->hits.begin();
                    int layer_id = -99;
                    bool hits_in_same_layer = true;

                    // compute MuGirl region index and check the consistency with the regLayer of the Hough transform
                    int hough_region = det_reg_id;
                    int MuGirl_region = (hough_region == 1) ? 0 : 1;

                    // the computation of MuGirl distance requires the Hough hits
                    int MuGirl_distance = 0;

                    ATH_MSG_DEBUG("----------------------------- Maxim. " << maximum
                            << " -----------------------------");
                    float max_value = max->max;
                    float max_position = max->pos;
                    float max_tetha = max->theta;
                    float max_tConfirm = max->triggerConfirmed;
                    float max_refPos;
                    float max_posMin;
                    float max_posMax;
                    int dummy = 0;
                    max->hough->pars(max->binposmin, dummy, max_refPos, max_posMin);
                    max->hough->pars(max->binposmax, dummy, max_refPos, max_posMax);

                    ATH_MSG_DEBUG("maximum value: " << max_value << ",  # of trigger hits associated: " << max_tConfirm);
                    ATH_MSG_DEBUG("reference layer:" << ((region > 2) ? "Z=" : "R=") << max_refPos
                            << ",  maximum position:" << ((region > 2) ? "R=" : "Z=") << max_position << ((region > 2) ? ",  R range: [" : ",  Z range: [")
                            << max_posMin << "," << max_posMax << "]");
                    ATH_MSG_DEBUG("tetha position:" << max_tetha);

                    while (it != max->hits.end()) {
                        const MuonHough::HitDebugInfo* info = (*it)->debugInfo();

                        if (sector_id == -99) sector_id = info->sector;
                        else hits_in_same_sector &= (sector_id == info->sector);

                        if (region_id == -99) region_id = info->region;
                        else hits_in_same_region &= (region_id == info->region);

                        if (layer_id == -99) layer_id = info->layer;
                        else hits_in_same_layer &= (layer_id == info->layer);

                        std::string side = "";
                        if ((*it)->prd) {
                            MuGirl_distance = det_lay_id; //m_pMuonLayerHoughTool->lay( (*it)->prd->identify() );
                            std::string chamber_name = m_MuonIdHelperTool->chamberNameString((*it)->prd->identify());
                            if (msgLvl(MSG::DEBUG)) {
                                const Amg::Vector2D locPos = (*it)->prd->localPosition();
                                const Trk::TrkDetElementBase* detEl = (*it)->prd->detectorElement();
                                const Amg::Vector3D* gPos = detEl->surface((*it)->prd->identify()).localToGlobal(locPos);
                                if (m_MuonIdHelperTool->isEndcap((*it)->prd->identify())) {
                                    side = (gPos->z() < 0.) ? ", side C" : ", side A";
                                }
                                ATH_MSG_DEBUG("Hough Hit has a PrePrawData belonging to chamber " << chamber_name.c_str() << side.c_str());
                            }
                        } else {
                            if ((*it)->tgc) {
                                std::vector< const Muon::TgcPrepData* > tgc = (*it)->tgc->etaCluster.hitList;
                                MuGirl_distance = det_lay_id; //lay( tgc[0]->identify() );
                                if (msgLvl(MSG::DEBUG)) {
                                    std::string chamber_name = m_MuonIdHelperTool->chamberNameString(tgc[0]->identify());
                                    const Amg::Vector2D locPos = tgc[0]->localPosition();
                                    const Trk::TrkDetElementBase* detEl = tgc[0]->detectorElement();
                                    const Amg::Vector3D* gPos = detEl->surface(tgc[0]->identify()).localToGlobal(locPos);
                                    if (m_MuonIdHelperTool->isEndcap(tgc[0]->identify())) {
                                        side = (gPos->z() < 0.) ? ", side C" : ", side A";
                                    }
                                    ATH_MSG_DEBUG("Hough Hit has not a PrepRawData but a TgcClusterObj3D belonging to " << chamber_name.c_str() << side.c_str());
                                }
                            } else ATH_MSG_DEBUG("Hough Hit has not a PrepRawData");
                        }

                        // set MuGirl_distance
			//         MuGirl     HoughTransformTool
			// BI      0          0
			// BM      1          2
			// BO      2          3
			// BEE     3          1
                        if (MuGirl_distance >= 1) MuGirl_distance++;
                        if (MuGirl_distance >= 4) MuGirl_distance = 1;//since det_lay_id 0-4. 
                        /** Old check done when the region index was not handled within the MuonIdhelper package.
                        // check consistency with MuGirl indices for every hits
                        if ( (MuGirl_distance==0 && (region!=0&&region!=4&&region!=9))  ||
                             (MuGirl_distance==2 && (region!=1&&region!=5&&region!=10)) ||
                             (MuGirl_distance==3 && (region!=2&&region!=6&&region!=11)) ||
                             (MuGirl_distance==1 && (region!=3&&region!=7&&region!=8&&region!=12)) ) {
                          ATH_MSG_ERROR("Inconsistency between MuGirl distance and Hough transform regLayer: distance=" << MuGirl_distance
                                     << ", regLayer=" << region);
                          return StatusCode::FAILURE;
                        }
                         */
                        it++;
                    }

                    ATH_MSG_DEBUG("MuGirl distance=" << MuGirl_distance << ", MuGirl region=" << MuGirl_region);

                    // keep maximum for the ntuple processing (check that we have
                    m_hough_data[sector][MuGirl_distance][MuGirl_region].push_back(max);

                    ++NumMaxima;

                    if (!hits_in_same_layer) ATH_MSG_WARNING("processHoughData: hits in Hough are not from the same layer");
                    else if (layer_id != -99) ATH_MSG_DEBUG("Hough maximum has hits from layer " << layer_id);
                }
                if (!hits_in_same_region) ATH_MSG_WARNING("processHoughData: hits in Hough are not from the same region");
                else if (region_id != -99) ATH_MSG_DEBUG("Hough maxima belongs to region " << region_id);
            }
            if (!hits_in_same_sector) ATH_MSG_WARNING("processHoughData: hits in Hough are not from the same sector");
            else if (sector_id != -99) ATH_MSG_DEBUG("Hough maxima belongs to sector " << sector_id);
        }
    }

    ATH_MSG_INFO("Retrieved n. " << NumMaxima << " Hough maxima");

    return StatusCode::SUCCESS;
}

void MuGirlRecoTool::associateMaxima(HoughData associated[16][4][2])
{
    std::vector<MuonHough::MuonLayerHough::Maximum*> maxvec;
    for (unsigned int i = 0; i < 16; i++) { // i == sector
        for (unsigned int j = 0; j < 4; j++) { // j == distance
            for (unsigned int k = 0; k < 2; k++) { // k == region
                std::vector<MuonHough::MuonLayerHough::Maximum*>& maxima = m_hough_data[i][j][k];
                std::vector<MuonHough::MuonLayerHough::Maximum*>::iterator m = maxima.begin();
                while (m != maxima.end()) {
                    if (m_pCandidate->isHoughMaxAssociated(*m, i, j, k)) {
                        associated[i][j][k].push_back(*m);
                        m = maxima.erase(m);
                    } else m++;
                }
            }
        }
    }
}

int MuGirlRecoTool::hough_data_size() const
{
    int size = 0;
    for (unsigned int i = 0; i < 16; i++) {
        for (unsigned int j = 0; j < 4; j++) {
            for (unsigned int k = 0; k < 2; k++) {
                size += m_hough_data[i][j][k].size();
            }
        }
    }
    return size;
}

void MuGirlRecoTool::clear_hough_data()
{
    for (unsigned int i = 0; i < 16; i++) {
        for (unsigned int j = 0; j < 4; j++) {
            for (unsigned int k = 0; k < 2; k++) {
                m_hough_data[i][j][k].clear();
            }
        }
    }
}

void MuGirlRecoTool::listTrackParticles(const InDetCandidateCollection& InDetSeeds)
{

    ATH_MSG_DEBUG("******************************************");
    ATH_MSG_DEBUG("********  list of TrackParticles  ********");
    ATH_MSG_DEBUG("******************************************");

    for (InDetCandidateCollection::const_iterator ipIt = InDetSeeds.begin();
            ipIt != InDetSeeds.end();
            ++ipIt) {
        const xAOD::TrackParticle& tp_id = (*ipIt)->indetTrackParticle();
        const Trk::Perigee pPerigee = tp_id.perigeeParameters();
        double charge_id = tp_id.charge();
        double d0_id = pPerigee.parameters()[Trk::d0];
        double z0_id = pPerigee.parameters()[Trk::z0];
        double phi_id = pPerigee.parameters()[Trk::phi];
        double eta_id = asinh(1. / tan(pPerigee.parameters()[Trk::theta]));
        double pt_id = pPerigee.pT();

        if (pt_id < m_ptLowerLimit) continue;

        ATH_MSG_DEBUG("pt =  " << pt_id <<
                " eta = " << eta_id <<
                " phi = " << phi_id <<
                " ch = " << charge_id <<
                " d0 = " << d0_id <<
                " z0 = " << z0_id);
    }

    ATH_MSG_DEBUG("******************************************");
    ATH_MSG_DEBUG("********           end           *********");
    ATH_MSG_DEBUG("******************************************");
}

void MuGirlRecoTool::doHoughTransformForNtuple(const xAOD::TrackParticle* pTrackParticle, MuGirlNS::CandidateSummary& summary)
{


    ATH_MSG_DEBUG("Hough " << m_doHoughTransform);
    // select the hough maxima associated to the candidate
    HoughData associated_maxima[16][4][2];
    if (m_doHoughTransform) {
        ATH_MSG_DEBUG("Associating the Hough Maxima to MuGirl candidate");
        associateMaxima(associated_maxima);
    }

    // filling the candidate info in NTuple
    //ATH_MSG_DEBUG("Calling m_pCandidate->fillNTuple");
    //if (m_pCandidate->fillNTuple().isFailure())
    //  ATH_MSG_WARNING("RunFromID: error during fill NTuple");


    // filling the Hough transfomr info in NTuple
    if (m_doHoughTransform) {
        ATH_MSG_DEBUG("Calling m_pCandidate->fillNTuple(with associated hough maxima)");
        if (m_pCandidate->fillNTuple(associated_maxima).isFailure())
            ATH_MSG_WARNING("RunFromID: Could not include the Hough data into the Candidate NTuple");
    }

    if (m_doNTuple && m_doTruth && pTrackParticle != NULL) {
        ATH_MSG_DEBUG("Calling m_pTruthTool->fillNTuple");

        if (m_pTruthTool->fillNTuple(&summary).isFailure())
            ATH_MSG_WARNING("RunFromID: Cannot fill Truth in NTuple");
    }

    // clear temporay hough data structure
    for (unsigned int i = 0; i < 16; i++)
        for (unsigned int j = 0; j < 4; j++)
            for (unsigned int k = 0; k < 2; k++) associated_maxima[i][j][k].clear();


}

void MuGirlRecoTool::doSAFit(const Trk::Track* RefittedTrack, MuGirlNS::CandidateSummary& summary)
{
  ATH_MSG_DEBUG("RunFromID: looking for an ms track ...");
  std::unique_ptr<const Trk::Track> msTrack =
  std::unique_ptr<const Trk::Track>
    ( m_pGlobalFitTool->standAloneRefit(*RefittedTrack) );
  if (msTrack) {
        msTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuGirl);
        ATH_MSG_DEBUG("MS refit Trk::Track p " << fabs(1. / (msTrack->perigeeParameters()->parameters())[Trk::qOverP]) 
                      << " pt " << msTrack->perigeeParameters()->pT() 
                      << " eta " << msTrack->perigeeParameters()->eta());
    } else {
        ATH_MSG_DEBUG("No refitted Track");
    }
    m_pCandidate-> fillMSTrack(NULL, std::move(msTrack), &summary);
}

