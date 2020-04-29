/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PRD_TruthTrajectory.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRKTRUTHTRACKS_PRD_TRUTHTRAJECTORY_H
#define TRK_TRKTRUTHTRACKS_PRD_TRUTHTRAJECTORY_H 1


#include "AtlasHepMC/GenParticle_fwd.h"

namespace Trk {
    
  class PrepRawData;
  
  /**
    @class PRD_TruthTrajectory
    
    simple definitiion of a PRD_TruhtTrajectory
    
    @author Andreas.Salzburger -at- cern.ch, Thijs.Cornelissen -at- cern.ch, Roland.Wolfgang.Jansky -at- cern.ch 
    
  */
  struct PRD_TruthTrajectory {
      
     /**  public members */ 
     std::vector<const Trk::PrepRawData* > prds;
     const HepMC::GenParticle*             genParticle;
     size_t                                nDoF;

     /** defualt constructor */
      PRD_TruthTrajectory()  :
       genParticle(0),
       nDoF(0)
     {}
     
     /** fast constructor */
     PRD_TruthTrajectory( const std::vector<const Trk::PrepRawData* >& prdVec,
                          const HepMC::GenParticle* gP    = 0,
                          size_t numberOfDegreesOfFreedom = 0) :
      prds(prdVec),
      genParticle(gP),
      nDoF(numberOfDegreesOfFreedom)
    {}
    
  };

}

#endif
