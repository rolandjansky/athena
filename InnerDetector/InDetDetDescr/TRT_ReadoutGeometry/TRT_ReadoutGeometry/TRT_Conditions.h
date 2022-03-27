/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_Conditions.h
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_TRT_CONDITIONS_H
#define INDETREADOUTGEOMETRY_TRT_CONDITIONS_H

#include "CxxUtils/checker_macros.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "CLHEP/Geometry/Transform3D.h"

#include <atomic>

namespace TRTCond {
  class StrawDxContainer;
}

namespace InDetDD {

    /** @class TRT_Conditions
        
        This class is an interface to conditions objects. There is a single instance shared by all TRT elements.
        The object of this class is owned by TRT_DetectorManager.
      */

  class TRT_Conditions {
  
    public:
  
      TRT_Conditions();
      ~TRT_Conditions() = default;
  
      const TRTCond::StrawDxContainer* dxContainer() const;

    protected:
      void setDxContainer(const TRTCond::StrawDxContainer* container) const;
    
    private:
      mutable std::atomic<const TRTCond::StrawDxContainer*> m_dxContainer;

  };

} // end namespace

#endif // INDETREADOUTGEOMETRY_TRT_CONDITIONS_H

