/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// -----------------------------------------
//
// 25.10.2004 Creation of the class by claire.bourdarios@cern.ch
//
// ***************************************************************************

#ifndef CALOTRACKINGGEOMETRY_ICALOSURFACEBUILDER_H
#define CALOTRACKINGGEOMETRY_ICALOSURFACEBUILDER_H

#include "GaudiKernel/IAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloDetDescr/CaloSubdetNames.h"

class CaloDetDescrManager;
class ICaloCoordinateTool;
class IMessageSvc;
class CaloDepthTool;
class ICaloRecoMaterialTool;
class ICaloRecoSimpleGeomTool;
class ICaloSurfaceHelper;

namespace Trk {
  class Surface;
  class CylinderLayer;
  class DiscLayer;
}


//<<<<<< INCLUDES >>>>>>

/** Interface to CaloSurfaceBuilder

 */
static const InterfaceID IID_ICaloSurfaceBuilder("ICaloSurfaceBuilder", 1, 0);

class ICaloSurfaceBuilder : virtual public IAlgTool {

  friend ICaloSurfaceHelper;

 public:
  /**Virtual destructor*/
  virtual ~ICaloSurfaceBuilder(){}
      
  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ICaloSurfaceBuilder; } 

  virtual void setCaloDepth(CaloDepthTool* mytool ) = 0;
  virtual CaloDepthTool* getCaloDepth() = 0;

  /** entrance of the sample, no eta dependence -> flat approximation */
  virtual Trk::Surface* CreateDDSurface (const CaloCell_ID::CaloSample sample, const int side) const = 0;


  /** overwrite DD radius/z by CaloDepth radius, and an offset can be added */
  virtual Trk::Surface* CreateUserSurface (const CaloCell_ID::CaloSample sample,
			                                     const double offset,
			                                     const double etaCaloLocal) const = 0;
  /** end of the sample for the last layers of the Calo */
  virtual Trk::Surface* CreateLastSurface (const CaloCell_ID::CaloSample sample,
			                                     const double offset,
			                                     const double etaCaloLocal) const = 0;

  /** Creates a surface at the end of the girder (Tile Calorimeter Iron Support extructure) */
  virtual Trk::Surface* CreateGirderSurface() const = 0;

  /** simplified geometry */
  virtual bool CreateDDLayers(CaloSubdetNames::ALIGNVOL alvol, std::vector< Trk::CylinderLayer*>* thelayer) const = 0;
  virtual bool CreateDDECLayers(CaloSubdetNames::ALIGNVOL alvol, std::vector< Trk::DiscLayer*>* thelayer) const = 0;

  /** These methods provide the default parameters used by the CaloTrackingGeometry and Surface Builders,
      clients should not need to use them */
  
  virtual bool get_cylinder_surface (CaloCell_ID::CaloSample sample, int side,
                             Amg::Transform3D* htrans, 
			     double& radius,  double& hphi, 
			     double& hlength, double& depth) const = 0;
  
  virtual bool get_disk_surface (CaloCell_ID::CaloSample sample, int side,
                         Amg::Transform3D* htrans, double& z, 
			 double& rmin, 
			 double& rmax, double& hphisec, double& depth) const = 0;   
  
  virtual bool get_cylinder_surface (CaloSubdetNames::ALIGNVOL alvol,
                             Amg::Transform3D* htrans,double& hphi,
                             std::vector<double>& radius,  
                             std::vector<double>& depth, 
                             std::vector<double>& hlength ) const = 0;
  
  virtual bool get_disk_surface (CaloSubdetNames::ALIGNVOL alvol,
                         Amg::Transform3D* htrans,double& hphi, 
                         std::vector<double>& z, 
                         std::vector<double>& depth,
                         std::vector<double>& rmin, std::vector<double>& rmax) const = 0;

  virtual  std::vector<const Trk::Surface*> allSampleSurfaces() const  = 0;
  virtual  std::vector<const Trk::Surface*> allTileBarSurfaces() const = 0;
  virtual  std::vector<const Trk::Surface*> allTileExtSurfaces() const = 0;
  virtual  std::vector<const Trk::Surface*> allTileGapSurfaces() const = 0;
  virtual  std::vector<const Trk::Surface*> allEMBSurfaces() const     = 0;
  virtual  std::vector<const Trk::Surface*> allEMESurfaces() const     = 0;
  virtual  std::vector<const Trk::Surface*> allHECSurfaces() const     = 0;

  virtual std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> > entrySurfaces() const = 0;
  virtual std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> > exitSurfaces() const = 0;
  

};

 
#endif // CALOTRACKINGGEOMETRY_ICALOSURFACEBUILDER_H






