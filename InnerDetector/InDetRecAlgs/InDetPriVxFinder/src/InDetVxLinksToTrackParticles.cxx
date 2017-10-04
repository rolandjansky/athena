/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetVxLinksToTrackParticles.cxx  -  Description
                             -------------------
    begin   : 19-03-2014
    authors; Kirill.Prokofiev@cern.ch
 ***************************************************************************/


#include "InDetPriVxFinder/InDetVxLinksToTrackParticles.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "CxxUtils/make_unique.h"

#include "ITrackToVertex/ITrackToVertex.h" 
#include "TrkSurfaces/PerigeeSurface.h"
#include "xAODTracking/Vertex.h"
// #include "TrkParameters/TrackParameters.h"
// #include "xAODTracking/TrackingPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"

namespace {
  inline void setDefiningParameters( xAOD::TrackParticle& tp, const Trk::Perigee& perigee ) {
    const AmgSymMatrix(5)* covMatrix = perigee.covariance();
    if( !covMatrix ) throw std::runtime_error("no covariancce");
    tp.setDefiningParameters(perigee.parameters()[Trk::d0],
                             perigee.parameters()[Trk::z0],
                             perigee.parameters()[Trk::phi0],
                             perigee.parameters()[Trk::theta],
                             perigee.parameters()[Trk::qOverP]);

    std::vector<float> covMatrixVec;
    Amg::compress(*covMatrix,covMatrixVec);
    tp.setDefiningParametersCovMatrixVec(covMatrixVec);
    const Amg::Vector3D& surfaceCenter = perigee.associatedSurface().center(); 
    tp.setParametersOrigin(surfaceCenter.x(), surfaceCenter.y(), surfaceCenter.z() );
  }
}

namespace std {
  inline MsgStream& operator<<(MsgStream &msg, const xAOD::TrackParticle &tp) {
    msg << "q/p=" << tp.qOverP()  << "+-" << tp.definingParametersCovMatrixVec().at(14)
        << " phi=" << tp.phi()    << "+-" << tp.definingParametersCovMatrixVec().at(5)
        << " theta=" << tp.theta()<< "+-" << tp.definingParametersCovMatrixVec().at(9) 
        << " d0=" << tp.d0()      << "+-" << tp.definingParametersCovMatrixVec().at(0) 
        << " z0=" << tp.z0()      << "+-" << tp.definingParametersCovMatrixVec().at(2) 
        << " vx=" << tp.vx() << "," << tp.vy() << "," << tp.vz() ;
    return msg;
  }
  inline std::ostream& operator<<(std::ostream &msg, const xAOD::TrackParticle &tp) {
    msg << "q/p=" << tp.qOverP()  << "+-" << tp.definingParametersCovMatrixVec().at(14)
        << " phi=" << tp.phi()    << "+-" << tp.definingParametersCovMatrixVec().at(5)
        << " theta=" << tp.theta()<< "+-" << tp.definingParametersCovMatrixVec().at(9) 
        << " d0=" << tp.d0()      << "+-" << tp.definingParametersCovMatrixVec().at(0) 
        << " z0=" << tp.z0()      << "+-" << tp.definingParametersCovMatrixVec().at(2) 
        << " vx=" << tp.vx() << "," << tp.vy() << "," << tp.vz() ;
    return msg;
  }
}

namespace InDet
{

   InDetVxLinksToTrackParticles::
   InDetVxLinksToTrackParticles( const std::string &n, ISvcLocator *pSvcLoc )
      : AthAlgorithm( n, pSvcLoc ),
        m_verticesName( "PrimaryVertices" ),
        m_tracksName( "TrackParticles" ),
        m_tracksOutName{}, // set to non empty to create a shallow copy "TrackParticlesPV" 
        m_trackToVertex(""), // set to non empty to extrapolate to PV "Reco::TrackToVertex/TrackToVertex"
        m_copy(false)
   {
      declareProperty ( "VerticesName",m_verticesName );
      declareProperty ( "TracksName",  m_tracksName );
      declareProperty ( "TrackToVertex", m_trackToVertex );
      declareProperty ( "TracksOutName", m_tracksOutName );
   }

   StatusCode InDetVxLinksToTrackParticles::initialize()
   {
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      if (!m_trackToVertex.empty()) {
        ATH_CHECK( m_trackToVertex.retrieve());        
        if (!m_tracksOutName.empty()) m_copy=true;
      }
      return StatusCode::SUCCESS;
   }

