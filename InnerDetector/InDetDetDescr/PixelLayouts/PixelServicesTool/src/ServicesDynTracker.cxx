/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelServicesTool/ServicesDynTracker.h"
#include "PixelServicesTool/PixelDynamicServiceXMLHelper.h"

#include "PixelServicesTool/ServicesDynLayer.h"
#include "PixelServicesTool/RoutingDynAuto.h"
#include "PixelServicesTool/RoutingDyn.h"

//#include "InDetServMatGeoModel/LinearService.h"

#include "PixelServicesTool/ComputeDynStaveServices.h"
#include "PixelServicesTool/ConvertDynStaveServices.h"

#include <map>
#include <iostream>

#include <iostream>  // for DEBUG only
using namespace std;

ServicesDynTracker::ServicesDynTracker(const PixelGeoBuilderBasics* basics, bool bSvcDynAuto, bool bSvcBrlModule):
  PixelGeoBuilder(basics),
  m_bSvcDynAuto(bSvcDynAuto),
  m_bSvcBrlModule(bSvcBrlModule),
  m_msg(basics->msgStream())
{
  m_barrelLayers.clear();
  m_barrelPixelLayers.clear();
  m_endcapPixelLayers.clear();
  m_barrelStripLayers.clear();
  m_endcapStripLayers.clear();
}

ServicesDynTracker::~ServicesDynTracker()
{

  for (std::vector<ServiceDynVolume *>::iterator iter = m_volumes.begin(); iter != m_volumes.end(); ++iter) delete *iter;
  
  for (LayerContainer::const_iterator iter=m_barrelLayers.begin(); iter!=m_barrelLayers.end(); ++iter) delete *iter;
  for (LayerContainer::const_iterator iter=m_endcapPixelLayers.begin(); iter!=m_endcapPixelLayers.end(); ++iter) delete *iter;
  for (LayerContainer::const_iterator iter=m_endcapStripLayers.begin(); iter!=m_endcapStripLayers.end(); ++iter) delete *iter;

}

void ServicesDynTracker::constructBarrelLayer( double radius, double zHalfLength, 
					       DetTypeDyn::Type type, int layerNum,
					       int nstaves, const std::string& suffix,
					       int nModulesPerStave, int nChipsPerModule)
{
   ServicesDynLayer* nl = new ServicesDynLayer( radius, zHalfLength, nstaves,
						type, DetTypeDyn::Barrel, layerNum, suffix,
						nModulesPerStave, nChipsPerModule);
   
   m_barrelLayers.push_back(nl);
   if (type == DetTypeDyn::Pixel) m_barrelPixelLayers.push_back(nl);
   else                        m_barrelStripLayers.push_back(nl);
}

void ServicesDynTracker::constructBarrelLayer( double radius, double zHalfLength, 
					       DetTypeDyn::Type type, int layerNum,
					       int nstaves, const std::string& suffix,
					       std::vector<int> nModulesPerStave, std::vector<int> nChipsPerModule)
{
   ServicesDynLayer* nl = new ServicesDynLayer( radius, zHalfLength, nstaves,
						type, DetTypeDyn::Barrel, layerNum, suffix,
						nModulesPerStave, nChipsPerModule);
   
   m_barrelLayers.push_back(nl);
   if (type == DetTypeDyn::Pixel) m_barrelPixelLayers.push_back(nl);
   else                        m_barrelStripLayers.push_back(nl);
}

void ServicesDynTracker::constructEndcapLayer( double zpos, double rmin, double rmax, 
					       DetTypeDyn::Type type, int layerNum,
					       int nstaves, const std::string& suffix,
					       int nModulesPerStave, int nChipsPerModule)
{
  
  PixelDynamicServiceXMLHelper svcDynHelper("PIXEL_PIXELDYNAMICSERVICE_GEO_XML", getBasics());

  if(m_bSvcDynAuto) {
    double rEosMin = svcDynHelper.EndcapEOSRMin(layerNum);  
    if(rmax<rEosMin) rmax=rEosMin;
  }

  ServicesDynLayer* nl = new ServicesDynLayer( zpos, rmin, rmax, nstaves,
					       type, (DetTypeDyn::Part)DetTypeDyn::Endcap, layerNum, 
					       suffix,
					       nModulesPerStave, nChipsPerModule);
  
  if (type == DetTypeDyn::Pixel) m_endcapPixelLayers.push_back(nl);
  else                        m_endcapStripLayers.push_back(nl);
}


