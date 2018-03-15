/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoComponentHelper_H
#define InDet_GeoComponentHelper_H

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoTransform.h"
#include "PixelGeoComponent/GeoComponent.h"

#include <vector>
#include <map>
#include <string>

class GeoFullPhysVol;
class GeoPhysVol;
class GeoAlignableTransform;
class GeoTransform;
class GeoIdentifierTag;
class GeoNameTag;
class GeoMaterial;
class GeoInDetMaterialManager;
/** Base class for components of the detector geometry.
    Intended for logical structuring of the components relevant for the readout and tracking geometries, 
    like barrel containing layers containing staves containing modules etc.
 */

namespace InDet {
  
  class MinMaxHelper;
  class MinMax3DHelper;
  class GeoComponentHelper {
  public:

    GeoComponentHelper( const GeoComponent&);

    virtual ~GeoComponentHelper();


    // Compute composite material corresponding to a material/volume input
    //    void computeEquivalentMaterial(const std::string& newMatName, InDetMaterialManager* matMgr, std::map<std::string,double> materialMap) const;
    GeoMaterial* computeEquivalentMaterial(InDetMaterialManager* matMgr, std::string, std::vector<std::string> ignList=std::vector<std::string>()) const;
    GeoMaterial* combineCompositeMaterial(InDetMaterialManager* matMgr, std::string newMatName,
					  const GeoMaterial* mat1, const GeoMaterial* mat2, double ratio1, double ratio2) const;

    // Compute all material/volume pair for a given GeoComponent branch
    virtual std::map<std::string,double> computeSubTreeComponentsMaterialAndVolume(std::vector<std::string> ignList=std::vector<std::string>()) const;


    // Compute/access GeoComponent volume 
    virtual double computeComponentVolume_basic(int iLevel=0) const; 
    double computeComponentVolume(std::vector<std::string> ignoreVol=std::vector<std::string>()) const; 
    virtual std::map<std::string,double> computeSubTreeComponentsVolume(bool bGroupVolume) const; 
    virtual std::map<std::string,double> computeSubTreeComponentsVolume_rec(const GeoComponent& currentComponent, bool bGroupVolume) const;
    virtual std::map<std::string,double> computeSubTreeComponentsVolume_level(const GeoComponent& currentComponent, int iLevel) const; 


    // get GeoComponent shape/material ( defined in logvol)
    virtual double getShapeVolume() const;
    virtual std::string getMaterialName() const;
    virtual const GeoMaterial* getMaterial() const;

    // Find GeoComponent based on name and parent branch name  ( /!\ unicity of response is not garanted, first found is returned)

    virtual HepGeom::Transform3D* getGeoComponentTransform3D(const std::string& volumeName) const;
    virtual MinMaxHelper getZminmax(const HepGeom::Transform3D& trf) const;
    virtual MinMax3DHelper get3Dminmax(const HepGeom::Transform3D& trf) const;
    virtual MinMaxHelper getRminmax(const HepGeom::Transform3D& trf) const;
    virtual double getRmax(const HepGeom::Transform3D& trf) const;
    virtual double getRmin(const HepGeom::Transform3D& trf) const;

    virtual InDet::GeoComponent* findGeoComponent(const std::string& volumeName) const;
    virtual std::vector<InDet::GeoComponent*> findGeoComponentList(const std::string& volumeName, const std::string& parentName="None") const; 
    virtual std::vector<InDet::GeoComponent*> findGeoComponentList_rec(const GeoComponent& comp, const std::string& volumeName) const; 

    //    virtual std::vector<GeoComponent*> getGeoComponentBranch(const std::string& volumeName, const std::string& branchName="None") const;
    virtual std::vector<GeoComponent*> getGeoComponentBranch(const std::string& compName) const;
    virtual std::vector<GeoComponent*> getGeoComponentBranch_Level(const GeoComponent& comp, const std::string& volumeName) const;


    // Geometric characteristics
    virtual double getHalfSizeAlongXAxis() const;
    virtual double getHalfSizeAlongYAxis() const;
    virtual double getHalfSizeAlongZAxis() const;
    virtual void getMaxSizeXAxis(double& vMin, double &vMax) const;
    virtual double getMaxSizeYAxis() const;
    virtual std::vector<HepGeom::Point3D<double> > getVertexShape() const;

    // Extract sensor informations to be saved in storegate
    void FindGeoComponentsMatchingName(const GeoComponent& currentComponent, const std::string& name, std::vector<GeoComponent*> &list) const;
    std::vector<int> getSensorModuleIndices() const; 


  protected:

    const InDet::GeoComponent& geoComponent;

  };


  class MinMaxHelper{
    
  public:
    MinMaxHelper(double min, double max){ vMin=min; vMax=max; }
    ~MinMaxHelper(){};
    
    double getMin() { return vMin; };
    double getMax() { return vMax; };
    void update(MinMaxHelper box)
    { if(box.vMin<vMin)vMin=box.vMin; if(box.vMax>vMax)vMax=box.vMax; };

  private:
    double vMin,vMax;
  };


  class MinMax3DHelper{
    
  public:
    MinMax3DHelper(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
      { xMin=xmin; xMax=xmax; yMin=ymin; yMax=ymax; zMin=zmin; zMax=zmax; }
    ~MinMax3DHelper(){};
    
    double getXMin() { return xMin; };
    double getXMax() { return xMax; };
    double getYMin() { return yMin; };
    double getYMax() { return yMax; };
    double getZMin() { return zMin; };
    double getZMax() { return zMax; };
    //    void update(MinMaxHelper box)
    //    { if(box.vMin<vMin)vMin=box.vMin; if(box.vMax>vMax)vMax=box.vMax; };

  private:
    double xMin,xMax;
    double yMin,yMax;
    double zMin,zMax;
  };




}


#endif
