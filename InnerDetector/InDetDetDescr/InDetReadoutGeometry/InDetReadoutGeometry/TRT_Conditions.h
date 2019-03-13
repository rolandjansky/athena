/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_Conditions.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_TRT_CONDITIONS_H
#define INDETREADOUTGEOMETRY_TRT_CONDITIONS_H

#include "GeoModelKernel/RCBase.h"
#include "CxxUtils/checker_macros.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "CLHEP/Geometry/Transform3D.h"

#include <mutex>

namespace TRTCond {
  class StrawDxContainer;
}

namespace InDetDD {

    /** @class TRT_Conditions
        
        This class is a interface to conditions objects. There is a single instance shared by all TRT elements
      */

  class TRT_Conditions : public RCBase {
  
    public:
  
      TRT_Conditions();
  
      const TRTCond::StrawDxContainer* dxContainer() const;
      void setDxContainer(const TRTCond::StrawDxContainer* container);

      const HepGeom::Transform3D & solenoidFrame() const;
      void setSolenoidFrame(const HepGeom::Transform3D & frame);

    private:
      const TRTCond::StrawDxContainer* m_dxContainer;
      mutable HepGeom::Transform3D m_solenoidFrame ATLAS_THREAD_SAFE; // Guarded by m_mutex
      mutable std::mutex m_mutex;

  };

} // end namespace

#endif // INDETREADOUTGEOMETRY_TRT_CONDITIONS_H

