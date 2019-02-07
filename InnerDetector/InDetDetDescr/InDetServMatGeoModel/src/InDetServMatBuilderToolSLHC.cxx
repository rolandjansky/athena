/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatBuilderToolSLHC.h"
#include "InDetServMatGeoModel/InDetServMatGeometryManager.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "InDetGeoModelUtils/ServiceVolume.h"
#include "InDetGeoModelUtils/ServiceVolumeMaker.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "GeoModelKernel/Units.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "InDetServMatGeoModel/ServicesTracker.h"
#include "InDetServMatGeoModel/ServicesTrackerBuilder.h"
#include "InDetServMatGeoModel/ServiceVolume.h"

//================ Constructor =================================================
InDetServMatBuilderToolSLHC::InDetServMatBuilderToolSLHC(const std::string& t,
						 const std::string& name,
						 const IInterface*  p )
  : AthAlgTool(t,name,p),
    m_geoDbTagSvc("GeoDbTagSvc",name),
    m_rdbAccessSvc("RDBAccessSvc",name),
    m_geometryDBSvc("InDetGeometryDBSvc",name),
    m_init(false),
    m_athenaComps(0),
    m_geoMgr(0)
{
  declareInterface<IInDetServMatBuilderTool>(this);
 
  declareProperty("RDBAccessSvc", m_rdbAccessSvc);
  declareProperty("GeometryDBSvc", m_geometryDBSvc);
  declareProperty("GeoDbTagSvc", m_geoDbTagSvc);
}


//================ Destructor =================================================
InDetServMatBuilderToolSLHC::~InDetServMatBuilderToolSLHC()
{
  delete m_athenaComps;
  delete m_geoMgr;
  for (std::vector<const InDetDD::ServiceVolume *>::iterator iter = m_services.begin(); iter != m_services.end(); ++iter) {
    delete *iter;
  }
}


//================ Initialize =================================================
StatusCode InDetServMatBuilderToolSLHC::initialize()
{
  
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;
  
  // Get the detector configuration.
  sc = m_geoDbTagSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoDbTagSvc" << endmsg;
    return (StatusCode::FAILURE);
  }

  // Get RDBAcessSvc
  sc = m_rdbAccessSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endmsg;
    return (StatusCode::FAILURE); 
  }  
  
  // Retrieve the Geometry DB Interface
  sc = m_geometryDBSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate Geometry DB Interface: " << m_geometryDBSvc.name() << endmsg;
    return (StatusCode::FAILURE); 
  }  

  // Pass the Athena components to the geometry manager
  //m_athenaComps = new InDetDD::AthenaComps(name());
  m_athenaComps = new InDetDD::AthenaComps("InDetServMatGeoModel");
  m_athenaComps->setDetStore(&*(detStore()));
  m_athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);
  m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
  m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);

  msg(MSG::INFO) << "initialize() successful in " << name() << endmsg;
  return StatusCode::SUCCESS;
}
	
//================ Finalize =================================================
StatusCode InDetServMatBuilderToolSLHC::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}
	
const std::vector<const InDetDD::ServiceVolume *> & InDetServMatBuilderToolSLHC::getServices()
{
  if (!m_init) build();
  return m_services;
}

void InDetServMatBuilderToolSLHC::geoInit()
{
  m_geoMgr = new InDetServMatGeometryManager(m_athenaComps);
}


