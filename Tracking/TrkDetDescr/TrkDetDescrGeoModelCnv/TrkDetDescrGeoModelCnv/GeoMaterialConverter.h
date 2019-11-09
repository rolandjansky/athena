///////////////////////////////////////////////////////////////////
// GeoMaterialConverter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRGEOMODELCNV_GEOMATERIALCONVERTER_H
#define TRKDETDESCRGEOMODELCNV_GEOMATERIALCONVERTER_H
// Trk
#include "TrkDetDescrGeoModelCnv/GeoShapeConverter.h"
#include "TrkGeometry/Material.h"
#include "TrkVolumes/Volume.h"
#include "TrkGeometry/LayerMaterialProperties.h"
//Amg
#include "GeoPrimitives/GeoPrimitives.h"
// STL
#include <iostream>
// GeoModelKernel
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShape.h"

class GeoVPhysVol;
class GeoPVConstLink;

struct MaterialElement{
  // constructor "static" - material defined
MaterialElement(std::string nameObj, float rn, float rx, float zn, float zx, float phin, float phix, const GeoMaterial* geomaterial,float geovolume ):
  name(nameObj),rmin(rn),rmax(rx),zmin(zn),zmax(zx),phimin(phin),phimax(phix),material(geomaterial),geoVolume(geovolume),layer(-1),isBarrelLayer(false),multi(1) {
    thmin=thMin();
    thmax=thMax();
    etamin=etaMin();
    etamax=etaMax();};  
  // constructor "dynamic" - material provided by separate tool 
  MaterialElement(std::string nameObj, float rn, float rx, float zn, float zx, float phin, float phix, int lay, bool barrel ):
  name(nameObj),rmin(rn),rmax(rx),zmin(zn),zmax(zx),phimin(phin),phimax(phix),material(0),layer(lay),isBarrelLayer(barrel),multi(1) {
    thmin=thMin();
    thmax=thMax();
    etamin=etaMin();
    etamax=etaMax();
    geoVolume=-1.;};

  std::string name;
  double rmin;
  double rmax;
  double zmin;
  double zmax;
  double etamin;
  double etamax;
  double phimin;
  double phimax;
  double thmin;
  double thmax;
  const GeoMaterial* material;
  float geoVolume;
  int   layer;
  bool  isBarrelLayer;
  Trk::Material matProp;
  int   multi;

  double mass() { 
    if (!material) return 0.;
    double dphi = phimax-phimin;
    if (dphi<=0 || dphi>2*acos(-1.)) dphi = 2*acos(-1.) ; 
     return material->getDensity()/CLHEP::g*CLHEP::mm3*geoVolume;
  }
  double volume() { 
    double dphi = phimax-phimin;
    if (dphi<=0 || dphi>2*acos(-1.)) dphi = 2*acos(-1.) ; 
    return (zmax-zmin)*dphi/2*(rmax*rmax-rmin*rmin);
  }
  double thMax() { 
    if (zmin==0.) return acos(-1.)/2.;
    double r = zmin<0 ? rmin : rmax;
    return (zmin>0.? atan(r/zmin) : atan(r/zmin)+acos(-1.)) ; 
  }
  double thMin() { 
    if (zmax==0.) return acos(-1.)/2.;
    double r = zmax<0 ? rmax : rmin;
    return (zmax>0.? atan(r/zmax) : atan(r/zmax)+acos(-1.)) ; 
  }
  double etaMin() { 
    if (zmin==0.) return 0;
    double r = zmin<0 ? rmin : rmax;
    double th = zmin>0.? atan(r/zmin) : atan(r/zmin)+acos(-1.) ;
    return -log(tan(th/2.)); 
  }
  double etaMax() { 
    if (zmax==0.) return 0;
    double r = zmax<0 ? rmax : rmin;
    double th = zmax>0.? atan(r/zmax) : atan(r/zmax)+acos(-1.) ;
    return -log(tan(th/2.)); 
  }
  void setMaterial(Trk::Material mp) { matProp = Trk::Material(mp); }

};

struct GeoObject{

