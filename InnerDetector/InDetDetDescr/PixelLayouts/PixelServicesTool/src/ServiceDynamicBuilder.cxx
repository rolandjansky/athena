/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelServicesTool/ServiceDynamicBuilder.h"
#include "PixelSCTDBHelper/GeoPixelGeneralDBHelper.h"
#include "PixelSCTDBHelper/GeoPixelServicesDBHelper.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "PixelServicesTool/ServicesDynTracker.h"
#include "PixelServicesTool/ServicesTrackerBuilder.h"

/*#include "PixelServicesTool/VolumeGeoCompBuilder.h"*/
#include "PixelServicesTool/VolumeStandardBuilder.h"
#include "PixelServicesTool/SimpleServiceVolumeMaker.h"

#include "InDetGeoModelUtils/ServiceVolume.h"
#include "InDetGeoModelUtils/VolumeSplitterUtils.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "PixelServicesTool/ServiceDynVolume.h"

#include "GeoModelKernel/GeoMaterial.h"

#include "PathResolver/PathResolver.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

ServiceDynamicBuilder::ServiceDynamicBuilder(const PixelGeoBuilderBasics* basics, InDetDD::Zone * pixZone, bool bSvcDynAuto, bool bBarrelModuleMaterial) 
  : PixelGeoBuilder(basics),
    m_volStdBuilder(0),
    m_services(0)
{
  
  msg(MSG::INFO)<<"ServiceDynamicBuilder::ServiceDynamicBuilder "<<bSvcDynAuto<<" "<<bBarrelModuleMaterial<<endreq;

  // Collect the layer&disc information from geometry
  //     and build the corresponding LayerContainers in ServiceDynTracker
  ServicesTrackerBuilder trackerBuilder;
  ServicesDynTracker* tracker = trackerBuilder.buildGeometry(basics,bSvcDynAuto,bBarrelModuleMaterial);
  
  if(tracker==0) return;

  // Compute the services for each layer
  tracker->computeServicesPerLayer();

  // Collect material names
  std::map<std::string,std::string> svcMap = tracker->getSvcMaterialNameTable();      
  m_svcMaterialNameTable.insert(svcMap.begin(), svcMap.end());

//   std::cout<<">> svcMaterialTable "<<endreq;
//   std::map<std::string,std::string>::iterator it;
//   for (it = m_svcMaterialNameTable.begin(); it != m_svcMaterialNameTable.end(); ++it)
//     std::cout<<"- svcMaterialTable : "<<it->first<<" "<<it->second<<endreq;

  // Add the defined services to the geometry
  typedef  std::vector<ServiceDynVolume*>::const_iterator iter;
  for (iter i=tracker->serviceVolumes().begin(); i!=tracker->serviceVolumes().end(); i++) 
    {
      addServiceDynVolume( **i);
    }

  delete tracker;

  // Envelope interface
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML");

  msg(MSG::INFO) <<" ************************** ServiceDynamicBuilder built - BEGIN  - pixServBuilder ****************************"<<endreq;


  InDetDD::VolumeStandardBuilder* volStdBuilder = new InDetDD::VolumeStandardBuilder(*pixZone, m_services,1);
  volStdBuilder->setMaterialManager(matMgr());

  m_volStdBuilder.push_back(volStdBuilder);

  msg(MSG::INFO)<<" ************************** ServiceDynamicBuilder built - END  - svcBuilderTool ****************************"<<endreq;


}

ServiceDynamicBuilder::~ServiceDynamicBuilder() 
{
  for (unsigned int i = 0; i < m_services.size(); ++i) {
    delete  m_services[i];
  }

  for(std::vector<InDetDD::VolumeStandardBuilder*>::iterator it=m_volStdBuilder.begin(); it!=m_volStdBuilder.end(); ++it)
    delete (*it);
}

void ServiceDynamicBuilder::addTopZone( InDetDD::Zone * pixZone)
{
  InDetDD::VolumeStandardBuilder* volStdBuilder = new InDetDD::VolumeStandardBuilder(*pixZone, m_services,m_volStdBuilder.size()+1);
  volStdBuilder->setMaterialManager(matMgr());

  m_volStdBuilder.push_back(volStdBuilder);
}



