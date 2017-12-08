/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITransportTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4INTERFACES_ITRANSPORTTOOL_H
#define ISF_GEANT4INTERFACES_ITRANSPORTTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Trk{
  class Track;
}
  
namespace ISF {
  class ISFParticle;    
}

namespace iGeant4 {
  
  /** 
   @class ITransportTool

   universal transport tool 

   - return of new ISFParticle on the next detector boundary
   - return 0 indicates that the particle did not reach the associated detector boundary
       
   @author Robert Harrington
   @author Andreas Schaelicke
   
   */
      
  class ITransportTool : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~ITransportTool(){}

       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(ITransportTool, 1, 0);

       /** Processes ISF particle */
       virtual StatusCode process(const ISF::ISFParticle& isp) = 0;

       /** Processes vector of ISF particles */
       virtual StatusCode processVector(const std::vector<const ISF::ISFParticle*>& ispVector) = 0;
  };

} // end of namespace

#endif 

