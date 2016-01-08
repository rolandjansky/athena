/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelServicesTool/SimpleServiceVolumeMaker.h"
#include "PixelServicesTool/PixelSimpleServiceXMLHelper.h"

#include "InDetGeoModelUtils/ServiceVolume.h"
/*#include "InDetGeoModelUtils/InDetDDAthenaComps.h"*/

#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"

#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"

#include "PathResolver/PathResolver.h"
#include "CLHEP/Units/SystemOfUnits.h"

namespace InDetDD {

ServiceVolumeSchema::ServiceVolumeSchema() 
{
  setSimpleSchema();
}

void ServiceVolumeSchema::setPixelSchema()
{
  m_rmin = "RIN";
  m_rmax = "ROUT";
  m_rmin2 = "RIN2";
  m_rmax2 = "ROUT2";
  m_zmin = "ZIN";
  m_zmax = "ZOUT";
  m_zsymm = "ZSYMM";
  m_materialName = "MATERIALNAME";
  m_repeat = "REPEAT";
  m_phiStart = "PHI";
  m_phiDelta = "WIDTH";
  m_width = "WIDTH";
  m_shapeType = "SHAPE";
  m_volName = "VOLNAME";
  m_radialDiv = "";
  m_phiStep = "";
  m_volId = "FRAMENUM";
}

void ServiceVolumeSchema::setDefaultSchema()
{
  m_rmin = "RMIN";
  m_rmax = "RMAX";
  m_rmin2 = "RMIN2";
  m_rmax2 = "RMAX2";
  m_zmin = "ZMIN";
  m_zmax = "ZMAX";
  m_zsymm = "ZSYMM";
  m_materialName = "MATERIAL";
  m_repeat = "NREPEAT";
  m_phiStart = "PHISTART";
  m_phiDelta = "PHIDELTA";
  m_width = "";
  m_shapeType = "";
  m_volName = "NAME";
  m_radialDiv = "RADIAL";
  m_phiStep = "PHISTEP";
  m_volId = "";
}

void ServiceVolumeSchema::setSimpleSchema()
{
  m_rmin = "RMIN";
  m_rmax = "RMAX";
  m_rmin2 = "";
  m_rmax2 = "";
  m_zmin = "ZMIN";
  m_zmax = "ZMAX";
  m_zsymm = "ZSYMM";
  m_materialName = "MATERIAL";
  m_repeat = "";
  m_phiStart = "";
  m_phiDelta = "";
  m_width = "";
  m_shapeType = "";
  m_volName = "NAME";
  m_radialDiv = "";
  m_phiStep = "";
  m_volId = "";
}

SimpleServiceVolumeMakerMgr::SimpleServiceVolumeMakerMgr(IRDBRecordset_ptr table, const ServiceVolumeSchema & schema, 
							 const OraclePixGeoAccessor *geoAccessor, bool readDataFromDB)
  : GeoXMLUtils(),
    m_table(table),
    m_schema(schema),
    m_geoAccessor(geoAccessor),
    m_simpleSrvXMLHelper(0),
    m_readFromDB(readDataFromDB),
    m_XMLdefined(false)
 {

  if(!m_readFromDB){
    m_simpleSrvXMLHelper = new PixelSimpleServiceXMLHelper(table, schema);
    m_XMLdefined = true;
  }
}

const IGeometryDBSvc *
SimpleServiceVolumeMakerMgr::db() const {
  //  return m_athenaComps->geomDB();
  return 0;
}

double SimpleServiceVolumeMakerMgr::rmin(int index) const
{
  if(m_readFromDB)
    return m_geoAccessor->getDouble(m_table, m_schema.rmin(), index) * CLHEP::mm;

  return m_simpleSrvXMLHelper->rmin(index);
}


double SimpleServiceVolumeMakerMgr::rmax(int index) const
{
  if(m_readFromDB)
    return m_geoAccessor->getDouble(m_table, m_schema.rmax(), index) * CLHEP::mm;

  return m_simpleSrvXMLHelper->rmax(index);
}


double SimpleServiceVolumeMakerMgr::rmin2(int index) const
{
  if(m_readFromDB)
    return m_geoAccessor->getDouble(m_table, m_schema.rmin2(), index) * CLHEP::mm;

  return m_simpleSrvXMLHelper->rmin2(index);
}

double SimpleServiceVolumeMakerMgr::rmax2(int index) const
{
  if(m_readFromDB)
    return m_geoAccessor->getDouble(m_table, m_schema.rmax2(), index) * CLHEP::mm;

  return m_simpleSrvXMLHelper->rmax2(index);
}

double SimpleServiceVolumeMakerMgr::zmin(int index) const
{
  if(m_readFromDB)
    return m_geoAccessor->getDouble(m_table, m_schema.zmin(), index) * CLHEP::mm;

  return m_simpleSrvXMLHelper->zmin(index);
}

double SimpleServiceVolumeMakerMgr::zmax(int index) const
{
  if(m_readFromDB)
    return m_geoAccessor->getDouble(m_table, m_schema.zmax(), index) * CLHEP::mm;

  return m_simpleSrvXMLHelper->zmax(index);
}

double SimpleServiceVolumeMakerMgr::phiDelta(int index) const
{
  if(m_readFromDB)
    return m_geoAccessor->getDouble(m_table, m_schema.phiDelta(), index) * CLHEP::deg;

  return m_simpleSrvXMLHelper->phiDelta(index);
}

double SimpleServiceVolumeMakerMgr::width(int index) const
{
  if(m_readFromDB){
    if (m_schema.has_width()) {
      return m_geoAccessor->getDouble(m_table, m_schema.width(), index) * CLHEP::mm;
    }
    return 0;
  }

  if (m_schema.has_width()) {
    return m_simpleSrvXMLHelper->width(index);
  }
  return 0;
}

double SimpleServiceVolumeMakerMgr::phiStart(int index) const
{
  if(m_readFromDB)
    return m_geoAccessor->getDouble(m_table, m_schema.phiStart(), index) * CLHEP::deg;

  return m_simpleSrvXMLHelper->phiStart(index);
}

double SimpleServiceVolumeMakerMgr::phiStep(int index) const
{
  if(m_readFromDB){
    if (m_schema.has_phiStep()) {
      return m_geoAccessor->getDouble(m_table, m_schema.phiStep(), index) * CLHEP::deg;
    } 
    return 0;
  }

  if (m_schema.has_phiStep()) {
    return m_simpleSrvXMLHelper->phiStep(index);
  } 
  return 0;
}

bool SimpleServiceVolumeMakerMgr::zsymm(int index) const
{
  if(m_readFromDB)
    return m_geoAccessor->getInt(m_table, m_schema.zsymm(), index);

  return m_simpleSrvXMLHelper->zsymm(index);
}


int SimpleServiceVolumeMakerMgr::repeat(int index) const
{
  if(m_readFromDB)
    return m_geoAccessor->getInt(m_table, m_schema.repeat(), index);

  return m_simpleSrvXMLHelper->repeat(index);
}

int SimpleServiceVolumeMakerMgr::radialDiv(int index) const
{
  if(m_readFromDB){
    if (m_schema.has_radial()) {  
      return m_geoAccessor->getInt(m_table, m_schema.radialDiv(), index);
    } else { 
      return 0;
    }
  }

  if (m_schema.has_radial()) {  
    return m_simpleSrvXMLHelper->radialDiv(index);
  } else { 
    return 0;
  }
}

std::string SimpleServiceVolumeMakerMgr::shapeType(int index) const
{
  if(m_readFromDB){
    if (m_schema.has_shapeType()) {  
      if (m_geoAccessor->testField(m_table, m_schema.shapeType(), index)) {
	return m_geoAccessor->getString(m_table, m_schema.shapeType(), index);
      } else {
	return "TUBE";
      }
    }
    return "UNKNOWN";
  }

  if (m_schema.has_shapeType()) {  
    std::string tmp=m_simpleSrvXMLHelper->shapeType(index);
    tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
    return tmp;
    }
  return "UNKNOWN";
}

std::string SimpleServiceVolumeMakerMgr::volName(int index) const
{
  if(m_readFromDB){
    if (m_geoAccessor->testField(m_table, m_schema.volName(), index)) {
      return m_geoAccessor->getString(m_table, m_schema.volName(), index);
    } 
    return "";
  }

  std::string tmp=m_simpleSrvXMLHelper->volName(index);
  tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
  return tmp;
}

std::string SimpleServiceVolumeMakerMgr::materialName(int index) const
{
  if(m_readFromDB)
    return m_geoAccessor->getString(m_table, m_schema.materialName(), index);

  std::string tmp=m_simpleSrvXMLHelper->materialName(index);
  tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
  return tmp;

}

unsigned int SimpleServiceVolumeMakerMgr::numElements() const
{
  if(m_readFromDB)
    return m_geoAccessor->getTableSize(m_table); 

  return m_simpleSrvXMLHelper->numElements();
}

int SimpleServiceVolumeMakerMgr::volId(int index) const
{
  if(m_readFromDB){
    if (m_schema.has_volId()) {  
      return m_geoAccessor->getInt(m_table, m_schema.volId(), index);
    }
    return 0;
  }

  return m_simpleSrvXMLHelper->volId(index);
}


SimpleServiceVolumeMaker::SimpleServiceVolumeMaker(const std::string & label,
						   IRDBRecordset_ptr table, const ServiceVolumeSchema & schema, 
						   const OraclePixGeoAccessor *geoAccessor,
						   bool readDataFromDB) 
//				       const InDetDD::AthenaComps * athenaComps)
  : m_label(label), m_geoAccessor(geoAccessor)
{
  m_mgr = new SimpleServiceVolumeMakerMgr(table, schema, geoAccessor, readDataFromDB);
}

SimpleServiceVolumeMaker::~SimpleServiceVolumeMaker()
{
  for (unsigned int i = 0; i < m_services.size(); i++) {
    delete m_services[i];
  }
  delete m_mgr;
}

const std::vector<const ServiceVolume *>& SimpleServiceVolumeMaker::makeAll()
{
  for (unsigned int ii = 0; ii < numElements(); ++ii) {
    m_services.push_back(make(ii));
  }
  return m_services;
}

unsigned int
SimpleServiceVolumeMaker::numElements() const {
  return m_mgr->numElements();
}

InDetDD::ServiceVolume *SimpleServiceVolumeMaker::make(int ii)
{
  //
  // Retrieve/calculate the parameters for the volume.
  //
  ServiceVolume * param = new ServiceVolume ;
  param->setMaterial(m_mgr->materialName(ii));
  param->setRmin(m_mgr->rmin(ii));
  param->setRmax(m_mgr->rmax(ii));
  param->setZmin(m_mgr->zmin(ii));
  param->setZmax(m_mgr->zmax(ii));
  param->setZsymm(m_mgr->zsymm(ii));
  param->setVolName(m_mgr->volName(ii));
  
  int volId = m_mgr->volId(ii);
  if (volId == 0) volId = ii+1;
  
  bool needsRotation = false;
  
  // For TUBE there is no need to read the rest 
  std::string shapeType = m_mgr->shapeType(ii);
  if (!m_mgr->schema().simple() && !shapeType.empty() && shapeType != "TUBE") {
      
    double rmin2 = m_mgr->rmin2(ii);
    double rmax2 = m_mgr->rmax2(ii);
    
    if (rmin2 <= 0) rmin2 = param->rmin(); 
    if (rmax2 <= 0) rmax2 = param->rmax(); 
    
    int radialDiv = m_mgr->radialDiv(ii);
    
    double phiDelta =  m_mgr->phiDelta(ii);
    
    bool fullPhiSector = false;
    if (phiDelta == 0 || phiDelta >=359.9*CLHEP::degree) {
      phiDelta = 360*CLHEP::degree;
      fullPhiSector = true;
    } 
    //else {
    //phiDelta -= 2*phiepsilon;
    //phiStart += phiepsilon;
    // }
    
    if (shapeType == "UNKNOWN") {
      if (radialDiv > 0) {
	shapeType = "RADIAL";
      } else if (param->rmin() == rmin2  &&  param->rmax() == rmax2 ) {
	if (fullPhiSector) {
	  shapeType = "TUBE";
	} else {
	  shapeType = "TUBS";
	}
      } else {
	shapeType = "CONS";
      } 
    }
    
    
    int repeat = m_mgr->repeat(ii);
    if (repeat == 0) repeat = 1;
    
    double phiStart =  m_mgr->phiStart(ii);
    double phiWidth =  phiDelta;
    
    if (shapeType == "CONS"  || shapeType == "TUBS") { 
      const double phiepsilon = 0.001*CLHEP::degree;
      phiWidth -= 2*phiepsilon;
      phiStart += phiepsilon;
    }
    
    // Can be in degree or CLHEP::mm. Usually it is CLHEP::deg expect for BOX, TRAP and ROD shape
    // Geometry manager makes no assumptions about units. So we must interpret here.
    if (shapeType == "BOX" || shapeType == "ROD" || shapeType=="ROD2" || shapeType == "TRAP") {
      phiWidth = m_mgr->width(ii); // in mm
    } 
    
    if (shapeType == "PGON"  || shapeType == "PGON2" || 
	shapeType == "CONE"  || shapeType == "CONS" || 
	shapeType == "PGON3" || shapeType == "PGON4") {
      if ((rmin2 != param->rmin()) || (rmax2 != param->rmax())) {
	needsRotation = true;
      }
    }
    
    int sides = 0;
    int nCopies = 1;
    if (shapeType == "PGON"  || shapeType == "PGON2" ||
	shapeType == "PGON3" || shapeType == "PGON4") {
      sides = repeat;
    } else {
      nCopies = repeat;
    }
    
    // Force nCopies to 1 for TUBE and CONE 
    if (shapeType.empty() || shapeType == "TUBE" || shapeType == "CONE") {
      nCopies = 1;
    }
    
    param->setShapeType(shapeType);
    param->setRmin2(rmin2);
    param->setRmax2(rmax2);
    param->setPhiLoc(phiStart);
    param->setPhiWidth(phiWidth);
    param->setSides(sides);
    param->setNCopies(nCopies);
    //param->setRadialDiv(radialDiv);
    //param->setPhiStep(phiStep);
  }
  
  param->setNeedsRotation(needsRotation);
  
  
  //
  // If zin is 0... (within 10^-5) this is a volume symmetric around
  // the origin
  //
  if(std::abs(param->zmin()) < 0.000001) {
    param->setZmin(-param->zmax());
    param->setZsymm(false);
  }	
  
  param->setLabel(m_label, volId); 

//   std::cout<<"SimpleServiceVoluemMaker "<<ii<<std::endl;
//   param->print();
  
  return param;
}

} // end namespace
