/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelServicesTool/VolumeGeoCompBuilder.h"

#include "InDetGeoModelUtils/ServiceVolume.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

//#include "InDetGeoModelUtils/GeoSimpleObject.h"
//#include "InDetGeoModelUtils/GeoComponentCnv.h"
//#include "InDetGeoModelUtils/GeoServiceAssembly.h"

#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"

#include "CLHEP/Geometry/Transform3D.h"

//#include <sstream>
//#include <iomanip>
namespace InDetDD {

VolumeGeoCompBuilder::VolumeGeoCompBuilder(const Zone & zone, const std::vector<const ServiceVolume * > & services) 
  : m_msg("InDetDDVolumeGeoCompBuilder"),
    m_region("None"), // Empty refers to a valid region. Set some default so we can check it is actually set.
    m_zcenter(0),
    m_services(0),
    m_servEnvelope(0),
    m_servChild(0),
    m_matManager(0)
{
  //  m_svcObjects.clear();
  m_splitter.splitAll(zone, services);

}

VolumeGeoCompBuilder::VolumeGeoCompBuilder(const std::vector<const ServiceVolume * > & services) 
  : m_msg("InDetDDVolumeGeoCompBuilder"),
    m_region("None"), // Empty refers to a valid region. Set some default so we can check it is actually set.
    m_zcenter(0),
    m_services(&services),
    m_servEnvelope(0),
    m_servChild(0),


