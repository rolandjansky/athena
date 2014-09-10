/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IntersectionSolution.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_INTERSECTIONSOLUTION_H
#define TRKEXUTILS_INTERSECTIONSOLUTION_H

// Trk
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include <vector>

class MsgStream;

namespace Trk {


      /** @class IntersectionSolution
         simple typedef of a vector of pointers to TrackIntersection(s)
        
         @author Andreas.Salzburger@cern.ch  
        */
      typedef std::vector<const Trk::TrackSurfaceIntersection* >::const_iterator IntersectionSolutionIter;

      class IntersectionSolution : public std::vector<const Trk::TrackSurfaceIntersection* >  {
        public:
          /** Default Constructor */
          IntersectionSolution();
          /** Explizit Constructor */
          IntersectionSolution(int dim);
          /** Destructor */          
          virtual ~IntersectionSolution();
        private:
          /** private method used for cleanup */
          void freeMemory();
          
      };
      
/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const IntersectionSolution& sf);
std::ostream& operator << ( std::ostream& sl, const IntersectionSolution& sf); 
                  
} // end of namespace

#endif // TRKEXUTILS_INTERSECTIONSOLUTION_H

