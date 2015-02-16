/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Construction of the walls in front of the H1 Cryostat
// 
// March 2007 M.Fincke

#ifndef LARGEOH6CRYOSTATS_WALLSCONSTRUCTION_H
#define LARGEOH6CRYOSTATS_WALLSCONSTRUCTION_H

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include <memory>
class IRDBAccessSvc;
class IGeoModelSvc;
class MsgStream;
 
namespace LArGeo {

  class WallsConstruction 
    {
    public:
    
    WallsConstruction();
    virtual ~WallsConstruction();
    
    // Get the envelope containing this detector:
    virtual GeoVPhysVol* GetEnvelope();

  private:
    std::unique_ptr<MsgStream> m_msg;
    GeoPhysVol          *m_WallsPhysical;
  };

} // namespace LArGeo

#endif // LARGEOH6CRYOSTATS_WALLSCONSTRUCTION_H
