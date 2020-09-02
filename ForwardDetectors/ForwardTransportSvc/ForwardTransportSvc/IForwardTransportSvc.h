/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IForwardTransportSvc_H
#define IForwardTransportSvc_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

#include "AtlasHepMC/GenEvent.h"

#include "ForwardTracker/ConfigData.h"

#include <string>

#include "G4ThreeVector.hh"

class Incident;

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IForwardTransportSvc("IForwardTransportSvc", 6, 0);

class IForwardTransportSvc: virtual public IInterface {

 public:
  
    static const InterfaceID& interfaceID() { return IID_IForwardTransportSvc; }

    virtual ForwardTracker::ConfigData getConfigData()    const = 0;    
    virtual bool                       getTransportFlag() const = 0;
    virtual double                     getEtaCut()        const = 0;
    virtual double                     getXiCut()         const = 0;
    virtual bool                       getFillRootTree()  const = 0;
    virtual std::string                getRootFilePath()  const = 0;
    virtual std::string                getMCkey()         const = 0;

    virtual bool selectedParticle(G4ThreeVector mom, int pid)                                                                                  = 0;
    virtual void fillMcInfo      (G4ThreeVector pos, double tim, G4ThreeVector mom, double ene)                                                = 0;
    virtual void addMcVertex     (G4ThreeVector pos, double tim, G4ThreeVector mom, double ene, int pid, bool isTransported, HepMC::GenEvent*) = 0;
    
    virtual const HepMC::GenEvent* getEvent() = 0;
};

#endif 
