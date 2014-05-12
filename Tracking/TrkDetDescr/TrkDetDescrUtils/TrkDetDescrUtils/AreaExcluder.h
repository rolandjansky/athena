/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AreaExcluder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_AREAEXCLUDER_H
#define TRKDETDESCRUTILS_AREAEXCLUDER_H

//Eigen
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {


/** \class AreaExcluder
    Pure abstract base class          

   @author sarka.todorova@cern.ch 
  */
   
   class AreaExcluder {
        
      public:
        /** Default constructor - needed for POOL */
        AreaExcluder(){}
        /** Virtual Destructor */
        virtual ~AreaExcluder(){}
        /** Implizit Constructor */
        virtual AreaExcluder* clone() const = 0;
          
        /** First bin from global position */
        virtual bool inside(const Amg::Vector3D& gp, double tol=0.) const = 0;

   };      

} // end of namespace Trk

#endif // TRKDETDESCRUTILS_AREAEXCLUDER_H