void InDetServMatBuilderToolSLHC::build()
{
  // Do nothing if services are not to be built.
  geoInit();
  m_init = true;
  if (!m_geoMgr->buildServices()) return;
  
  // build the geometry
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "pixelNumLayers: " << m_geoMgr->pixelNumLayers() << endmsg;
    for (int i = 0; i < m_geoMgr->pixelNumLayers(); i++) {
      msg(MSG::DEBUG) << " pixelLayerRadius(" << i << ")        : " << m_geoMgr->pixelLayerRadius(i) << endmsg;
      msg(MSG::DEBUG) << " pixelLayerLength(" << i << ")        : " << m_geoMgr->pixelLayerLength(i) << endmsg;
      msg(MSG::DEBUG) << " pixelNumSectorsForLayer(" << i << ") : " << m_geoMgr->pixelNumSectorsForLayer(i) << endmsg;
    }
    msg(MSG::DEBUG) << "pixelNumDisk: " << m_geoMgr->pixelNumDisks() << endmsg;
    for (int i = 0; i < m_geoMgr->pixelNumDisks(); i++) {
      msg(MSG::DEBUG) << " pixelDiskZ(" << i << ")    : " << m_geoMgr->pixelDiskZ(i) << endmsg;
      msg(MSG::DEBUG) << " pixelDiskRMax(" << i << ") : " << m_geoMgr->pixelDiskRMax(i) << endmsg;
      msg(MSG::DEBUG) << " pixelDiskSectors(" << i << ") : " << m_geoMgr->pixelEndcapNumSectorsForLayer(i) << endmsg;
      msg(MSG::DEBUG) << " pixelModulesPerSectors(" << i << ") : " << m_geoMgr->pixelModulesPerEndcapSector(i) << endmsg;
      msg(MSG::DEBUG) << " pixelChipsPerModuleForDisk(" << i << ") : " << m_geoMgr->pixelChipsPerModuleForDisk(i) << endmsg;
    }
    msg(MSG::DEBUG) << "sctNumLayers: " << m_geoMgr->sctNumLayers() << endmsg;
    for (int i = 0; i < m_geoMgr->sctNumLayers(); i++) {
      msg(MSG::DEBUG) << " sctLayerRadius(" << i << ")        : " << m_geoMgr->sctLayerRadius(i) << endmsg;
      msg(MSG::DEBUG) << " sctLayerLength(" << i << ")        : " << m_geoMgr->sctLayerLength(i) << endmsg;
      msg(MSG::DEBUG) << " sctLayerType(" << i << ")          : " << m_geoMgr->sctLayerType(i) << endmsg;
      msg(MSG::DEBUG) << " sctNumSectorsForLayer(" << i << ") : " << m_geoMgr->sctNumSectorsForLayer(i) << endmsg;
    }
    msg(MSG::DEBUG) << "sctNumDisk: " << m_geoMgr->sctNumDisks() << endmsg;
    for (int i = 0; i < m_geoMgr->sctNumDisks(); i++) {
      msg(MSG::DEBUG) << " sctDiskZ(" << i << ")    : " << m_geoMgr->sctDiskZ(i) << endmsg;
      msg(MSG::DEBUG) << " sctDiskRMax(" << i << ") : " << m_geoMgr->sctDiskRMax(i) << endmsg;
    }
    msg(MSG::DEBUG) << "sctInnerSupport radius: " << m_geoMgr->sctInnerSupport() << endmsg;
    msg(MSG::DEBUG) << "PST inner radius: " << m_geoMgr->SupportTubeRMin("PST") << endmsg;
    msg(MSG::DEBUG) << "Pixel Envelope radius: " << m_geoMgr->pixelEnvelopeRMax() << endmsg;
  }

  ServicesTracker* tracker = ServicesTrackerBuilder().buildGeometry( *m_geoMgr);
  // compute the services for each layer
  tracker->computeServicesPerLayer();

  tracker->finaliseServices();

  fixMissingMaterials();

  typedef  std::vector<ServiceVolume*>::const_iterator iter;
  for (iter i=tracker->serviceVolumes().begin(); i!=tracker->serviceVolumes().end(); i++) {
    /*
    msg(MSG::INFO) << "Calling ServiceVolume->dump() for the " << n++ << "th time" << endmsg;
    (**i).dump();  // for DEBUG only
    msg(MSG::INFO) << "Call to ServiceVolume->dump() returned" << endmsg;
    */
    addServiceVolume( **i);
  }

  // Add services from table (if there are any)
  InDetDD::ServiceVolumeSchema schema;
  schema.setSimpleSchema();
  InDetDD::ServiceVolumeMaker volMaker("ISM",m_geoMgr->simpleServiceTable(),schema,m_athenaComps);
  for (unsigned int i = 0; i < volMaker.numElements(); ++i) {
    addService(volMaker.make(i));
  }

  delete tracker;
  // Some test volumes
  /*
  addService("Test1", "std::Carbon", 10,   1040, 410, 420, false);
  addService("Test2", "std::Carbon", 10,   1040, 450, 455, true);
  addService("Test3", "std::Carbon", 1041, 1048, 455, 3000, true);
  addService("Test4", "std::Carbon", 1120, 1122, -4000, 4000, false);
  addService("Test5", "std::Copper", 10,   1200, -1001, -1000, false);
  addService("Test6", "std::Copper", 121,   139, -3120,  3120, false);
  addService("Test7", "std::Copper", 10,   1200, 1200, 1201, true);
  addService("Test8", "std::Copper", 10,   1040, 2000,  2005, true);
  addService("Test9", "std::Copper", 800,   802, -3120,  3120, false);
  */
}


