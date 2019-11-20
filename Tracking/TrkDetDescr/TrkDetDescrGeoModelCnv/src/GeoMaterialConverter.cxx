/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeoMaterialConverter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/Volume.h"
// GeoModelKernel
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoPVConstLink.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
/// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// Amg
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

double Trk::GeoMaterialConverter::s_densityCnvFactor = 1./CLHEP::gram;

Trk::Material Trk::GeoMaterialConverter::convert(const GeoMaterial* gm) const {
  // get the obvious things 
  float x0   = gm->getRadLength();
  float l0   = gm->getIntLength();
  float rho  = gm->getDensity() * s_densityCnvFactor;
  //float dEdX = gm->getDeDxMin();
  float A    = 0.;
  float Z    = 0.;
  // get the number of elements that build the material 
  int numberOfElements = gm->getNumElements();
  // loop over the elements
  for (int iEl=0; iEl<numberOfElements; iEl++){
        const GeoElement* geoEl = gm->getElement(iEl);
        float fraction = gm->getFraction(iEl);
        A += fraction*(geoEl->getA()/(CLHEP::gram));
        Z += fraction*(geoEl->getZ());             
   }
  // return the result
  return Trk::Material(x0,l0,A,Z,rho);

}

std::vector<Trk::GeoObject*> Trk::GeoMaterialConverter::decodeGMtree(const GeoPVConstLink pv) const {

  Amg::Transform3D transf=Amg::Transform3D(Trk::s_idTransform);
  std::vector<int> tree;
  Trk::GeoObject* topElement=new Trk::GeoObject("top",0,tree,0,transf,"");
    
  Amg::Transform3D tr =  Amg::CLHEPTransformToEigen(pv->getX());
 
  double dummyVol = 0.;
  std::vector<Trk::GeoObject*> geoContent;
  std::vector<Trk::GeoObject*> geoWaste ;
  decodeMaterialContent(pv,topElement,0,tr,dummyVol,geoContent,geoWaste);

  for (auto geo: geoWaste) delete geo;
  delete topElement;

  return geoContent; 
}

void Trk::GeoMaterialConverter::decodeMaterialContent(const GeoPVConstLink pv, Trk::GeoObject* mother, int index, Amg::Transform3D& transf, double& volSubtr, std::vector<Trk::GeoObject*>& geoContent, std::vector<Trk::GeoObject*>& geoWaste) const {

  const GeoLogVol* lv = pv->getLogVol();
  const GeoShape*  sh = lv->getShape();

  Trk::GeoObject* newElement=new Trk::GeoObject(lv->getName(),sh,mother->geotree,index,transf,mother->name);
  if (lv->getName()=="AssemblyLV") newElement->setEmbedLevel(-1);
  else newElement->setEmbedLevel(mother->embedLevel+1);
  // this is the volume to subtract from the mother
  volSubtr = lv->getName()=="AssemblyLV" ? 0. : newElement->volumeSize;
  unsigned int nc = pv->getNChildVols();
  for (unsigned int ic=0; ic<nc; ic++) {
    const PVConstLink cv = pv->getChildVol(ic);
    Amg::Transform3D transfc = transf*Amg::CLHEPTransformToEigen(pv->getXToChildVol(ic));
    double volumeEmbedded=0.;          // 
    decodeMaterialContent(cv,newElement,ic,transfc,volumeEmbedded,geoContent,geoWaste);
    if ( lv->getName()=="AssemblyLV" ) newElement->volumeSize = newElement->volumeSize + volumeEmbedded;
    else newElement->volumeSize = newElement->volumeSize - volumeEmbedded;
  }

  if (lv->getName()=="AssemblyLV") volSubtr = newElement->volumeSize;

  // save first instance found, transforms for the 'clones'
  bool found = lv->getName()=="AssemblyLV" ? true : false;    // don't save dummy assembly volume  
  if (!found) { 
    for (unsigned int ig=0; ig<geoContent.size(); ig++) {
      // check both name and size
      if (geoContent[ig]->name==newElement->name ) {
	if (fabs(newElement->volumeSize-geoContent[ig]->volumeSize)<1.e-2*geoContent[ig]->volumeSize) found=true; 
      }
      if (found) {
        geoContent[ig]->addClone(transf);
	break; 
      }
    }
    if (!found) { 
      newElement->setMaterial(lv->getMaterial());
      geoContent.push_back(newElement);
      if (lv->getName()=="PetalCore") {
	Trk::GeoShapeConverter geoShapeConverter; 
	geoShapeConverter.decodeShape(sh); 
      }
    } else geoWaste.push_back(newElement);
  }  

  return;
}

void Trk::GeoMaterialConverter::convertGeoObject(Trk::GeoObject* geo, std::vector< Trk::MaterialElement>& material, bool fixedVolume) const{

  if (!geo->trVolume) return;
  Trk::GeoShapeConverter geoShapeConverter; 

  double volSize = geo->volumeSize;
  if (volSize<0) return;    // can happen for logical envelopes

  unsigned int it=0;
  Amg::Transform3D tr0=geo->transform[0];
  for ( auto tr : geo->transform ) {
    Trk::Volume clone(*geo->trVolume,tr*tr0.inverse());
    Trk::Volume* envelope = fixedVolume ? geoShapeConverter.cylEnvelope(&clone,volSize) : geoShapeConverter.cylEnvelope(&clone);
    if (envelope) material.push_back(envelope2element(envelope,geo->material,geo->name,volSize));
    delete envelope;
    it++;
  }

  return;
}

Trk::MaterialElement Trk::GeoMaterialConverter::envelope2element(Trk::Volume* envelope, const GeoMaterial* material, std::string name, double volSize) const {

  const Trk::CylinderVolumeBounds* cyl=dynamic_cast<const Trk::CylinderVolumeBounds*> (&(envelope->volumeBounds()));
  float minR = cyl->innerRadius(); float maxR = cyl->outerRadius();
  float dPhi = cyl->halfPhiSector(); float hZ = cyl->halflengthZ(); 

  Amg::Vector3D testPoint = envelope->transform()*Amg::Vector3D(1.,0.,0.);
  double refPhi=0.; 
  if ( dPhi<M_PI ) {
    refPhi = atan(testPoint.x()/testPoint.y());
    if (testPoint.y() < 0 )  refPhi+=M_PI;   
  }
  
  return  Trk::MaterialElement(name,minR,maxR,envelope->center().z()-hZ,envelope->center().z()+hZ, refPhi-dPhi, refPhi+dPhi, material,volSize);
} 
