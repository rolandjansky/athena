/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MWPCConstruction
// Return an envelope that contains the LAr Beam Instrumentation.
// Apr-2006 mgf

#ifndef __MWPCConstruction_H__
#define __MWPCConstruction_H__

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"

class IRDBAccessSvc;
class IGeoModelSvc;
class MsgStream;

namespace LArGeo {

  class MWPCConstruction 
    {
    public:
    
    MWPCConstruction(double Step = 1.*CLHEP::mm);

    virtual ~MWPCConstruction();
    
    virtual GeoVPhysVol* GetEnvelope();

    MsgStream *msg;

      
  private:

    double wireStep; 

    GeoPhysVol          *MWPCPhysical;


    IRDBAccessSvc       *pAccessSvc;
    IGeoModelSvc        *geoModelSvc;
    
  };

} // namespace LArGeo

#endif // __MWPCConstruction_H__
