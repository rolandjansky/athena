/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ToolHandle.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloDetDescr/CaloSubdetNames.h"

#include <mutex>

class CaloDetDescrManager;
class ICaloCoordinateTool;
class IMessageSvc;
class CaloDepthTool;
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


//<<<<<< INCLUDES >>>>>>

/** This class creates the Surfaces needed to perform track extrapolation.
    All numbers are provided by the CaloDetDescr tools
 */

class CaloSurfaceBuilder : public AthAlgTool,
                           virtual public ICaloSurfaceBuilder 

{

public:
  
  // Constructor:

  CaloSurfaceBuilder(const std::string& type, 
		  const std::string& name, 
		  const IInterface* parent); 
  
  virtual ~CaloSurfaceBuilder();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual void setCaloDepth(CaloDepthTool* mytool ) override;
  virtual CaloDepthTool* getCaloDepth() override;

  /** entrance of the sample, no eta dependence -> flat approximation */
  virtual
  Trk::Surface* CreateDDSurface (const CaloCell_ID::CaloSample sample, const int side) const override;


  /** overwrite DD radius/z by CaloDepth radius, and an offset can be added */
  virtual
  Trk::Surface* CreateUserSurface (const CaloCell_ID::CaloSample sample,
			           const double offset,
			           const double etaCaloLocal) const override;

  /** end of the sample for the last layers of the Calo */
  virtual
  Trk::Surface* CreateLastSurface (const CaloCell_ID::CaloSample sample,
                                   const double offset,
				   const double etaCaloLocal) const override;


  /** Creates a surface at the end of the girder (Tile Calorimeter Iron Support extructure) */
  virtual Trk::Surface* CreateGirderSurface() const override;

  /** simplified geometry */
  virtual bool CreateDDLayers(CaloSubdetNames::ALIGNVOL alvol, std::vector< Trk::CylinderLayer*>* thelayer) const override;
  virtual bool CreateDDECLayers(CaloSubdetNames::ALIGNVOL alvol, std::vector< Trk::DiscLayer*>* thelayer) const override;

  /** These methods provide the default parameters used by the CaloTrackingGeometry and Surface Builders,
      clients should not need to use them */
  
  virtual   
  bool get_cylinder_surface (CaloCell_ID::CaloSample sample, int side,
                             Amg::Transform3D* htrans, 
                             double& radius,  double& hphi, 
                             double& hlength, double& depth) const override;
  
  virtual   
  bool get_disk_surface (CaloCell_ID::CaloSample sample, int side,
                         Amg::Transform3D* htrans, double& z, 
                         double& rmin, 
                         double& rmax, double& hphisec, double& depth) const override;
  
  virtual   
  bool get_cylinder_surface (CaloSubdetNames::ALIGNVOL alvol,
                             Amg::Transform3D* htrans,double& hphi,
                             std::vector<double>& radius,  
                             std::vector<double>& depth, 
                             std::vector<double>& hlength ) const override;

  virtual   
  bool get_disk_surface (CaloSubdetNames::ALIGNVOL alvol,
                         Amg::Transform3D* htrans,double& hphi, 
                         std::vector<double>& z, 
                         std::vector<double>& depth,
                         std::vector<double>& rmin, std::vector<double>& rmax)
    const override;

  virtual std::vector<const Trk::Surface*> allSampleSurfaces() const override;
  virtual std::vector<const Trk::Surface*> allTileBarSurfaces() const override;
  virtual std::vector<const Trk::Surface*> allTileExtSurfaces() const override;
  virtual std::vector<const Trk::Surface*> allTileGapSurfaces() const override;
  virtual std::vector<const Trk::Surface*> allEMBSurfaces() const override;
  virtual std::vector<const Trk::Surface*> allEMESurfaces() const override;
  virtual std::vector<const Trk::Surface*> allHECSurfaces() const override;

  virtual std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> > entrySurfaces() const override;
  virtual std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> > exitSurfaces() const override;

private:

  // fill cashed surfaces
  void fill_tg_surfaces() const;

  // CaloDetDescr usal stuff
  //const CaloCell_ID* m_calo_id;
  const mutable CaloDetDescrManager* m_calo_dd;

  //TileDetDescr stuff
  const TileDetDescrManager* m_tile_dd;

  // cash the surfaces for TG builder
  mutable std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> > m_layerEntries;
  mutable std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> > m_layerExits;
  mutable std::once_flag m_fillOnce;

// needed to cover TestBeam
//  ToolHandle<ICaloCoordinateTool>               m_calo_tb_coord;

  // Simplified geometry :
  ToolHandle<ICaloRecoMaterialTool>             m_lar_mat;
  ToolHandle<ICaloRecoSimpleGeomTool>           m_lar_simplegeom;

  // Defines the depths for DD and User surfaces : 
  ToolHandle<CaloDepthTool>                     m_calodepth;
 
  //ToolHandle<Trk::ITrackingVolumeBuilder>       m_tileVolumeBuilder;             //!< Volume Builder for the Tile Calorimeter


  //IToolSvc* m_toolsvc;
  //IMessageSvc*  m_msgSvc;
};

inline std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> > CaloSurfaceBuilder::entrySurfaces ATLAS_THREAD_SAFE() const
{
  std::call_once( m_fillOnce, [this](){fill_tg_surfaces();} );
  return m_layerEntries;
}

inline std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> > CaloSurfaceBuilder::exitSurfaces ATLAS_THREAD_SAFE() const
{
  std::call_once( m_fillOnce, [this](){fill_tg_surfaces();} );
  return m_layerExits;
}

 
#endif // CALOTRACKINGGEOMETRY_CALOSURFACEBUILDER_H






