/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexFitters_SequentialVertexFitter_H
#define TrkVertexFitters_SequentialVertexFitter_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBase.h"

//xAOD includes
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/NeutralParticle.h"


/**
 *
 * @class Trk::SequentialVertexFitter 
 *
 * An iterative vertex fitter, updating the vertex estimate with
 * a single at the time. The measurement equation is relinearized
 * with the position of the resulting estimate and the vertex is refit
 * The iterations are performed until the vertex position improves or
 * the maximal niumber of iterations is reached. 
 * Smoothing (refit of tracks with the knowledge of the vertex position) 
 * is then performed on request.
 *
 * The fitter can be used with arbitrary VertexUpdator,
 * VertexSmoother and LinearizedTrackFactory 
 *
 * @author Kirill Prokofiev, November 2005
 *
 * ---------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-19)
 * EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
 *                         from Trk::RecVertex   to xAOD::Vertex,
 *                         from Trk::Vertex      to Amg::Vector3D
 */


namespace Trk
{
 class Track;
 class VxTrackAtVertex;
 class IVertexUpdator;
 class IVertexSmoother;
 class IVertexLinearizedTrackFactory;
 
 class SequentialVertexFitter : public extends<AthAlgTool, IVertexFitter>
 {
  public:
    // The following 'using' can be removed when IVertexFitter::fit has been fully migrated to the one with the EventContext
    using Trk::IVertexFitter::fit;
  
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /**
     * constructor
     */
    
    SequentialVertexFitter(const std::string& t, const std::string& n, const IInterface*  p);

    /**
     * destructor
     */
    
    virtual ~SequentialVertexFitter();
    
    /** 
     *Interface for xAOD::TrackParticle and xAOD::NeutralParticle with starting point 
     */
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
			       const std::vector<const xAOD::NeutralParticle*>& vectorNeut,
			       const Amg::Vector3D& startingPoint) const override;
    /** 
     *Interface for xAOD::TrackParticle with starting point 
     */
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
			       const Amg::Vector3D& startingPoint) const override
   {return fit(vectorTrk, std::vector<const xAOD::NeutralParticle*>(), startingPoint);};

    /** 
     * Interface for xAOD::TrackParticle and xAOD::NeutralParticle with vertex constraint 
     * the position of the constraint is ALWAYS the starting point 
     */
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
			       const std::vector<const xAOD::NeutralParticle*>& vectorNeut,
			       const xAOD::Vertex& constraint) const override;
    /** 
     * Interface for xAOD::TrackParticle with vertex constraint 
     * the position of the constraint is ALWAYS the starting point 
     */
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
                                 const xAOD::Vertex& constraint)  const override
   {return fit(vectorTrk, std::vector<const xAOD::NeutralParticle*>(), constraint);};

    
    
    /**
     * Vertex fit from list of track and neutral parameters
     * and a starting point
     */   
    virtual xAOD::Vertex * fit(const std::vector<const Trk::TrackParameters*> & perigeeList, 
			       const std::vector<const Trk::NeutralParameters*> & neutralPerigeeList,
                               const Amg::Vector3D& startingPoint) const override;
    /**
     * Vertex fit from list of track and parameters
     * and a starting point
     */   
    virtual xAOD::Vertex * fit(const std::vector<const Trk::TrackParameters*> & paramList,
                               const Amg::Vector3D& startingPoint) const override
   {return fit(paramList, std::vector<const Trk::NeutralParameters*>(), startingPoint);};
    
    /**
     * Vertex fit from the vector of track and neutral parameters
     * with a preliminary knowledge (vertex constraint)
     */ 
    virtual xAOD::Vertex * fit(const std::vector<const Trk::TrackParameters*> & perigeeList, 
			       const std::vector<const Trk::NeutralParameters*> & neutralPerigeeList,
                               const xAOD::Vertex& constraint) const override;
    virtual xAOD::Vertex * fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
			       const xAOD::Vertex& constraint) const override
   {return fit(perigeeList, std::vector<const Trk::NeutralParameters*>(), constraint);};

    /**
     * Additional fit methods.
     * These will later work with selected finder..
     * So far these methods work with default (0,0,0)
     * starting point and "huge" diagonal error matrix
     */

   virtual xAOD::Vertex* fit(
     const std::vector<const Trk::TrackParameters*>& perigeeList,
     const std::vector<const Trk::NeutralParameters*>& neutralPerigeeList)
     const override;
   virtual xAOD::Vertex* fit(
     const std::vector<const Trk::TrackParameters*>& perigeeList) const override
   {return fit(perigeeList, std::vector<const Trk::NeutralParameters*>());};
    
    
 private:
    
   /**
    * Internal method related to the linearization of tracks (initial linearization)
    */

   std::vector<Trk::VxTrackAtVertex> linearizeTracks(
     const std::vector<const Trk::TrackParameters*>& perigeeList,
     const std::vector<const Trk::NeutralParameters*>& neutralPerigeeList,
     const xAOD::Vertex& vrt) const;

   /**
    * Relinearization on iterations
    */

   void reLinearizeTracks(std::vector<Trk::VxTrackAtVertex>& tracks, const Amg::Vector3D & vrt)const;
 
   ToolHandle< IVertexUpdator > m_Updator;
   ToolHandle< IVertexSmoother > m_Smoother;
   ToolHandle< Trk::IVertexLinearizedTrackFactory > m_LinTrkFactory;  
   
   /**
    * Flag controlling optional smoothing
    */
   
   bool m_doSmoothing; 
   
   /**
    * Max number of iterations to perform (in case of no convergence)
    */
   
   unsigned int m_maxStep;
   
   /**
    * Max shift (represents the convergence criterion)
    */
   
   float m_maxShift;

/**
 * Use loose convergence criterium (maxShift) or hard (+maxDeltaChi2)
 */

   bool m_useLooseConvergence;
   
/**
 * Max DeltaChi2 allowed in hard convergence criterium
 */

   float m_maxDeltaChi2;

    /*
     * Avoid vertex to be initialize outside ID envelope given bu m_maxR and m_maxZ
     */

    double m_maxR;
    double m_maxZ;

 }; //end of class description
}//end of namespace definition

#endif
