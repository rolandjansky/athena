/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// -----------------------------------------
//

#ifndef CALOTRACKINGGEOMETRY_CALOSURFACEHELPER_H
#define CALOTRACKINGGEOMETRY_CALOSURFACEHELPER_H

#include "GeoPrimitives/GeoPrimitives.h" 

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloTrackingGeometry/ICaloSurfaceHelper.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloDetDescr/CaloSubdetNames.h"

class IMessageSvc;
class ICaloSurfaceBuilder;

namespace Trk {
  class Surface;
}

/** This class stores the (simplified) Surfaces needed to perform track extrapolation.
    Surfaces are built at the initialization using CaloDetDescription tools

 */

class CaloSurfaceHelper : public AthAlgTool,
                          virtual public ICaloSurfaceHelper 

{
public:
  
  // Constructor:

  CaloSurfaceHelper(const std::string& type, 
		  const std::string& name, 
		  const IInterface* parent); 

  ~CaloSurfaceHelper(); 
  
  StatusCode initialize();
  StatusCode finalize();

  /** entrance of the sample, no eta dependence -> flat approximation */
  const Trk::Surface& GetFlatEntrySurface (const CaloCell_ID::CaloSample sample, int side);

  /** Layer entry */
  const Trk::Surface& GetEntrySurface (const CaloCell_ID::CaloSample sample, int side);

  /** Layer exit */
  const Trk::Surface& GetExitSurface (const CaloCell_ID::CaloSample sample, int side);

#if 0
  /** MBTS surfaces */
  const Trk::Surface& GetMBTSSurface(int side);
  void LoadMBTSSurfaces(std::pair<const Trk::Surface*,const Trk::Surface*> sfs);
#endif

private:
  /** private method to fill in flat surfaces */
  void get_flat_surfaces();

  ToolHandle<ICaloSurfaceBuilder>  m_surfBuilder;                    //!< tool required for definition of active volumes

  std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> >   m_entrySurfs;            // owned by CaloSurfaceBuilder
  std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> >   m_flatEntrySurfs;        // owned by this class
  std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> >   m_exitSurfs;             // owned by CaloSurfaceBuider
#if 0
  std::pair<const Trk::Surface*,const Trk::Surface*>                 m_mbtsSurfs;             // owned by this
#endif
};

#if 0
inline void CaloSurfaceHelper::LoadMBTSSurfaces(std::pair<const Trk::Surface*,const Trk::Surface*> sfs) {
   m_mbtsSurfs= sfs;
}
#endif

 
#endif // CALOTRACKINGGEOMETRY_CALOSURFACEHELPER_H






