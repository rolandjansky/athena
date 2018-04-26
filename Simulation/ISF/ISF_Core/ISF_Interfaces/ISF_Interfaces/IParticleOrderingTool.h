/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleOrderingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef ISF_INTERFACES_IORDERINGTOOL_H
#define ISF_INTERFACES_IORDERINGTOOL_H 1


#include "GaudiKernel/IAlgTool.h"
 
// Simulation includes
#include "ISF_Event/ISFParticle.h" 
#include "ISF_Event/ParticleOrder.h" 
 
namespace ISF {

  class ISFParticle;
 
  /**
   @class IParticleOrderingTool
       
   The implementation of this AtheanAlgTool interface has to compute an order for
   each given particle. It is required that particles assigned to the same SimSvc
   have to have the same order. It is not allowed to have particles with different
   assigned SimSvc have the same order.

   @author Elmar.Ritsch -at- cern.ch
   */
     
  class IParticleOrderingTool : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IParticleOrderingTool(){}

       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(IParticleOrderingTool, 1, 0);
       
       /** set the order for the given particle */
       inline void setOrder( ISFParticle& p );

       /** compute the order for the given ISF particle */
       virtual ISF::ParticleOrder computeOrder( const ISFParticle &p ) const = 0;
  };


  //
  // inline methods
  //

  /** set the order for the given particle */
  inline void ISF::IParticleOrderingTool::setOrder( ISFParticle& p ) {
    ISF::ParticleOrder order = computeOrder(p);
    p.setOrder( order );
  }

} // end of namespace

#endif // ISF_INTERFACES_IORDERINGTOOL_H
