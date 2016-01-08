/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelServicesTool/VolumeStandardBuilder.h"

#include "InDetGeoModelUtils/ServiceVolume.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"

#include "CLHEP/Geometry/Transform3D.h"

//#include <sstream>
//#include <iomanip>
namespace InDetDD {

 VolumeStandardBuilder::VolumeStandardBuilder(const Zone & zone, const std::vector<const ServiceVolume * > & services, int volIndex) 
  : m_msg("InDetDDVolumeStandardBuilder"),
    m_region("None"), // Empty refers to a valid region. Set some default so we can check it is actually set.
    m_zcenter(0),
    m_volumeIndex(volIndex),
    m_services(0),
    m_servEnvelope(0),
    m_servChild(0),
    m_matManager(0)
{

  //  int iCmpt=0;
  //   for(Zone::ChildIterator iter = zone.begin(); iter != zone.end(); ++iter) {
  //     const Zone * child = *iter;
  //   }

  m_splitter.splitAll(zone, services);
}

VolumeStandardBuilder::VolumeStandardBuilder(const std::vector<const ServiceVolume * > & services) 
  : m_msg("InDetDDVolumeStandardBuilder"),
    m_region("None"), // Empty refers to a valid region. Set some default so we can check it is actually set.
    m_zcenter(0),
    m_services(&services),
    m_servEnvelope(0),
    m_servChild(0),


    m_matManager(0)
{}
  
VolumeStandardBuilder::VolumeStandardBuilder(const Zone & zone, const std::vector<const ServiceVolume * > & services,
			     const std::vector<const ServiceVolume * > & servEnvelope,
			     const std::vector<const ServiceVolume * > & servChild ) 
  : m_msg("InDetDDVolumeStandardBuilder"),
    m_region("None"), // Empty refers to a valid region. Set some default so we can check it is actually set.
    m_zcenter(0),
    m_services(0),
    m_servEnvelope(&servEnvelope),
    m_servChild(&servChild),
    m_matManager(0)
{
  m_splitter.splitAll(zone, services);
}
  
void VolumeStandardBuilder::addServices(const Zone & zone, const std::vector<const ServiceVolume * > & services)
{
  m_splitter.splitAll(zone, services);
}
  
void VolumeStandardBuilder::setRegion(const std::string & region, double zcenter) 
{
  m_region = region;
  m_zcenter = zcenter;
}

const std::vector<const ServiceVolume * > & VolumeStandardBuilder::services() 
{
  // Return volumes defined in VolumeSplitter
  if (m_services) return *m_services;
  return m_splitter.getVolumes();
}

const std::vector<const ServiceVolume * > & VolumeStandardBuilder::servicesEnv() 
{
  return *m_servEnvelope;
}

const std::vector<const ServiceVolume * > & VolumeStandardBuilder::servicesChild() 
{
  return *m_servChild;
}

void VolumeStandardBuilder::buildAndPlace(const std::string & region, GeoPhysVol * parent, double zcenter, std::vector<std::string> svcList)
{

  // Get volumes defined by Volume splitter and add them on top GeoPhysVol
  setRegion(region, zcenter);
  for (unsigned int iElement = 0; iElement < services().size(); ++iElement) 
    if(!isEnvelopeOrChild(iElement))
    {
	const InDetDD::ServiceVolume* svc = services()[iElement];
	std::string svcName = svc->volName();

	bool bSelect = true;
	if(svcList.size()>0){
	  for (int j=0; j<(int)svcList.size(); j++){
	    std::string tmp = svcList[j];
	    std::string prefix = tmp.substr(0,1);
	    std::string pattern = tmp.substr(1,tmp.size()-1);

	    std::size_t found=svcName.find(pattern);
	    if(prefix=="-"&&found!=std::string::npos) bSelect=false;
	    if(prefix=="+"&&found==std::string::npos) bSelect=false;
	  }
	}

	if(bSelect){
	  GeoVPhysVol* physVol = build(iElement);
	  if (physVol) {
	    for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) { 
	      parent->add(getPlacement(iElement,iCopy));
	      parent->add(physVol);
	    }
	  }
	}
    }  

}

  
void VolumeStandardBuilder::buildAndPlace(const std::string& region, GeoFullPhysVol* parent, double zcenter, std::vector<std::string> svcList)
{

  // Get volumes defined by Volume splitter and add them on top GeoPhysVol
  setRegion(region, zcenter);
  for (unsigned int iElement = 0; iElement < services().size(); ++iElement) 
    if(!isEnvelopeOrChild(iElement))
      {

	const InDetDD::ServiceVolume* svc = services()[iElement];
	std::string svcName = svc->volName();

	bool bSelect = true;
	if(svcList.size()>0){
	  for (int j=0; j<(int)svcList.size(); j++){
	    std::string tmp = svcList[j];
	    std::string prefix = tmp.substr(0,1);
	    std::string pattern = tmp.substr(1,tmp.size()-1);

	    std::size_t found=svcName.find(pattern);
	    if(prefix=="-"&&found!=std::string::npos) bSelect=false;
	    if(prefix=="+"&&found==std::string::npos) bSelect=false;
	  }
	}

	if(bSelect){
	  //    GeoVPhysVol* physVol = build(iElement);
	  GeoVPhysVol* physVol = build(iElement);
	  if (physVol) 
	    {
	      for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) 
		{ 
		  parent->add(getPlacement(iElement,iCopy));
		  parent->add(physVol);
		}
	    }
	}
      }  