/// General service adding method.
void InDetServMatBuilderToolSLHC::addService(InDetDD::ServiceVolume * param)
{
  int count = m_services.size() + 1;
  param->setLabel("ISM",count);
  m_services.push_back(param);
}

void InDetServMatBuilderToolSLHC::addServiceVolume( const ServiceVolume& vol) 
{
  msg(MSG::DEBUG) << "Entering InDetServMatBuilderToolSLHC::addServiceVolume for volume " << vol.name() 
		  << " with " << vol.materials().size() << " materials" << endmsg;

  InDetDD::ServiceVolume * param = new InDetDD::ServiceVolume;
  //std::unique_ptr<InDetDD::ServiceVolume> param (new InDetDD::ServiceVolume); This doesn't work for some reason...
  std::vector<std::string> linearComponents;
  std::vector<double>      linWeights;

  param->setVolName(vol.name());
  param->setRmin(vol.rMin());
  param->setRmax(vol.rMax());
  param->setZmin(vol.zMin());
  param->setZmax(vol.zMax());
  param->setZsymm(true);
  for (std::vector<ServiceMaterial>::const_iterator ism=vol.materials().begin(); ism!=vol.materials().end(); ++ism) {
    for ( ServiceMaterial::EntryIter ient= ism->components().begin(); ient!=ism->components().end(); ient++) {
      /*
      msg(MSG::INFO) << "Inside components loop, comp = " << ient->name 
		     << " number " << ient->number 
		     << " weight " << ient->weight 
		     << " linear " << ient->linear 
		     << endmsg;
      */
      //      if (ient->weight*ient->number > 0.00001) {
      //std::string pre = "pix::";
      std::string prefix = "indet::";
      std::string prename = addPrefix( prefix, ient->name);

      if (ient->linear) {
	linearComponents.push_back( prename);
	linWeights.push_back( fabs( ient->number * ient->weight));
      }
      else {
	linearComponents.push_back( prename);      // the distiction between linear and not is done in the
	linWeights.push_back( fabs( ient->weight*ient->number));  // InDetMaterialmanager, based on the weight table flag
      }
    }
  }
  if ( !linearComponents.empty()) {

    const GeoMaterial * newMat = m_geoMgr->matMgr()->getMaterialForVolumeLength( vol.name(), 
										 linearComponents, linWeights, 
										 param->volume(), vol.length());
    printNewVolume( vol, *newMat, *param);

    param->setMaterial(newMat);
    addService(param);
    //    addService(param.release()); 
  }
  else delete param;
}

std::string InDetServMatBuilderToolSLHC::addPrefix( const std::string& prefix, const std::string& name) const
{
  // only add prefix to names which don't have one
  if ( name.find("::") == std::string::npos) return prefix+name;
  else return name;
}

