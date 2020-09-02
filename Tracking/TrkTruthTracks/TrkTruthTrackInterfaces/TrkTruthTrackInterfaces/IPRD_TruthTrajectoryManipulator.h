/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPRD_TruthTrajectoryManipulator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRKTRUTHTRACKS_IPRD_TRUTHTRAJECTORYMANIPULATOR_H
#define TRK_TRKTRUTHTRACKS_IPRD_TRUTHTRAJECTORYMANIPULATOR_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "TrkTruthTrackInterfaces/PRD_TruthTrajectory.h"


#include "AtlasHepMC/GenParticle_fwd.h"

namespace Trk {
 
  class PrepRawData;

  static const InterfaceID IID_IPRD_TruthTrajectoryManipulator("IPRD_TruthTrajectoryManipulator", 1, 0);
   
  /**
   @class IPRD_TruthTrajectoryManipulator

   @brief The interface for the truth PRD trajectory manipulator
       
   @author Andreas.Salzburger -at- cern.ch
   */
     
  class IPRD_TruthTrajectoryManipulator : virtual public IAlgTool {

     public:     
       /** Virtual destructor */
       virtual ~IPRD_TruthTrajectoryManipulator(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IPRD_TruthTrajectoryManipulator; }
       
       /** manipulate method - boolean indicates if manipulation was done or not */
       virtual bool manipulateTruthTrajectory( PRD_TruthTrajectory& ) const = 0;


  };

} // end of namespace

#endif // TRK_TRKTRUTHTRACKS_IPRD_TRUTHTRAJECTORYMANIPULATOR_H
