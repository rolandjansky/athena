/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * ATLAS Reconstruction Software - (C) 2005 - 2007
 *
 * @class Trk::KalmanVertexOnJetAxisUpdator
 *
 * @author Giacinto Piacquadio, (giacinto.piacquadio@physik.uni-freiburg.de)
 * @author Christian Weiser, (christian.weiser@physik.uni-freiburg.de)
 *
 * A concrete implementation of the Vertex Updator 
 * using the Kalman filter algorithm. 
 *
 * This is a special implementation (part of the JetFitter algorithm) 
 * which is able to fit a vertex correcting the jet axis parameters 
 * to find the best overall agreement.
 *
 * Basic approach is based on R. Fruhwirth et al. Comp. Phys. Comm 96(1996) 189 
 *
 * The first version implemented in TrkVertexFitterUtils, for general vertexing 
 * purposes, was created by Kirill Prokofiev and G.P.
 *
 * First version: December 2006
 * Updated version for athena: Februar 2007
 *
 * Alberts-Ludwig-Universita"t Freiburg
 */



#ifndef TrkVertexFitterUtils_KalmanVertexOnJetAxisUpdator_H
#define TrkVertexFitterUtils_KalmanVertexOnJetAxisUpdator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
 
namespace Trk
{
  class VxJetCandidate;
  class VxTrackAtVertex;
  class LinearizedTrack;
  class RecVertexPositions;
  class VxVertexOnJetAxis;

  typedef Eigen::Matrix<double, 3, Eigen::Dynamic> AmgMatrix3X;
 
  static const InterfaceID IID_KalmanVertexOnJetAxisUpdator("Trk::KalmanVertexOnJetAxisUpdator", 1, 0);


 class KalmanVertexOnJetAxisUpdator : public AthAlgTool
 {
  public:
     
   static const InterfaceID& interfaceID()
     {
       return IID_KalmanVertexOnJetAxisUpdator;
     };


  virtual StatusCode initialize() override;

  /**
   * Constructor
   */
  KalmanVertexOnJetAxisUpdator(const std::string& t, const std::string& n, const IInterface*  p);
 
  /**
   * Destructor
   */

  ~KalmanVertexOnJetAxisUpdator();

  /**
   * Method updating the fit along the flight axis (VxJetCandidate) with a track (VxTrackOnJetAxis) which is added 
   * to the vertex along the jet axis provided (VxVertexOnJetAxis).
   *
   * To update the fit with a new vertex (VxVertexOnJetAxis) you have to iteratively add all its tracks to the fit 
   * using this method. In the RecVertexPositions datamember of the VxJetCandidate the result will appear as the 
   * value of the distance of this fitted vertex from the primary vertex. All the other vertices and the jet direction 
   * will of course be also dynamically adjusted while adding tracks to the fit.
   */
  
  void add(VxTrackAtVertex* trackToAdd,const VxVertexOnJetAxis* vertexToUpdate,VxJetCandidate* candidateToUpdate) const;
  void addWithFastUpdate(VxTrackAtVertex* trackToAdd,const VxVertexOnJetAxis* vertexToUpdate,VxJetCandidate* candidateToUpdate) const;
 
  /**
   * Method removing already added track from the fit along the flight axis, where you specify from what vertex along 
   * the jet axis the track should be removed.
   *
   * Usefull for smoothing and for estimating the compatibility of single vertices with the rest of the fit (the smoothed 
   * chi2 of the vertex can be obtained by iteratively removing all its tracks and storing the intermediate result at 
   * every time).
   *
   * Just adding the chi2 of every track involved (as in Fruehwirth's reference) would lead here to misleading results.
   */
 
  void remove(VxTrackAtVertex* trackToRemove,const VxVertexOnJetAxis* vertexToUpdate,VxJetCandidate* candidateToUpdate) const;
  void removeWithFastUpdate (VxTrackAtVertex* trackToRemove,const VxVertexOnJetAxis* vertexToUpdate,VxJetCandidate* candidateToUpdate) const;

  /**
   * Obsolete (will be removed - not in use, however migrated to eigen before finding out -WL)
   * Updates only the chi2 + ndf information, in case the weight matrix is not inverted 
   * at every step, so that there is no info about the fitted position at intermediate 
   * steps of the fit. Dropped because the chi2 obtained is not reliable.
   */
  void updateChi2NdfInfo(VxTrackAtVertex* trackToUpdate,
			 const VxVertexOnJetAxis* vertexToUpdate,
			 VxJetCandidate* vertexCandidate) const;

  
  /**
   * Obsolete (will be removed - not in use, however moved to eigen before finding out -WL).
   * Updates the chi2/ndf of the entire fit with the final information about the position.
   */
  void updateVertexChi2(VxJetCandidate* vertexCandidate) const;


  /**
   * Method to do the Kalman Update of the positions of the fit along the flight axis
   * with a single new track (which belongs to the VxVertexOnJetAxis number numVertex).
   */

  Trk::RecVertexPositions positionUpdate (VxJetCandidate& vertexCandidate,
					  const LinearizedTrack * trk, 
					  double trackWeight, int sign, 
					  int numVertex,bool isPrimary,
                                          bool doFastUpdate) const;


  /**
   * Method to create the Jacobian needed to go to the space of single vertices 
   * to the space of the variables representing the full fit along the flight axis 
   * (primary vertex, phi and theta of jet direction, distance to primary vertex 
   * of the remaining vertices along the jetaxis).
   *
   * As a result the vector of position where the linearization was done and the jacobian 
   * are provided.
   *
   * If the parameter truncate is true, the method also cuts the size
   * of the matrix down to the only needed elements 
   * (if you work on vertex number 5, all the ones which come after are cut out from 
   * the jacobian to make the fit faster).
   */

  std::pair<Amg::Vector3D,Eigen::Matrix3Xd> createTransformJacobian(const Amg::VectorX & initialjetdir,
                                                                    int numVertex, bool isPrimary,
                                                                    bool truncate) const;

  void smartInvert(Amg::MatrixX & new_vrt_cov) const;
   
 
 private:

  /** 
   * Internal method to do the Kalman Update.
   * This is where the real fit iteration is done.
   */  
  
  void update(VxTrackAtVertex* trackToUpdate,const VxVertexOnJetAxis* vertexToUpdate,VxJetCandidate* candidateToUpdate,int sign, bool doFastUpdate) const ;
  
  /**
   * Internal method where the chi2/ndf update is actually done
   *
   */

  double calculateTrackChi2 (const VxJetCandidate& vertexCandidate,
			    const LinearizedTrack * trk, 
			    double trackWeight, 
			    int numVertex,bool isPrimary=false) const;



  
 private:

  double m_initialMomentumError;

 };//end of class definition
}//end of namespace definition

#endif
