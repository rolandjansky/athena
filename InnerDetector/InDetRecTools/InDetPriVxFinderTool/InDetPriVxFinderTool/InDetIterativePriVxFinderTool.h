/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * @class InDet::InDetIterativePriVxFinderTool
 *
 * @author Giacinto Piacquadio (Freiburg University)
 *
 *
 * This class provides an implementation for a primary 
 * vertex finding tool, which uses the Adaptive Vertex 
 * Fitter to reject outliers not belonging to the primary 
 * vertex interaction.
 *
 *
 ***************************************************************************/

//implemented using as template the InDetPriVxFinderTool class of A. Wildauer and F. Akesson

#ifndef INDETPRIVXFINDERTOOL_INDETITERATIVEPRIVXFINDERTOOL_H
#define INDETPRIVXFINDERTOOL_INDETITERATIVEPRIVXFINDERTOOL_H

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
// class IVertexTrackCompatibilityEstimator;
// class ImpactPoint3dAtaPlaneFactory;
 class IVertexLinearizedTrackFactory;
//  class ITrkDistanceFinder;
  
 class IVxCandidateXAODVertex;
}

namespace InDet
{
  class IInDetTrackSelectionTool;
  
 class InDetIterativePriVxFinderTool : public AthAlgTool, virtual public IVertexFinder
 {

public:

   /**
    * Constructor
    */
   
   InDetIterativePriVxFinderTool(const std::string& t, const std::string& n, const IInterface*  p);
   
   /**
    * Destructor
    */
   
   virtual ~InDetIterativePriVxFinderTool();
    
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

   double compatibility(const Trk::TrackParameters& measPerigee,
                        const Trk::RecVertex & vertex) const;

   void countTracksAndNdf(Trk::VxCandidate * myVxCandidate,
                          double & ndf, int & ntracks) const;

   ToolHandle< Trk::IVertexFitter > m_iVertexFitter;
   ToolHandle< InDet::IInDetTrackSelectionTool > m_trkFilter;
   ToolHandle< Trk::IVertexSeedFinder > m_SeedFinder;
//   ToolHandle< Trk::ITrkDistanceFinder > m_distFinder;
   ToolHandle< Trk::IImpactPoint3dEstimator > m_ImpactPoint3dEstimator;
//   ToolHandle< Trk::IVertexTrackCompatibilityEstimator > m_TrackCompatibilityEstimator;
//   ToolHandle< Trk::ImpactPoint3dAtaPlaneFactory > m_ImpactPoint3dAtaPlaneFactory;
   ToolHandle< Trk::IVertexLinearizedTrackFactory > m_LinearizedTrackFactory;
   ToolHandle< Trk::IVxCandidateXAODVertex > m_VertexEdmFactory;
   
   ServiceHandle<IBeamCondSvc> m_iBeamCondSvc; //!< pointer to the beam condition service

   bool m_useBeamConstraint;
   double m_significanceCutSeeding;
   double m_maximumChi2cutForSeeding;
   double m_maxVertices;

   bool m_createSplitVertices;
   int m_splitVerticesTrkInvFraction; ///< Integer: 1./fraction of tracks to be assigned to the tag split vertex 

   bool m_reassignTracksAfterFirstFit;

   bool m_doMaxTracksCut; 
   unsigned int m_maxTracks;

   void m_SGError(std::string errService);

   /**
    * Internal method to print the parameters setting
    */

   virtual void m_printParameterSettings();
 



 };//end of class definitions
}//end of namespace definitions
#endif
                                                                                                             
