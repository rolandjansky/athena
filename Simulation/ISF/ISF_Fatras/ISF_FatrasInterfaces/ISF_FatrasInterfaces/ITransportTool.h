/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITransportTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASINTERFACES_ITRANSPORTTOOL_H
#define ISF_FATRASINTERFACES_ITRANSPORTTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Trk{
  class Track;
}
  
namespace ISF {
  class ISFParticle;    
}

namespace iFatras {
  
  static const InterfaceID IID_ITransportTool("ITransportTool", 1, 0);
    
  /** 
   @class ITransportTool

   universal transport tool 

   - return of new ISFParticle on the next detector boundary
   - return 0 indicates that the particle did not reach the associated detector boundary
       
   @author Sarka.Todorova -at- cern.ch
   @author Andreas.Salzburger -at- cern.ch
   
   */
      
  class ITransportTool : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~ITransportTool(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_ITransportTool; }

       /** Creates a new ISFParticle from a given ParticleState, 
          universal transport tool */
       virtual ISF::ISFParticle* process(const ISF::ISFParticle& isp) = 0;
  };

} // end of namespace

#endif 

