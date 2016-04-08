/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPRD_TruthTrajectorySelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRKTRUTHTRACKS_IPRD_TRUTHTRAJECTORYSELECTOR_H
#define TRK_TRKTRUTHTRACKS_IPRD_TRUTHTRAJECTORYSELECTOR_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "TrkTruthTrackInterfaces/PRD_TruthTrajectory.h"

namespace Trk {
 
  class PrepRawData;

  static const InterfaceID IID_IPRD_TruthTrajectorySelector("IPRD_TruthTrajectorySelector", 1, 0);
   
  /**
   @class IPRD_TruthTrajectorySelector

   @brief The interface for the truth PRD trajectory selector
       
   @author Andreas.Salzburger -at- cern.ch
   */
     
  class IPRD_TruthTrajectorySelector : virtual public IAlgTool {

     public:     
       /** Virtual destructor */
       virtual ~IPRD_TruthTrajectorySelector(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IPRD_TruthTrajectorySelector; }

       /** selector method - boolean indicates if PRD truth trajectory passes or not */
       virtual bool pass( const PRD_TruthTrajectory& ) const = 0;

  };

} // end of namespace

#endif // TRK_TRKTRUTHTRACKS_IPRD_TRUTHTRAJECTORYSELECTOR_H