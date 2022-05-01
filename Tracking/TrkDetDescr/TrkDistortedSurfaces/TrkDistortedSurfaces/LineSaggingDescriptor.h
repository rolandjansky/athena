/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistortionDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDISTORTEDSURFACES_LINESAGGINGDESCRIPTOR_H
#define TRKDISTORTEDSURFACES_LINESAGGINGDESCRIPTOR_H

//STL
#include <iosfwd>
//#include <iomanip>

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
      
  class LineSaggingDescriptor final : public DistortionDescriptor {

    public:

      /**Default Constructor*/
      LineSaggingDescriptor():m_wireLength{},m_wireTension{},m_linearDensity{}{
        //nop
      }

      /** Constructor  with arguments */
      LineSaggingDescriptor(double wireLength, 
                            double wireTension,
                            double linearDensity);

      /** Constructor  with arguments */
      LineSaggingDescriptor(const LineSaggingDescriptor& lsd);

      /**Destructor*/
       ~LineSaggingDescriptor() = default;

      /** clone() method to make deep copy in Surface copy constructor and for assigment operator
        of the Surface class.*/
      virtual LineSaggingDescriptor* clone() const override final;

      /** Assignment operator */
      LineSaggingDescriptor& operator=(const LineSaggingDescriptor& lsd);

      /** Get the HepGeom::Transform3D* for the corrected Surface */
      Amg::Transform3D* correctedSurfaceTransform(const Amg::Vector2D& locp, 
                                                  const Amg::Vector3D& lineCenter,
                                                  const Amg::Vector3D& lineDirection,
                                                  double cosFi=0.) const;

      /** Output Method for MsgStream, to be overloaded by child classes */
      MsgStream& dump(MsgStream& sl) const override final;

      /** Output Method for std::ostream, to be overloaded by child classes */
      std::ostream& dump(std::ostream& sl) const override final;

    protected:
     //!< the wire end positions 
     double                  m_wireLength;
     double                  m_wireTension;
     double                  m_linearDensity;

     /** direction of gravity */
     static const double           s_elecStatFactor;
     alignas(16)  static const Amg::Vector3D    s_gravityDirection;
     alignas(16) static const Amg::Vector3D    s_referenceDirection;


           
  };
    
  // clone method
  inline LineSaggingDescriptor* LineSaggingDescriptor::clone() const
  { return new LineSaggingDescriptor(*this); }
                          
} // end of namespace

#endif
