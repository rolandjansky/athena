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


namespace InDet
{

   InDetVxLinksToTrackParticles::
   InDetVxLinksToTrackParticles( const std::string &n, ISvcLocator *pSvcLoc )
      : AthAlgorithm( n, pSvcLoc ),
        m_verticesName( "PrimaryVertices" ),
        m_tracksName( "TrackParticles" )
   {
      declareProperty ( "VerticesName",m_verticesName );
      declareProperty ( "TracksName",  m_tracksName );
   }

   StatusCode InDetVxLinksToTrackParticles::initialize()
   {
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      return StatusCode::SUCCESS;
   }

   StatusCode InDetVxLinksToTrackParticles::execute()
   {

      //non-const access to the container
      xAOD::TrackParticleContainer* trackParticleCollection( 0 );
      if( ! evtStore()->transientContains< xAOD::TrackParticleContainer >( m_tracksName ) ) {
         ATH_MSG_WARNING( "No modifyable xAOD::TrackParticleContainer "
                          << "with key \"" << m_tracksName << "\" found" );
         return StatusCode::SUCCESS;
      }
      ATH_CHECK( evtStore()->retrieve( trackParticleCollection,
                                       m_tracksName ) );

      const xAOD::VertexContainer* vertexCollection( 0 ); 
      if( ! evtStore()->contains< xAOD::VertexContainer >( m_verticesName ) ) {
         ATH_MSG_WARNING( "No xAOD::VertexContainer with key \""
                          << m_verticesName << "\" found" );
         return StatusCode::SUCCESS;
      }
      ATH_CHECK( evtStore()->retrieve( vertexCollection, m_verticesName ) );

      // Iterating over the vertex container in the evt Store
      const size_t nvx = vertexCollection->size();
      for( size_t i = 0; i < nvx; ++i )
         {
            const std::vector< ElementLink< xAOD::TrackParticleContainer > >&
               tpLinks = ( *vertexCollection )[ i ]->trackParticleLinks();

            ElementLink< xAOD::VertexContainer > link;
            link.resetWithKeyAndIndex( m_verticesName, i );

            //iterating over the links
            const size_t tp_size = tpLinks.size();
            for( size_t tp = 0; tp < tp_size; ++tp )
               {
                  const ElementLink< xAOD::TrackParticleContainer >& tpl =
                     tpLinks[ tp ];

                  // Skip invalid element links:
                  if( ! tpl.isValid() )
                     {
                        ATH_MSG_WARNING( "Invalid ElementLink found on "
                                         << "vertex number " << i );
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
