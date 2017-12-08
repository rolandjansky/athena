/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IChargedSmearer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PARSIMINTERFACES_ICHARGEDSMEARER_H
#define ISF_PARSIMINTERFACES_ICHARGEDSMEARER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Trk
//#include "TrkParameters/TrackParameters.h"  // Don't need it for now

// xAOD Track Particle
#include "xAODTracking/TrackParticle.h"  // Instead of namespace xAOD
#include <utility>


namespace ISF {
  class ISFParticle;
}  

namespace iParSim {
     
  static const InterfaceID IID_IChargedSmearer("IChargedSmearer", 1, 0);
    
  /** 
   @class IChargedSmearer

   Interface definition for a smearer swallowing a ISFParticle and 
   returning an xAOD::TrackParticle object
 
   @author Andreas.Salzburger@cern.ch
   */
      
  class IChargedSmearer : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IChargedSmearer(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IChargedSmearer; }

       /** Return the xAOD::TrackPartilce object - return 0 means efficiency correction killed the particle */
       //virtual const xAOD::TrackParticle* smear(const ISF::ISFParticle& isp) const = 0;

       virtual bool smear(xAOD::TrackParticle* xaodTP) const = 0;

       
       /** Return the abs(pdg) to which they should be applied : 0 means default */
       virtual unsigned int pdg() const = 0;
       
  };

} // end of namespace

#endif 