void ServiceDynamicBuilder::addServiceDynVolume( const ServiceDynVolume& vol) 
{

  Athena::MsgStreamMember msgRouting(Athena::Options::Eager,"RoutingDyn - SvcDynBuilder");

  msgRouting << MSG::INFO << "Entering InDetServMatBuilderToolSLHC::addServiceDynVolume for volume " << vol.name() 
	     << " with " << vol.materials().size() << " materials" << endreq;

  InDetDD::ServiceVolume * param = new InDetDD::ServiceVolume;
  //  std::unique_ptr<InDetDD::ServiceVolume>  param( new InDetDD::ServiceVolume); 
  std::vector<std::string> linearComponents;
  std::vector<double>      linWeights;

  if(vol.length()<0.001) {
    msg(MSG::WARNING)<<"\nMALFORMED SERVICEVOLUME : "<<vol.name()<<" "<<vol.zMin()<<"  "<<vol.zMax()<<endreq;
    return;
  }

  param->setVolName(vol.name());
  param->setRmin(vol.rMin());
  param->setRmax(vol.rMax());
  param->setZmin(vol.zMin());
  param->setZmax(vol.zMax());
  param->setZsymm(true);

  for (std::vector<ServiceDynMaterial>::const_iterator ism=vol.materials().begin(); ism!=vol.materials().end(); ++ism) {
    for ( ServiceDynMaterial::EntryIter ient= ism->components().begin(); ient!=ism->components().end(); ient++) {

      msgRouting<<MSG::DEBUG << "Inside components loop, comp = " << ient->name 
		     << " number " << ient->number 
		     << " weight " << ient->weight 
		     << " linear " << ient->linear 
		     << endreq;

      //      if (ient->weight*ient->number > 0.00001) {
      //std::string pre = "pix::";
      std::string prefix = "indet::";
      std::string prename = addPrefix( prefix, ient->name);

      if (ient->linear) {
	std::vector<std::string>::iterator it=std::find(linearComponents.begin(), linearComponents.end(), prename);
	if(it!=linearComponents.end()){
	  int index = std::distance(linearComponents.begin(),it);
	  linWeights[index] += fabs( ient->number * ient->weight);
	}
	else{
	  linearComponents.push_back( prename);
	  linWeights.push_back( fabs( ient->number * ient->weight));
	}
      }
      else {
	linearComponents.push_back( prename);      // the distiction between linear and not is done in the
	linWeights.push_back( fabs( ient->weight*ient->number));  // InDetMaterialmanager, based on the weight table flag
      }
    }
  }

  if ( !linearComponents.empty()) {

    for(int i=0; i<(int)linearComponents.size(); i++)
      msgRouting<<"* "<<linearComponents[i]<<" "<<linWeights[i]<<endreq;

    msgRouting<<MSG::INFO << "build material for volume " << vol.name() <<"  shape volume : "<<param->volume()/(CLHEP::cm3)<<" [cm3]   service length : "<<vol.length()/(CLHEP::mm)<<" [mm]"<<endreq;
    const GeoMaterial * newMat = matMgr()->getMaterialForVolumeLength( vol.name(), 
								       linearComponents, linWeights, 
								       param->volume(), vol.length());

    
    msgRouting<<MSG::INFO << "  => final material    " << newMat->getName()<<"   density : "<<newMat->getDensity()/(CLHEP::g/CLHEP::cm3)<<" g/cm3     X0 : "<<newMat->getRadLength()/CLHEP::mm<<"mm"<<endreq;
    msgRouting<<MSG::DEBUG << "  dataMat ("<<(vol.zMin()+vol.zMax())*.5<<","<<(vol.rMin()+vol.rMax())*.5<<","<<newMat->getRadLength()/CLHEP::mm<<"),"<<endreq;

    param->setMaterial(newMat);
    addService(param);
    param->print();
    printNewVolume( vol, *newMat, *param);
    //    addService(param.release()); 
  }
  else delete param;
}


std::string ServiceDynamicBuilder::addPrefix( const std::string& prefix, const std::string& name) const
{
  // only add prefix to names which don't have one
  if ( name.find("::") == std::string::npos) return prefix+name;
  else return name;
}

void ServiceDynamicBuilder::addService(InDetDD::ServiceVolume * param)
{
  int count = m_services.size() + 1;
  param->setLabel("ISM",count);
  msg(MSG::INFO)<<"Add service : "<<param->volName()<<" "<<param->region()<<endreq;
  m_services.push_back(param);
}

/// Add tube like service after creating material
void ServiceDynamicBuilder::addService(const std::string & name, const GeoMaterial * material, 
					     double rmin, double rmax, double zmin, double zmax, bool zsymm)
{
  InDetDD::ServiceVolume * param = new InDetDD::ServiceVolume;
  param->setVolName(name);
  param->setMaterial(material);
  param->setRmin(rmin);
  param->setRmax(rmax);
  param->setZmin(zmin);
  param->setZmax(zmax);
  param->setZsymm(zsymm);
  addService(param);
}

/// Add tube-like service with predefined material.
void ServiceDynamicBuilder::addService(const std::string & name, const std::string & materialName, 
					     double rmin, double rmax, double zmin, double zmax, bool zsymm)
{
  InDetDD::ServiceVolume * param = new InDetDD::ServiceVolume;
  param->setVolName(name);
  param->setMaterial(materialName);
  param->setRmin(rmin);
  param->setRmax(rmax);
  param->setZmin(zmin);
  param->setZmax(zmax);
  param->setZsymm(zsymm);
  addService(param);
}

void ServiceDynamicBuilder::printNewVolume( const ServiceDynVolume& vol, 
						  const GeoMaterial& mat,
						  const InDetDD::ServiceVolume& param) const
{
  double dens = mat.getDensity();
  double weight = dens*param.volume();

  if (msgLvl(MSG::INFO)) {
   msg(MSG::INFO) << "---> name " << vol.name() << " density " << dens * CLHEP::cm3 / CLHEP::g 
		   << " [g/cm3] weight " << dens*param.volume()/CLHEP::kg  << " [kg]" << endreq;
    msg(MSG::INFO) << "Creating service volume with rmin " << vol.rMin()
		   << " rmax " << vol.rMax() 
		   << " zmin " << vol.zMin() 
		   << " zmax " << vol.zMax() << endreq;
 
   } 

  if (msgLvl(MSG::DEBUG)) {   // FIXME: change to VERBOSE when done!
    msg(MSG::DEBUG) << "Number of elements: " << mat.getNumElements() << endreq;
    for (unsigned int i=0; i< mat.getNumElements(); i++) {
      msg(MSG::DEBUG) << "Element " << mat.getElement(i)->getName() 
		      << " weight " << mat.getFraction(i) * weight / CLHEP::g << endreq;
    }
  }
}
