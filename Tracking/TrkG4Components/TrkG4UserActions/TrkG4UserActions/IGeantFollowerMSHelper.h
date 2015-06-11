/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IGeantFollowerMSHelperFollowerHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef IGeantFollowerMSHelper_H
#define IGeantFollowerMSHelper_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "G4ThreeVector.hh"

namespace Trk {

  /** Interface ID for IGeantFollowerMSHelperFollowerHelper */  
  static const InterfaceID IID_IGeantFollowerMSHelper("IGeantFollowerMSHelper", 1, 0);
  
  /** @class IGeantFollowerMSHelperFollowerHelper

     @author Andreas.Salzburger@cern.ch
    */
  class IGeantFollowerMSHelper : virtual public IAlgTool {
    public:

      /**Virtual destructor*/
      virtual ~IGeantFollowerMSHelper(){}
       
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_IGeantFollowerMSHelper; }
    
      // Follower interface
      // a) begin event - initialize follower process
      virtual void beginEvent() const = 0;
      // b) track the particle 
      virtual void trackParticle(const G4ThreeVector& pos, const G4ThreeVector& mom, int pdg, double charge, float t, float X0) const = 0;
      // c) end event - ntuple writing
      virtual void endEvent() const = 0;
  
  };


} // end of namespace

#endif // IGeantFollowerMSHelper_H