/// Add tube like service after creating material
void InDetServMatBuilderToolSLHC::addService(const std::string & name, const GeoMaterial * material, 
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
void InDetServMatBuilderToolSLHC::addService(const std::string & name, const std::string & materialName, 
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

void InDetServMatBuilderToolSLHC::printNewVolume( const ServiceVolume& vol, 
						  const GeoMaterial& mat,
						  const InDetDD::ServiceVolume& param) const
{
  double dens = mat.getDensity();
  double weight = dens*param.volume();
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Creating service volume with rmin " << vol.rMin()
		   << " rmax " << vol.rMax() 
		   << " zmin " << vol.zMin() 
		   << " zmax " << vol.zMax() << endmsg;
 
    msg(MSG::DEBUG) << "name " << vol.name() << " density " << dens * Gaudi::Units::cm3 / GeoModelKernelUnits::g 
		   << " [g/cm3] weight " << dens*param.volume()/Gaudi::Units::kg  << " [kg]" << endmsg;
  } 
  if (msgLvl(MSG::DEBUG)) {   // FIXME: change to VERBOSE when done!
    msg(MSG::DEBUG) << "Number of elements: " << mat.getNumElements() << endmsg;
    for (unsigned int i=0; i< mat.getNumElements(); i++) {
      msg(MSG::DEBUG) << "Element " << mat.getElement(i)->getName() 
		      << " weight " << mat.getFraction(i) * weight / GeoModelKernelUnits::g << endmsg;
    }
  }
}

void InDetServMatBuilderToolSLHC::fixMissingMaterials() 
{
  /*
  m_geoMgr->matMgr()->addWeightEntry( "CoaxialHVCable",    "indet::CoaxialHVCableBase",    0.0026*GeoModelKernelUnits::gram, true, "indet"); 
  m_geoMgr->matMgr()->addWeightEntry( "TwistedPair_awg36", "indet::TwistedPair_awg36Base", 0.000381*GeoModelKernelUnits::gram, true, "indet"); 
  m_geoMgr->matMgr()->addWeightEntry( "ScreenedTwistP_36", "indet::ScreenedTwistP_36Base", 0.000978*GeoModelKernelUnits::gram, true, "indet"); 
  m_geoMgr->matMgr()->addWeightEntry( "Twinax",            "indet::TwinaxBase",            0.00517*GeoModelKernelUnits::gram, true, "indet"); 
  */
  //m_geoMgr->matMgr()->addWeightEntry( "ShieldingSleeve",   "indet::ShieldingSleeveBase",   0.033*GeoModelKernelUnits::gram, true, "indet"); 
  //m_geoMgr->matMgr()->addWeightEntry( "ProtectionSleeve",   "indet::ProtectionSleeveBase", 0.004*GeoModelKernelUnits::gram, true, "indet"); 
  /*
  // the wight is 1, because the real weight is in ServiceMaterial NOT NEEDED
  m_geoMgr->matMgr()->addWeightEntry( "CoolingPipe", "std::Titanium", 1*GeoModelKernelUnits::gram, true, "indet"); 

  std::vector<std::string> comps(2);
  std::vector<double> fracs(2);
  comps[0] = "std::Aluminium"; fracs[0] = 0.731;
  comps[1] = "std::Copper";    fracs[1] = 0.269;
  const GeoMaterial* geomat = m_geoMgr->matMgr()->getMaterial( "indet::CCAW", comps, fracs, 3.33);
  if (geomat) msg(MSG::INFO) << "Material with name name " << "CCAW" << " added" << endmsg;
  else        msg(MSG::INFO) << "Material with name name " << "CCAW" << " failed to be added" << endmsg;

  comps[0] = "indet::Polyetherimide"; fracs[0] = 0.409;
  comps[1] = "std::Copper";           fracs[1] = 0.591;
  geomat = m_geoMgr->matMgr()->getMaterial( "indet::TwistedPair_awg36Base", comps, fracs, 1.);
  if (geomat) msg(MSG::INFO) << "Material with name name " << "TwistedPair_awg36Base" << " added" << endmsg;
  else        msg(MSG::INFO) << "Material with name name " << "TwistedPair_awg36Base" << " failed to be added" << endmsg;

  comps[0] = "std::Carbon"; fracs[0] = 0.27;
  comps[1] = "Oxygen"; fracs[1] = 0.73;
  geomat = m_geoMgr->matMgr()->getMaterial( "pix::CO2_Liquid", comps, fracs, 1.);
  if (geomat) msg(MSG::INFO) << "Material with name name " << "pix::CO2_Liquid" << " added" << endmsg;
  else        msg(MSG::INFO) << "Material with name name " << "pix::CO2_Liquid" << " failed to be added" << endmsg;

  comps.resize(3); fracs.resize(3);
  comps[0] = "indet::Polyetherimide"; fracs[0] = 0.276;
  comps[1] = "std::Copper";           fracs[1] = 0.346;
  comps[2] = "std::Aluminium";        fracs[2] = 0.378;
  geomat = m_geoMgr->matMgr()->getMaterial( "indet::ScreenedTwistP_36Base", comps, fracs, 1.);
  if (geomat) msg(MSG::INFO) << "Material with name name " << "ScreenedTwistP_36Base" << " added" << endmsg;
  else        msg(MSG::INFO) << "Material with name name " << "ScreenedTwistP_36Base" << " failed to be added" << endmsg;

  std::vector<std::string> comps(3);
  std::vector<double> fracs(3);
  comps[0] = "indet::CCAW";           fracs[0] = 0.104;
  comps[1] = "std::Aluminium";        fracs[1] = 0.070;
  comps[2] = "indet::Polyetherimide"; fracs[2] = 0.826;
  const GeoMaterial* geomat = m_geoMgr->matMgr()->getMaterial( "indet::TwinaxBase", comps, fracs, 1.);
  if (geomat) msg(MSG::INFO) << "Material with name name " << "TwinaxBase" << " added" << endmsg;
  else        msg(MSG::INFO) << "Material with name name " << "TwinaxBase" << " failed to be added" << endmsg;

  std::vector<std::string> comps(1);
  std::vector<double> fracs(1);
  comps.resize(1); fracs.resize(1);
  comps[0] = "std::Copper"; fracs[0] = 1.0;
  addMissingMaterial( "indet::ShieldingSleeveBase", comps, fracs);

  comps[0] = "std::Polyethylene"; fracs[0] = 1.0;
  addMissingMaterial( "indet::ProtectionSleeveBase", comps, fracs);
  */
}

void InDetServMatBuilderToolSLHC::addMissingMaterial( const std::string& name, 
						      const std::vector<std::string>& comps, 
						      const std::vector<double>& fracs, double dens)
{
  const GeoMaterial* geomat = m_geoMgr->matMgr()->getMaterial( name, comps, fracs, dens);
  if (geomat) msg(MSG::DEBUG) << "Material with name name " << name << " added" << endmsg;
  else        msg(MSG::INFO) << "Material with name name " << name << " failed to be added" << endmsg;
}

/*
std::string InDetServMatBuilderToolSLHC::getKnownName( const std::string& name, InDetMaterialManager* matMgr) 
{
  std::string kname = name;
  const GeoMaterial* geomat = matMgr->getMaterial(name);
  if (geomat) {
    msg(MSG::INFO) << "Material with name name " << name << " found" << endmsg;
    return name;
  }
  else {
    kname = "indet::"; kname += name;
    geomat = matMgr->getMaterial(kname);
    if (geomat) {
      msg(MSG::INFO) << "Material with name name " << kname << " found" << endmsg;
      return kname;
    }
    else {
      if (name == "CCAW") {
	std::vector<std::string> comps(2);
	comps[0] = "std::Aluminium";
	comps[1] = "std::Copper";
	std::vector<double> fracs(2);
	fracs[0] = 0.731;
	fracs[1] = 0.269;
	geomat = matMgr->getMaterial( name, comps, fracs, 3.33);
      }
      else if (name == "UnshieldedTwistedPair_awg36") {
	std::vector<std::string> comps(2);
	comps[0] = "indet::Polyetherimide";
	comps[1] = "std::Copper";
	std::vector<double> fracs(2);
	fracs[0] = 0.4;
	fracs[1] = 0.6;
	geomat = matMgr->getMaterial( name, comps, fracs, 1.);

      }
      else {
	msg(MSG::INFO) << "Error: Material with name name " << name << " or " << kname << " not found" << endmsg;
	return "";
      }
      msg(MSG::INFO) << "Material with name name " << name << " created" << endmsg;
      return name;
    }
  }
}
*/