    m_matManager(0)
{}
  
VolumeGeoCompBuilder::VolumeGeoCompBuilder(const Zone & zone, const std::vector<const ServiceVolume * > & services,
			     const std::vector<const ServiceVolume * > & servEnvelope,
			     const std::vector<const ServiceVolume * > & servChild ) 
  : m_msg("InDetDDVolumeGeoCompBuilder"),
    m_region("None"), // Empty refers to a valid region. Set some default so we can check it is actually set.
    m_zcenter(0),
    m_services(0),
    m_servEnvelope(&servEnvelope),
    m_servChild(&servChild),
    m_matManager(0)
{
  //  m_svcObjects.clear();
  m_splitter.splitAll(zone, services);
}
  
VolumeGeoCompBuilder::~VolumeGeoCompBuilder()
{
  resetServices();
}


void VolumeGeoCompBuilder::resetServices()
{
//   int nbObj=m_svcObjects.size();
//   for(int i=0; i<nbObj; i++){
//     m_svcObjects[i]->UnRef();
//     delete m_svcObjects[i];
//   }
//   m_svcObjects.clear();
}


void VolumeGeoCompBuilder::addServices(const Zone & zone, const std::vector<const ServiceVolume * > & services)
{
  m_splitter.splitAll(zone, services);
}
  
void VolumeGeoCompBuilder::setRegion(const std::string & region, double zcenter) 
{
  m_region = region;
  m_zcenter = zcenter;
}

const std::vector<const ServiceVolume * > & VolumeGeoCompBuilder::services() 
{
  // Return volumes defined in VolumeSplitter
  if (m_services) return *m_services;
  return m_splitter.getVolumes();
}

const std::vector<const ServiceVolume * > & VolumeGeoCompBuilder::servicesEnv() 
{
  return *m_servEnvelope;
}

const std::vector<const ServiceVolume * > & VolumeGeoCompBuilder::servicesChild() 
{
  return *m_servChild;
}

void VolumeGeoCompBuilder::buildAndPlace(const std::string & region, GeoPhysVol* /*parent*/, double zcenter) 
{


  // Get volumes defined by Volume splitter and add them on top GeoPhysVol
  setRegion(region, zcenter);
  for (unsigned int iElement = 0; iElement < services().size(); ++iElement) 
    if(!isEnvelopeOrChild(iElement))
    {

      msg(MSG::DEBUG)<<"VolumeGeoCompBuilder :: BuildAndPlace "<<services()[iElement]->volName()<<endreq;

      //      InDet::GeoComponentCnv cmpConverter = InDet::GeoComponentCnv();
      //      GeoVPhysVol* physVol = build(iElement);

//       if (physVol) {

// 	InDet::GeoSimpleObject *objBase= cmpConverter.convertIntoGeoComponent(physVol);

// 	for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) { 
// 	  //	  parent->add(getPlacement(iElement,iCopy));
// 	  //	  parent->add(physVol);

// 	  InDet::GeoSimpleObject *obj= objBase->place(getPlacement(iElement,iCopy)->getTransform() ,0);
// 	  m_svcObjects.push_back(obj);
// 	}
	
// 	objBase->UnRef();
// 	delete objBase;
//      }
    }  

}

  
void VolumeGeoCompBuilder::buildAndPlace(const std::string& region, GeoFullPhysVol* parent, double zcenter) 
{

  // Get volumes defined by Volume splitter and add them on top GeoPhysVol
  setRegion(region, zcenter);

  for (unsigned int iElement = 0; iElement < services().size(); ++iElement) 
    if(!isEnvelopeOrChild(iElement))
      {
	
	msg(MSG::DEBUG)<<"VolumeGeoCompBuilder :: BuildAndPlace "<<services()[iElement]->volName()<<endreq;

	//	InDet::GeoComponentCnv cmpConverter = InDet::GeoComponentCnv();
	//	GeoVPhysVol* physVol = build(iElement);

// 	if (physVol) 
// 	  {
// 	    InDet::GeoSimpleObject *objBase= cmpConverter.convertIntoGeoComponent(physVol);
	    
// 	    for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) { 
// 	      //	  parent->add(getPlacement(iElement,iCopy));
// 	      //	  parent->add(physVol);
	      
// 	      InDet::GeoSimpleObject *obj= objBase->place(getPlacement(iElement,iCopy)->getTransform() ,0);
// 	      m_svcObjects.push_back(obj);

// 	    }

// 	    objBase->UnRef();
// 	    delete objBase;
// 	  }
      }  


// if region is not Pixel -> stop here
  if(region.compare("Pixel")!=0) return;

  for (unsigned int iElement = 0; iElement < services().size(); ++iElement) 
    if(getEnvelopeNum(iElement)>0&&services()[iElement]->envelopeParent()==0)
      {

	buildAndPlaceEnvelope(region, parent, -1, iElement, zcenter); 
      }
}



void VolumeGeoCompBuilder::buildAndPlaceEnvelope(const std::string& region, GeoPhysVol* /*parent*/, int /*iParent*/, int iElement, double zcenter)
{

  msg(MSG::DEBUG)<<"VolumeGeoCompBuilder:: BuildAndPlace envelope "<<services()[iElement]->volName()<<endreq;

  GeoPhysVol* physVol = dynamic_cast<GeoPhysVol*>(build(iElement));

  if (physVol) 
    {
      for (unsigned int iChild = 0; iChild < services().size(); ++iChild) 
	{
	  if(isChildService(iElement,iChild)&&services()[iChild]->envelopeNum()>0)
	    {
	      msg(MSG::DEBUG)<<"***************** Build sub envelope "<<endreq;
	      // if volume is a child volume : build and place it
	      buildAndPlaceEnvelope(region, physVol, iElement, iChild, zcenter);
	      msg(MSG::DEBUG)<<"***************** Build sub envelope - done "<< physVol->getNChildVols()<<endreq;
// 	      int nbChildren=physVol->getNChildVols();
// 	      for(int iChild=0; iChild<nbChildren; iChild++)
// 		{
// 		  const GeoVPhysVol* vol2 = &(*(physVol->getChildVol(iChild)));
// 		  std::cout<<"    - child "<<iChild<<" "<<vol2->getNChildVols()<<std::endl;
// 		}
	    }
	}

      for (unsigned int iChild = 0; iChild < services().size(); ++iChild) 
	{
	  if(isChildService(iElement,iChild)&&services()[iChild]->envelopeNum()==0)
	    {
	      // if volume is not a child volume
	      GeoVPhysVol* physVol_child = build(iChild);
	      if (physVol_child) {
		for (int iCopy2 = 0; iCopy2 < numCopies(iChild); ++iCopy2) 
		  { 
		    physVol->add(getPlacementEnvelope(iChild,iCopy2,iElement));
		    physVol->add(physVol_child);
		  }
	      }
	    }
	}

      
      msg(MSG::DEBUG)<<"BuildAndPlace envelope - conversion debut"<<services()[iElement]->volName()<<endreq;

      //      InDet::GeoComponentCnv cmpConverter = InDet::GeoComponentCnv();
      //      InDet::GeoSimpleObject *objBase= 0;
      //      if(iParent<0) objBase = cmpConverter.convertIntoGeoComponent(physVol);

      msg(MSG::DEBUG)<<"BuildAndPlace envelope - conversion fin"<<services()[iElement]->volName()<<endreq;

      for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) 
	{ 
	  // add all the copies
// 	  if(iParent<0)parent->add(getPlacement(iElement,iCopy));
// 	  else parent->add(getPlacementEnvelope(iElement,iCopy,iParent));
// 	  parent->add(physVol);

	  msg(MSG::DEBUG)<<"BuildAndPlace envelope "<<services()[iElement]->volName()<<"   copie "<<iCopy<<endreq;

// 	  InDet::GeoSimpleObject *obj= 0;
// 	  if(iParent<0){
// 	    obj = objBase->place(getPlacement(iElement,iCopy)->getTransform(),0);
// 	    m_svcObjects.push_back(obj);
// 	  }
// 	  else{
// 	    parent->add(getPlacementEnvelope(iElement,iCopy,iParent));
// 	    parent->add(physVol);
// 	  }
	}

//       if(objBase){
// 	objBase->UnRef();
// 	delete objBase;
//       }
    }

}


