/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DerivationFrameworkBPhys/VertexPlus1TrackCascade.h"

#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

namespace DerivationFramework {


    typedef std::vector<const xAOD::TrackParticle*> TrackBag;

    StatusCode VertexPlus1TrackCascade::initialize() {

        // retrieving vertex Fitter
        if ( m_iVertexFitter.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_iVertexFitter);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_DEBUG("Retrieved tool " << m_iVertexFitter);
        }
        
        // Get the track selector tool from ToolSvc
        if ( m_trkSelector.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tool " << m_trkSelector);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_DEBUG("Retrieved tool " << m_trkSelector);
        }
        if(!m_vertexContainerKey.key().empty()) ATH_CHECK(m_vertexContainerKey.initialize());
	if(!m_TrackPContainerKey.key().empty()) ATH_CHECK(m_TrackPContainerKey.initialize());
	if(!m_MuonsUsedInJpsiKey.key().empty()) ATH_CHECK(m_MuonsUsedInJpsiKey.initialize());

        return StatusCode::SUCCESS;
    }

    StatusCode VertexPlus1TrackCascade::finalize() {
        
        return StatusCode::SUCCESS;
        
    }

    VertexPlus1TrackCascade::VertexPlus1TrackCascade(const std::string& t, const std::string& n, const IInterface* p)  : AthAlgTool(t,n,p),
    m_vertexContainerKey(""),
    m_TrackPContainerKey(""),
    m_MuonsUsedInJpsiKey(""),
    m_Vtx1MassConstraint(0.),
    m_Vtx2MassConstraint(0.0),
    m_trkThresholdPt(0.0),
    m_trkMaxEta(102.5),
