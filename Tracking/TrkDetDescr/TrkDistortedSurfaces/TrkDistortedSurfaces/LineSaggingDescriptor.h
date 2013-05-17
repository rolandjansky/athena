/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistortionDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDISTORTEDSURFACES_LINESAGGINGDESCRIPTOR_H
#define TRKDISTORTEDSURFACES_LINESAGGINGDESCRIPTOR_H

//STD
#include <iostream>
#include <iomanip>

//GaudiKernel
#include "GaudiKernel/MsgStream.h"
// Trk
#include "TrkDistortedSurfaces/DistortionDescriptor.h"
// Geo & Maths
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
          
  /**
   @class LineSaggingDescriptor

   Fill in documentation

   @author Andreas.Salzburger@cern.ch
   */
      
  class LineSaggingDescriptor : public DistortionDescriptor {

    public:

      /**Default Constructor*/
      LineSaggingDescriptor(){}

      /** Constructor  with arguments */
      LineSaggingDescriptor(double wireLength, 
                            double wireTension,
                            double linearDensity);

      /** Constructor  with arguments */
      LineSaggingDescriptor(const LineSaggingDescriptor& lsd);

      /**Destructor*/
       ~LineSaggingDescriptor(){}

      /** clone() method to make deep copy in Surface copy constructor and for assigment operator
        of the Surface class.*/
      LineSaggingDescriptor* clone() const;

      /** Assignment operator */
      LineSaggingDescriptor& operator=(const LineSaggingDescriptor& lsd);

      /** Get the HepGeom::Transform3D* for the corrected Surface */
      Amg::Transform3D* correctedSurfaceTransform(const Amg::Vector2D& locp, 
                                                  const Amg::Vector3D& lineCenter,
                                                  const Amg::Vector3D& lineDirection,
                                                  double cosFi=0.) const;

      /** Output Method for MsgStream, to be overloaded by child classes */
      MsgStream& dump(MsgStream& sl) const;

      /** Output Method for std::ostream, to be overloaded by child classes */
      std::ostream& dump(std::ostream& sl) const;

    protected:
     //!< the wire end positions 
     double                  m_wireLength;
     double                  m_wireTension;
     double                  m_linearDensity;

     /** direction of gravity */
     static double           s_elecStatFactor;
     static Amg::Vector3D    s_gravityDirection;
     static Amg::Vector3D    s_referenceDirection;


           
  };
    
  // clone method
  inline LineSaggingDescriptor* LineSaggingDescriptor::clone() const
  { return new LineSaggingDescriptor(*this); }
                          
} // end of namespace

#endif
