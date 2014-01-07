/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
{}

/*
ServicesTracker::ServicesTracker( bool toy) 
{
  if (!toy) return;

  constructBarrelLayer( 5, 50, DetType::Pixel);
  constructBarrelLayer( 8, 60, DetType::Pixel);
  constructBarrelLayer( 15, 80, DetType::Pixel);
  constructBarrelLayer( 20, 80, DetType::Pixel);
  constructBarrelLayer( 35, 120, DetType::ShortStrip);
  constructBarrelLayer( 45, 120, DetType::ShortStrip);
  constructBarrelLayer( 60, 120, DetType::ShortStrip);
  constructBarrelLayer( 75, 120, DetType::LongStrip);
  constructBarrelLayer( 95, 120, DetType::LongStrip);

  constructEndcapLayer( 90, 15, 25, DetType::Pixel);
  constructEndcapLayer( 100, 15, 25, DetType::Pixel);
  constructEndcapLayer( 110, 15, 25, DetType::Pixel);
  constructEndcapLayer( 120, 15, 25, DetType::Pixel);
}
*/

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
/*
void ServicesTracker::constructBarrelLayer( double radius, double zHalfLength, DetType::Type type)
{
  using namespace std;
  // FIXME
  double stavew;
  if (type == DetType::Pixel) stavew = 4;
  else stavew = 10;
  int nstaves = int(2.*M_PI*radius / stavew);

  msg(MSG::INFO) << "layer at radius " << radius << " has " << nstaves << " staves" << endreq;

  constructBarrelLayer( radius, zHalfLength, type, 0, nstaves, "");
}
*/

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
/*
void ServicesTracker::constructEndcapLayer( double zpos, double rmin, double rmax, 
					    DetType::Type type)
{
  using namespace std;
  // FIXME
  double stavew;
  if (type == DetType::Pixel) stavew = 4;
  else stavew = 10;
  int nstaves = int(2.*M_PI*rmax / stavew);

  msg(MSG::INFO) << "endcap layer at z " << zpos << " has " << nstaves << " staves" << endreq;

  constructEndcapLayer( zpos, rmin, rmax, type, 0, nstaves, "");
}
*/

void ServicesTracker::computeServicesPerLayer()
{
  Routing2 routing(msgStream());
  routing.createRoutingVolumes(*this);
}
/*
void ServicesTracker::finaliseServices()
{
  using namespace std;

  msg(MSG::INFO) << "Entering ServicesTracker::finaliseServices()" << endreq;

  typedef  std::vector<ServiceVolume*>::iterator VolumeIter;
  for (VolumeIter iv=m_volumes.begin(); iv!=m_volumes.end(); iv++) {
    std::vector<LinearService> result = (**iv).materials(); // preserve already present mat. (EOS)
    std::map<std::string, double> res;
    ServiceVolume::LayerContainer layers = (**iv).layers();
    for (ServiceVolume::LayerContainer::const_iterator il=layers.begin(); il!=layers.end(); ++il) {
      const ServicesStave& stave = *(**il).stave();
      double totalLength =  (**il).nStaves() * (**iv).length();
      add( res, stave.inletPipe(), totalLength*stave.nInletPipes());
      add( res, stave.outletPipe(), totalLength*stave.nOutletPipes());
      add( res, stave.cable()+(**il).layerSuffix(), totalLength*stave.nCables());
      add( res, stave.fibre(), totalLength*stave.nFibres());
    }
    for (std::map<std::string, double>::iterator i = res.begin(); i!= res.end(); i++) {
      result.push_back( LinearService( i->first, i->second));
    }
    (**iv).setMaterials( result);
  }
}


void ServicesTracker::add( std::map<std::string, double>& res, const std::string& name, double len) 
{
  using namespace std;
  msg(MSG::INFO) << "Entering NEW ServicesTracker::finaliseServices()" << endreq;

  std::map<std::string, double>::iterator i=res.find(name);
  if (i != res.end()) i->second += len;
  else res[name] = len;
}
*/
void ServicesTracker::finaliseServices()
{
  msg(MSG::INFO) << "ServicesTracker::finaliseServices called for " << m_volumes.size() << " volumes" << endreq;

  ComputeStaveServices staveComputer(msgStream());
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
						     layer.modulesPerStave(), layer.chipsPerModule());
	layerMat = staveConverter.convertStaveServices( sserv);

	layerMat.multiply( layer.nStaves()); // scale from one stave to full layer

	// add cooling, with possibility for manifilding
	staveConverter.addCooling( layer.type(), layer.part(), layer.number(), layer.nStaves(), layerMat);

	if(msgLvl(MSG::DEBUG)){
	  msg(MSG::DEBUG) <<"Dumping services for layer " << layer.number()<< endreq;
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
