/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZdcHARDWAREID_H
#define ZdcHARDWAREID_H

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/HWIdentifier.h"
#include "AthenaKernel/CLASS_DEF.h"

// based on the Tile calo identifier class

class ZdcHardwareID : public AtlasDetectorID {

 public:

  typedef Identifier::size_type  size_type ;


  // default constructor
  ZdcHardwareID(void);

  // default destructor
  virtual ~ZdcHardwareID();

  static int ppm(const HWIdentifier& hwid) ;
  static int channel(const HWIdentifier& hwid) ;
  static HWIdentifier ppm_id(int ppm) ;// {return HWIdentifier(0);}
  static HWIdentifier channel_id(int ppm, int channel) ;// {return HWIdentifier(0);}

 private:

};

CLASS_DEF( ZdcHardwareID , 179020 , 1 )

#endif
