/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MWPCConstruction
// Return an envelope that contains the LAr Beam Instrumentation.
// Apr-2006 mgf

#ifndef LARGEOH6CRYOSTATS_MWPCCONSTRUCTION_H
#define LARGEOH6CRYOSTATS_MWPCCONSTRUCTION_H

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <memory>

class IRDBAccessSvc;
class IGeoModelSvc;
class MsgStream;

namespace LArGeo {

  class MWPCConstruction 
    {
    public:
    
    MWPCConstruction(double Step = 1.*GeoModelKernelUnits::mm);

    virtual ~MWPCConstruction();
    
    virtual GeoVPhysVol* GetEnvelope();


      
  private:
    std::unique_ptr<MsgStream> m_msg;
    double m_wireStep; 
    GeoPhysVol          *m_MWPCPhysical;
  };

} // namespace LArGeo

#endif // LARGEOH6CRYOSTATS_MWPCCONSTRUCTION_H
