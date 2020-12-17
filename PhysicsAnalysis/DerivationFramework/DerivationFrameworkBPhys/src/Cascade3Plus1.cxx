/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////
// Cascade3Plus1.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
#include "DerivationFrameworkBPhys/Cascade3Plus1.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "DerivationFrameworkBPhys/BPhysPVCascadeTools.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"
#include "xAODBPhys/BPhysHelper.h"
#include "Math/Vector4D.h"

namespace DerivationFramework {
typedef ElementLink<xAOD::VertexContainer> VertexLink;
typedef std::vector<VertexLink> VertexLinkVector;
typedef std::vector<const xAOD::TrackParticle*> TrackBag;
/// Base 4 Momentum type for TrackParticle
typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > GenVecFourMom_t;

template<size_t N>
struct Candidate {
    std::array<const xAOD::TrackParticle*, N> tracks;
};

struct VertexCand : Candidate<4> {
    std::unique_ptr<Trk::VxCascadeInfo> cascVertex;
};

template<size_t N>
GenVecFourMom_t SumVector(const std::array<GenVecFourMom_t, N> &vectors) {
    GenVecFourMom_t total = vectors[0];
    for(size_t i =1; i<N; i++) total+= vectors[i];
    return total;
}

Cascade3Plus1::Cascade3Plus1(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
    m_trkSelector("InDet::TrackSelectorTool"),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_V0Tools("Trk::V0Tools"),
    m_CascadeTools("DerivationFramework::CascadeTools"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter"),
    m_cascadeOutputsKeys{ "CascadeVtx1", "CascadeVtx2" }
{

    declareProperty("TrackMassHyp",     m_trackMasses);
    declareProperty("CascadeVertexCollections",  m_cascadeOutputsKeys);
    declareProperty("TwoTrackMassMin",  m_2trackmassMin);
    declareProperty("TwoTrackMassMax",  m_2trackmassMax);
    declareProperty("ThreeTrackMassMin",  m_3trackmassMin);
    declareProperty("ThreeTrackMassMax",  m_3trackmassMax);
    declareProperty("FourTrackMassMin",  m_4trackmassMin);
    declareProperty("FourTrackMassMax",  m_4trackmassMax);
    declareProperty("TwoTracksMass",  m_2tracksMass);
    declareProperty("ThreeTracksMass",  m_3tracksMass);
    declareProperty("FourTracksMass",  m_4tracksMass);
    declareProperty("TrackSelectorTool",m_trkSelector);
    declareProperty("CascadeTools",              m_CascadeTools);
    declareProperty("MinNTracksInPV",            m_PV_minNTracks          = 0);
    declareProperty("HypothesisName",            m_hypoName               = "Bs");
    declareProperty("Track3Name",            m_3TrackName               = "Ds");
    declareProperty("MaxnPV",                    m_PV_max                 = 999);
    declareProperty("DoVertexType",              m_DoVertexType           = 7);
    declareProperty("RefitPV",                   m_refitPV                = true);
    declareProperty("RefPVContainerName", m_refPVContainerName  = "RefittedPrimaryVertices");
    declareProperty("PVRefitter",                m_pvRefitter);
    declareProperty("TrkVertexFitterTool",       m_iVertexFitter);
    declareProperty("PVContainerName", m_VxPrimaryCandidateName);
    declareProperty("ThreeTrackMassConstraint", m_3TrackMassConstraint);
    declareProperty("TwoTrackMassConstraint", m_2TrackMassConstraint);
    declareProperty("Chi2NDFCut", m_Chi2NDFCut);

    declareProperty("FourTrackMassFinalMin",  m_4trackmassFinalMin);
    declareProperty("FourTrackMassFinalMax",  m_4trackmassFinalMax);
    declareProperty("FourTrackTauCut", m_tauCut);
    declareProperty("UseMuonsForTracks", m_requireMuonsOnTrack);
    declareProperty("ThreeVertexOutputContainer", m_3TrackVertexOutput);
    declareProperty("VertexEstimator", m_vertexEstimator);
    declareProperty("ThreeTrackChi2NDF", m_3TrackChi2NDFCut);
    declareProperty("EliminateBad3Tracksfrom4Track", m_eliminateBad3Tracksfrom4Track);
    declareProperty("CopyAllVertices", m_copyAllVertices);
    declareProperty("PTCutPerTrack", m_ptCutPerTrack);
    m_ptCutPerVertex.fill(0);
    declareProperty("PTCutVertex1", m_ptCutPerVertex[0]);
    declareProperty("PTCutVertex2", m_ptCutPerVertex[1]);
    declareProperty("PTCutVertex3", m_ptCutPerVertex[2]);
}

StatusCode Cascade3Plus1::initialize() {
    if(m_trackMasses.size()!=4) {
        ATH_MSG_ERROR("4 mass hypotheses must be provided");
        return StatusCode::FAILURE;
    }
    if(m_cascadeOutputsKeys.size() !=s_topoN)  {
        ATH_MSG_FATAL("Incorrect number of VtxContainers");
        return StatusCode::FAILURE;
    }
    // retrieving vertex Fitter
    ATH_CHECK( m_iVertexFitter.retrieve());

    // retrieving the Cascade tools
    ATH_CHECK( m_CascadeTools.retrieve());

    // Get the beam spot service
    CHECK( m_beamSpotKey.initialize() );

    ATH_CHECK(m_vertexEstimator.retrieve());
    if(m_eliminateBad3Tracksfrom4Track && m_3TrackChi2NDFCut<=0.0) {
        ATH_MSG_FATAL("Invalid configuration");
        return StatusCode::FAILURE;
    }

    if(m_ptCutPerTrack.size() == 1 || m_ptCutPerTrack.size() > 4){
        ATH_MSG_FATAL("Invalid configuration");
        return StatusCode::FAILURE;
    }
    if(m_ptCutPerTrack.size() >=2 && m_ptCutPerTrack[0] != m_ptCutPerTrack[1]){
        ATH_MSG_FATAL("Invalid configuration");
        return StatusCode::FAILURE;
    }
    m_muonTrackBit.reset();
    for(int i : m_requireMuonsOnTrack) {
       if(i>=4) {
          ATH_MSG_FATAL("Invalid configuration" << " muon track " << i);
          return StatusCode::FAILURE;
       }
       m_muonTrackBit[i] = true;
    }
    m_requireMuonsOnTrack.clear();
    m_requireMuonsOnTrack.shrink_to_fit();

    if(m_muonTrackBit[0] != m_muonTrackBit[1])
    {
        ATH_MSG_FATAL("Invalid configuration" << " variable is " << m_muonTrackBit.to_string());
        return StatusCode::FAILURE;
    }


    return StatusCode::SUCCESS;
}

Cascade3Plus1::~Cascade3Plus1() { }


const TrackBag& Cascade3Plus1::ApplyAdditionalCuts(const TrackBag& alltracks, const TrackBag& muonTracks, TrackBag& cuttracks, size_t track) const {
   const TrackBag& tracks = m_muonTrackBit[track] ? muonTracks : alltracks;
   if(track >= m_ptCutPerTrack.size()) return tracks;
   double ptCut = m_ptCutPerTrack.at(track);
   if(ptCut <=0.0) return tracks;
   cuttracks.clear();//reset any previous selections
   for(auto ptr : tracks){
      if(ptr->pt() > ptCut) cuttracks.push_back(ptr);
   }
   return cuttracks;
}

StatusCode Cascade3Plus1::addBranches() const
{
    const xAOD::TrackParticleContainer  *trackContainer(nullptr);
    ATH_CHECK(evtStore()->retrieve(trackContainer, "InDetTrackParticles"      ));

    //----------------------------------------------------
    // Try to retrieve refitted primary vertices
    //----------------------------------------------------
    xAOD::VertexContainer*    refPvContainer    = nullptr;
    xAOD::VertexAuxContainer* refPvAuxContainer = nullptr;
    if (m_refitPV) {
        if (evtStore()->contains<xAOD::VertexContainer>(m_refPVContainerName)) {
            // refitted PV container exists. Get it from the store gate
            ATH_CHECK(evtStore()->retrieve(refPvContainer, m_refPVContainerName       ));
            ATH_CHECK(evtStore()->retrieve(refPvAuxContainer, m_refPVContainerName + "Aux."));
        } else {
            // refitted PV container does not exist. Create a new one.
            refPvContainer = new xAOD::VertexContainer;
            refPvAuxContainer = new xAOD::VertexAuxContainer;
            refPvContainer->setStore(refPvAuxContainer);
            ATH_CHECK(evtStore()->record(refPvContainer, m_refPVContainerName));
            ATH_CHECK(evtStore()->record(refPvAuxContainer, m_refPVContainerName+"Aux."));
        }
    }

    std::array<xAOD::VertexContainer*, s_topoN> Vtxwritehandles;
    std::array<xAOD::VertexAuxContainer*, s_topoN> Vtxwritehandlesaux;

    for(int i =0; i<s_topoN; i++) {
        Vtxwritehandles[i] = new xAOD::VertexContainer();
        Vtxwritehandlesaux[i] = new xAOD::VertexAuxContainer();
        Vtxwritehandles[i]->setStore(Vtxwritehandlesaux[i]);
        ATH_CHECK(evtStore()->record(Vtxwritehandles[i], m_cascadeOutputsKeys[i]       ));
        ATH_CHECK(evtStore()->record(Vtxwritehandlesaux[i], m_cascadeOutputsKeys[i] + "Aux."));
    }
    xAOD::VertexContainer *v3container = nullptr;
    if(!m_3TrackVertexOutput.empty()) {
        v3container    = new xAOD::VertexContainer();
        auto vcontaineraux = new xAOD::VertexAuxContainer();
        v3container->setStore(vcontaineraux);
        ATH_CHECK(evtStore()->record(v3container, m_3TrackVertexOutput       ));
        ATH_CHECK(evtStore()->record(vcontaineraux, m_3TrackVertexOutput + "Aux."));
    }
    //----------------------------------------------------
    // retrieve primary vertices
    //----------------------------------------------------

    const xAOD::Vertex * primaryVertex(nullptr);
    const xAOD::VertexContainer *pvContainer(nullptr);
    ATH_CHECK(evtStore()->retrieve(pvContainer, m_VxPrimaryCandidateName));

    if (pvContainer->size()==0) {
        ATH_MSG_WARNING("You have no primary vertices: " << pvContainer->size());
        return StatusCode::RECOVERABLE;
    } else {
        primaryVertex = (*pvContainer)[0];
    }


    TrackBag theIDTracksAfterSelection;
    TrackBag theIDTracksAfterAdditionalSelection;
    for(auto x : *trackContainer) {
        if ( m_trkSelector->decision(*x, nullptr) ) theIDTracksAfterSelection.push_back(x);
    }
    ATH_MSG_DEBUG("Found good tracks N="<<theIDTracksAfterSelection.size());
    TrackBag theMuonsAfterSelection;
    if(m_muonTrackBit.any()) {
        const xAOD::MuonContainer* importedMuonCollection(0);
        ATH_CHECK(evtStore()->retrieve(importedMuonCollection, "Muons"));
        for(auto muon : *importedMuonCollection) {
            if(muon->muonType() == xAOD::Muon::SiliconAssociatedForwardMuon) continue;
            auto ptr = muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
            if(ptr) theMuonsAfterSelection.push_back(ptr);
        }
    }

    std::vector<Candidate<2>> Initialcandidates;
    {  //Isolate scope for safety
       const TrackBag &Tracksfor2Vertex = ApplyAdditionalCuts(theIDTracksAfterSelection, theMuonsAfterSelection, theIDTracksAfterAdditionalSelection, 0);
       for(auto track1itr = Tracksfor2Vertex.cbegin(); track1itr != Tracksfor2Vertex.cend(); ++track1itr) {
           Candidate<2> cand;
           std::array<GenVecFourMom_t, 2> vectors;
           cand.tracks[0] = *track1itr;
           vectors[0].SetCoordinates(cand.tracks[0]->pt(), cand.tracks[0]->eta(), cand.tracks[0]->phi(),  m_trackMasses[0]);
           for(auto track2itr = track1itr+1; track2itr != Tracksfor2Vertex.cend(); ++track2itr) {
               cand.tracks[1] = *track2itr;
               if(cand.tracks[0]->qOverP() * cand.tracks[1]->qOverP() >= 0.) continue; //Skip same signed
               vectors[1].SetCoordinates(cand.tracks[1]->pt(), cand.tracks[1]->eta(), cand.tracks[1]->phi(),  m_trackMasses[1]);
               GenVecFourMom_t pair = SumVector(vectors);
               if(pair.Pt() < m_ptCutPerVertex[0]) continue;
               if(pair.M() >= m_2trackmassMin && pair.M() < m_2trackmassMax) {
                   ATH_MSG_VERBOSE("2 Track candidate found: " << pair.M() << " Within " << m_2trackmassMin << " and " << m_2trackmassMax);
                   Initialcandidates.push_back(cand);
               }
           }
       }
    }
    ATH_MSG_DEBUG("2 Track candidates found: " << Initialcandidates.size());
    if(Initialcandidates.empty()) {
        //No work to do Leave method early
        return StatusCode::SUCCESS;
    }
    std::vector<Candidate<3>> Candidates3;

    {//isolate scope
    const TrackBag &Tracksfor3Vertex = ApplyAdditionalCuts(theIDTracksAfterSelection, theMuonsAfterSelection, theIDTracksAfterAdditionalSelection, 2);
       for(auto &c : Initialcandidates) {
           Candidate<3> c3;
           std::copy(c.tracks.begin(), c.tracks.end(), c3.tracks.begin());
           std::array<GenVecFourMom_t, 3> vectors;
           vectors[0].SetCoordinates(c.tracks[0]->pt(), c.tracks[0]->eta(), c.tracks[0]->phi(),  m_trackMasses[0]);
           vectors[1].SetCoordinates(c.tracks[1]->pt(), c.tracks[1]->eta(), c.tracks[1]->phi(),  m_trackMasses[1]);
           for(auto track3itr = Tracksfor3Vertex.cbegin(); track3itr != Tracksfor3Vertex.cend(); ++track3itr) {
               if(std::find(c3.tracks.begin(), c3.tracks.end(), *track3itr) != c3.tracks.end()) continue;
               c3.tracks[2] = *track3itr;
               vectors[2].SetCoordinates(c3.tracks[2]->pt(), c3.tracks[2]->eta(), c3.tracks[2]->phi(),  m_trackMasses[2]);
               GenVecFourMom_t tripple = SumVector(vectors);
               if(tripple.Pt() < m_ptCutPerVertex[1]) continue;
               if(tripple.M() >= m_3trackmassMin && tripple.M() < m_3trackmassMax) {
                   ATH_MSG_VERBOSE("3 Track candidate found: " << tripple.M() << " Within " << m_3trackmassMin << " and " << m_3trackmassMax);
                   Candidates3.push_back(c3);
               }
           }
       }
    }
    Initialcandidates.clear();
    Initialcandidates.shrink_to_fit();

    ATH_MSG_DEBUG("3 Track candidates found: " << Candidates3.size());
    std::map<const std::array<const xAOD::TrackParticle*, 3>, xAOD::Vertex* > threeVertexMap;

    if(!m_3TrackVertexOutput.empty()) {
        SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
        if(not beamSpotHandle.isValid()) ATH_MSG_ERROR("Cannot Retrieve " << m_beamSpotKey.key() );
        BPhysPVTools helper(&(*m_V0Tools), beamSpotHandle.cptr());
        helper.SetMinNTracksInPV(0);
        helper.SetSave3d(false);
        std::vector<const xAOD::TrackParticle*> tracksforfit;
        std::vector<Candidate<3>> Candidates3PassCuts;
        if(m_eliminateBad3Tracksfrom4Track) Candidates3PassCuts.reserve(Candidates3.size());
        for(const auto &c3 : Candidates3) {
            tracksforfit.assign(c3.tracks.begin(), c3.tracks.end());
            auto v = StandardFit(tracksforfit, trackContainer);
            if(v==nullptr) {
                ATH_MSG_DEBUG("3Vertex fit returned null");
                continue;
            }
            if(m_3TrackChi2NDFCut > 0. && v->chiSquared() / v->numberDoF() > m_3TrackChi2NDFCut) {
                ATH_MSG_DEBUG("Rejecting 3 track vertex because Chi " << v->chiSquared() / v->numberDoF() << " > " << m_3TrackChi2NDFCut);
                continue;
            }
            if(m_eliminateBad3Tracksfrom4Track) Candidates3PassCuts.push_back(c3);
            threeVertexMap[c3.tracks] = v.get();
            xAOD::BPhysHelper bHelper(v.get());//"get" does not "release" still automatically deleted
            bHelper.setRefTrks();
            v3container->push_back(v.release());
        }

        if(v3container->size() >0) ATH_CHECK(helper.FillCandExistingVertices(v3container, pvContainer, 1));

        if(m_eliminateBad3Tracksfrom4Track) {
            ATH_MSG_DEBUG("Swapping container to N = "<< Candidates3PassCuts.size() << " from " << Candidates3.size());
            Candidates3PassCuts.swap(Candidates3);//Swap old container with one that passed cuts
        }

    }
    std::vector<VertexCand> Candidates4;
    {//isolate scope
    const TrackBag &Tracksfor4Vertex = ApplyAdditionalCuts(theIDTracksAfterSelection, theMuonsAfterSelection, theIDTracksAfterAdditionalSelection, 3);
       for(auto &c : Candidates3) {
           VertexCand c4;
           std::copy(c.tracks.begin(), c.tracks.end(),   c4.tracks.begin());
           std::array<GenVecFourMom_t, 4> vectors;
           vectors[0].SetCoordinates(c.tracks[0]->pt(), c.tracks[0]->eta(), c.tracks[0]->phi(),  m_trackMasses[0]);
           vectors[1].SetCoordinates(c.tracks[1]->pt(), c.tracks[1]->eta(), c.tracks[1]->phi(),  m_trackMasses[1]);
           vectors[2].SetCoordinates(c.tracks[2]->pt(), c.tracks[2]->eta(), c.tracks[2]->phi(),  m_trackMasses[2]);
           for(auto track4itr = Tracksfor4Vertex.cbegin(); track4itr != Tracksfor4Vertex.cend(); ++track4itr) {
               if(std::find(c4.tracks.begin(), c4.tracks.end(), *track4itr) != c4.tracks.end()) continue;
               c4.tracks[3] = *track4itr;
               if(c4.tracks[2]->qOverP() * c4.tracks[3]->qOverP() >= 0.) continue; //Skip same signed
               vectors[3].SetCoordinates(c4.tracks[3]->pt(), c4.tracks[3]->eta(), c4.tracks[3]->phi(),  m_trackMasses[3]);
               GenVecFourMom_t fourtrack = SumVector(vectors);
               if(fourtrack.Pt() < m_ptCutPerVertex[2]) continue;
               if(fourtrack.M() >= m_4trackmassMin && fourtrack.M() < m_4trackmassMax) {
                   ATH_MSG_VERBOSE("3 Track candidate found: " << fourtrack.M() << " Within " << m_4trackmassMin << " and " << m_4trackmassMax);
                   Candidates4.push_back(std::move(c4));
               }
           }
       }
    }
    Candidates3.clear();
    Candidates3.shrink_to_fit();

    ATH_MSG_DEBUG("4 Track candidates found: " << Candidates4.size() << " running cascade");
    for(auto &c : Candidates4) {
        c.cascVertex = CascadeFit(c.tracks);
        if(c.cascVertex!=nullptr) {
            c.cascVertex->setSVOwnership(true);
        }
    }

    SG::AuxElement::Decorator<VertexLinkVector> CascadeLinksDecor("CascadeVertexLinks");
    SG::AuxElement::Decorator<VertexLink> Vertex3Decor(m_3TrackName+ "_VertexLink");
    SG::AuxElement::Decorator<float> chi2_decor("ChiSquared");
    SG::AuxElement::Decorator<float> ndof_decor("NumberDoF");
//        SG::AuxElement::Decorator<float> TotalProb_decor("TotalProb");
    SG::AuxElement::Decorator<float> Pt_decor("Pt");
    SG::AuxElement::Decorator<float> PtErr_decor("PtErr");
    SG::AuxElement::Decorator<float> Mass_svdecor(m_3TrackName+ "_mass");
    SG::AuxElement::Decorator<float> MassErr_svdecor(m_3TrackName+"_massErr");
    SG::AuxElement::Decorator<float> Pt_svdecor(m_3TrackName+"_Pt");
    SG::AuxElement::Decorator<float> PtErr_svdecor(m_3TrackName+"_PtErr");
    SG::AuxElement::Decorator<float> Lxy_svdecor(m_3TrackName+"_Lxy");
    SG::AuxElement::Decorator<float> LxyErr_svdecor(m_3TrackName+"_LxyErr");
    SG::AuxElement::Decorator<float> Tau_svdecor(m_3TrackName+"_Tau");
    SG::AuxElement::Decorator<float> TauErr_svdecor(m_3TrackName+"_TauErr");


    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
    if(not beamSpotHandle.isValid()) ATH_MSG_ERROR("Cannot Retrieve " << m_beamSpotKey.key() );
    BPhysPVCascadeTools helper(&(*m_CascadeTools), beamSpotHandle.cptr());
    helper.SetMinNTracksInPV(m_PV_minNTracks);
    helper.m_copyAllVertices = this->m_copyAllVertices;



    int totalnotnull=0;
    for(auto &c : Candidates4) {
        if(c.cascVertex==nullptr) {
            totalnotnull++;
            continue;
        }
        auto x = c.cascVertex.get();
        const std::vector<xAOD::Vertex*> &cascadeVertices = x->vertices();
        if(cascadeVertices.size()!=s_topoN)  {
            ATH_MSG_ERROR("Incorrect number of vertices");
            continue;
        }
        if(cascadeVertices[0] == nullptr || cascadeVertices[1] == nullptr) {
            ATH_MSG_ERROR("Error null vertex");
            continue;
        }
        if( m_Chi2NDFCut > 0.0 && (x->fitChi2() / x->nDoF()) > m_Chi2NDFCut) {
            continue;
        }
        BPhysPVCascadeTools::PrepareVertexLinks(c.cascVertex.get(), trackContainer);
        const std::vector< std::vector<TLorentzVector> > &moms = x->getParticleMoms();
        double mass1 = m_CascadeTools->invariantMass(moms[1]);
        if(m_4trackmassFinalMin > 0. && mass1 < m_4trackmassFinalMin) continue;
        if(m_4trackmassFinalMax > 0. && mass1 > m_4trackmassFinalMax) continue;
        double tau = m_CascadeTools->tau(moms[1],cascadeVertices[1],primaryVertex);
        if(tau < m_tauCut) continue;
//           ATH_MSG_INFO("Total chi " << x->fitChi2()<< " sum chi2 " <<  cascadeVertices[0]->chiSquared() + cascadeVertices[1]->chiSquared() );
        // Keep vertices (bear in mind that they come in reverse order!)
        for(int i =0; i<s_topoN; i++) Vtxwritehandles[i]->push_back(cascadeVertices[i]);
        x->setSVOwnership(false); // Prevent Container from deleting vertices
        const auto mainVertex = cascadeVertices[1];   // this is the B vertex

        // Set links to cascade vertices
        std::vector<const xAOD::Vertex*> verticestoLink;
        verticestoLink.push_back(cascadeVertices[0]);
        if(!BPhysPVCascadeTools::LinkVertices(CascadeLinksDecor, verticestoLink, Vtxwritehandles[0], cascadeVertices[1])) {
            ATH_MSG_ERROR("Error decorating with cascade vertices");
        }

        // loop over candidates -- Don't apply PV_minNTracks requirement here
        // because it may result in exclusion of the high-pt PV.
        // get good PVs

        xAOD::BPhysHypoHelper vtx(m_hypoName, mainVertex);

        // Get refitted track momenta from all vertices, charged tracks only
        BPhysPVCascadeTools::SetVectorInfo(vtx, x);

        // Decorate main vertex
        //
        // 1.a) mass, mass error

        BPHYS_CHECK( vtx.setMass(m_CascadeTools->invariantMass(moms[1])) );
        BPHYS_CHECK( vtx.setMassErr(m_CascadeTools->invariantMassError(moms[1],x->getCovariance()[1])) );
        // 1.b) pt and pT error (the default pt of mainVertex is != the pt of the full cascade fit!)
        Pt_decor(*mainVertex) = m_CascadeTools->pT(moms[1]);
        PtErr_decor(*mainVertex) = m_CascadeTools->pTError(moms[1],x->getCovariance()[1]);
        // 1.c) chi2 and ndof (the default chi2 of mainVertex is != the chi2 of the full cascade fit!)
        chi2_decor(*mainVertex) = x->fitChi2();
        ndof_decor(*mainVertex) = x->nDoF();
        ATH_CHECK(helper.FillCandwithRefittedVertices(m_refitPV, pvContainer,
                  refPvContainer, &(*m_pvRefitter), m_PV_max, m_DoVertexType, x, 1, m_4tracksMass, vtx));
        // 4) decorate the main vertex with D0 vertex mass, pt, lifetime and lxy values (plus errors)
        // D0 points to the main vertex, so lifetime and lxy are w.r.t the main vertex
        Mass_svdecor(*mainVertex) = m_CascadeTools->invariantMass(moms[0]);
        MassErr_svdecor(*mainVertex) = m_CascadeTools->invariantMassError(moms[0],x->getCovariance()[0]);
        Pt_svdecor(*mainVertex) = m_CascadeTools->pT(moms[0]);
        PtErr_svdecor(*mainVertex) = m_CascadeTools->pTError(moms[0],x->getCovariance()[0]);
        Lxy_svdecor(*mainVertex) = m_CascadeTools->lxy(moms[0],cascadeVertices[0],cascadeVertices[1]);
        LxyErr_svdecor(*mainVertex) = m_CascadeTools->lxyError(moms[0],x->getCovariance()[0], cascadeVertices[0],cascadeVertices[1]);
        Tau_svdecor(*mainVertex) = m_CascadeTools->tau(moms[0],cascadeVertices[0],cascadeVertices[1]);
        TauErr_svdecor(*mainVertex) = m_CascadeTools->tauError(moms[0],x->getCovariance()[0], cascadeVertices[0],cascadeVertices[1]);
        if(!threeVertexMap.empty()) {
            std::array<const xAOD::TrackParticle*, 3> lookuparray;
            std::copy(c.tracks.begin(), c.tracks.begin()+3, lookuparray.begin());
            auto ptr = threeVertexMap[lookuparray];
            if(ptr == nullptr) ATH_MSG_WARNING("3Vertex lookup found null");
            Vertex3Decor(*mainVertex) =  ptr ? VertexLink(ptr, *v3container) : VertexLink();
        }
    }


    ATH_MSG_DEBUG("Found " << Vtxwritehandles[0]->size() << " candidates " << totalnotnull << " were null");
    if(Vtxwritehandles[0]->size() > 200) ATH_MSG_WARNING("A lot of candidates N=" << Vtxwritehandles[0]->size());
    return StatusCode::SUCCESS;
}

std::unique_ptr<Trk::VxCascadeInfo>  Cascade3Plus1::CascadeFit(std::array<const xAOD::TrackParticle*, 4> &Track) const {
//  ATH_MSG_DEBUG("Running Cascade Fit");
    std::vector<const xAOD::TrackParticle*> tracksDs(Track.begin(), Track.begin()+3);
    std::vector<const xAOD::TrackParticle*> tracksBs(1, Track[3]);

    std::vector<double> massesDs(m_trackMasses.begin(), m_trackMasses.begin()+3);
    std::vector<double> massesBs(1, m_trackMasses[3]);

    std::unique_ptr<Trk::IVKalState> state = m_iVertexFitter->makeState();
    int robustness = 0;
    m_iVertexFitter->setRobustness(robustness, *state);
//  if(tracksDs.size() != massesDs.size()) ATH_MSG_ERROR("Track sizes do not match");
//  for(int i =0;i < tracksDs.size();i++) ATH_MSG_DEBUG("Num " << i << " track " << tracksDs[i] << " mass " << massesDs[i]);
    // Vertex list
    std::vector<Trk::VertexID> vrtList;
    // Ds vertex
    auto vID = m_iVertexFitter->startVertex(tracksDs, massesDs, *state, m_3TrackMassConstraint ? m_3tracksMass : 0.0);
    std::vector<Trk::VertexID> cnstV;
    if (m_2TrackMassConstraint && !m_iVertexFitter->addMassConstraint(vID, tracksDs, cnstV, *state, m_2tracksMass).isSuccess() ) {
        ATH_MSG_WARNING("addMassConstraint failed");
    }
    vrtList.push_back(vID);
    // Bs vertex
    m_iVertexFitter->nextVertex(tracksBs,massesBs,vrtList, *state);
    // Do the work
    auto x = std::unique_ptr<Trk::VxCascadeInfo> (m_iVertexFitter->fitCascade(*state));
    if(x==nullptr) ATH_MSG_VERBOSE("Cascade returned null");
    return x;
}

std::unique_ptr<xAOD::Vertex> Cascade3Plus1::StandardFit(const std::vector<const xAOD::TrackParticle*> &inputTracks, const xAOD::TrackParticleContainer* importedTrackCollection) const {
    assert(inputTracks.size() >=2);
    const Trk::Perigee& aPerigee1 = inputTracks[0]->perigeeParameters();
    const Trk::Perigee& aPerigee2 = inputTracks[1]->perigeeParameters();
    int sflag = 0;
    int errorcode = 0;
    Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(&aPerigee1,&aPerigee2,sflag,errorcode);
    if (errorcode != 0) {
        startingPoint(0) = 0.0;
        startingPoint(1) = 0.0;
        startingPoint(2) = 0.0;
    }
    std::unique_ptr<Trk::IVKalState> state = m_iVertexFitter->makeState();
    auto theResult = std::unique_ptr<xAOD::Vertex>( m_iVertexFitter->fit(inputTracks, startingPoint, *state));
    if(theResult) BPhysPVTools::PrepareVertexLinks(theResult.get(), importedTrackCollection);
    return theResult;
}

}