void ServicesDynTracker::computeServicesPerLayer()
{
  if(m_bSvcDynAuto) {
    // Code as defined in the PixelGeoModel package // maintained for LoI geometries
    RoutingDynAuto routing(msgStream(),getBasics());
    routing.createRoutingVolumes(*this);
    finaliseServices();
  }
  else {
    // Code that read routes from XML file
    Athena::MsgStreamMember msgRouting(Athena::Options::Eager,"RoutingDyn");
    RoutingDyn routing(msgRouting,getBasics());
    routing.createRoutingVolumes(*this);
    routing.addRouteMaterial(getBasics());
    if(m_bSvcBrlModule){
      routing.computeBarrelModuleMaterial(getBasics());
      std::map<std::string,std::string> svcMap = routing.getSvcMaterialNameTable();      
      m_svcMaterialNameTable.insert(svcMap.begin(), svcMap.end());
    }
  }
}


void ServicesDynTracker::finaliseServices()
{
  msg(MSG::INFO) << "----------------------------------------------------------------------"<<endreq;
  msg(MSG::INFO) << "ServicesDynTracker::finaliseServices called for " << m_volumes.size() << " volumes" << endreq;
  
  msg(MSG::INFO) << "ComputeDynStaveServices"<<endreq;
  ComputeDynStaveServices staveComputer(msgStream());
  ConvertDynStaveServices staveConverter;
  
  std::map<const ServicesDynLayer*, ServiceDynMaterial> layerMaterial; // cache the layer services
  
  typedef  std::vector<ServiceDynVolume*>::iterator VolumeIter;
  for (VolumeIter iv=m_volumes.begin(); iv!=m_volumes.end(); iv++) {

    msg(MSG::INFO) << "-- Volume : "<<(**iv).name()<<"  add material "<<endreq;

    std::vector<ServiceDynMaterial> result; // = (**iv).materials(); // preserve already present mat. (EOS)
    if ((**iv).isEOS()) addEosMaterial(**iv, result);
    
    msg(MSG::INFO) << "-- volume :  loop over layers "<<endreq;
    ServiceDynVolume::LayerContainer layers = (**iv).layers();

    for (ServiceDynVolume::LayerContainer::const_iterator il=layers.begin(); il!=layers.end(); ++il) {

      const ServicesDynLayer& layer( **il);

      ServiceDynMaterial layerMat;
      std::map<const ServicesDynLayer*, ServiceDynMaterial>::iterator iMat = layerMaterial.find(*il);
      if (iMat !=  layerMaterial.end()) 
	layerMat = iMat->second;
      else {
	StaveDynServices sserv = staveComputer.compute( layer.type(), layer.part(), layer.number(),
							layer.modulesPerStaveFirst(), layer.chipsPerModuleFirst());
	layerMat = staveConverter.convertStaveServices( sserv);
	
	layerMat.multiply( layer.nStaves()); // scale from one stave to full layer
	
	// add cooling, with possibility for manifilding
	staveConverter.addCooling( layer.type(), layer.part(), layer.number(), layer.nStaves(), layerMat);
	
// 	if(msgLvl(MSG::DEBUG)){
// 	  msg(MSG::DEBUG) <<"Dumping services for layer " << layer.number()<< endreq;
// 	  layerMat.dump(cout);
// 	}
	layerMaterial[*il] = layerMat;
      }
      result.push_back( layerMat);
    }
    
    (**iv).setMaterials( result);
  }
}

void ServicesDynTracker::addEosMaterial( const ServiceDynVolume& vol, std::vector<ServiceDynMaterial>& result) 
{
  string name;
  if (vol.name().find("Pixel") != string::npos) {
    name = "pix::PixelEOS";
  }
  else {
    name = "pix::StripEOS";
  }
  
  const ServicesDynLayer* layer = vol.layers().front();
  
  ServiceDynMaterial::Entry eos( name, layer->nStaves(), 0);
  ServiceDynMaterial::EntryContainer entries( 1, eos);
  result.push_back( ServiceDynMaterial( name, entries));
  // msg(MSG::INFO) << "Added EOS material to volume " << vol.name() << endreq;
}

/*
From the DB:
name of cable linear material for each cable type, for each part
e.g. pixel low voltage cable -> CoaxialHVCable (Or TwistedPair_awg36 etc.)
The table holding this information must have the format
   SUBDETECOR                SERVICETYPE 
['Pixel','SCT']        ['HV','DCS','Data','LV','InletPipe','OutletPipe']

For all services except LV the material name in this table is used directly, in combination with the 
computed length of the service in the volume, to define te volume material.
For the LV, the "material name" is a switch between different conductor materials (Al, CCAW, ...), but the
actual linear material weight is computed using the information about the stave power, voltage, etc.

The powering scheme for each part is defined in another table, the structure of which is
   SUBDETECTOR       POWERING           
['Pixel','SCT']    ['Serial','DCDC']

In addition we need the following information:
For each module type: current, voltage
for each stave type: number and type of modules, module grouping for HV, LV, Data

 */
