/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGEOENDCAP_ENDCAPDMCONSTRUCTION_H
#define LARGEOENDCAP_ENDCAPDMCONSTRUCTION_H

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
