/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/ServicesTracker.h"

#include "InDetServMatGeoModel/ServicesLayer.h"
#include "InDetServMatGeoModel/Routing2.h"
#include "InDetServMatGeoModel/ServicesStave.h"
//#include "InDetServMatGeoModel/LinearService.h"

#include "InDetServMatGeoModel/ComputeStaveServices.h"
#include "InDetServMatGeoModel/ConvertStaveServices.h"

#include <map>
#include <iostream>

#include <iostream>  // for DEBUG only
using namespace std;

ServicesTracker::ServicesTracker(const Athena::MsgStreamMember& msg):
  m_geoMgr(0),
  m_msg(msg)
{
  m_barrelLayers.clear();
  m_barrelPixelLayers.clear();
  m_endcapPixelLayers.clear();
  m_barrelStripLayers.clear();
  m_endcapStripLayers.clear();
}

ServicesTracker::~ServicesTracker()
{

  for (std::vector<ServiceVolume *>::iterator iter = m_volumes.begin(); iter != m_volumes.end(); ++iter) delete *iter;
  
  for (LayerContainer::const_iterator iter=m_barrelLayers.begin(); iter!=m_barrelLayers.end(); ++iter) delete *iter;
  for (LayerContainer::const_iterator iter=m_endcapPixelLayers.begin(); iter!=m_endcapPixelLayers.end(); ++iter) delete *iter;
  for (LayerContainer::const_iterator iter=m_endcapStripLayers.begin(); iter!=m_endcapStripLayers.end(); ++iter) delete *iter;

}

void ServicesTracker::constructBarrelLayer( double radius, double zHalfLength, 
					    DetType::Type type, int layerNum,
					    int nstaves, const std::string& suffix,
					    int nModulesPerStave, int nChipsPerModule)
{
  ServicesLayer* nl = new ServicesLayer( radius, zHalfLength, nstaves,
					 type, DetType::Barrel, layerNum, suffix,
					 nModulesPerStave, nChipsPerModule);

  m_barrelLayers.push_back(nl);
  if (type == DetType::Pixel) m_barrelPixelLayers.push_back(nl);
  else                        m_barrelStripLayers.push_back(nl);
}

void ServicesTracker::constructEndcapLayer( double zpos, double rmin, double rmax, 
					    DetType::Type type, int layerNum,
					    int nstaves, const std::string& suffix,
					    int nModulesPerStave, int nChipsPerModule)
{
  ServicesLayer* nl = new ServicesLayer( zpos, rmin, rmax, nstaves,
					 type, DetType::Endcap, layerNum,
					 suffix,
					 nModulesPerStave, nChipsPerModule);

  if (type == DetType::Pixel) m_endcapPixelLayers.push_back(nl);
  else                        m_endcapStripLayers.push_back(nl);
}

void ServicesTracker::computeServicesPerLayer()
{
  Routing2 routing(msgStream());
  routing.createRoutingVolumes(*this);
}


void ServicesTracker::finaliseServices()
{
  msg(MSG::INFO) << "ServicesTracker::finaliseServices called for " << m_volumes.size() << " volumes" << endmsg;

  ComputeStaveServices staveComputer;
  ConvertStaveServices staveConverter;

  std::map<const ServicesLayer*, ServiceMaterial> layerMaterial; // cache the layer services

  typedef  std::vector<ServiceVolume*>::iterator VolumeIter;
  for (VolumeIter iv=m_volumes.begin(); iv!=m_volumes.end(); iv++) {
    std::vector<ServiceMaterial> result; // = (**iv).materials(); // preserve already present mat. (EOS)
    if ((**iv).isEOS()) addEosMaterial(**iv, result);
    
    ServiceVolume::LayerContainer layers = (**iv).layers();
    for (ServiceVolume::LayerContainer::const_iterator il=layers.begin(); il!=layers.end(); ++il) {
      const ServicesLayer& layer( **il);
      ServiceMaterial layerMat;

      std::map<const ServicesLayer*, ServiceMaterial>::iterator iMat = layerMaterial.find(*il);
      if (iMat !=  layerMaterial.end()) layerMat = iMat->second;
      else {
	StaveServices sserv = staveComputer.compute( layer.type(), layer.part(), layer.number(),
						     layer.modulesPerStave(), layer.chipsPerModule(),
                                                     msgStream().get());
	layerMat = staveConverter.convertStaveServices( sserv);

	layerMat.multiply( layer.nStaves()); // scale from one stave to full layer

	// add cooling, with possibility for manifilding
	staveConverter.addCooling( layer.type(), layer.part(), layer.number(), layer.nStaves(), layerMat);

	if(msgLvl(MSG::DEBUG)){
	  msg(MSG::DEBUG) <<"Dumping services for layer " << layer.number()<< endmsg;
	  layerMat.dump(cout);
	}
	layerMaterial[*il] = layerMat;
      }
      result.push_back( layerMat);
    }

    (**iv).setMaterials( result);
  }
}

void ServicesTracker::addEosMaterial( const ServiceVolume& vol, std::vector<ServiceMaterial>& result) 
{
  string name;
  if (vol.name().find("Pixel") != string::npos) {
    name = "pix::PixelEOS";
  }
  else {
    name = "pix::StripEOS";
  }

  const ServicesLayer* layer = vol.layers().front();

  ServiceMaterial::Entry eos( name, layer->nStaves(), 0);
  ServiceMaterial::EntryContainer entries( 1, eos);
  result.push_back( ServiceMaterial( name, entries));
  // msg(MSG::INFO) << "Added EOS material to volume " << vol.name() << endmsg;
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