//    m_BThresholdPt(0.0),
//    m_BMassUpper(0.0),
//    m_BMassLower(0.0),
    m_roughMassLower(0.0),
    m_roughMassUpper(0.0),
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_trkSelector("InDet::TrackSelectorTool")
    {
       declareProperty("InitialVertices", m_vertexContainerKey);
       declareProperty("TrackParticleCollection", m_TrackPContainerKey);
       declareProperty("MuonCollection", m_MuonsUsedInJpsiKey);
       declareProperty("MassHypthesis", m_massHypothesis);
       declareProperty("MassContraintTracksVtx1", m_massConstraintTracksVtx1);
       declareProperty("MassContraintTracksVtx2", m_massConstraintTracksVtx2);

       declareProperty("Vtx1MassConstraint", m_Vtx1MassConstraint);
       declareProperty("Vtx2MassConstraint", m_Vtx2MassConstraint);

       declareProperty("trkThresholdPtCut", m_trkThresholdPt);
       declareProperty("trkMassEtaCut", m_trkMaxEta);
//       declareProperty("BThresholdPtCut", m_BThresholdPt);
//       declareProperty("BMassUpperCut", m_BMassUpper);
//       declareProperty("BMassLowerCut", m_BMassLower);

       declareProperty("RoughMassUpperCut", m_roughMassLower);
       declareProperty("RoughMassLowerCut", m_roughMassUpper);

    }

    VertexPlus1TrackCascade::~VertexPlus1TrackCascade(){ }

    double VertexPlus1TrackCascade::getInvariantMass(const TrackBag &Tracks, const std::vector<double> &massHypotheses){

      TLorentzVector total;
      total.SetVectM(Tracks[0]->p4().Vect(), massHypotheses[0]);
      TLorentzVector temp;
      for(size_t i=1; i < Tracks.size(); i++){
           temp.SetVectM(Tracks[i]->p4().Vect(), massHypotheses[i]);
           total += temp;
      }
      return total.M();
    }

    bool VertexPlus1TrackCascade::isContainedIn(const xAOD::TrackParticle* theTrack, const xAOD::MuonContainer* theColl) {
        bool isContained(false);
        xAOD::MuonContainer::const_iterator muItr;
        for (muItr=theColl->begin(); muItr!=theColl->end(); ++muItr) {
            if ( (*muItr)->inDetTrackParticleLink().cachedElement() == theTrack ) {isContained=true; break;}
        }
        return isContained;
    }

    StatusCode VertexPlus1TrackCascade::performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer) const
    {
        ATH_MSG_DEBUG( "VertexPlus1TrackCascade::performSearch" );
        assert(cascadeinfoContainer!=nullptr);
        SG::ReadHandle<xAOD::VertexContainer>  vertexContainer(m_vertexContainerKey);
        if(!vertexContainer.isValid()){
            ATH_MSG_ERROR("No VertexContainer with key " << m_vertexContainerKey.key() << " found in StoreGate. BCandidates will be EMPTY!");
            return StatusCode::FAILURE;
        }

        // Get tracks
        SG::ReadHandle<xAOD::TrackParticleContainer> TrackPContainer(m_TrackPContainerKey);
        if(!TrackPContainer.isValid()){
            ATH_MSG_ERROR("No track particle collection with name " << m_TrackPContainerKey.key() << " found in StoreGate!");
            return StatusCode::FAILURE;
        }


        // Get the muon collection used to build the J/psis
        const xAOD::MuonContainer*  importedMuonCollection = nullptr;
        if (!m_MuonsUsedInJpsiKey.key().empty()) {
            SG::ReadHandle<xAOD::MuonContainer>  handle(m_MuonsUsedInJpsiKey);
            if(handle.isValid()) importedMuonCollection = handle.cptr();
            else {
              ATH_MSG_FATAL("problem retrieving MuonContainer " << m_MuonsUsedInJpsiKey.key());
              return StatusCode::FAILURE;
            }
            ATH_MSG_DEBUG("Muon container size "<< importedMuonCollection->size());
        }
        
        // Select the inner detector tracks
        TrackBag theIDTracksAfterSelection;
        for (auto tp : *TrackPContainer){
            if ( tp->pt()<m_trkThresholdPt ) continue;
            if ( fabs(tp->eta())>m_trkMaxEta ) continue;
            if (importedMuonCollection!=NULL) {
                if (isContainedIn(tp, importedMuonCollection)) continue;
            }
            if ( m_trkSelector->decision(*tp, NULL) ) theIDTracksAfterSelection.push_back(tp);
        }

        const std::vector<double> &fullMassHypoth = (m_massHypothesis);
        const std::vector<double> initialVertexMassHypo(fullMassHypoth.begin(), fullMassHypoth.end()-1);

        TrackBag originalVertexTracks(initialVertexMassHypo.size());
        TrackBag secondVertexTracks(fullMassHypoth.size());

        const std::vector< Trk::VertexID > emptyVtxList;
        TrackBag ConstraintTracksVtx1(m_massConstraintTracksVtx1.size());
        TrackBag ConstraintTracksVtx2(m_massConstraintTracksVtx2.size());        

        assert(fullMassHypoth.size() == secondVertexTracks.size());

        for(auto vertex : *vertexContainer){ //Iterate over previous vertices

	   size_t OriginaltrackNum = vertex->nTrackParticles();
           if(initialVertexMassHypo.size() != OriginaltrackNum){
               ATH_MSG_FATAL("Mass hypothesis not correctly set");
               return StatusCode::FAILURE;
           }
           for(size_t i = 0;i<OriginaltrackNum;i++) 
              originalVertexTracks[i] = secondVertexTracks[i] =  (vertex->trackParticle(i));
           
           for(auto newtrack : theIDTracksAfterSelection){
              //Skip any track already used in vertex
              if(std::find(originalVertexTracks.begin(), originalVertexTracks.end(), newtrack) != originalVertexTracks.end()) continue;

              secondVertexTracks.back() = newtrack;

              double roughmass = getInvariantMass(secondVertexTracks, fullMassHypoth);

              if(m_roughMassUpper > 0.0 && (roughmass < m_roughMassLower || roughmass > m_roughMassUpper)) continue;

              m_iVertexFitter->setDefault();
              m_iVertexFitter->setRobustness( 0 );

              auto vID1 = m_iVertexFitter->startVertex( originalVertexTracks, initialVertexMassHypo );
              auto vID2 = m_iVertexFitter->nextVertex( secondVertexTracks, fullMassHypoth );

              if(!m_massConstraintTracksVtx1.empty()){
                  for(size_t i =0; i<m_massConstraintTracksVtx1.size(); i++) ConstraintTracksVtx1[i] = originalVertexTracks.at(m_massConstraintTracksVtx1[i]);
                  if( !m_iVertexFitter->addMassConstraint( vID1, ConstraintTracksVtx1, emptyVtxList, m_Vtx1MassConstraint ).isSuccess() ) {
                      ATH_MSG_WARNING( "cascade fit: addMassConstraint failed" );
                  }
              }

              if(!m_massConstraintTracksVtx2.empty()){
                  for(size_t i =0; i<m_massConstraintTracksVtx2.size(); i++) ConstraintTracksVtx2[i] = secondVertexTracks.at(m_massConstraintTracksVtx2[i]);
                  if( !m_iVertexFitter->addMassConstraint( vID2, ConstraintTracksVtx2, emptyVtxList, m_Vtx2MassConstraint ).isSuccess() ) {
                      ATH_MSG_WARNING( "cascade fit: addMassConstraint failed" );
                  }
              }
              
              auto result  = m_iVertexFitter->fitCascade();
              if(result ==nullptr ){  ATH_MSG_WARNING("Cascade Fit failed"); continue; }
              assert(result->vertices().size()==2);
              cascadeinfoContainer->push_back(result);
              
           }

        }
        ATH_MSG_DEBUG("cascadeinfoContainer size " << cascadeinfoContainer->size());
        return StatusCode::SUCCESS;
    }


}


