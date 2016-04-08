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
 
class VxContainer;
class IBeamCondSvc;
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

namespace Trk
{
 class IVertexFitter;
 class VxCandidate;
 class Track;
 class TrackParticleBase;
 class RecVertex;
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

   VxContainer* findVertex(const TrackCollection* trackTES);//by GP: deleted const before VxContainer*
   VxContainer* findVertex(const Trk::TrackParticleBaseCollection* trackTES);
   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>  findVertex(const xAOD::TrackParticleContainer* trackParticles);

   StatusCode finalize();
   
 private:
   
   VxContainer* findVertex(const std::vector<Trk::ITrackLink*> & trackVector) const;

   void removeCompatibleTracks(Trk::VxCandidate * myVxCandidate,
                               std::vector<const Trk::TrackParameters*> & perigeesToFit,
                               std::vector<Trk::ITrackLink*> & seedTracks) const;
   
   void removeAllFrom(std::vector<const Trk::TrackParameters*> & perigeesToFit,
                      std::vector<Trk::ITrackLink*> & seedTracks) const;

   double compatibility(const Trk::TrackParameters & measPerigee,
                        const Trk::RecVertex & vertex) const;

   void countTracksAndNdf(Trk::VxCandidate * myVxCandidate,
                          double & ndf, int & ntracks) const;

   double distanceAndError(const Trk::TrackParameters* params, const Trk::Vertex * vertex, double & err) const;


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
                                                                                                             
