/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BPCConstruction
// Return an envelope that contains the LAr Beam Instrumentation.
// adopted from BPCConstruction
// Pavol S. May-2007

#ifndef __BPCConstruction_H__
#define __BPCConstruction_H__

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

    IGeoModelSvc        *m_geoModelSvc;
    MsgStream           *m_msg;
    
  };

} // namespace LArGeo

#endif // __BPCConstruction_H__
