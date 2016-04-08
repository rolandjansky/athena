/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexFitterUtils_KalmanVertexUpdator_H
#define TrkVertexFitterUtils_KalmanVertexUpdator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexUpdator.h"

/**
 * @class Trk::KalmanVertexUpdator 
 * A concrete implementation of the Vertex Updator 
 * using the Kalman filter algorithm. The algorithm
 * adds or removes a single track from the vertex estimate.
 *
 * Based on R. Fruhwirth et al. Comp. Phys. Comm 96(1996) 189 
 *
 * @author Kirill Prokofiev, November 2005
 */
 
namespace Trk
{
 class RecVertex;
 class VxCandidate;
 class VxTrackAtVertex;
 class LinearizedTrack;
 
 class KalmanVertexUpdator : public AthAlgTool, virtual public IVertexUpdator
 {
  public:
  
  StatusCode initialize();
  StatusCode finalize();
/**
 * Constructor 
 */
  KalmanVertexUpdator(const std::string& t, const std::string& n, const IInterface*  p);

/**
 * Destructor
 */ 
  ~KalmanVertexUpdator();

/**
 * Method adding a single track to the vertex estimate
 */
   VxCandidate * add(VxCandidate& vtx,  VxTrackAtVertex& trk) const;
 
/**
 * Method removing already added track from the vertex estimate
 */
   VxCandidate * remove(VxCandidate& vtx,  VxTrackAtVertex& trk) const;

/**
 * Position update method  
 */
  Trk::RecVertex positionUpdate (const RecVertex& vtx, const LinearizedTrack * trk, double trackWeight, int sign) const;

  private:
  
/**
 * Method where the fit is actually done
 */
  VxCandidate* update(VxCandidate& vtx,  VxTrackAtVertex& trk,int sign) const;
  
/**
 * Helper method calculating the interstep Chi2 increment
 */ 
  float trackParametersChi2(const RecVertex& new_vtx, const LinearizedTrack * trk)const;
  
/** 
 * Helper method calculating the vertex displacement-related part of the chi2   
 */
  float vertexPositionChi2(const RecVertex& old_vtx, const RecVertex& new_vtx)const;  
    
 };//end of class definition
}//end of namespace definition

#endif
