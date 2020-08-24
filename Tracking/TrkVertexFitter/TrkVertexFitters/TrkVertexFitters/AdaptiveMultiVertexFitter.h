/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRKVERTEXFITTERS_ADAPTIVEMULTIVERTEXFITTER_H
#define TRKVERTEXFITTERS_ADAPTIVEMULTIVERTEXFITTER_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//xAOD includes
#include "xAODTracking/Vertex.h"
#include <vector>

/**
 * @class Trk::AdaptiveMultiVertexFitter
 *
 * 
 * @author N. Giacinto Piacquadio (Freiburg ATLAS Group)
 *
 *
 * This class implements a multi vertex fitting algorithm 
 * which is adaptive and robust, quite insensitive to the presence 
 * of outliers.
 *
 * The multi-vertex version of the fit is more complicate than the 
 * single-vertex version and needs the input to be provided in the form 
 * of a vector of already initialized MVFVxCandidate objects. Details of the 
 * the objects to provide in the fit() and addVtxToFit() functions.
 *
 * During the multi-vertex fit all the vertices are fit, using the tracks 
 * which are contained in their respective vertex candidates. Tracks which are 
 * shared among more than one vertex get their weights adjusted dynamically 
 * iteration after iteration thanks to a Deterministic Annealing 
 * procedure, so that in the end they are dinamically assigned to the 
 * vertex they most probably belong to.
 *
 * As for the single-vertex fitter, for 
 * greater modularity the algorithm is divided into 6 
 * modules (AlgTools in Athena):
 *
 * 1. LinearizedTrackFactory
 * 2. TrackCompatibilityEstimator
 * 2b. ImpactPoint3dEstimator
 * 3. VertexUpdator
 * 4. VertexSmoother
 * plus another one, which is responsible for letting the temperature go down 
 * (which implements the so called "annealing technique")
 * 5. AnnealingMaker
 *
 * --------------------------------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-19)
 *
 * EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
 *                         from Trk::RecVertex   to xAOD::Vertex,
 *                         from Trk::Vertex      to Amg::Vector3D
 *
 * Also, VxMultiVertex EDM has been migrated to the following:
 *   
 *   Trk::MvfFitInfo  
 *     constraintVertex     now uses xAOD::Vertex
 *     seedVertex           now uses Amg::Vector3D
 *     linearizationVertex  now uses Amg::Vector3D
 *
 *     Vertex objects stored using this class are now xAOD::Vertex
 *
 * Instead of using the MVFVxCandidate class, xAOD::Vertex is employed by decorating it
 * with the multi-vertex information:
 *
 *   bool                              isInitialized
 *   MvfFitInfo*                       MvfFitInfo
 *   std::Vector<VxTrackAtVertex*>     VTAV
 *
 *   This last decoration is needed in order to be able to use MVFVxTrackAtVertex objects
 *   which have the additional information of xAOD::Vertices associated to the track
 *   and (through polymorphism) to still be able to pass them to the KalmanVertexUpdator as
 *   VxTracksAtVertex objects.
 *
 *   This is obviously not an ideal implementation and could be avoided if xAOD::Vertex
 *   stored a std::vector<VxTrackAtVertex*> instead of a std::vector<VxTrackAtVertex>,
 *   but I think it must remain this way until such a time as the xAOD::Vertex EDM
 *   is changed.
 *
 */


namespace Trk
{

  static const InterfaceID IID_AdaptiveMultiVertexFitter("AdaptiveMultiVertexFitter", 1, 0);


  //  class MeasuredPerigee;
  class Track;
  class VxTrackAtVertex;
  class IVertexLinearizedTrackFactory;
  class IImpactPoint3dEstimator;
  class IVertexTrackCompatibilityEstimator;
  class IVertexUpdator;
  class IVertexSmoother;
  class IVertexAnnealingMaker;

  class TrackToVtxLink;

  class AdaptiveMultiVertexFitter : public AthAlgTool
  {
  public:
    StatusCode initialize();
    StatusCode finalize();
  
    
    /**
     * default constructor due to Athena interface
     */

    AdaptiveMultiVertexFitter(const std::string& t, const std::string& n, const IInterface*  p);

    /**
     * destructor
     */

    virtual ~AdaptiveMultiVertexFitter();

    /**
     * fit all the provided MVFVxCandidate, which have to be already properly initialized. 
     * 
     * The information about the Vertices to Tracks association is provided in the following way:
     * the MVFVxCandidate contains a list of MVFVxTrackAtVertex, which contain each a TrackToVtxLink class: 
     * this class contains the information about all the vertices (MVFVxCandidate objects) the tracks belongs to and 
     * is shared among all MVFVxTrackAtVertex which represents the same track at the different vertices.
     * 
     * This has the clear advantage that the equivalent matrix of the assignments between vertices and tracks can be
     * made block diagonal, so that no time is wasted in case of tracks which are clearly incompatible with some of 
     * the other vertices.
     *
     */