   StatusCode InDetVxLinksToTrackParticles::execute()
   {
     const SG::DataProxy* proxy =
       evtStore()->proxy (ClassID_traits<xAOD::TrackParticleContainer>::ID(),
                          m_tracksName);
     if (!proxy) {
       ATH_MSG_WARNING( "No xAOD::TrackParticleContainer "
                        << "with key \"" << m_tracksName << "\" found" );
       return StatusCode::SUCCESS;
     }

     std::pair< xAOD::TrackParticleContainer*, xAOD::ShallowAuxContainer* >
       shallow_copy_track_collection = std::make_pair(static_cast<xAOD::TrackParticleContainer*>(nullptr),
                                               static_cast<xAOD::ShallowAuxContainer*>(nullptr));

     xAOD::TrackParticleContainer* trackParticleCollection=nullptr;


     if (m_copy) {
       // create a shallow copy
       const xAOD::TrackParticleContainer* orig_track_collectionl = nullptr;
       ATH_CHECK( evtStore()->retrieve (orig_track_collectionl,
                                        m_tracksName) );
       shallow_copy_track_collection  = xAOD::shallowCopyContainer( *orig_track_collectionl );
       ATH_CHECK( evtStore()->record( shallow_copy_track_collection.first,  m_tracksOutName ) );
       ATH_CHECK( evtStore()->record( shallow_copy_track_collection.second,  m_tracksOutName+"Aux." ) );
       trackParticleCollection = shallow_copy_track_collection.first;
     }
     else if (proxy->isConst()) {
       const xAOD::TrackParticleContainer* orig_track_collectionl = nullptr;
       ATH_CHECK( evtStore()->retrieve (orig_track_collectionl,
                                        m_tracksName) );

       trackParticleCollection = new xAOD::TrackParticleContainer;
       auto store = CxxUtils::make_unique<xAOD::TrackParticleAuxContainer>();
       trackParticleCollection->setStore (store.get());
       for (const xAOD::TrackParticle* p : *orig_track_collectionl) {
         trackParticleCollection->push_back (new xAOD::TrackParticle);
         *trackParticleCollection->back() = *p;
       }
       ATH_CHECK( evtStore()->overwrite (trackParticleCollection,
                                     m_tracksName,
                                     true, false) );
       ATH_CHECK( evtStore()->overwrite (std::move(store),
                                     m_tracksName + "Aux.",
                                     true, false) );
     }
     else {
       ATH_CHECK( evtStore()->retrieve (trackParticleCollection,
                                        m_tracksName) );
     }

      const xAOD::VertexContainer* vertexCollection( 0 ); 
      if( ! evtStore()->contains< xAOD::VertexContainer >( m_verticesName ) ) {
         ATH_MSG_WARNING( "No xAOD::VertexContainer with key \""
                          << m_verticesName << "\" found" );
         return StatusCode::SUCCESS;
      }
      ATH_CHECK( evtStore()->retrieve( vertexCollection, m_verticesName ) );

      
      // Iterating over the vertex container in the evt Store
      const size_t nvx = vertexCollection->size();
      size_t first_vertex=nvx;
      for( size_t vertex_i = 0; vertex_i < nvx; ++vertex_i )
         {
            const std::vector< ElementLink< xAOD::TrackParticleContainer > >&
               tpLinks = ( *vertexCollection )[ vertex_i ]->trackParticleLinks();

            ElementLink< xAOD::VertexContainer > link;
            link.resetWithKeyAndIndex( m_verticesName, vertex_i );
            
            //iterating over the links
            const size_t tp_size = tpLinks.size();

            // remember the first primary vertex with associated track particles.
            if (first_vertex >= nvx && ( *vertexCollection )[ vertex_i ]->vertexType() == xAOD::VxType::PriVtx ) {
              first_vertex=vertex_i;
            }
            for( size_t tp = 0; tp < tp_size; ++tp )
               {
                  const ElementLink< xAOD::TrackParticleContainer >& tpl =
                     tpLinks[ tp ];

                  // Skip invalid element links:
                  if( ! tpl.isValid() )
                     {
                        ATH_MSG_WARNING( "Invalid ElementLink found on "
                                         << "vertex number " << vertex_i );
                        continue;
                     }

                  //checking a container name consitency
                  if( tpl.dataID() != m_tracksName )
                     {
                        ATH_MSG_WARNING( "The key received from tracks in "
                                         << "vertex: " << tpl.key()
                                         << " does not correspond to the "
                                         << "one of the track collection "
                                         << m_tracksName );
                        continue;
                     }

                  const size_t tpl_index = tpl.index();
                  ( *trackParticleCollection )[ tpl_index ]->setVertexLink( link );

               }//end of track particle collection size check
         }//end of the vertex loop

      // if there is a track to vertex tool modify track parameters to those wrt the first primary vertex with at 
      // least one associated track particle.
      if (first_vertex < nvx && !m_trackToVertex.empty()) { 
        const xAOD::Vertex *vxCandidate = vertexCollection->at(first_vertex);
        for (xAOD::TrackParticle *track_particle: *trackParticleCollection) {
          const Trk::Perigee* result =  m_trackToVertex->perigeeAtVertex( *track_particle, vxCandidate->position());
          if (result) {
            // ATH_MSG_DEBUG( "Updated track parameters for  " << *track_particle << "to primary vertex " << vxCandidate->position() );
            const AmgSymMatrix(5)* covMatrix = result->covariance();
            if( !covMatrix ) ATH_MSG_WARNING( "Perigee without covariance" << *result );
            else  setDefiningParameters(*track_particle, *result);
            // ATH_MSG_DEBUG( "new parameters  " << *track_particle << "." );
	    delete result;
          }
          else {
            ATH_MSG_WARNING( "Failed to extrapolate " << *track_particle << "to primary vertex " << vxCandidate->position() );
          }

        }
      }
      return StatusCode::SUCCESS;
   } //end of execute method

   StatusCode InDetVxLinksToTrackParticles::finalize()
   {
      ATH_MSG_INFO( "Summary from Primary Vertex Finder "
                    "(InnerDetector/InDetRecAlgs/"
                    "InDetVxLinksToTrackParticles)" );
      return StatusCode::SUCCESS;
   }

} // end namespace InDet
