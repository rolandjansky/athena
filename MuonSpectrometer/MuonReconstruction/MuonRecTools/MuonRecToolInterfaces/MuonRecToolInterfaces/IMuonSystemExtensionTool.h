/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSYSTEMEXTENSIONTOOL_H
#define MUON_IMUONSYSTEMEXTENSIONTOOL_H

/** 
    @class IMuonSystemExtensionTool

    @author Niels.Van.Eldik@cern.ch
*/

#include "GaudiKernel/IAlgTool.h"

#include "xAODTracking/TrackParticle.h"


namespace Muon {
  
  class MuonSystemExtension;

  /** Interface ID*/  
  static const InterfaceID IID_IMuonSystemExtensionTool("Muon::IMuonSystemExtensionTool", 1, 0);

  class IMuonSystemExtensionTool : virtual public IAlgTool {
  public:
     /**Virtual destructor*/
    virtual ~IMuonSystemExtensionTool(){};
  
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IMuonSystemExtensionTool; }
    
    /** get muon system extension */
    virtual bool muonSystemExtension(  const xAOD::TrackParticle& indetTrackParticle,
    			const MuonSystemExtension*& muonSystemExtention ) const = 0;
    
  };

}

#endif
