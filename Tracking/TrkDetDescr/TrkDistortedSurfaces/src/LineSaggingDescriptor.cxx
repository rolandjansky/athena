/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LineSaggingDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkDistortedSurfaces/LineSaggingDescriptor.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include <iomanip>
#include <ostream>

// static gravity direction
alignas(16)
const Amg::Vector3D Trk::LineSaggingDescriptor::s_gravityDirection = -1 * Amg::Vector3D::UnitY();

// a reference direction
alignas(16)
const Amg::Vector3D Trk::LineSaggingDescriptor::s_referenceDirection(1./sqrt(3.),1./sqrt(3.),1./sqrt(3.));

// and a electro static scale factor
const double Trk::LineSaggingDescriptor::s_elecStatFactor(1.);

Trk::LineSaggingDescriptor::LineSaggingDescriptor(double wireLength,
                                                  double wireTension,
                                                  double linearDensity) :
   Trk::DistortionDescriptor(),
   m_wireLength(wireLength),
   m_wireTension(wireTension),
   m_linearDensity(linearDensity)
{

}


Trk::LineSaggingDescriptor::LineSaggingDescriptor(const LineSaggingDescriptor& lsd) :
   Trk::DistortionDescriptor(),
   m_wireLength(lsd.m_wireLength),
   m_wireTension(lsd.m_wireTension),
   m_linearDensity(lsd.m_linearDensity)
{}


Trk::LineSaggingDescriptor& Trk::LineSaggingDescriptor::operator=(const Trk::LineSaggingDescriptor& lsd)
{
  if (this!=&lsd){
    m_wireLength    = lsd.m_wireLength;
    m_wireTension   = lsd.m_wireTension;
    m_linearDensity = lsd.m_linearDensity;
  }
  return(*this);
}


Amg::Transform3D* Trk::LineSaggingDescriptor::correctedSurfaceTransform(const Amg::Vector2D& locp, 
                                                                        const Amg::Vector3D& lineCenter,
                                                                        const Amg::Vector3D& lineDirection,
                                                                        double cosFi) const
{
  // calculate scale product between gravity and wire
  double cosLineGrav = s_gravityDirection.dot(lineDirection);

  // calculate cos angle between gravity direction and wire sagging direction
  cosFi = sqrt( 1 - cosLineGrav*cosLineGrav );
  
  // create new vector pointing perpendicular to the wire
  // sag should be pointing down, init with global direction of global negative y-axis 
  Amg::Vector3D sagDir = s_gravityDirection;
  
  // if gravity and wire sagging direction are not point in same direction, recalculate sag direction
  if (cosFi > 0.001 ){
      sagDir = (s_gravityDirection - cosLineGrav*lineDirection);
      sagDir = sagDir.unit();
  }
  
  // wlsquare  
  double wireLengthSquare= m_wireLength*m_wireLength;

  // calculate reference sag per meter square
  double refSagPerMeterSq = 100.0*(m_linearDensity*s_elecStatFactor)/(8.*m_wireTension);

  // compute maximum sag in microns use wireln in m,
  double sagMax = refSagPerMeterSq*(wireLengthSquare*1e-9); // (*10e-6*0.001)
  double actualSagMax = sagMax*cosFi;
  double s=locp[Trk::locZ]; 

  // sag in microns where at longitudinal location of the hit
  double sag=(1.0-4.*((s*s)/(wireLengthSquare)))*actualSagMax;

  // new center position
  Amg::Translation3D newCenter(lineCenter+sag*sagDir);

  // new z direction - build d(sag)/d(z)
  //double dsagdz = 8.0*s*sagMax/wireLengthSquare;
  // and the vector
  const Amg::Vector3D& newLineDir(lineDirection);
  // build the other vectors
  Amg::Vector3D newLineY(newLineDir.cross(sagDir)); 
  // build the x vector
  Amg::Vector3D newLineX(newLineY.cross(newLineDir));
  
  Amg::RotationMatrix3D rot;
  rot.col(0) =  newLineX;
  rot.col(1) =  newLineY;
  rot.col(2) =  newLineDir;
  
  // new transform
  Amg::Transform3D* newLineTransform = new Amg::Transform3D;
  (*newLineTransform) = rot;
  (*newLineTransform) *= newCenter;

  return newLineTransform; // ! (newRotation, newCenter);

}


MsgStream& Trk::LineSaggingDescriptor::dump( MsgStream& sl ) const
{
    sl << std::setiosflags(std::ios::fixed);
    sl << std::setprecision(7);
    sl << "Trk::LineSaggingDescriptor";
    sl <<  '\t' << "- wire length    :" << m_wireLength    << std::endl;
    sl <<  '\t' << "- wire tension   :" << m_wireTension   << std::endl;
    sl <<  '\t' << "- linear density :" << m_linearDensity << std::endl;
    sl << std::setprecision(-1);
    return sl;
}

std::ostream& Trk::LineSaggingDescriptor::dump( std::ostream& sl ) const 
{
    sl << std::setiosflags(std::ios::fixed);
    sl << std::setprecision(7);
    sl << "Trk::LineSaggingDescriptor:";
    sl <<  '\t' << "- wire length    :" << m_wireLength    << std::endl;
    sl <<  '\t' << "- wire tension   :" << m_wireTension   << std::endl;
    sl <<  '\t' << "- linear density :" << m_linearDensity << std::endl;
    sl << std::setprecision(-1);
    return sl;
}
