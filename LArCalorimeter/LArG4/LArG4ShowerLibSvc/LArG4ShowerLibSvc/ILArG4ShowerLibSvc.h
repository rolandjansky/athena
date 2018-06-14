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

class ILArG4ShowerLibSvc : virtual public IInterface {
public:

  virtual ~ILArG4ShowerLibSvc() {}

  DeclareInterfaceID(ILArG4ShowerLibSvc,1,0);

  virtual bool                    checkLibrary( G4int, int ) = 0;

  virtual std::vector<EnergySpot> getShower(const G4FastTrack&, int ) = 0;

  virtual double                  getContainmentZ(const G4FastTrack&, int ) = 0;
  virtual double                  getContainmentR(const G4FastTrack&, int ) = 0;
};

#endif
