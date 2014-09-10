/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SolutionSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_SOLUTIONSELECTOR_H
#define TRKEXUTILS_SOLUTIONSELECTOR_H

#include "TrkEventPrimitives/PropDirection.h"

namespace Trk {

      enum SelectedSolution { first=0, second=1, none=2 };

      /** @class SolutionSelector
        
         Given a PropDirection and two pathlengths it will chose the solution
        
         @author Andreas.Salzburger@cern.ch
        */
     class SolutionSelector {
        public:
         /**Constructor */
         SolutionSelector();
         /** Destructor */
         virtual ~SolutionSelector();

         /** Seclect method */
         SelectedSolution select(Trk::PropDirection dir, double pl1, double pl2);
               
     };
        
} // end of namespace

#endif // TRKEXUTILS_SOLUTIONSELECTOR_H