    void 
    fit(std::vector<xAOD::Vertex*> & allVertices) const;

    /**
     * Adds a new MVFVxCandidate to a previous multi-vertex fit and fits everything together.
     *
     *  
     * Please note that the new MVFVxCandidate provided has to be properly initialized, which 
     * means it has to have already a proper seed vertex, a constraint rec vertex, a list of initialized 
     * MVFVxTrackAtVertex, each with a link to the TrackToVtxLink object, which has to be already be updated to 
     * contain also a pointer to the MVFVxCandidate among the vertex candidates the track belongs to.
     * 
     * There are three steps:
     *
     * 1) the new vertex is added to the fit (all the tracks get initialized, so that the Plane through their 
     *    IP point and the seed vertex (IP3dAtAPlane) is created, to be later able to estimate in a fast way 
     *    the compatibility of the tracks to their respective vertices.
     *
     * 2) all tracks belonging to the new vertex are scanned and all the vertices which shares tracks with the new vertex 
     *    to be fit are also added to the fit.
     *
     * 3) the multivertex fit is performed with all the involved vertices.
     *
     * This has the clear advantage that only vertices which are really affected by adding the new vertex are being 
     * refitted, improving the overall finding performance.
     *
     */

    void 
    addVtxToFit(xAOD::Vertex* pVtx) const; 
    //
    void 
    addVtxTofit(xAOD::Vertex* pVtx)  const { return addVtxToFit(pVtx);}
    
    static const InterfaceID& 
    interfaceID() {
      return IID_AdaptiveMultiVertexFitter;
    }
   
  private:

    /**
     * Internal method to find a VxCandidate among a vector of VxCandidates. Returns true if the vertex 
     * is found.
     */

    bool 
    findAmongVertices(const xAOD::Vertex* vertex,const std::vector<xAOD::Vertex*> & previousVertices) const;

    /**
     * Internal function to collect the weights of the tracks partecipating to all the possible vertices (needed 
     * to renormalize the probability of the track to belong to the vertex under consideration, to be provided 
     * to the AnnealingMaker class). The input is the TrackToVtxLink, where the weights are stored.
     *
     */

    std::vector<double> 
    collectWeights(const TrackToVtxLink & tracklink) const;

    /**
     * Internal function to prepare the compatibility information of all the tracks of the 
     * new vertex (an IP3dAtaPlane is added, which makes later the estimation of the compatibilities 
     * of the tracks to the vertex for the downweighting faster). Further information in the 
     * TrkVertexFitterUtils package (IP3dAtAPlaneFactory class).
     *
     */

    void 
    prepareCompatibility(xAOD::Vertex* newvertex) const;

    /**
     * Max number of iterations.
     */

    long int 
    m_maxIterations;

    
    /**
     * Maximum distance of linearization point of track to actual fitted vertex 
     * before needing to relinearize (in mm)
     */
    
    double 
    m_maxDistToLinPoint;
    
    /**
     * Initial error in form of diagonal elements of the inverse of the covariance matrix
     * (name is misleading: take the error, square it and initialize the variable with its inverse)
     */

    double 
    m_initialError;

    /**
     * True if smoothing after fit iterations has to be performed: otherwise the Smoother AlgoTool 
     * provided to the fitter will be ignored.
     */

    bool 
    m_doSmoothing;

    /**
     * Minimum weight a track as to have in order to be considered in the fit of one of the vertices.
     * This should make the fit slightly faster.
     */

    double 
    m_minweight;

    /**
     * Maximum shift allowed for last iteration... (in terms of Delta|VecR|/sigma|VecR|)
     *
     */

    double 
    m_maxRelativeShift;
    
    ToolHandle< Trk::IVertexLinearizedTrackFactory      > m_LinearizedTrackFactory;
    ToolHandle< Trk::IVertexTrackCompatibilityEstimator > m_TrackCompatibilityEstimator;
    ToolHandle< Trk::IImpactPoint3dEstimator            > m_ImpactPoint3dEstimator;
    ToolHandle< Trk::IVertexUpdator                     > m_VertexUpdator;
    ToolHandle< Trk::IVertexSmoother                    > m_VertexSmoother;
    ToolHandle< Trk::IVertexAnnealingMaker              > m_AnnealingMaker;
    
  };
}

#endif
