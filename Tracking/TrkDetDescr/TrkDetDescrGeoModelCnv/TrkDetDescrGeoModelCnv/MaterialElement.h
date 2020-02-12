/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialElement.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRGEOMODELCNV_MATERIALELEMENT_H
#define TRKDETDESCRGEOMODELCNV_MATERIALELEMENT_H
// Trk
#include "TrkGeometry/Material.h"
//#include "TrkVolumes/Volume.h"
//Amg
#include "GeoPrimitives/GeoPrimitives.h"
// GeoModelKernel
#include "GeoModelKernel/GeoMaterial.h"

namespace Trk {

  /**
     @class MaterialElement
     
     A Simple Helper Class for transcript of the GM material objects.
     
     @author sarka.todorova@cern.ch
  */
  
  class MaterialElement{
    
  public:
    std::string         name;
    double              rmin;
    double              rmax;
    double              zmin;
    double              zmax;
    double              etamin;
    double              etamax;
    double              phimin;
    double              phimax;
    double              thmin;
    double              thmax;
    const GeoMaterial* material;
    float                  geoVolume;
    int                     layer;
    bool                  isBarrelLayer;
    Trk::Material     matProp;
    int   multi;

    // Constructor with pre-defined material
  MaterialElement(std::string nameObj, float rn, float rx, float zn, float zx, float phin, float phix, 
		  const GeoMaterial* geomaterial,float geovolume ):
    name(nameObj)
      ,rmin(rn)
      ,rmax(rx)
      ,zmin(zn)
      ,zmax(zx)
      ,phimin(phin)
      ,phimax(phix)
      ,material(geomaterial)
      ,geoVolume(geovolume)
      ,layer(-1)
      ,isBarrelLayer(false)
      ,multi(1)
      {
	thmin=thMin();
	thmax=thMax();
	etamin=etaMin();
	etamax=etaMax();
      };
    
      // Constructor "dynamic" - material provided by separate tool 
  MaterialElement(std::string nameObj, float rn, float rx, float zn, float zx, float phin, float phix, int lay, bool barrel ):
    name(nameObj)
      ,rmin(rn)
      ,rmax(rx)
      ,zmin(zn)
      ,zmax(zx)
      ,phimin(phin)
      ,phimax(phix)
      ,material(0)
      ,layer(lay)
      ,isBarrelLayer(barrel)
      ,multi(1) 
      {
	thmin=thMin();
	thmax=thMax();
	etamin=etaMin();
	etamax=etaMax();
	geoVolume=-1.;
      };
    
    /** Destructor*/
    virtual ~MaterialElement(){};
    
    /** simple helper functions */    
    double mass(); 
    double volume();
    double thMax();
    double thMin(); 
    double etaMin(); 
    double etaMax();

    /** material update, used also for scaling */
    void setMaterial(Trk::Material mp); 
    
  };

} // end of namespace Trk

#endif

