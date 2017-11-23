/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVGENPRODTOOLS_IEVGENOTFTOPUPSVC_H
#define EVGENPRODTOOLS_IEVGENOTFTOPUPSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>

/// @brief IService to hold some information for topping up
///  algorithms that are running on the fly.  It's a bit tricky,
///  but this is mostly just a service for holding some numbers...
///

static const InterfaceID IID_IEvgenOTFTopUpSvc("IEvgenOTFTopUpSvc", 1 , 0);

class IEvgenOTFTopUpSvc : virtual public IInterface {
public:

  // Getters
  virtual long   getNPerFile()    const = 0;
  virtual long   getNUsedSoFar()  const = 0;
  virtual double getEfficiency()  const = 0;
  virtual bool   getNewFileFlag() const = 0;

  // Setters - first for Pythia/Herwig to call when they use up events
  virtual void   usedEvents() = 0;
  virtual void   usedEvents( const int ) = 0;

  // New file created by the OTF generator algorithm
  virtual void   newFile( std::string& ) = 0;

  // New file grabbed by the showering generator
  virtual const std::string& getFile() = 0;

  static const InterfaceID& interfaceID() { return IID_IEvgenOTFTopUpSvc; };

};

#endif
