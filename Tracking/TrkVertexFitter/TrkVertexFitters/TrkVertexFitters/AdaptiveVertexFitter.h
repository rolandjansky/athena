/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKADAPTIVEFITTERTOOLS_ADAPTIVEVERTEXFITTER_H
#define TRKADAPTIVEFITTERTOOLS_ADAPTIVEVERTEXFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"

//xAOD includes
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/NeutralParticle.h"



namespace Trk
{
  class Track;
  class TrackParticleBase;
  class VxTrackAtVertex;

  class IVertexSeedFinder;
  class IVertexLinearizedTrackFactory;
  class IImpactPoint3dEstimator;
  class IVertexTrackCompatibilityEstimator;
  class IVertexUpdator;
  class IVertexSmoother;
  class IVertexAnnealingMaker;

  /**
   * @class Trk::AdaptiveVertexFitter
   * 
   * @author N. Giacinto Piacquadio (Freiburg ATLAS Group)
   *
   * This class implements a vertexing fitting algorithm which is 
   * adaptive and robust, quite insensitive to the presence of outliers.
   *
   * The vertex algorithm is based on the typical Kalman-Filtering 
   * approach, with the difference that the inverse of the covariance matrices 
   * of the track parameters of the tracks which enter the fit 
   * are reweighted according to the probability of the tracks to be compatible  
   * with the vertex to be fit. In this way outliers/tracks which do not belong to 
   * the vertex are smoothly rejected.
   *
   * Since the estimation of the vertex position during the first fit iterations is 
   * not particularly good and the compatiblity estimation of the tracks not always 
   * reliable, an annealing technique is also implemented, in order to make the rejection 
   * of outliers soft in the beginning and harder towards the end of the fit 
   * (for this the Annealing Maker class is responsible: as a default the Temperature of this 
   * thermodinamical procedure is lowered according to few predefined steps)
   *
   *  
   * For greater modularity the algorithm is divided into 6 modules (AlgTools in Athena):
   * 1. SeedFinder
   * 2. LinearizedTrackFactory
   * 3. TrackCompatibilityEstimator
   * 3b. ImpactPoint3dEstimator
   * 4. VertexUpdator
   * 5. VertexSmoother
   * plus another one, which is responsible for letting the temperature go down 
   * (the so called "annealing technique")
   * 6. AnnealingMaker

   * This kind of algorithm was implemented for the first time by CMS: 
   * for information about the theoretical motivations a good 
   * reference is Wolfgang Waltenberger's PhD Thesis  and many of the 
   * calculations implemented have been extensively shown in 
   * theoretical & experimental papers by Fruehwirth, Waltenberger, Strandlie et al.
   * 
   * -------------------------------------------------------------------------------
   * Changes:
   *
   * David Shope <david.richard.shope@cern.ch> (2016-04-19)
   * EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
   *                         from Trk::RecVertex   to xAOD::Vertex,
   *                         from Trk::Vertex      to Amg::Vector3D
   *
   */
  

  class AdaptiveVertexFitter : public extends<AthAlgTool, IVertexFitter>
  {
  public:
    // The following 'using' can be removed when IVertexFitter::fit has been fully migrated to the one with the EventContext
    using Trk::IVertexFitter::fit;
  
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    //same kind error as FullVertexFitter code to be similar in error response - NOT IMPLEMENTED YET
    //enum FitError {FITOK,MATINV,NEGTRCHI2,MAXCHI2,MAXTRCHI2,NOTRKS,NOFIT};
    
    /**
     *default constructor due to Athena interface
     */
    AdaptiveVertexFitter(const std::string& t, const std::string& n, const IInterface*  p);
    
    /**
     *destructor
     */
    
    virtual ~AdaptiveVertexFitter();
    
