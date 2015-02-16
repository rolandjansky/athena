/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BPCConstruction
// Return an envelope that contains the LAr Beam Instrumentation.
// adopted from BPCConstruction
// Pavol S. May-2007

#ifndef LARGEOH6CRYOSTATS_BPCCONSTRUCTION_H
#define LARGEOH6CRYOSTATS_BPCCONSTRUCTION_H

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"

class IRDBAccessSvc;
class IGeoModelSvc;
class MsgStream;

namespace LArGeo {

  class BPCConstruction 
    {
    public:
    
    BPCConstruction(bool old = false);

    virtual ~BPCConstruction();
    
    virtual GeoVPhysVol* GetEnvelope();


      
  private:

    bool                 m_oldType; 

    GeoPhysVol          *m_BPCPhysical;

    MsgStream           *m_msg;
    
  };

} // namespace LArGeo

#endif // LARGEOH6CRYOSTATS_BPCCONSTRUCTION_H
