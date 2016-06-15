/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * @class InDet::InDetMultiPriVxFinderTool
 *
 * @author Matt Rudolph and Lars Egholm Pedersen
 *
 *
 * Use seed finder multi seeding interface method similarly to iterative vertex finder
 *
 * --------------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-26)
 *
 * EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex
 *
 *   findVertex will now always return an xAOD::VertexContainer,
 *   even when using a TrackCollection or a TrackParticleBaseCollection
 *   as input.
 *
 ***************************************************************************/

#ifndef INDETPRIVXFINDERTOOL_INDETMULTIPRIVXFINDERTOOL_H
#define INDETPRIVXFINDERTOOL_INDETMULTIPRIVXFINDERTOOL_H

#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkTrack/TrackCollection.h" // type def ...
#include "TrkParticleBase/TrackParticleBaseCollection.h" // type def ...
#include "TrkParameters/TrackParameters.h"

/**
 * Forward declarations 
 */
 
class IBeamCondSvc;
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

namespace Trk
{
 class IVertexFitter;
 class Track;
 class TrackParticleBase;
 class ITrackLink;
 class IVertexSeedFinder;
 class IImpactPoint3dEstimator;
 class IVertexCollectionSortingTool;
 class IVertexCollectionMergingTool;

 class IVxCandidateXAODVertex;
}

namespace InDet
{
  class IInDetTrackSelectionTool;
  
 class InDetMultiPriVxFinderTool : public AthAlgTool, virtual public IVertexFinder
 {

public:

   /**
    * Constructor
    */
   
   InDetMultiPriVxFinderTool(const std::string& t, const std::string& n, const IInterface*  p);
   
   /**
    * Destructor
    */
   
   virtual ~InDetMultiPriVxFinderTool();
    
   StatusCode initialize();

   /** 
    * Finding method.
    * Has as input a track collection and as output 
    * a VxContainer.
    */

   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const TrackCollection* trackTES);
   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const Trk::TrackParticleBaseCollection* trackTES);
   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const xAOD::TrackParticleContainer* trackParticles);

   StatusCode finalize();
   
 private:
   
   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const std::vector<Trk::ITrackLink*> & trackVector) const;

   void removeCompatibleTracks(xAOD::Vertex * myxAODVertex,
                               std::vector<const Trk::TrackParameters*> & perigeesToFit,
                               std::vector<Trk::ITrackLink*> & seedTracks) const;
   
   void removeAllFrom(std::vector<const Trk::TrackParameters*> & perigeesToFit,
                      std::vector<Trk::ITrackLink*> & seedTracks) const;

   double compatibility(const Trk::TrackParameters & measPerigee,
                        const xAOD::Vertex & vertex) const;

   void countTracksAndNdf(xAOD::Vertex * myxAODVertex,
                          double & ndf, int & ntracks) const;

   double distanceAndError(const Trk::TrackParameters* params, const Amg::Vector3D * vertex, double & err) const;


   ToolHandle< Trk::IVertexFitter > m_iVertexFitter;
   ToolHandle< InDet::IInDetTrackSelectionTool > m_trkFilter;
   ToolHandle< Trk::IVertexSeedFinder > m_SeedFinder;
   ToolHandle< Trk::IImpactPoint3dEstimator > m_ImpactPoint3dEstimator;
   ToolHandle< Trk::IVxCandidateXAODVertex > m_VertexEdmFactory;


   ServiceHandle<IBeamCondSvc> m_iBeamCondSvc; //!< pointer to the beam condition service

   bool m_useBeamConstraint;
   double m_significanceCutSeeding;
   double m_maximumChi2cutForSeeding;
   double m_maxVertices;

   bool m_createSplitVertices;
   int m_splitVerticesTrkInvFraction; ///< Integer: 1./fraction of tracks to be assigned to the tag split vertex 

   bool m_reassignTracksAfterFirstFit;
   
   /// enable merging of vertices after finding
   //   bool m_doRemerging;
   
   void m_SGError(std::string errService);

   /**
    * Internal method to print the parameters setting
    */

   virtual void m_printParameterSettings();
 



 };//end of class definitions
}//end of namespace definitions
#endif
                                                                                                             