  std::string name;
  std::string motherName;
  const GeoShape* geoshape;
  std::vector<int> geotree;
  std::vector<Amg::Transform3D> transform;
  float        volumeSize;
  mutable Trk::Volume* trVolume;
  int  embedLevel; 
  const GeoMaterial* material;

  // constructor 
  GeoObject(std::string nameObj, const GeoShape* shape, std::vector<int> tree, int index,  Amg::Transform3D tr, std::string mname ):
  name(nameObj),motherName(mname),geoshape(shape), volumeSize(0.), trVolume(0), embedLevel(-2),material(0) {

    geotree.clear();
    for (auto il : tree) geotree.push_back(il);
    geotree.push_back(index);
    transform.push_back(tr);
    Trk::GeoShapeConverter geoShapeConverter; 
    if (nameObj=="BeamPipeFwd" || nameObj=="BeamPipeCentral") {
      volumeSize=0.;
      embedLevel=-1.; 
    } else if (geoshape) {
      trVolume = geoShapeConverter.translateGeoShape(geoshape,&tr); 
      volumeSize = trVolume ? geoShapeConverter.calculateVolume(trVolume) : 0.;
    } else { 
      std::cout << "GeoShape not defined for GeoObject:"<< name<<":"<<mname<<std::endl;
    }
  };  
  //
  ~GeoObject() 
  { delete trVolume; trVolume=0;}
  
  void setEmbedLevel(int elevel) {embedLevel=elevel;}
  void setMaterial(const GeoMaterial* mat) {material=mat;}
  void addClone(Amg::Transform3D transf) { transform.push_back(transf); }

};

namespace Trk {

  class Surface;
  class CylinderLayer;
  class Volume;

  /**
    @class GeoMaterialConverter
    
    A Simple Helper Class that convertes the GeoMaterial object
    used in GeoModel full detector description to an appropriate
    Trk::ExtendedMaterialProperties object.
        
    @author Andreas.Salzburger@cern.ch
    */
    
    class GeoMaterialConverter {
      
      public:
        /** Default constructor*/
        GeoMaterialConverter()
       {}

        /** Destructor*/
        virtual ~GeoMaterialConverter(){}
    
        /** Single conversion , input type GeoMaterial - output type Trk::Material */
        Material convert(const GeoMaterial* gm) const;

        /** Material properties : combined material+shape conversion, projection from volume to surface */
        //MaterialProperties* convertMaterialPropertiesWithSurface(const GeoVPhysVol* geoVol, const Surface& surf) const;
        //MaterialProperties* convertMaterialPropertiesWithArea(const GeoVPhysVol* geoVol, double surf) const;

	void printInfo(const GeoPVConstLink pv) const;

	//void getMaterialElements(const GeoPVConstLink pv, std::vector<MaterialElement>& material, Amg::Transform3D& ) const;

	std::vector<GeoObject*> decodeGMtree(const GeoPVConstLink pv) const;

	void decodeMaterialContent(const GeoPVConstLink pv, GeoObject* mother, int index, Amg::Transform3D& transf, double& vol) const;

	void convertGeoObject(GeoObject* geo, std::vector<MaterialElement>& material, bool fixedVolume) const;

	//Trk::CylinderLayer* buildCylinderLayer(std::vector< MaterialElement>& material, double, double, double, double) const;


      private:
        static double s_densityCnvFactor; //!< the conversion factor from GeoUnits to Tracking
	//void printChildren(const GeoPVConstLink pv, Amg::Transform3D trIn, int level) const;
	//void extractMaterialProperties(const GeoPVConstLink geoVol, const Trk::Surface& surf, double projSurf,
	//			       Trk::MaterialProperties& matProp) const; 
    //void extractMaterialProperties(const GeoPVConstLink geoVol, double projSurf,
	//			       Trk::MaterialProperties& matProp) const; 
	MaterialElement envelope2element(Trk::Volume* envelope, const GeoMaterial* material, std::string name, double geoVolSize = 0.) const;

	mutable std::vector<GeoObject*> m_geoContent;
	mutable std::vector<GeoObject*> m_geoWaste;
   };
 



} // end of namespace Trk

#endif