void VolumeGeoCompBuilder::buildAndPlaceEnvelope(const std::string& region, GeoFullPhysVol* /*parent*/, int /*iParent*/, int iElement, double zcenter)
{

  msg(MSG::DEBUG)<<"VolumeGeoCompBuilder:: BuildAndPlace envelope (GeoFullPhysVol) "<<services()[iElement]->volName()<<endreq;

  GeoPhysVol* physVol = dynamic_cast<GeoPhysVol*>(build(iElement));

  if (physVol) 
    {
      for (unsigned int iChild = 0; iChild < services().size(); ++iChild) 
	{
	  if(isChildService(iElement,iChild)&&services()[iChild]->envelopeNum()>0)
	    {
	      msg(MSG::DEBUG)<<"***************** Build sub envelope "<<endreq;
	      // if volume is a child volume : build and place it
	      buildAndPlaceEnvelope(region, physVol, iElement, iChild, zcenter);
	      msg(MSG::DEBUG)<<"***************** Build sub envelope - done "<< physVol->getNChildVols()<<endreq;
	      int nbChildren=physVol->getNChildVols();
	      for(int iChild=0; iChild<nbChildren; iChild++)
		{
		  const GeoVPhysVol* vol2 = &(*(physVol->getChildVol(iChild)));
		  msg(MSG::DEBUG)<<"    - child "<<iChild<<" "<<vol2->getNChildVols()<<endreq;
		}
	    }
	}

      for (unsigned int iChild = 0; iChild < services().size(); ++iChild) 
	{
	  if(isChildService(iElement,iChild)&&services()[iChild]->envelopeNum()==0)
	    {
	      // if volume is not a child volume
	      GeoVPhysVol* physVol_child = build(iChild);
	      if (physVol_child) {
		for (int iCopy2 = 0; iCopy2 < numCopies(iChild); ++iCopy2) 
		  { 
		    physVol->add(getPlacementEnvelope(iChild,iCopy2,iElement));
		    physVol->add(physVol_child);
		  }
	      }
	    }
	}

      
      msg(MSG::DEBUG)<<"BuildAndPlace envelope - conversion debut"<<services()[iElement]->volName()<<endreq;

      //      InDet::GeoComponentCnv cmpConverter = InDet::GeoComponentCnv();
      //      InDet::GeoSimpleObject *objBase= 0;
      //      if(iParent<0) objBase = cmpConverter.convertIntoGeoComponent(physVol);

      msg(MSG::DEBUG)<<"BuildAndPlace envelope - conversion fin"<<services()[iElement]->volName()<<endreq;

      for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) 
	{ 
	  // add all the copies
// 	  if(iParent<0)parent->add(getPlacement(iElement,iCopy));
// 	  else parent->add(getPlacementEnvelope(iElement,iCopy,iParent));
// 	  parent->add(physVol);

	  msg(MSG::DEBUG)<<"BuildAndPlace envelope "<<services()[iElement]->volName()<<"   copie "<<iCopy<<endreq;

// 	  InDet::GeoSimpleObject *obj= 0;
// 	  if(iParent<0){
// 	    obj = objBase->place(getPlacement(iElement,iCopy)->getTransform(),0);
// 	    m_svcObjects.push_back(obj);
// 	  }
// 	  else{
// 	    parent->add(getPlacementEnvelope(iElement,iCopy,iParent));
// 	    parent->add(physVol);
// 	  }
	}

//       if(objBase){
// 	objBase->UnRef();
// 	delete objBase;
//       }
    }

}

