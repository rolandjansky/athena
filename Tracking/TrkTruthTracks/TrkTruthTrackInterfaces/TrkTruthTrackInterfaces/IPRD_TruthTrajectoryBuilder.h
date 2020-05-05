/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPRD_TruthTrajectoryBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRKTRUTHTRACKS_IPRD_TRUTHTRAJECTORYBUILDER_H
#define TRK_TRKTRUTHTRACKS_IPRD_TRUTHTRAJECTORYBUILDER_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "TrkTruthTrackInterfaces/PRD_TruthTrajectory.h"

#include "AtlasHepMC/GenParticle_fwd.h"

namespace Trk {
 

  static const InterfaceID IID_IPRD_TruthTrajectoryBuilder("IPRD_TruthTrajectoryBuilder", 1, 0);
   
  /**
   @class IPRD_TruthTrajectoryBuilder

   @brief The interface for the truth PRD trajectory finder
       
   @author Andreas.Salzburger -at- cern.ch, Thijs.Cornelissen -at- cern.ch, Roland.Wolfgang.Jansky -at- cern.ch 
   */
     
  class IPRD_TruthTrajectoryBuilder : virtual public IAlgTool {

     public:     
       /** Virtual destructor */
       virtual ~IPRD_TruthTrajectoryBuilder(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IPRD_TruthTrajectoryBuilder; }

       /** return a vector of PrepRawData trajectories - uses internal cache**/
       virtual const std::map< const HepMC::GenParticle*, PRD_TruthTrajectory >& truthTrajectories() const = 0;
       
       /** Event refresh - can't be an IIncident, because it has to run after PRD creation and PRD truth creation */
       virtual StatusCode refreshEvent() = 0;       

  };

} // end of namespace

#endif // TRK_TRKTRUTHTRACKS_IPRD_TRUTHTRAJECTORYBUILDER_H