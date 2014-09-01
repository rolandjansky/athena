/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IStackFiller.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INTERFACES_ISTACKFILLER_H
#define ISF_INTERFACES_ISTACKFILLER_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
 
// Simulation includes
#include "ISF_Event/SimSvcID.h" 
#include "ISF_Event/ISFParticleContainer.h"
 
namespace ISF {

  class ISFParticle;
 
  static const InterfaceID IID_IStackFiller("IStackFiller", 1, 0);
   
  /**
   @class IStackFiller
       
   Interface for the stack filling from generator/user defined input.
   This should also be able to be implemented in python for simple
   simulation feeding.       
       
   @author Andreas.Salzburger -at- cern.ch
   */
     
  class IStackFiller : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IStackFiller(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IStackFiller; }
       
       /** Returns the Particle Stack, should register truth */
       virtual StatusCode fillStack(ISFParticleContainer& stack) const = 0;
       
  };

} // end of namespace

#endif // ISF_INTERFACES_ISTACKFILLER_H
