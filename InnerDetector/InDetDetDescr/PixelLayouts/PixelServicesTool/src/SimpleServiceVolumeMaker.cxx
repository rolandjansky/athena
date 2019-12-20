/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

SimpleServiceVolumeMaker::SimpleServiceVolumeMaker(const std::string & table, const std::string & label, const PixelGeoBuilderBasics* basics, bool readDataFromDB) 
  : GeoXMLUtils(),
    PixelGeoBuilder(basics),
    m_table(table),
    m_label(label),
    m_simpleSrvXMLHelper(0),
    m_readFromDB(readDataFromDB),
    m_XMLdefined(false)
{
  if(!m_readFromDB){
    m_simpleSrvXMLHelper = new PixelSimpleServiceXMLHelper(table, basics);
    m_XMLdefined = true;
  }
}


SimpleServiceVolumeMaker::~SimpleServiceVolumeMaker()
{
  for (unsigned int i = 0; i < m_services.size(); i++) {
    delete m_services[i];
  }
}

const std::vector<const ServiceVolume *>& SimpleServiceVolumeMaker::makeAll()
{
  for (unsigned int ii = 0; ii < m_simpleSrvXMLHelper->numElements(); ++ii) {
    m_services.push_back(make(ii));
  }
  return m_services;
}

unsigned int SimpleServiceVolumeMaker::numElements() const {
  return m_simpleSrvXMLHelper->numElements();
}

InDetDD::ServiceVolume *SimpleServiceVolumeMaker::make(int ii)
{
  //
  // Retrieve/calculate the parameters for the volume.
  //
  ServiceVolume * param = new ServiceVolume ;
  param->setMaterial(m_simpleSrvXMLHelper->materialName(ii));
  param->setRmin    (m_simpleSrvXMLHelper->rmin(ii));
  param->setRmax    (m_simpleSrvXMLHelper->rmax(ii));
  param->setZmin    (m_simpleSrvXMLHelper->zmin(ii));
  param->setZmax    (m_simpleSrvXMLHelper->zmax(ii));
  param->setZsymm   (m_simpleSrvXMLHelper->zsymm(ii));
  param->setVolName (m_simpleSrvXMLHelper->volName(ii));

  bool needsRotation = false;
  
  // For TUBE there is no need to read the rest 
  std::string shapeType =  m_simpleSrvXMLHelper->shapeType(ii);
  if (!shapeType.empty() && shapeType != "TUBE") {
      
    double rmin2 = m_simpleSrvXMLHelper->rmin2(ii);
    double rmax2 = m_simpleSrvXMLHelper->rmax2(ii);
    
    if (rmin2 <= 0) rmin2 = param->rmin(); 
    if (rmax2 <= 0) rmax2 = param->rmax(); 
    
    // TODO: radialDiv not fully implemented, saving for future ref
    //int radialDiv = 0; 
    
    double phiDelta =  m_simpleSrvXMLHelper->phiDelta(ii);
    
    bool fullPhiSector = false;
    if (phiDelta == 0 || phiDelta >=359.9*CLHEP::degree) {
      phiDelta = 360*CLHEP::degree;
      fullPhiSector = true;
    } 
    
    if (shapeType == "UNKNOWN") {
      // TODO: radialDiv not implemented, saving for future ref
      //if (radialDiv > 0) {
      //  shapeType = "RADIAL";
      //} else 
      if (param->rmin() == rmin2  &&  param->rmax() == rmax2 ) {
	if (fullPhiSector)
	  shapeType = "TUBE";
	else
	  shapeType = "TUBS";
      } else
	shapeType = "CONS";
    }
    
    
    int repeat = m_simpleSrvXMLHelper->repeat(ii);
    if (repeat == 0) repeat = 1;
    
    double phiStart =  m_simpleSrvXMLHelper->phiStart(ii);
    double phiWidth =  phiDelta;
    
    if (shapeType == "CONS"  || shapeType == "TUBS") { 
      const double phiepsilon = 0.001*CLHEP::degree;
      phiWidth -= 2*phiepsilon;
      phiStart += phiepsilon;
    }
    
    // Can be in degree or CLHEP::mm. Usually it is CLHEP::deg expect for BOX, TRAP and ROD shape
    // Geometry manager makes no assumptions about units. So we must interpret here.
    if (shapeType == "BOX" || shapeType == "ROD" || shapeType=="ROD2" || shapeType == "TRAP") {
      phiWidth = m_simpleSrvXMLHelper->width(ii); // in mm
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
    //param->setRadialDiv(radialDiv); // not implemented, saving for future ref
    //param->setPhiStep(phiStep); // not implemented, saving foir future ref
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
  
  param->setLabel(m_label); 

//   std::cout<<"SimpleServiceVoluemMaker "<<ii<<std::endl;
//   param->print();
  
  return param;
}

} // end namespace
