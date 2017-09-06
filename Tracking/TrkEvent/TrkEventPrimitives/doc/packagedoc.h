/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkEventPrimitives_page The TrkEventPrimitives package


@section TrkEventPrimitives_TrkEventPrimIntro Introduction
The TrkEventPrimitives defines event objects used for tracking that are not defined in the Amg packages GeoPrimitivies or EventPrimitives.
All matrix and vector classes are based on the Amg/Eigen.

@section TrkEventPrimitives_TrkEventPrimEdmDescr List of Classes - EDM Building Blocks

Measurement defintiation
- Trk::LocalParameters is a dynamically long vector with identification schema

Classes for track parameter handling
- Trk::ParamDefs definitions for parameters to access entries in LocalPosition, Parameters, Weight- & CovarianceMatrix, extended parameters/
- Trk::DefinedParameter is a std::pair of a double + a Trk::ParamDefs enum to identify the parameter chosen 
- Trk::ChargeDefinition base class for the two above.
- Trk::ProjectionMatricesSet is the set of 32 x 2 projection and reduction matrices to be used together with the Trk::LocalParameters

@section TrkEventPrimitives_TrkEventPrimOtherDescr List of Classes - Other Purposes
- Trk::DriftCircleSide is an enum which defines on which side of a wire the track passed (e.g. for the TRT or MDT detectors)
- Trk::DriftCircleStatus is an enum which describes how the signed radius is
   to be interpreted.
- Trk::FitQualityOnSurface obsolete class, use Trk::FitQuality instead.
- Trk::ParticleHypothesis charged particle type (i.e. mass) hypothesis for
   use in material interaction calculations.
- Trk::PropDirection enum describing the propagation direction wrt to the
  momentum direction.
- Trk::ResidualPull class for hit-residuals and pulls.
- Trk::SearchDirection enum describing the search direction in the detector
  for navigation and pattern recognition.
- Trk::TrackScore typedef for the result from the scoring functions
- Trk::VertexType enum distinguishing different type of vertices which
   a particle can be associated with.
- Trk::CurvilinearUVT is a helper utility for spanning a curvilinear 3D frame

@section TrkEventPrimitives_TrkEventPrimJacoDescr List of Classes - Jacobians
- Trk::JacobianCotThetaPtToThetaP his is the 5x5 jacobian for the transformation of track parameters and errors having a momentum representation of @f$ (\phi, cot\theta, q/p_{T}) @f$ to the new standard representation of @f$ (\phi, \theta, q/p) \f$, while the first two variables that express the local position representation keep unchanged.
- Trk::JacobianThetaPToCotThetaPt This is the 5x5 jacobian for the transformation of track parameters and errors having the new standard momentum representation of @f$ (\phi, \theta, q/p) @f$ to the representation of @f$( \phi, cot\theta, q/p_{T}) \f$, while the first two variables that express the local position representation keep unchanged.
- Trk::JacobianPolarToCartesian is the 5x5 jacobian to guarantee transformation between polar and cartesian coordinates and errors.
- Trk::JacobianCartesianToPolar is the 5x5 jacobian to guarantee transformation between cartesian and polar coordinates and errors.
- Trk::JacobianPerigeeToCartesian is the 7x5 jacobian to transform from perigee
  parameters to cartesian fourmomentum+coordinate vectors and errors.
- Trk::JacobianCartesianToPerigee is the 5x7 jacobian to transform from
  cartesian back to 5-dimensional perigee parameters or errors.
- Trk::JacobianLocalAnglesPhiTheta dummy, needs to be written !
- Trk::JacobianPhiThetaLocalAngles dummy, needs to be written !
- Trk::JacobianPxyzToPhiThetaQoverPcartesian is the 3x3 jacobian expressed in
  cartesian coordinates to transform from cartesian to polar momentum
  coordinates.
- Trk::JacobianPxyzToPhiThetaQoverPspherical is the 3x3 jacobian expressed in
  polar coordinates to transform from cartesian to polar momentum
  coordinates.
- Trk::JacobianPhiThetaQoverPToPxyz is the 3x3 jacobian to transform from 
  polar to cartesian momentum coordinates.

@section TrkEventPrimitives_TrkEventPrimContacts Authors
Please send any comments to 
@author Andreas.Salzburger@cern.ch
@author Edward.Moyse@cern.ch

@section TrkEventPrimitives_TrkEventPrimPages Extra Pages

*/
