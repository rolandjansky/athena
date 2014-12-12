/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Intersection.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef TRKDETDESCRUTILS_INTERSECTION_H
#define TRKDETDESCRUTILS_INTERSECTION_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/PropDirection.h"

namespace Trk {
       
   /**
     @struct Intersection 
     
     @author Andreas.Salzburger -at- cern.ch 
     */
      
    struct Intersection {
        Amg::Vector3D position;
        double        pathLength;
        double        distance;
        bool          valid;
        
        Intersection(const Amg::Vector3D& sinter,
                     double slenght,
                     bool svalid,
                     double dist = 0.) :
          position(sinter),
          pathLength(slenght),
          distance(dist),
          valid(svalid)
       {}
          
       // smaller operator for sorting
       bool operator< (const Intersection& si ) const 
           { return (valid && pathLength < si.pathLength); }
    };


   /** class extensions to return also the object */
   template <class T> class ObjectIntersection {
     public:  
       Intersection  intersection;
       mutable const T*    object;
       PropDirection       pDirection;
   
       /** Object intersection */
       ObjectIntersection(const Intersection& sInter,
                          const T* sObject,
                          PropDirection dir=alongMomentum):
         intersection(sInter),
         object(sObject),
         pDirection(dir)
       {}
         
        /** smaller operator for ordering & sorting */
        bool operator< ( const ObjectIntersection<T>& oi ) const {
             return ( intersection <  oi.intersection );
        }
   };

    /** Class extension to return the object, a represenation & the result */
    template <class T, class R, class S> class FullIntersection {
      public:  
        Intersection  intersection;
        mutable const T*    object;
        mutable const R*    representation;
        mutable const S*    result;
        PropDirection       pDirection;
    
        /** Full intersection */
        FullIntersection(const Intersection& sInter,
                         const T* sObject,
                         const R* sRepresentation,
                         const S* sResult,
                         PropDirection dir=alongMomentum):
          intersection(sInter),
          object(sObject),
          representation(sRepresentation),
          result(sResult),
          pDirection(dir)
        {}
          
         /** smaller operator for ordering & sorting */
         bool operator< ( const FullIntersection<T,R, S>& oi ) const {
              return ( intersection <  oi.intersection );
         }
    };


}

#endif