// if region is not Pixel -> stop here
  if(region.compare("Pixel")!=0) return;

  for (unsigned int iElement = 0; iElement < services().size(); ++iElement) 
    if(getEnvelopeNum(iElement)>0&&services()[iElement]->envelopeParent()==0)
      {

	buildAndPlaceEnvelope(region, parent, -1, iElement, zcenter); 
      }
}


void VolumeStandardBuilder::buildAndPlaceEnvelope(const std::string& region, GeoFullPhysVol* parent, int iParent, int iElement, double zcenter)
{

  GeoPhysVol* physVol = dynamic_cast<GeoPhysVol*>(build(iElement));
  
  if (physVol) 
    {
      for (unsigned int iChild = 0; iChild < services().size(); ++iChild) 
	{
	  if(isChildService(iElement,iChild)&&services()[iChild]->envelopeNum()>0)
	    {
	      // if volume is a child volume : build and place it
	      buildAndPlaceEnvelope(region, physVol, iElement, iChild, zcenter);
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

      for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) 
	{ 
	  // add all the copies
	  if(iParent<0)parent->add(getPlacement(iElement,iCopy));
	  else parent->add(getPlacementEnvelope(iElement,iCopy,iParent));
	  parent->add(physVol);
	}
    }


} 

void VolumeStandardBuilder::buildAndPlaceEnvelope(const std::string& region, GeoPhysVol* parent, int iParent, int iElement, double zcenter)
{

  GeoPhysVol* physVol = dynamic_cast<GeoPhysVol*>(build(iElement));
  
  if (physVol) 
    {
      for (unsigned int iChild = 0; iChild < services().size(); ++iChild) 
	{
	  if(isChildService(iElement,iChild)&&services()[iChild]->envelopeNum()>0)
	    {
	      // if volume is a child volume : build and place it
	      buildAndPlaceEnvelope(region, physVol, iElement, iChild, zcenter);
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
      
      for (int iCopy = 0; iCopy < numCopies(iElement); ++iCopy) 
	{ 
	  // add all the copies
	  if(iParent<0)parent->add(getPlacement(iElement,iCopy));
	  else parent->add(getPlacementEnvelope(iElement,iCopy,iParent));
	  parent->add(physVol);
	}
    }

}

GeoVPhysVol*  VolumeStandardBuilder::build(int iElement) 
{

  if (m_region == "None") 
    {
      msg(MSG::ERROR) << "No region set. Cannot build services" << endreq;
      return 0;
    }

  const ServiceVolume & param = *(services()[iElement]);

  // If the subelement does not belong to the current region return 0.
  if (param.region() != m_region){
    return 0;
  }
  
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


bool VolumeStandardBuilder::isEnvelopeOrChild(int iElement){

  const ServiceVolume& param = *(services()[iElement]);
  if(param.envelopeNum()==0&&param.envelopeParent()==0)return false;
  return true;
}

int  VolumeStandardBuilder::getEnvelopeNum(int iElement){
  const ServiceVolume& param = *(services()[iElement]);
  return param.envelopeNum();
}

int  VolumeStandardBuilder::getParentNum(int iElement){
  const ServiceVolume& param = *(services()[iElement]);
  return param.envelopeParent();
}

double VolumeStandardBuilder::getZcenter(int iElement){
  const ServiceVolume& param = *(services()[iElement]);
  return (param.zmin()+param.zmax())*0.5;
}

bool VolumeStandardBuilder::isChildService(int iElt,int iChld)
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

int VolumeStandardBuilder::numCopies(int iElement)
{
  return services()[iElement]->nCopies();
}


GeoTransform *  VolumeStandardBuilder::getPlacement(int iElement, int iCopy)
{
  const ServiceVolume & param = *(services()[iElement]);

  // NB. Corrected for placement in endcaps
  double zpos = param.zposition() - m_zcenter;

  // Check if we need to rotate around Y axis.
  bool rotateAroundY = false;
  if (param.needsRotation()) { // zpos will always be negative in this case
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


GeoTransform *  VolumeStandardBuilder::getPlacementEnvelope(int iElement, int iCopy, int iEnvElement)
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

  double VolumeStandardBuilder::computeRmin(const std::string & /*region*/, std::vector<std::string> svcList) 
{
  double rMin = 999999.;

  for (unsigned int iElement = 0; iElement < services().size(); ++iElement) 
    {
      const InDetDD::ServiceVolume* svc = services()[iElement];
      std::string svcName = svc->volName();
      
      bool bSelect = true;
      if(svcList.size()>0){
	for (int j=0; j<(int)svcList.size(); j++){
	  std::string tmp = svcList[j];
	  std::string prefix = tmp.substr(0,1);
	  std::string pattern = tmp.substr(1,tmp.size()-1);
	  
	  std::size_t found=svcName.find(pattern);
	  if(prefix=="-"&&found!=std::string::npos) bSelect=false;
	  if(prefix=="+"&&found==std::string::npos) bSelect=false;
	}
      }
      
      if(bSelect){
	rMin=std::min(rMin,svc->rmin());
      }

    }

  return rMin;
}

  double VolumeStandardBuilder::computeRmax(const std::string & /*region*/, std::vector<std::string> svcList) 
{
  double rMax = -999999.;

  for (unsigned int iElement = 0; iElement < services().size(); ++iElement) 
    {
      const InDetDD::ServiceVolume* svc = services()[iElement];
      std::string svcName = svc->volName();
      
      bool bSelect = true;
      if(svcList.size()>0){
	for (int j=0; j<(int)svcList.size(); j++){
	  std::string tmp = svcList[j];
	  std::string prefix = tmp.substr(0,1);
	  std::string pattern = tmp.substr(1,tmp.size()-1);
	  
	  std::size_t found=svcName.find(pattern);
	  if(prefix=="-"&&found!=std::string::npos) bSelect=false;
	  if(prefix=="+"&&found==std::string::npos) bSelect=false;
	}
      }
      
      if(bSelect){
	rMax=std::max(rMax,svc->rmax());
      }

    }

  return rMax;



}


  double VolumeStandardBuilder::computeZmin(const std::string & /*region*/, std::vector<std::string> svcList) 
{
  double rMin = 999999.;

  for (unsigned int iElement = 0; iElement < services().size(); ++iElement) 
    {
      const InDetDD::ServiceVolume* svc = services()[iElement];
      std::string svcName = svc->volName();
      
      bool bSelect = true;
      if(svcList.size()>0){
	for (int j=0; j<(int)svcList.size(); j++){
	  std::string tmp = svcList[j];
	  std::string prefix = tmp.substr(0,1);
	  std::string pattern = tmp.substr(1,tmp.size()-1);
	  
	  std::size_t found=svcName.find(pattern);
	  if(prefix=="-"&&found!=std::string::npos) bSelect=false;
	  if(prefix=="+"&&found==std::string::npos) bSelect=false;
	}
      }
      
      if(bSelect){
	rMin=std::min(rMin,svc->zmin());
      }

    }

  return rMin;
}

double VolumeStandardBuilder::computeZmax(const std::string & /*region*/, std::vector<std::string> svcList) 
{
  double rMax = -999999.;

  for (unsigned int iElement = 0; iElement < services().size(); ++iElement) 
    {
      const InDetDD::ServiceVolume* svc = services()[iElement];
      std::string svcName = svc->volName();
      
      bool bSelect = true;
      if(svcList.size()>0){
	for (int j=0; j<(int)svcList.size(); j++){
	  std::string tmp = svcList[j];
	  std::string prefix = tmp.substr(0,1);
	  std::string pattern = tmp.substr(1,tmp.size()-1);
	  
	  std::size_t found=svcName.find(pattern);
	  if(prefix=="-"&&found!=std::string::npos) bSelect=false;
	  if(prefix=="+"&&found==std::string::npos) bSelect=false;
	}
      }
      
      if(bSelect){
	rMax=std::max(rMax,svc->zmax());
      }

    }

  return rMax;



}


} // end namespace


