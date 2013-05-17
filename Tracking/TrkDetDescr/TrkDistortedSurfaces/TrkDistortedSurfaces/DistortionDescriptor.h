/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistortionDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDISTORTEDSURFACES_DISTORTIONDESCRIPTOR_H
#define TRKDISTORTEDSURFACES_DISTORTIONDESCRIPTOR_H

//STD
#include <iostream>
#include <iomanip>

//GaudiKernel
#include "GaudiKernel/MsgStream.h"


namespace Trk {

  class LocalPosition;
          
  /**
   @class DistortionDescriptor

   Absract Base Class for 
   describing distortions from nominal 
   surface descriptions.

   Due to the various different possibilities,
   no concrete interface despite a common base,
   the clone and the dump
   methods can be imposed

   @author Andreas.Salzburger@cern.ch
   */
      
  class DistortionDescriptor {

    public:

      /**Default Constructor*/
      DistortionDescriptor(){}

      /**Destructor*/
      virtual ~DistortionDescriptor(){}

      /** clone() method to make deep copy in Surface copy constructor and for assigment operator
        of the Surface class.*/
      virtual DistortionDescriptor* clone() const = 0;

      /** Output Method for MsgStream, to be overloaded by child classes */
      virtual MsgStream& dump(MsgStream& sl) const = 0;

      /** Output Method for std::ostream, to be overloaded by child classes */
      virtual std::ostream& dump(std::ostream& sl) const = 0;                  
  };
 
/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const DistortionDescriptor& sb);
std::ostream& operator << ( std::ostream& sl, const DistortionDescriptor& sb);      
                              
} // end of namespace

#endif // TRKDISTORTEDSURFACES_DISTORTIONDESCRIPTOR_H
