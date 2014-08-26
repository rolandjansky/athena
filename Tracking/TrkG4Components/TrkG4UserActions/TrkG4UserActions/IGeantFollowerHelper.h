/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IGeantFollowerHelperFollowerHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef IGeantFollowerHelper_H
#define IGeantFollowerHelper_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "G4ThreeVector.hh"

namespace Trk {

  /** Interface ID for IGeantFollowerHelperFollowerHelper */  
  static const InterfaceID IID_IGeantFollowerHelper("IGeantFollowerHelper", 1, 0);
  
  /** @class IGeantFollowerHelperFollowerHelper

     @author Andreas.Salzburger@cern.ch
    */
  class IGeantFollowerHelper : virtual public IAlgTool {
    public:

      /**Virtual destructor*/
      virtual ~IGeantFollowerHelper(){}
       
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_IGeantFollowerHelper; }
    
      // Follower interface
      // a) begin event - initialize follower process
      virtual void beginEvent() const = 0;
      // b) track the particle 
      virtual void trackParticle(const G4ThreeVector& pos, const G4ThreeVector& mom, int pdg, double charge, float t, float X0) const = 0;
      // c) end event - ntuple writing
      virtual void endEvent() const = 0;
  
  };


} // end of namespace

#endif // IGeantFollowerHelper_H