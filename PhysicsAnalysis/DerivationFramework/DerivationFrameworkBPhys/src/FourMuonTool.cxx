/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// FourMuonTool
// James Catmore <James.Catmore@cern.ch>
// Evelina Bouhova-Thacker <e.bouhova@cern.ch>
// ----------------------------------------------------------------------------
// ****************************************************************************

#include "DerivationFrameworkBPhys/FourMuonTool.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"
#include "xAODBPhys/BPhysHelper.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkV0Fitter/TrkV0VertexFitter.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "DataModel/ElementLink.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"

#include <algorithm>

namespace DerivationFramework {
    
    StatusCode FourMuonTool::initialize() {
        
        // retrieving vertex Fitter
        if ( m_iVertexFitter.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_iVertexFitter);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_iVertexFitter);
        }
        
        // retrieving V0 Fitter
        if ( m_iV0VertexFitter.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_iV0VertexFitter);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_iV0VertexFitter);
        }
        
        // Get the track selector tool from ToolSvc
        if ( m_trkSelector.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_trkSelector);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_trkSelector);
        }
        
        // uploading the V0 tools
        if ( m_V0Tools.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_V0Tools);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_V0Tools);
        }
        
        // Get the vertex point estimator tool from ToolSvc
        if ( m_vertexEstimator.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_vertexEstimator);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tool " << m_vertexEstimator);
        }

        // Get the beam spot service
        if ( m_beamSpotSvc.retrieve().isFailure() ) {
             ATH_MSG_FATAL("Failed to retrieve service " << m_beamSpotSvc);
             return StatusCode::FAILURE;
        } else {
             ATH_MSG_INFO("Retrieved service " << m_beamSpotSvc);
        }        

        ATH_MSG_INFO("Initialize successful");
        
        return StatusCode::SUCCESS;
        
    }
    
    StatusCode FourMuonTool::finalize() {
        
        ATH_MSG_INFO("Finalize successful");
        return StatusCode::SUCCESS;
        
    }
    FourMuonTool::FourMuonTool(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
    m_ptCut(0.0),
    m_etaCut(0.0),
    m_useV0Fitter(false),
    m_muonCollectionKey("Muons"),
    m_TrkParticleCollection("TrackParticleCandidate"),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_iV0VertexFitter("Trk::V0VertexFitter"),
    m_V0Tools("Trk::V0Tools"),
    m_trkSelector("InDet::TrackSelectorTool"),
    m_vertexEstimator("InDet::VertexPointEstimator"),
    m_beamSpotSvc("BeamCondSvc",n) 
    {
        declareInterface<FourMuonTool>(this);
        declareProperty("ptCut",m_ptCut);
        declareProperty("etaCut",m_etaCut);
        declareProperty("useV0Fitter",m_useV0Fitter);
        declareProperty("muonCollectionKey",m_muonCollectionKey);
        declareProperty("TrackParticleCollection",m_TrkParticleCollection);
        declareProperty("TrkVertexFitterTool",m_iVertexFitter);
        declareProperty("V0VertexFitterTool",m_iV0VertexFitter);
        declareProperty("V0Tools",m_V0Tools);
        declareProperty("TrackSelectorTool",m_trkSelector);
        declareProperty("VertexPointEstimator",m_vertexEstimator);
    }
    
    FourMuonTool::~FourMuonTool() { }
    
    //-------------------------------------------------------------------------------------
    // Find the candidates
    //-------------------------------------------------------------------------------------
    StatusCode FourMuonTool::performSearch(xAOD::VertexContainer*& pairVxContainer, xAOD::VertexAuxContainer*& pairVxAuxContainer,
                                           xAOD::VertexContainer*& quadVxContainer, xAOD::VertexAuxContainer*& quadVxAuxContainer, bool &selectEvent)
    {
        ATH_MSG_DEBUG( "FourMuonTool::performSearch" );
	selectEvent = false;	

        // pairs
        pairVxContainer = new xAOD::VertexContainer;
        pairVxAuxContainer = new xAOD::VertexAuxContainer;
        pairVxContainer->setStore(pairVxAuxContainer);
        // quads
        quadVxContainer = new xAOD::VertexContainer;
        quadVxAuxContainer = new xAOD::VertexAuxContainer;
        quadVxContainer->setStore(quadVxAuxContainer);
        
        // Get the ToolSvc
        IToolSvc* toolsvc;
        StatusCode sc1=service("ToolSvc",toolsvc);
        if (sc1.isFailure() ) {
            ATH_MSG_WARNING("Problem loading tool service. Candidates will be EMPTY!");
            return StatusCode::SUCCESS;;
        };
        
        // Get the muons from StoreGate
        const xAOD::MuonContainer* importedMuonCollection;
        StatusCode sc = evtStore()->retrieve(importedMuonCollection,m_muonCollectionKey);
        if(sc.isFailure()){
            ATH_MSG_WARNING("No muon collection with key " << m_muonCollectionKey << " found in StoreGate. Candidates will be EMPTY!");
            return StatusCode::SUCCESS;;
        }
        ATH_MSG_DEBUG("Muon container size "<<importedMuonCollection->size());
        
        // Get ID tracks
        const xAOD::TrackParticleContainer* importedTrackCollection(0);
        sc = evtStore()->retrieve(importedTrackCollection,m_TrkParticleCollection);
        if(sc.isFailure()){
            ATH_MSG_WARNING("No TrackParticle collection with name " << m_TrkParticleCollection << " found in StoreGate!");
            return StatusCode::SUCCESS;;
        }
        ATH_MSG_DEBUG("ID TrackParticle container size "<< importedTrackCollection->size());
        
        // Select the muons
        std::vector<const xAOD::Muon*>  theMuonsAfterSelection;
        xAOD::MuonContainer::const_iterator muItr;
        SG::AuxElement::Decorator< int > muonDecorator("BPHY4MuonIndex"); // Decorator for muons
        unsigned int nCombMuons = 0;
        unsigned int nSegmentTaggedMuons = 0;
        const xAOD::Vertex* vx = 0;
        for (muItr=importedMuonCollection->begin(); muItr!=importedMuonCollection->end(); ++muItr) {
            if ( *muItr == NULL ) continue;
            muonDecorator(**muItr) = -1; // all muons must be decorated
            if (  ((*muItr)->muonType() != xAOD::Muon::Combined ) && ((*muItr)->muonType() != xAOD::Muon::SegmentTagged ) ) continue;
            if (!(*muItr)->inDetTrackParticleLink().isValid()) continue; // No muons without ID tracks
            auto& link = (*muItr)->inDetTrackParticleLink();
            const xAOD::TrackParticle* muonTrk = *link;
            if ( muonTrk==NULL) continue;
            if ( !m_trkSelector->decision(*muonTrk, vx) ) continue; // all ID tracks must pass basic tracking cuts
            if ( fabs(muonTrk->pt())<m_ptCut ) continue; //  pt cut
            if ( fabs(muonTrk->eta())>m_etaCut ) continue; //  eta cut
            if ( (*muItr)->muonType() == xAOD::Muon::Combined ) ++nCombMuons;
            if ( (*muItr)->muonType() == xAOD::Muon::SegmentTagged ) ++nSegmentTaggedMuons;
            theMuonsAfterSelection.push_back(*muItr);
        }
        unsigned int nSelectedMuons = theMuonsAfterSelection.size();
        ATH_MSG_DEBUG("Number of muons after selection: " << nSelectedMuons);
        ATH_MSG_DEBUG("of which " << nCombMuons << " are combined");
        ATH_MSG_DEBUG("and " << nSegmentTaggedMuons << " are segment tagged");
        if ( (nSelectedMuons < 4) || (nCombMuons < 1) ) {
            ATH_MSG_DEBUG("Muon criteria not met. Skipping event.");
            return StatusCode::SUCCESS;
        }
        selectEvent = true; // if we got this far we should definitively accept the event 
  
        // Decorators
        SG::AuxElement::Decorator< std::string > indexDecorator("CombinationCode");
        SG::AuxElement::Decorator< std::string > chargeDecorator("ChargeCode");
        //SG::AuxElement::Decorator< double > acdcDecorator("ACminusDC");
        //SG::AuxElement::Decorator< double > ssdcDecorator("SSminusDC");
 
        // Order by pT
        std::sort(theMuonsAfterSelection.begin(), theMuonsAfterSelection.end(), [](const xAOD::Muon *a, const xAOD::Muon *b) {
            return b->pt() < a->pt();
        });

        // Decorate the selected muons (now pT ordered) with their index
        std::vector<const xAOD::Muon*>::iterator selMuonIt = theMuonsAfterSelection.begin();
	unsigned int muonIndex(0);
	for (; selMuonIt!=theMuonsAfterSelection.end(); ++selMuonIt) {
	    muonDecorator(**selMuonIt) = muonIndex;
            ++muonIndex;            
	}

        // Quadruplet combinatorics
        std::vector<Combination> quadruplets;
        std::vector<Combination> pairs;
        buildCombinations(theMuonsAfterSelection,pairs,quadruplets,nSelectedMuons);
        if (quadruplets.size()==0) {
            ATH_MSG_DEBUG("No acceptable quadruplets");
            return StatusCode::SUCCESS;
        }
       
        // Get the beam spot (for the vertexing starting point)
        const Amg::Vector3D beamSpot = m_beamSpotSvc->beamPos();
 
        // fit pairs
        ATH_MSG_DEBUG("Successful pairs.....");
        for (std::vector<Combination>::iterator pairItr = pairs.begin(); pairItr!=pairs.end(); ++pairItr) {
            std::vector<const xAOD::TrackParticle*> theTracks; theTracks.clear();
            theTracks = (*pairItr).trackParticles("pair1");
            xAOD::Vertex* pairVxCandidate = fit(theTracks,importedTrackCollection,beamSpot); // This line actually does the fitting and object making
            if (pairVxCandidate != 0) {
                // decorate the candidate with its codes
                indexDecorator(*pairVxCandidate) = (*pairItr).combinationIndices();
                chargeDecorator(*pairVxCandidate) = (*pairItr).combinationCharges();
                // decorate the candidate with refitted tracks and muons via the BPhysHelper
                xAOD::BPhysHelper helper(pairVxCandidate);
                helper.setRefTrks();
                std::vector<const xAOD::Muon*> theStoredMuons;
                theStoredMuons = (*pairItr).muons;
                helper.setMuons(theStoredMuons,importedMuonCollection);
                // Retain the vertex
                pairVxContainer->push_back(pairVxCandidate);
                ATH_MSG_DEBUG("..... indices: " << (*pairItr).combinationIndices() <<
                              " charges: " << (*pairItr).combinationCharges() <<
                              " chi2:    " << pairVxCandidate->chiSquared());
            } else { // fit failed
                ATH_MSG_DEBUG("Fitter failed!");
            }
        }
        ATH_MSG_DEBUG("pairContainer size " << pairVxContainer->size());
        
        // fit quadruplets
        ATH_MSG_DEBUG("Successful quadruplets.....");
        for (std::vector<Combination>::iterator quadItr = quadruplets.begin(); quadItr!=quadruplets.end(); ++quadItr) {
            std::vector<const xAOD::TrackParticle*> theDCTracks; theDCTracks.clear();
            //std::vector<const xAOD::TrackParticle*> theACTracks; theACTracks.clear();
            //std::vector<const xAOD::TrackParticle*> theSSTracks; theSSTracks.clear();
            theDCTracks = (*quadItr).trackParticles("DC");
            //theACTracks = (*quadItr).trackParticles("AC");
            //theSSTracks = (*quadItr).trackParticles("SS");
            xAOD::Vertex* dcVxCandidate = fit(theDCTracks,importedTrackCollection, beamSpot);
            //xAOD::Vertex* acVxCandidate = fit(theACTracks,importedTrackCollection, beamSpot);
            //xAOD::Vertex* ssVxCandidate = fit(theSSTracks,importedTrackCollection, beamSpot);
            // Get the chi2 for each one
            //double acChi2(0.0);
            //double ssChi2(0.0);
            //if (acVxCandidate != 0) {acChi2 = acVxCandidate->chiSquared();}
            //if (ssVxCandidate != 0) {ssChi2 = ssVxCandidate->chiSquared();}
            if (dcVxCandidate != 0) {
                // decorate the candidate with its codes
                indexDecorator(*dcVxCandidate) = (*quadItr).combinationIndices();
                chargeDecorator(*dcVxCandidate) = (*quadItr).combinationCharges();
                // Decorate the DC candidate with the differences between its chi2 and the other
                double dcChi2 = dcVxCandidate->chiSquared();
                //acdcDecorator(*dcVxCandidate) = acChi2 - dcChi2;
                //ssdcDecorator(*dcVxCandidate) = ssChi2 - dcChi2;
                // decorate the candidate with refitted tracks and muons via the BPhysHelper
                xAOD::BPhysHelper helper(dcVxCandidate);
                helper.setRefTrks();
                std::vector<const xAOD::Muon*> theStoredMuons;
                theStoredMuons = (*quadItr).muons;
                helper.setMuons(theStoredMuons,importedMuonCollection);
                // Retain the vertex
                quadVxContainer->push_back(dcVxCandidate);
                ATH_MSG_DEBUG("..... indices: " << (*quadItr).combinationIndices() <<
                              " charges: " << (*quadItr).combinationCharges() <<
                              " chi2(DC): " << dcChi2);
                              //" chi2(AC): " << acChi2 <<
                              //" chi2(SS): " << ssChi2);
            } else { // fit failed
                ATH_MSG_DEBUG("Fitter failed!");
            }
        }
        ATH_MSG_DEBUG("quadruplet container size " << quadVxContainer->size());
        
        return StatusCode::SUCCESS;;
    }
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // fit - does the fit
    // ---------------------------------------------------------------------------------
    
    xAOD::Vertex* FourMuonTool::fit(const std::vector<const xAOD::TrackParticle*> &inputTracks,
                                    const xAOD::TrackParticleContainer* importedTrackCollection,
                                    const Amg::Vector3D &beamSpot) {
        
        Trk::TrkV0VertexFitter* concreteVertexFitter=0;
        if (m_useV0Fitter) {
            // making a concrete fitter for the V0Fitter
            concreteVertexFitter = dynamic_cast<Trk::TrkV0VertexFitter * >(&(*m_iV0VertexFitter));
            if(concreteVertexFitter == 0) {
                ATH_MSG_FATAL("The vertex fitter passed is not a V0 Vertex Fitter");
                return NULL;
            }
        }
        
        //int sflag = 0;
        //int errorcode = 0;
        //Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(&aPerigee1,&aPerigee2,sflag,errorcode);
        //startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
        //Trk::Vertex vertex(beamSpot);

        xAOD::Vertex* myVxCandidate = nullptr;
        if (m_useV0Fitter) {
            myVxCandidate = concreteVertexFitter->fit(inputTracks, beamSpot /*vertex startingPoint*/ );
        } else {
            myVxCandidate = m_iVertexFitter->fit(inputTracks, beamSpot /*vertex startingPoint*/ );
        }
        
        if(myVxCandidate) BPhysPVTools::PrepareVertexLinks(myVxCandidate, importedTrackCollection);
        
        return myVxCandidate;
        
    } // End of fit method
    
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // getQuadIndices: forms up index lists
    // ---------------------------------------------------------------------------------
    
    std::vector<std::vector<unsigned int> > FourMuonTool::getQuadIndices(unsigned int length) {
        
        std::vector<std::vector<unsigned int> > quadIndices = mFromN(4,length);
        return(quadIndices);
    }
    
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // mFromN and combinatorics
    // ---------------------------------------------------------------------------------
    std::vector<std::vector<unsigned int> > FourMuonTool::mFromN(unsigned int m, unsigned int N) {
        
        std::vector<std::vector<unsigned int> > allCombinations;
        std::vector<unsigned int> mainList;
        std::vector<unsigned int> combination;
        for (unsigned int i=0; i<N; ++i) mainList.push_back(i);
        combinatorics(0,m,combination,mainList,allCombinations);
        return allCombinations;
    }
    
    void FourMuonTool::combinatorics(unsigned int offset,
                       unsigned int k,
                       std::vector<unsigned int> &combination,
                       std::vector<unsigned int> &mainList,
                       std::vector<std::vector<unsigned int> > &allCombinations) {
        if (k==0) {
            allCombinations.push_back(combination);
            return;
        }
        if (k>0) {
            for (unsigned int i=offset; i<=mainList.size()-k; ++i) {
                combination.push_back(mainList[i]);
                combinatorics(i+1,k-1,combination,mainList,allCombinations);
                combination.pop_back();
            }
        }
    }
    
    // ---------------------------------------------------------------------------------
    // getPairIndices
    // ---------------------------------------------------------------------------------
    
    std::vector<std::pair<unsigned int, unsigned int> > FourMuonTool::getPairIndices(unsigned int length){
        
        std::vector<std::pair<unsigned int, unsigned int> > uniquePairs;
        std::vector<std::vector<unsigned int> > doublets = mFromN(2,length);
        for (std::vector<std::vector<unsigned int> >::iterator it=doublets.begin(); it!=doublets.end(); ++it) {     
                std::pair<unsigned int, unsigned int> tmpPair = std::make_pair((*it).at(0),(*it).at(1));
                uniquePairs.push_back(tmpPair);
        }
        
        return(uniquePairs);
    }
    
    
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // buildCombinations: forms up the quadruplet of muons/tracks
    // ---------------------------------------------------------------------------------
    
    void FourMuonTool::buildCombinations(const std::vector<const xAOD::Muon*> &muonsIn,
                                         std::vector<Combination> &pairs,
                                         std::vector<Combination> &quadruplets,
                                         unsigned int nSelectedMuons) {
        
        std::vector<std::vector<unsigned int> > quadrupletIndices = getQuadIndices(nSelectedMuons);
        std::vector<std::pair<unsigned int, unsigned int> > pairIndices = getPairIndices(nSelectedMuons);
        
        // Quadruplets
        std::vector<std::vector<unsigned int> >::iterator quadItr;
        for (quadItr=quadrupletIndices.begin(); quadItr!=quadrupletIndices.end(); ++quadItr) {
            std::vector<unsigned int> quad = (*quadItr);
            std::vector<const xAOD::Muon*> theMuons = {muonsIn[quad[0]],muonsIn[quad[1]],muonsIn[quad[2]],muonsIn[quad[3]]};
            if (!passesQuadSelection(theMuons)) continue;
            Combination tmpQuad;
            tmpQuad.muons = theMuons;
            tmpQuad.quadIndices = quad;
            quadruplets.push_back(tmpQuad);
        }
        if (quadruplets.size() == 0) return;
        
        // pairs
        std::vector<std::pair<unsigned int, unsigned int> >::iterator pairItr;
        for (pairItr=pairIndices.begin(); pairItr!=pairIndices.end(); ++pairItr) {
            std::pair<unsigned int, unsigned int> pair = (*pairItr);
            Combination tmpPair;
            std::vector<const xAOD::Muon*> theMuons = {muonsIn[pair.first],muonsIn[pair.second]};
            tmpPair.muons = theMuons;
            tmpPair.pairIndices = pair;
            pairs.push_back(tmpPair);
        }
        
        return;
        
    }
    
    // *********************************************************************************
    
    // ---------------------------------------------------------------------------------
    // passesQuadSelection: 4-muon selection
    // ---------------------------------------------------------------------------------
    
    bool FourMuonTool::passesQuadSelection(const std::vector<const xAOD::Muon*> &muons) {
        bool accept(false);
        bool charges(true);
        bool quality(false);
        //unsigned int sumCharges = abs(mu0->charge() + mu1->charge() + mu2->charge() + mu3->charge());
        //if (sumCharges<4) charges = true;
        if ((  muons.at(0)->muonType() == xAOD::Muon::Combined ) ||
            (  muons.at(1)->muonType() == xAOD::Muon::Combined ) ||
            (  muons.at(2)->muonType() == xAOD::Muon::Combined ) ||
            (  muons.at(3)->muonType() == xAOD::Muon::Combined )
            ) quality = true;
        if (charges && quality) accept = true;
        return accept;
    }
    
}
