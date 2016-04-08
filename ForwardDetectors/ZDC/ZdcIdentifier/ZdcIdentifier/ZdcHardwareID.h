/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZdcHARDWAREID_H
#define ZdcHARDWAREID_H

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/HWIdentifier.h"
#include "CLIDSvc/CLASS_DEF.h"

// based on the Tile calo identifier class

class ZdcHardwareID : public AtlasDetectorID {

 public:

  typedef Identifier::size_type  size_type ;


  // default constructor
  ZdcHardwareID(void);

  // default destructor
  virtual ~ZdcHardwareID();

  int ppm(const HWIdentifier& hwid) const;
  int channel(const HWIdentifier& hwid) const;
  HWIdentifier ppm_id(int ppm) const;// {return HWIdentifier(0);}
  HWIdentifier channel_id(int ppm, int channel) const;// {return HWIdentifier(0);}

 private:

};

CLASS_DEF( ZdcHardwareID , 179020 , 1 )

#endif
