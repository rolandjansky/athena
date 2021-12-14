/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// -----------------------------------------
//
// 25.10.2004 Creation of the class by claire.bourdarios@cern.ch
//
// 13.6.2006  Extention to include the tiles will work from 12.0.2 and 12.1.0
//            by Gustavo.Ordonez.Sanz@cern.ch
//
// 20.3.2014  entry/exit surfaces cashed for use by tracking geometry
//            sarka.todorova@cern.ch
// ***************************************************************************

#ifndef CALOTRACKINGGEOMETRY_CALOSURFACEBUILDER_H
#define CALOTRACKINGGEOMETRY_CALOSURFACEBUILDER_H

#include "GeoPrimitives/GeoPrimitives.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloDetDescr/CaloDepthTool.h"
#include "CaloDetDescr/CaloSubdetNames.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <CxxUtils/checker_macros.h>
#include <vector>

class CaloDetDescrManager;
class ICaloCoordinateTool;
class IMessageSvc;
class ICaloRecoMaterialTool;
class ICaloRecoSimpleGeomTool;
class ICaloSurfaceHelper;
class TileDetDescrManager;
class TileDetDescriptor;

namespace Trk {
class Surface;
class CylinderLayer;
class DiscLayer;
class MaterialProperties;
class LayerMaterialProperties;
class ITrackingVolumeBuilder;
}

/** This class creates the Surfaces needed to perform track extrapolation.
    All numbers are provided by the CaloDetDescr tools
 */

class CaloSurfaceBuilder
  : public AthAlgTool
  , virtual public ICaloSurfaceBuilder

{

public:
  // Constructor:

  CaloSurfaceBuilder(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloSurfaceBuilder();

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  virtual void setCaloDepth(CaloDepthTool* mytool) override final;
  virtual CaloDepthTool* getCaloDepth() override final;

  /** entrance of the sample, no eta dependence -> flat approximation */
  virtual Trk::Surface* CreateDDSurface(const CaloCell_ID::CaloSample sample,
                                        const int side) const override final;

  /** overwrite DD radius/z by CaloDepth radius, and an offset can be added */
  virtual Trk::Surface* CreateUserSurface(
    const CaloCell_ID::CaloSample sample,
    const double offset,
    const double etaCaloLocal,
    const CaloDetDescrManager* calo_dd) const override final;

  /** end of the sample for the last layers of the Calo */
  virtual Trk::Surface* CreateLastSurface(
    const CaloCell_ID::CaloSample sample,
    const double offset,
    const double etaCaloLocal,
    const CaloDetDescrManager* calo_dd) const override final;

  /** Creates a surface at the end of the girder (Tile Calorimeter Iron Support
   * extructure) */
  virtual Trk::Surface* CreateGirderSurface() const override final;

  /** simplified geometry */
  virtual bool CreateDDLayers(
    CaloSubdetNames::ALIGNVOL alvol,
    std::vector<Trk::CylinderLayer*>* thelayer) const override final;
  virtual bool CreateDDECLayers(
    CaloSubdetNames::ALIGNVOL alvol,
    std::vector<Trk::DiscLayer*>* thelayer) const override final;

  /** These methods provide the default parameters used by the
     CaloTrackingGeometry and Surface Builders, clients should not need to use
     them */

  virtual bool get_cylinder_surface(
    CaloCell_ID::CaloSample sample,
    int side,
    Amg::Transform3D* htrans,
    double& radius,
    double& hphi,
    double& hlength,
    double& depth,
    const CaloDetDescrManager* calo_dd) const override final;

  virtual bool get_disk_surface(
    CaloCell_ID::CaloSample sample,
    int side,
    Amg::Transform3D* htrans,
    double& z,
    double& rmin,
    double& rmax,
    double& hphisec,
    double& depth,
    const CaloDetDescrManager* calo_dd) const override final;

  virtual bool get_cylinder_surface(
    CaloSubdetNames::ALIGNVOL alvol,
    Amg::Transform3D* htrans,
    double& hphi,
    std::vector<double>& radius,
    std::vector<double>& depth,
    std::vector<double>& hlength) const override final;

  virtual bool get_disk_surface(CaloSubdetNames::ALIGNVOL alvol,
                                Amg::Transform3D* htrans,
                                double& hphi,
                                std::vector<double>& z,
                                std::vector<double>& depth,
                                std::vector<double>& rmin,
                                std::vector<double>& rmax) const override final;

  virtual std::vector<std::pair<const Trk::Surface*, const Trk::Surface*>>
  entrySurfaces(const CaloDetDescrManager* calo_dd) const override final;
  virtual std::vector<std::pair<const Trk::Surface*, const Trk::Surface*>>
  exitSurfaces(const CaloDetDescrManager* calo_dd) const override final;

private:
  // fill cashed surfaces
  void fill_tg_surfaces(const CaloDetDescrManager* calo_dd) const;

  const TileDetDescrManager* m_tile_dd;

  // cache the surfaces for TG builder
  // These are protected by calls to fill once
  mutable std::vector<std::pair<const Trk::Surface*, const Trk::Surface*>>
    m_layerEntries ATLAS_THREAD_SAFE;
  mutable std::vector<std::pair<const Trk::Surface*, const Trk::Surface*>>
    m_layerExits ATLAS_THREAD_SAFE;
  mutable std::once_flag m_fillOnce ATLAS_THREAD_SAFE;

  // Simplified geometry :
  ToolHandle<ICaloRecoMaterialTool> m_lar_mat;
  ToolHandle<ICaloRecoSimpleGeomTool> m_lar_simplegeom;

  // Defines the depths for DD and User surfaces :
  ToolHandle<CaloDepthTool> m_calodepth{ this,
                                         "CaloDepthTool",
                                         "CaloDepthTool/CaloDepthTool",
                                         "CaloDepthTool to be used" };
};

inline std::vector<std::pair<const Trk::Surface*, const Trk::Surface*>>
CaloSurfaceBuilder::entrySurfaces(const CaloDetDescrManager* calo_dd) const
{
  std::call_once(m_fillOnce, [this, calo_dd]() { fill_tg_surfaces(calo_dd); });
  return m_layerEntries;
}

inline std::vector<std::pair<const Trk::Surface*, const Trk::Surface*>>
CaloSurfaceBuilder::exitSurfaces(const CaloDetDescrManager* calo_dd) const
{
  std::call_once(m_fillOnce, [this, calo_dd]() { fill_tg_surfaces(calo_dd); });
  return m_layerExits;
}

#endif // CALOTRACKINGGEOMETRY_CALOSURFACEBUILDER_H

