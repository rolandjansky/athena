/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
 *
 * ----------------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-19)
 *
 * EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
 *                         from Trk::RecVertex   to xAOD::Vertex,
 *                         from Trk::Vertex      to Amg::Vector3D
 */
 
namespace Trk
{
 class VxTrackAtVertex;
 class LinearizedTrack;
 
 class KalmanVertexUpdator : public AthAlgTool, virtual public IVertexUpdator
 {
  public:
  
  virtual StatusCode initialize() override;
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
   virtual xAOD::Vertex * add(xAOD::Vertex& vtx, VxTrackAtVertex& trk) const override;

/**
 * Method removing already added track from the vertex estimate
 */
   virtual xAOD::Vertex * remove(xAOD::Vertex& vtx,  VxTrackAtVertex& trk) const override;

/**
 * Position update method. The 'mode' parameter is used to indicate whether we want to add or remove a track. 
 */
  virtual IVertexUpdator::positionUpdateOutcome positionUpdate (const xAOD::Vertex& vtx, const LinearizedTrack * trk, double trackWeight, IVertexUpdator::updateMode mode) const override;

/**
 * Method calculating the interstep Chi2 increment
 */ 
  virtual float trackParametersChi2(const xAOD::Vertex& new_vtx, const LinearizedTrack * trk) const override;
  virtual float trackParametersChi2(const IVertexUpdator::positionUpdateOutcome& new_vtx, const LinearizedTrack * trk) const override;

/** 
 * Method calculating the vertex displacement-related part of the chi2   
 */
  virtual float vertexPositionChi2(const xAOD::Vertex& old_vtx, const xAOD::Vertex& new_vtx) const override;
  virtual float vertexPositionChi2(const xAOD::Vertex& old_vtx, const IVertexUpdator::positionUpdateOutcome& new_vtx) const override;

  private:

  // internal methods performing the actual calculations
  float trackParametersChi2(const Amg::Vector3D & new_position, const LinearizedTrack * trk, const AmgSymMatrix(5) & trkParametersWeight) const;
  float vertexPositionChi2(const xAOD::Vertex& old_vtx, const Amg::Vector3D & new_position) const;

/**
 * Method where the fit is actually done
 */
  xAOD::Vertex* update(xAOD::Vertex& vtx, const VxTrackAtVertex& trk, IVertexUpdator::updateMode mode) const;
   
 };//end of class definition
}//end of namespace definition

#endif
