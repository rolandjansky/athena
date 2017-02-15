/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4AtlantisDumper_H
#define G4DEBUGGINGTOOLS_G4AtlantisDumper_H


#include <fstream>
#include <string>

#include "G4AtlasInterfaces/ISteppingAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "AthenaBaseComps/AthMessaging.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

namespace G4UA{ 
  
  class G4AtlantisDumper: 
  public AthMessaging, public ISteppingAction,  public IEndEventAction,  public IBeginEventAction
  {
    
  public:
    
    struct Config
    {
      double tedep_cut=0.1,
	length_cut=1.0,
	p_cut=10.0,
	ke_cut=10.0;
    };

    G4AtlantisDumper(const Config& config);
    virtual void processStep(const G4Step*) override;
    virtual void endOfEvent(const G4Event*) override;
    virtual void beginOfEvent(const G4Event*) override;
  private:
    Config m_config;
    
    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to StoreGate (event store by default)
    mutable StoreGateSvc_t m_evtStore;
    /// Pointer to StoreGate (detector store by default)
    mutable StoreGateSvc_t m_detStore;
    int m_nsteps;
    std::ofstream *m_file;
    std::string m_filename;
    
}; // class G4AtlantisDumper


} // namespace G4UA 



#endif //G4DEBUGGINGTOOLS_G4AtlantisDumper_H