    /** 
     *Interface for xAOD::TrackParticle with starting point 
     */
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
			       const std::vector<const xAOD::NeutralParticle*>& vectorNeut,
			       const Amg::Vector3D& startingPoint) const override;
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
			       const Amg::Vector3D& startingPoint)  const override
    {return fit(vectorTrk, std::vector<const xAOD::NeutralParticle*>(), startingPoint);};


    /** 
     * Interface for xAOD::TrackParticle with vertex constraint 
     * the position of the constraint is ALWAYS the starting point 
     */
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
			       const std::vector<const xAOD::NeutralParticle*>& vectorNeut,
			       const xAOD::Vertex& constraint) const override;
    virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
			       const xAOD::Vertex& constraint)  const override
    {return fit(vectorTrk, std::vector<const xAOD::NeutralParticle*>(), constraint);};

    /**
     * Interface for MeasuredPerigee with starting point
     */
    virtual xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const std::vector<const Trk::NeutralParameters*>& neutralPerigeeList,
      const Amg::Vector3D& startingPoint) const override;
    virtual xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const Amg::Vector3D& startingPoint) const override;

    /**
     * Interface for MeasuredPerigee with vertex constraint
     */
    virtual xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const std::vector<const Trk::NeutralParameters*>& neutralPerigeeList,
      const xAOD::Vertex& constraint) const override;
    virtual xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const xAOD::Vertex& constraint) const override;

    /**
     * fit providing vector of tracks, constraint and startingPoint
     */

    xAOD::Vertex* fit(const std::vector<const Trk::Track*>& Vectortrk,
                      const xAOD::Vertex& constraint,
                      const Amg::Vector3D& startingPoint) const;

    /**
     * fit providing vector of ParametersBase, constraint and startingPoint
     */

    xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const std::vector<const Trk::NeutralParameters*>& neutralPerigeeList,
      const xAOD::Vertex& constraint,
      const Amg::Vector3D& startingPoint) const;
    xAOD::Vertex * fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
		       const xAOD::Vertex& constraint,
		       const Amg::Vector3D & startingPoint) const;


    
    /**
     * fit providing vector of TrackParameters and NeutralParameters
     */

    xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParameters*>& perigeeList,
      const std::vector<const Trk::NeutralParameters*>& neutralPerigeeList)
      const override;
    xAOD::Vertex* fit(const std::vector<const Trk::TrackParameters*>&
                        perigeeList) const override;

    /**
     * fit providing vector of TrackParticleBase
     */

    xAOD::Vertex* fit(
      const std::vector<const Trk::TrackParticleBase*>& perigeeList) const;

  private:

    /**
     * Internal method for fitting a list of TrackParameters and NeutralParameters, with or without constraint and
     * starting point
     */
    xAOD::Vertex * _fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
		        const std::vector<const Trk::NeutralParameters*> & neutralPerigeeList=std::vector<const Trk::NeutralParameters*>(),
		       const xAOD::Vertex& constraint=xAOD::Vertex(),
		       const Amg::Vector3D & startingPoint=Amg::Vector3D(),
		       bool IsConstraint=false,
	               bool IsStartingPoint=false) const;
    xAOD::Vertex * _fit(const std::vector<const Trk::TrackParameters*> & perigeeList,
		       const xAOD::Vertex& constraint=xAOD::Vertex(),
		       const Amg::Vector3D & startingPoint=Amg::Vector3D(),
		       bool IsConstraint=false,
		       bool IsStartingPoint=false) const;

    /**
     * Internal method for fitting a list of Tracks, with or without constraint and 
     * starting point
     */
    
    xAOD::Vertex * _fit(const std::vector<const Trk::Track*> & VectorTrk,
                       const xAOD::Vertex& constraint=xAOD::Vertex(),
                       const Amg::Vector3D & startingPoint=Amg::Vector3D(),
                       bool IsConstraint=false,
                       bool IsStartingPoint=false) const;
 
   /**
     * Internal method, called by the two _fit internal functions, in order to perform the fit, 
     * after having initialized the input (constraint + seed). The real fit happens here.
     */

    xAOD::Vertex * dothefit(const xAOD::Vertex & ConstraintVertex,
		            const Amg::Vector3D & SeedVertex,
			    std::vector<VxTrackAtVertex> & myLinTracks) const;

    ToolHandle< Trk::IVertexSeedFinder > m_SeedFinder;
    ToolHandle< Trk::IVertexLinearizedTrackFactory > m_LinearizedTrackFactory;
    ToolHandle< Trk::IVertexTrackCompatibilityEstimator > m_TrackCompatibilityEstimator;
    ToolHandle< Trk::IImpactPoint3dEstimator > m_ImpactPoint3dEstimator;
    ToolHandle< Trk::IVertexUpdator > m_VertexUpdator;
    ToolHandle< Trk::IVertexSmoother > m_VertexSmoother;
    ToolHandle< Trk::IVertexAnnealingMaker > m_AnnealingMaker;

    /**
     * Number of maximum iterations
     */

    long int m_maxIterations;

    /**
     * Maximum distance of linearization point of track to actual fitted vertex 
     * before needing to relinearize (in mm)
     */

    double m_maxDistToLinPoint;

    /**
     * Initial error in form of diagonal elements of the inverse of the covariance matrix
     * (name is misleading: take the error, square it and initialize the variable with its inverse)
     */

    double m_initialError;

    /**
     * Variable is true if seeding has to be performed only on the z direction. In this case, 
     * as a guess of the initial position in the transverse plane, the x and y position of 
     * the beam spot are used.
     */

    bool m_onlyzseed;

    /**
     * True if smoothing after fit iterations has to be performed: otherwise the Smoother AlgoTool 
     * provided to the fitter will be ignored.
     */

    bool m_doSmoothing;
    
  };
}

#endif
