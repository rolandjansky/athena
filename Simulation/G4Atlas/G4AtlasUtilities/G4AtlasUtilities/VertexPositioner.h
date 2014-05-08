/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-C++-*-
#ifndef VertexPositioner_H
#define VertexPositioner_H

//! This class randomly offsets events so that the primary vertex gets
//! distributed according to a beam luminous region.  It should be used
//! for those generators which set the vertex position at (0,0,0) by
//! default (e.g. Pythia) but NOT for e.g. SingleParticleGenerator.
//!
//! The luminous region is defined by the following steps.
//!
//! 1) A 3-dimensional Gaussian is generated at (0,0,0) with sigmaX,
//!    sigmaY, sigmaZ being given by vertexSpread.
//!
//! 2) The Gaussian is rotated.  The rotation transforms the (X,Y,Z)
//!    axes of the coordinate system into (X',Y',Z').   The projection
//!    of the Z' axis onto the (ZX) plane makes angle alphaY with the Z
//!    axis, and its projection onto the (ZY) plane makes angle alphaX
//!    with the Z axis.  (In other words IBeamCondSvc.beamTilt(0)
//!    corresponds to rotationAngles.y() and IBeamCondSvc.beamTilt(1)
//!    to rotationAngles.x().)  The signs of the angles are defined so that
//!    positive slopes in the projections dx/dz>0, dy/dz>0, correspond
//!    to positive angles.
//!
//!    The remaining rotation parameter is the alphaZ angle between
//!    the projection of X' and X in the (XY) plane.
//!    (dy/dx>0 for positive angle.)
//!
//!    The angles (alphaX,alphaY,alphaZ) are input in radians through
//!    the SetRotationAngles() method.
//!
//! 3) Finally the luminous region is translated to have its center at
//!    referenceVertexPosition.
//!
//! @author Andrei Gaponenko and Juerg Beringer, added luminous region rotations.
//! @author Andy Buckley, cleanup and conversion to a sim filter.

#include <string>
#include <vector>
#include "FadsKinematics/VertexManipulator.h"
#include "G4ThreeVector.hh"
#include "AthenaKernel/MsgStreamMember.h"

class StoreGateSvc;
class IBeamCondSvc;
namespace CLHEP { 
  class HepRandomEngine; 
  class RandGaussZiggurat;
}


class VertexPositioner : public FADS::VertexManipulator {
public:

  VertexPositioner(const std::string& s);

  ~VertexPositioner();

  void DoTimeOffset(bool a) { m_doTimeOffset = a; }
  bool DoTimeOffset() const { return m_doTimeOffset; }
  void DoPositioning(bool a) { m_doPositioning = a; }
  bool DoPositioning() const { return m_doPositioning; }

  bool EditVertex(G4PrimaryVertex *);
  void EventInitialization();

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) { return m_msg.get().level() <= lvl; }

private:
  bool readInVertexOverridePositions();
  bool overrideVertexPosition();
  bool smearVertexPosition();

  G4bool m_doTimeOffset;
  G4bool m_doPositioning;

  StoreGateSvc *p_sgsvc;
  IBeamCondSvc *p_BeamCondSvc;
  CLHEP::HepRandomEngine *p_randomengine;
  CLHEP::RandGaussZiggurat *p_gauss;

  G4ThreeVector m_vertexPositionOffset; //!< The new position generated

  bool m_CheckForVertexPositionsInFile;
  std::string m_VertexOverrideFile; //!< 
  std::string m_VertexOverrideEventFile; //!< 
  std::map< int, std::map< int, std::vector<double> > > m_VertexOverrideMap; //!< 
  std::vector<int> m_RunVectorVertexOverrideEventFile; //!< 
  std::vector<int> m_EventVectorVertexOverrideEventFile; //!< 
  unsigned int m_IndexVertexOverrideEventFile; //!< 
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
};

#endif
