/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatFactorySLHC.h"
#include "InDetServMatGeoModel/InDetServMatAthenaComps.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

// Extra material
//#include "InDetGeoModelUtils/DistortedMaterialManager.h"
//#include "InDetGeoModelUtils/ExtraMaterial.h"

// General Service Builder
#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/VolumeSplitterUtils.h"
#include "InDetGeoModelUtils/ServiceVolume.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h"

// GeoModel includes
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoShapeUnion.h"  
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"

// StoreGate includes
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GaudiKernel/PhysicalConstants.h"
#include <iostream>

InDetServMatFactorySLHC::InDetServMatFactorySLHC(const InDetServMatAthenaComps * athenaComps)
  :  InDetDD::DetectorFactoryBase(athenaComps),
     m_athenaComps(athenaComps),
     m_manager(0)
{
   // create a new det manager
  m_manager = new InDetDD::InDetServMatManager();

}


InDetServMatFactorySLHC::~InDetServMatFactorySLHC()
{
  delete m_manager;
}



//## Other Operations (implementation)
void InDetServMatFactorySLHC::create(GeoPhysVol *world )
{

  // Get the material manager:  
  InDetMaterialManager * materialManager = new InDetMaterialManager("InDetServMatMaterialManager",m_athenaComps);

  fetchTables();

  InDetDD::Zone * zone = 0;
  const GeoShape * envelopeShape = 0;
  if (simpleEnvelope() || oldEnvelope()) { 
    // tube 

    double innerRadius;
    double outerRadius;
    double cylLength;
    
    if (oldEnvelope()) {
      innerRadius = geomDB()->getDouble("","SERVICESINNERRADIUS") * Gaudi::Units::mm;
      outerRadius = geomDB()->getDouble("","SERVICESOUTERRADIUS") * Gaudi::Units::mm; 
      cylLength   = geomDB()->getDouble("","SERVICESCYLLENGTH") * Gaudi::Units::mm; 
    } else {
      innerRadius = envelopeRMin();
      outerRadius = envelopeRMax();
      cylLength   = envelopeLength();
    }
    envelopeShape = new GeoTube(innerRadius, outerRadius, 0.5*cylLength);
    zone = new InDetDD::TubeZone("InDetServMat", -0.5*cylLength, 0.5*cylLength, innerRadius, outerRadius);
  } else {
    GeoPcon* envelopeShapeTmp  = new GeoPcon(0.,2*Gaudi::Units::pi);
    // table contains +ve z values only and envelope is assumed to be symmetric around z.
    int numPlanes = envelopeNumPlanes();
    for (int i = 0; i < numPlanes * 2; i++) {
      int tableIndex = (i < numPlanes) ?  numPlanes - 1 - i :  i - numPlanes;
      int sign       = (i < numPlanes) ? -1 : 1;
      double z = envelopeZ(tableIndex) * sign;
      double rmin = envelopeRMin(tableIndex);
      double rmax = envelopeRMax(tableIndex);
      envelopeShapeTmp->addPlane(z, rmin, rmax);
    }
    envelopeShape = envelopeShapeTmp;
    zone = new InDetDD::PconZone("InDetServMat",envelopeShapeTmp);
  }
  const GeoMaterial* air = materialManager->getMaterial("std::Air");
  GeoLogVol* servLog = new GeoLogVol("ServLog", envelopeShape, air);
  GeoPhysVol* servPhys = new GeoPhysVol(servLog);

  // build general services
  InDetDD::UnboundedZone topZone("");
  topZone.add(zone);
  if (m_athenaComps->builderTool()) {
    const std::vector<const InDetDD::ServiceVolume *> & services = m_athenaComps->builderTool()->getServices();
    InDetDD::VolumeBuilder builder(topZone, services);
    builder.setMaterialManager(materialManager);
    builder.buildAndPlace("InDetServMat", servPhys);
  }

  // Add this to the world and register it to the manager so GeoG4 will 
  // find it.
  //
  GeoNameTag *tag = new GeoNameTag("InDetServMat");         
  world->add(tag);
  world->add(servPhys);
  m_manager->addTreeTop(servPhys);
  
  delete materialManager;
}


const InDetDD::InDetServMatManager* InDetServMatFactorySLHC::getDetectorManager () const {
  return m_manager;
}

void
InDetServMatFactorySLHC::fetchTables() 
{
  DecodeVersionKey indetVersionKey(geoDbTagSvc(), "InnerDetector");

  msg(MSG::INFO) << "Building InDet Service Material with InDet Version Tag: "  
		 << indetVersionKey.tag() << " at Node: " << indetVersionKey.node() << endmsg;
  msg(MSG::INFO) << " InDetServices Version " 
		 << rdbAccessSvc()->getChildTag("InDetServices", indetVersionKey.tag(), indetVersionKey.node()) << endmsg;
 
  m_InDetServGenEnvelope = rdbAccessSvc()->getRecordsetPtr("InDetServGenEnvelope", indetVersionKey.tag(), indetVersionKey.node());
  msg(MSG::DEBUG) << "Table InDetServGenEnvelope Fetched" << endmsg;
}

double 
InDetServMatFactorySLHC::envelopeLength() const
{
  if (geomDB()->getTableSize(m_InDetServGenEnvelope)) {
    // The table should contain only +ve z values.
    return 2*envelopeZ(geomDB()->getTableSize(m_InDetServGenEnvelope) - 1);
  } else {
    msg(MSG::ERROR) << "Unexpected error. InDetServGenEnvelope has zero size" << endmsg;
    return 0;
  }
}

bool 
InDetServMatFactorySLHC::simpleEnvelope() const
{
  // Return true if the envelope can be built as a simple tube.
  // otherwise it will be built as a PCON.
  // True if size is 1.
  return (geomDB()->getTableSize(m_InDetServGenEnvelope) == 1);
}

bool 
InDetServMatFactorySLHC::oldEnvelope() const
{
  // Return true if the envelope is not provided. 
  // True if size is 0.
  return (geomDB()->getTableSize(m_InDetServGenEnvelope) == 0);
}


unsigned int 
InDetServMatFactorySLHC::envelopeNumPlanes() const
{
  return geomDB()->getTableSize(m_InDetServGenEnvelope);
}

double 
InDetServMatFactorySLHC::envelopeZ(int i) const 
{
  double zmin =  geomDB()->getDouble(m_InDetServGenEnvelope,"Z",i) * Gaudi::Units::mm;
  if (zmin < 0) msg(MSG::ERROR) << "InDetServGenEnvelope table should only contain +ve z values" << endmsg;
  return std::abs(zmin);
}

double 
InDetServMatFactorySLHC::envelopeRMin(int i) const 
{
  return geomDB()->getDouble(m_InDetServGenEnvelope,"RMIN",i) * Gaudi::Units::mm;
}

double 
InDetServMatFactorySLHC::envelopeRMax(int i) const
{
  return geomDB()->getDouble(m_InDetServGenEnvelope,"RMAX",i) * Gaudi::Units::mm;
}
