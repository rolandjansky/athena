/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 *
 * @class InDet::InDetIterativeSecVtxFinderTool
 * @author  Lianyou SHAN ( IHEP )
 *
 * developed upon the InDetPriVxFinderTool by :
 * @author Giacinto Piacquadio (Freiburg University)
 *
 *
 * This class provides an implementation for a secondary
 * vertex finding tool, which uses the Adaptive Vertex 
 * Fitter to reject outliers not belonging to the vertex interaction.
 *
 ***************************************************************************/

//implemented using as template the InDetPriVxFinderTool class of A. Wildauer and F. Akesson

#ifndef INDETPRIVXFINDERTOOL_INDETITERATIVESECVXFINDERTOOL_H
#define INDETPRIVXFINDERTOOL_INDETITERATIVESECVXFINDERTOOL_H

#include "InDetRecToolInterfaces/IInDetIterativeSecVtxFinderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkTrack/TrackCollection.h" // type def ...
#include "TrkParticleBase/TrackParticleBaseCollection.h" // type def ...
#include "TrkParameters/TrackParameters.h"
// we may save out some private stuff
#include "TTree.h"
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
// class IVertexTrackCompatibilityEstimator;
// class ImpactPoint3dAtaPlaneFactory;
 class IVertexLinearizedTrackFactory;
// class ITrkDistanceFinder;
 class IVxCandidateXAODVertex;
}
//
//
namespace InDet
{
 class IInDetTrackSelectionTool;
  
//  InnerDetector/InDetValidation/InDetVertexSplitter/InDetVertexSplitterHist
//  InnerDetector/InDetDigitization/FastSiDigitization/SiSmearedDigitizationTool
//  Tracking/TrkVertexFitter/TrkVertexSeedFinderUtils/share/ImagingSeedTuningAlg_jobOptions.py
 class InDetIterativeSecVtxFinderTool : public AthAlgTool, virtual public ISecVertexFinder
 {

public:

   /**
    * Constructor
    */
   
   InDetIterativeSecVtxFinderTool(const std::string& t, const std::string& n, const IInterface*  p);
   
   /**
    * Destructor
    */
   
   virtual ~InDetIterativeSecVtxFinderTool();
    
   StatusCode initialize();
   StatusCode finalize();

   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> 
         findVertex(const TrackCollection* trackTES);
   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> 
         findVertex(const Trk::TrackParticleBaseCollection* trackTES);

   /** 
    * Finding method.
    * Has as input a track collection and as output 
    * a VxContainer.
    */

   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> 
         findVertex(const xAOD::TrackParticleContainer* trackParticles);  // overwritten 
   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
         findVertex( const std::vector<const xAOD::IParticle*> & inputTracks)  ;

   void setPriVtxPosition( double , double , double ) ;

   int getModes1d( std::vector<int> *, std::vector<int> *, std::vector<int> * ) const ;

 private:

   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> 
     findVertex( const std::vector<Trk::ITrackLink*> & trackVector) ;

   bool passHitsFilter( const Trk::TrackParameters* , float vtxR, float absvz ) const;

   bool V0kine( const std::vector<Amg::Vector3D>, const Amg::Vector3D &position , float & , float & ) const;


   const std::vector<Amg::Vector3D> getVertexMomenta( xAOD::Vertex * myxAODVertex ) const ;
//   double MomentumDirection( int, const Amg::Vector3D & ) const ;


   double VrtVrtDist( xAOD::Vertex * v1, xAOD::Vertex * v2 ) const;

   float removeTracksInBadSeed( xAOD::Vertex * myxAODVertex, std::vector<const Trk::TrackParameters*> & ) const ;

   void FillXcheckdefauls() ;

   int m_filterLevel ;   // from 1 up to 5 options

   float m_privtxRef, m_minVtxDist ;

   Amg::Vector3D m_privtx ;
   mutable std::vector< Amg::Vector3D > m_TrkAtVtxMomenta ;

// watch below for possible update from InDetIterativePriVxFinderTool 

   void removeCompatibleTracks(xAOD::Vertex * myxAODVertex,
                               std::vector<const Trk::TrackParameters*> & perigeesToFit,
                               std::vector<Trk::ITrackLink*> & seedTracks) const;

   void removeAllFrom(std::vector<const Trk::TrackParameters*> & perigeesToFit,
                      std::vector<Trk::ITrackLink*> & seedTracks) const;

//   double compatibility(const Trk::TrackParameters& measPerigee,
//                        const xAOD::Vertex & vertex  ) const;
   double compatibility(const Trk::TrackParameters& measPerigee,
                  const AmgSymMatrix(3) &covariancePosition , const Amg::Vector3D &position ) const ;
   void countTracksAndNdf(xAOD::Vertex * myxAODVertex,
                          float & ndf, int & ntracks) const;



   ToolHandle< Trk::IVertexFitter > m_iVertexFitter;
   ToolHandle< InDet::IInDetTrackSelectionTool > m_trkFilter;
   ToolHandle< InDet::IInDetTrackSelectionTool > m_SVtrkFilter;
   ToolHandle< Trk::IVertexSeedFinder > m_SeedFinder;
   ToolHandle< Trk::IImpactPoint3dEstimator > m_ImpactPoint3dEstimator;
   ToolHandle< Trk::IVertexLinearizedTrackFactory > m_LinearizedTrackFactory;
   
   ServiceHandle<IBeamCondSvc> m_iBeamCondSvc; //!< pointer to the beam condition service

   bool m_useBeamConstraint;
   double m_significanceCutSeeding;
   double m_maximumChi2cutForSeeding;
   double m_minWghtAtVtx ;
   double m_maxVertices;
   double m_CutHitsFilter ;

   bool m_createSplitVertices;
   int m_splitVerticesTrkInvFraction; ///< Integer: 1./fraction of tracks to be assigned to the tag split vertex 

   bool m_reassignTracksAfterFirstFit;

   bool m_doMaxTracksCut; 
   unsigned int m_maxTracks;

   std::vector<const Trk::TrackParameters*> * m_seedperigees;

   void SGError(std::string errService);

   virtual void printParameterSettings();
 
   TTree*              m_OTree ;

   long int m_evtNum ;
   int m_iterations ;

   std::vector<int> *m_leastmodes ;
   std::vector< std::vector < float > > * m_sdFsmwX;
   std::vector< std::vector < float > > * m_sdFsmwY;
   std::vector< std::vector < float > > * m_sdFsmwZ;
   std::vector< std::vector < float > > * m_sdcrsWght;

   std::vector<int> * m_nperiseed ;
   std::vector < float > *m_seedX ;
   std::vector < float > *m_seedY ;
   std::vector < float > *m_seedZ ;
   std::vector < float > *m_seedXYdist ;
   std::vector < float > *m_seedZdist ;
   std::vector < int > *m_seedac ;

   mutable float m_v0mass, m_v0ee, m_dir, m_ndf, m_hif ;
   mutable int m_ntracks ;
   mutable bool m_goodVertex ;
   mutable std::vector< Amg::VectorX > m_trkdefiPars ;

 };//end of class definitions
}//end of namespace definitions
#endif
                                                                                                             
