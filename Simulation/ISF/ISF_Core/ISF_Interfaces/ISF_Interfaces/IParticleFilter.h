/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INTERFACES_IPARTICLEFILTER_H
#define ISF_INTERFACES_IPARTICLEFILTER_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
 
// Simulation includes
#include "ISF_Event/SimSvcID.h" 
 
namespace ISF {

  class ISFParticle;
 
  /**
   @class IParticleFilter
       
   @author Andreas.Salzburger -at- cern.ch
   */
     
  class IParticleFilter : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IParticleFilter(){}

       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(IParticleFilter, 1, 0);
       
       /** Returns a pass boolean on the particle  */
       virtual bool passFilter(const ISFParticle& isp) const = 0;
       
  };

} // end of namespace

#endif // ISF_INTERFACES_IPARTICLEFILTER_H
