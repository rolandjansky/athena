/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **************************************************************************
                           ITrackParticleAndJetMerger.h  -  Description
                             -------------------
    begin   : 27.01.05
    authors : A. Wildauer
    email   : andreas.wildauer@cern.ch
              
   ***************************************************************************/

#ifndef PARTICLEJETTOOLS_ITRACKPARTICLEANDJETMERGER_H
#define PARTICLEJETTOOLS_ITRACKPARTICLEANDJETMERGER_H

#include "GaudiKernel/IAlgTool.h"

namespace Rec
{
  class TrackParticleContainer;
}

class ParticleJetContainer;

namespace Analysis
{
 
 static const InterfaceID IID_ITrackParticleAndJetMerger("Analysis::ITrackParticleAndJetMerger", 1, 0);
  
  /** \class ITrackParticleAndJetMerger
    Interface class for the TrackParticleAndJetMerger AlgTool, it inherits from IAlgTool
    Detailed information about private members and member functions can be found in the actual implementation class
    TrackParticleAndJetMerger.
       
    @author Andreas.Wildauer@cern.ch
    */
    
  class ITrackParticleAndJetMerger : virtual public IAlgTool
  {
    public:
    
      /** destructor */
      virtual ~ITrackParticleAndJetMerger() {};
      
      /** AlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_ITrackParticleAndJetMerger; };
      
      /** Method to merge tracks to JetParticles */
      virtual void mergeTrackWithJets(
                            ParticleJetContainer * SGParticleJetContainer,
                            const Rec::TrackParticleContainer * particleContainer
                            )=0;
       
  }; // End class
} // End namespace

#endif
