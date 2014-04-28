/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __EndcapDMConstruction_H__
#define __EndcapDMConstruction_H__

class GeoFullPhysVol;

namespace LArGeo {

  class EndcapDMConstruction
  {
  public:
    EndcapDMConstruction();
    ~EndcapDMConstruction();

    void create(GeoFullPhysVol* envelope);

  private:
    EndcapDMConstruction(const EndcapDMConstruction&);
    EndcapDMConstruction& operator=(const EndcapDMConstruction&);
  };

}

#endif