GeoVPhysVol*  VolumeGeoCompBuilder::build(int iElement) 
{

  if (m_region == "None") 
    {
      msg(MSG::ERROR) << "No region set. Cannot build services" << endreq;
      return 0;
    }

  const ServiceVolume & param = *(services()[iElement]);

  // If the subelement does not belong to the current region return 0.
  if (param.region() != m_region) return 0;
  
  const GeoShape * serviceShape = param.getShape();
  double volume = param.origVolume();
  
  std::string logName = param.fullLabel();

  const GeoMaterial* serviceMat = param.material();
  std::string materialName;
  if (!serviceMat) 
    {
      materialName = param.materialName();
      if (m_matManager) 
	{
	  //serviceMat = m_matManager->getMaterialForVolume(materialName,volume/param.fractionInRegion());
	  // FIXME
	  serviceMat = m_matManager->getMaterialForVolume(materialName,volume);
	} 
      else {
	msg(MSG::ERROR) << "Material manager not available. Cannot build material."  << endreq;  
	return 0;
      }  
    } else {
    materialName = serviceMat->getName();
  }

  if (msgLvl(MSG::DEBUG)) {
    std::cout << "Volume/material: " << logName << "/";
    std::cout << materialName << std::endl;
    if (!param.shapeType().empty()) std::cout << " shape: " << param.shapeType() << std::endl;
    std::cout << " volume (CLHEP::cm3): " << volume/CLHEP::cm3 << std::endl;
    std::cout << " rmin,rmax,zmin,zmax: "
	      << param.rmin() << ", "
	      << param.rmax() << ", "
	      << param.zmin() << ", "
	      << param.zmax() << std::endl;
  }
  
  // Or use volume of original volume in param.
  //const GeoMaterial* serviceMat = mat_mgr->getMaterialForVolume(param.material(),param.origVolume());
  GeoLogVol* serviceLog = new GeoLogVol(logName,serviceShape,serviceMat);
  GeoPhysVol* servicePhys = new GeoPhysVol(serviceLog);


  return servicePhys;
}


bool VolumeGeoCompBuilder::isEnvelopeOrChild(int iElement){

  const ServiceVolume& param = *(services()[iElement]);
  if(param.envelopeNum()==0&&param.envelopeParent()==0)return false;
  return true;
}

int  VolumeGeoCompBuilder::getEnvelopeNum(int iElement){
  const ServiceVolume& param = *(services()[iElement]);
  return param.envelopeNum();
}

int  VolumeGeoCompBuilder::getParentNum(int iElement){
  const ServiceVolume& param = *(services()[iElement]);
  return param.envelopeParent();
}

double VolumeGeoCompBuilder::getZcenter(int iElement){
  const ServiceVolume& param = *(services()[iElement]);
  return (param.zmin()+param.zmax())*0.5;
}

bool VolumeGeoCompBuilder::isChildService(int iElt,int iChld)
{
  const ServiceVolume& param1 = *(services()[iElt]);
  const ServiceVolume& param2 = *(services()[iChld]);
  
  if(iElt==iChld||param1.envelopeNum()!=param2.envelopeParent())return false;

  if(param1.zsymm()==1)
    {
      double zmin=(param1.zmin()*param2.zmin());
      double zmax=(param1.zmax()*param2.zmax());
      
      if(zmin>0&&zmax>0) return true;
      return false;
    }

//   double zmin=(param1.zmin()*param2.zmin());
//   double zmax=(param1.zmax()*param2.zmax());
  
//   if(zmin>0&&zmax>0) return true;
  return true;

}

int VolumeGeoCompBuilder::numCopies(int iElement)
{
  return services()[iElement]->nCopies();
}


GeoTransform *  VolumeGeoCompBuilder::getPlacement(int iElement, int iCopy, bool forceRotation)
{
  const ServiceVolume & param = *(services()[iElement]);

  // NB. Corrected for placement in endcaps
  double zpos = param.zposition() - m_zcenter;

  // Check if we need to rotate around Y axis.
  bool rotateAroundY = false;
  if (param.needsRotation()||forceRotation) { // zpos will always be negative in this case
    zpos = -zpos;
    rotateAroundY = true;
  }
  
  // Most are just translated in z
  HepGeom::Transform3D xform = HepGeom::TranslateZ3D(zpos);
  
  double phiStart = 0;
  
  // BOX, ROD and TRAP need special treatment.
  const std::string & shapeType = param.shapeType();
  if (shapeType == "TRAP" || shapeType == "TRAP2") 
    {
      // Need to rotate by -90 deg.
      xform = HepGeom::RotateZ3D(-90.*CLHEP::deg) * xform;
    }
  if ( shapeType == "TRAP2") 
    {
      // Need to rotate by -90 deg.
      //      xform = HepGeom::RotateZ3D(-90.*CLHEP::deg) * HepGeom::RotateY3D(-90.*CLHEP::deg) * xform;

      xform =  HepGeom::RotateZ3D(-90.*CLHEP::deg) * xform;  // * HepGeom::RotateX3D(-90.*CLHEP::deg);
    }
  if (shapeType == "BOX" || shapeType == "TRAP" || shapeType == "TRAP2") 
    {
      double radius = 0.5*(param.rmin() + param.rmax());
      xform = HepGeom::TranslateX3D(radius) * xform;
      phiStart = param.phiLoc(); 
    } 
  else if (shapeType == "ROD" || shapeType == "ROD2") 
    {
      double radius = param.rmin();
      xform = HepGeom::TranslateX3D(radius) * xform;
      phiStart = param.phiLoc(); 
    }
  
  // For volumes that are placed more than once.
  double deltaPhi = 0;
  if (iCopy > 0) {
    deltaPhi = 2.*M_PI/param.nCopies();
  }
  
  double phi =  phiStart + deltaPhi * iCopy;
  if (phi) {
    xform = HepGeom::RotateZ3D(phi) * xform;
  }
  
  // For shapes that are not symmetric about a rotation around Y axis. We need to rotate.
  if (rotateAroundY) {  
    xform = HepGeom::RotateY3D(180.*CLHEP::degree) * xform;
  }
  
  return new GeoTransform(xform);
}


