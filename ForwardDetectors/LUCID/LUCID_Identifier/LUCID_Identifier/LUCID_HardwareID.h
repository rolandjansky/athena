/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_HARDWAREID_H
#define LUCID_HARDWAREID_H

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/HWIdentifier.h"
#include "CLIDSvc/CLASS_DEF.h"

// based on the Tile calo identifier class

class LUCID_HardwareID : public AtlasDetectorID {

 public:

  typedef Identifier::size_type  size_type ;


  // default constructor
  LUCID_HardwareID(void);

  // default destructor
  virtual ~LUCID_HardwareID();

 private:

};

CLASS_DEF( LUCID_HardwareID , 17902 , 1 )

#endif
