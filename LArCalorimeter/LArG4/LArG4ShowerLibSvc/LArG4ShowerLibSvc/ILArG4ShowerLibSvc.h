/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SHOWERLIBSVC_ILIBLARSHOWERLIBSVC_H
#define LARG4SHOWERLIBSVC_ILIBLARSHOWERLIBSVC_H

// Include Files
#include "GaudiKernel/IInterface.h"

#include <string>
#include <vector>

#include "G4FastTrack.hh"
#include "LArG4Code/EnergySpot.h"

// Forward declarations
class IIncidentListener;
class Incident;

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ILArG4ShowerLibSvc("ILArG4ShowerLibSvc", 0 , 0);


class ILArG4ShowerLibSvc : virtual public IInterface {
public:

	virtual ~ILArG4ShowerLibSvc() {}

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_ILArG4ShowerLibSvc; }

    virtual bool                    checkLibrary( G4int, int ) = 0;

    virtual std::vector<EnergySpot> getShower(const G4FastTrack&, int ) = 0;

    virtual double                  getContainmentZ(const G4FastTrack&, int ) = 0;
    virtual double                  getContainmentR(const G4FastTrack&, int ) = 0;
};

#endif 