GeoTransform *  VolumeGeoCompBuilder::getPlacementEnvelope(int iElement, int iCopy, int iEnvElement)
{
  const ServiceVolume & param = *(services()[iElement]);

  const ServiceVolume& paramEnv = *(services()[iEnvElement]);
  double zCenter=(paramEnv.zmin()+paramEnv.zmax())*0.5;
  double rCenter=0.;

  bool bMoveToCenter=false;
  if(paramEnv.shapeType()=="BOX")bMoveToCenter=true;
  if(paramEnv.shapeType()=="TUBE"&&paramEnv.zsymm()==1&&fabs(paramEnv.zmin())>0.01)bMoveToCenter=true;
  //  if(paramEnv.shapeType()=="PGON31")bMoveToCenter=true;
  if(bMoveToCenter)rCenter=(paramEnv.rmin()+paramEnv.rmax())*0.5;

  // NB. Corrected for placement in endcaps
  double zpos = param.zposition() - zCenter;
  //  double rpos = param.rposition() - rCenter;

  // Check if we need to rotate around Y axis.
   bool rotateAroundY = false;
  if (param.needsRotation()) { // zpos will always be negative in this case
    zpos = -zpos;
    rotateAroundY = true;
  }

  // Most are just translated in z
  HepGeom::Transform3D xform = HepGeom::TranslateZ3D(zpos);

  const std::string & shapeType = param.shapeType();
  //  if (shapeType == "BOX") 
  //    xform = HepGeom::TranslateY3D(rpos) *HepGeom::TranslateZ3D(zpos);
  
  double phiStart = 0;
  
  // BOX, ROD and TRAP need special treatment.
  //  const std::string & shapeType = param.shapeType();
  if (shapeType == "TRAP" ) 
    {
      // Need to rotate by -90 deg.
      xform = HepGeom::RotateZ3D(-90.*CLHEP::deg) * xform;
    }
  if ( shapeType == "TRAP2") 
    {
      // Need to rotate by -90 deg.
      //      xform = HepGeom::RotateZ3D(-90.*CLHEP::deg) * HepGeom::RotateY3D(-90.*CLHEP::deg) * xform;

      xform = HepGeom::RotateX3D(-90.*CLHEP::deg) * xform;
    }
  if (shapeType == "BOX" || shapeType == "TRAP"|| shapeType == "TRAP2") 
    {
      double radius = 0.5*(param.rmin() + param.rmax()) - rCenter;
      xform = HepGeom::TranslateX3D(radius) * xform;
      phiStart = param.phiLoc(); 
    } 
  else if (shapeType == "ROD" || shapeType == "ROD2") 
    {
      double radius = param.rmin();
      xform = HepGeom::TranslateX3D(radius) * xform;
      phiStart = param.phiLoc(); 
    }
  
  // For volumes that are placed more than once.
  double deltaPhi = 0;
  if (iCopy > 0) {
    deltaPhi = 2.*M_PI/param.nCopies();
  }
  
  double phi =  phiStart + deltaPhi * iCopy;
  if (phi) {
    xform = HepGeom::RotateZ3D(phi) * xform;
  }
  
  // For shapes that are not symmetric about a rotation around Y axis. We need to rotate.
  if (rotateAroundY) {  
    xform = HepGeom::RotateY3D(180.*CLHEP::degree) * xform;
  }
  
  return new GeoTransform(xform);
}

} // end namespace


