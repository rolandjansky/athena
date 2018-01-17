/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MagneticFieldProperties.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_MAGNETICFIELDPROPERTIES_H
#define TRKGEOMETRY_MAGNETICFIELDPROPERTIES_H

//Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
//Trk
#include "TrkGeometry/MagneticFieldMode.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
//STD
#include <iostream>

namespace Trk {

  /** 
   @class MagneticFieldProperties

     new magnetic field properties to steer the behavior of the extrapolation
     
   @author Andreas.Salzburger@cern.ch 
   */
  class MagneticFieldProperties {
      
    public:
      /**Constructor for magnetic field mode - full field is default */
      MagneticFieldProperties(MagneticFieldMode mode=Trk::FullField);

      /**Constructor for magnetic field mode */
      MagneticFieldProperties(const Amg::Vector3D& field);
      
      /**Copy Constructor */  
      MagneticFieldProperties(const MagneticFieldProperties& matprop);
    
      /**Destructor*/
      virtual ~MagneticFieldProperties(){}
      
      /**Assignment operator */
      MagneticFieldProperties& operator=(const MagneticFieldProperties& matprop);
      
      /**Cast operator*/
      operator MagneticFieldMode () const;
            
      /**Returns the MagneticFieldMode as specified */
      MagneticFieldMode magneticFieldMode() const;
            
      /** Get the magnetic field - in case of constant field only - throws exception if mode is not constant */
      const Amg::Vector3D& magneticField() const;

    protected:
      mutable MagneticFieldMode              m_magneticFieldMode;
      Amg::Vector3D                          m_magneticField;
  };


  inline MagneticFieldProperties::operator MagneticFieldMode () const { return m_magneticFieldMode; }  

  inline MagneticFieldMode MagneticFieldProperties::magneticFieldMode() const { return m_magneticFieldMode; }  
  
  inline const Amg::Vector3D& MagneticFieldProperties::magneticField() const { 
      if ( m_magneticFieldMode != Trk::ConstantField ) 
          throw GaudiException("Trk::MagneticFieldProperties", "You can only ask for a field value if you have a constant field!", StatusCode::FAILURE);
      return m_magneticField;
  }


/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const MagneticFieldProperties& mprop);

std::ostream& operator << ( std::ostream& sl, const MagneticFieldProperties& mprop);
    
} // end of namespace

#endif // TRKGEOMETRY_MAGNETICFIELDPROPERTIES_H


