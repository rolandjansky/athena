/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelLayoutECRing/GeoPixelLayerECRingTool.h"
#include "PixelLayoutECRing/GeoPixelRingECRing.h"
#include "PixelLayoutECRing/PixelRingSupportXMLHelper.h"

#include "PixelInterfaces/IPixelServicesTool.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include <sstream>
#include <iostream>

GeoPixelLayerECRingTool::GeoPixelLayerECRingTool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_layer(-1),
    m_IDserviceTool("InDetservicesTool/PixelServicesTool"),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{
  declareInterface<IGeoPixelEndcapLayerTool>(this);
}

GeoPixelLayerECRingTool::~GeoPixelLayerECRingTool()
{
}

//================ Initialisation =================================================

StatusCode GeoPixelLayerECRingTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_INFO("GeoPixelLayerECRingTool initialize() successful in " << name());

//   sc = m_IDserviceTool.retrieve(); 
//   if (sc.isFailure()){
//     msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
//     return sc;
//   }
//   msg(MSG::INFO) << "Service builder tool retrieved: " << endreq;

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
  }

  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelLayerECRingTool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelLayerECRingTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelLayerECRingTool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

void GeoPixelLayerECRingTool::preBuild(const PixelGeoBuilderBasics* basics, int layer)
{
  
  m_layer = layer;
  m_ringPos.clear();
  m_ringListF.clear();
  m_ringListB.clear();

  // disc datas
  InDet::EndcapLayerTmp* discTmp = m_xmlReader->getPixelEndcapLayerTemplate(m_layer); 

  double rLayerMin = 9999999.;
  double rLayerMax = -9999999.;
  double zLayerMin = 9999999.;
  double zLayerMax = -9999999.;

  std::vector<double> v_ringPosition = discTmp->ringpos;
  std::vector<double> v_ringRadius = discTmp->innerRadius;
  std::vector<double> v_ringOuterRadius = discTmp->outerRadius;
  std::vector<double> v_zOffset = discTmp->zoffset;
  std::vector<double> v_phiOffset = discTmp->phioffset;
  std::vector<int> v_numModules = discTmp->nsectors;
  std::vector<std::string> v_moduleType = discTmp->modtype;

  int nrings = (int)v_ringPosition.size();
  // Fixme SES
  std::vector<int> v_discNumber;
  std::vector<int> v_discSide;
  v_discSide.push_back(0);
  for(int iRing=0; iRing<nrings; iRing++) v_discNumber.push_back(iRing);

  for(int iRing=0; iRing<nrings; iRing++){

    //    int ringNumber = iRing;
    int discNumber = getValueFromVector(v_discNumber, iRing);
    double ringPosition = getValueFromVector(v_ringPosition, iRing);
    double ringRadius = getValueFromVector(v_ringRadius, iRing); 
    double ringOuterRadius = getValueFromVector(v_ringOuterRadius, iRing); 
    double zOffset = getValueFromVector(v_zOffset, iRing);
    double phiOffset = getValueFromVector(v_phiOffset, iRing);
    int numModules = getValueFromVector(v_numModules, iRing);
    std::string moduleType = getValueFromVector(v_moduleType, iRing);
    double halfZoffset = zOffset*.5;

    std::cout<<"**** BUILD ring "<<iRing<<"/"<<nrings<<"  - layer "<<m_layer<<"   "<<"  - zpos "<<ringPosition<<"  "<<ringPosition-halfZoffset<<"/"<<ringPosition+halfZoffset<<std::endl;
        
    int iSide = getValueFromVector(v_discSide,iRing);
    GeoPixelRingECRing ringF(m_layer,iRing,ringRadius, ringOuterRadius, zOffset, phiOffset, iSide, numModules, moduleType, discNumber, 1);
    ringF.preBuild(basics);
    m_ringListF.push_back(ringF);
    m_ringPos.push_back(ringPosition-halfZoffset+ringF.getRingZShift());

    //    double zLengthF = ringF.getRingZMax()-ringF.getRingZMin();

    rLayerMin = std::min(rLayerMin,ringF.getRingRMin());
    rLayerMax = std::max(rLayerMax,ringF.getRingRMax());
    zLayerMin = std::min(zLayerMin,ringPosition-halfZoffset+ringF.getRingZMin());
    zLayerMax = std::max(zLayerMax,ringPosition-halfZoffset+ringF.getRingZMax());

    GeoPixelRingECRing ringB(m_layer,iRing,ringRadius, ringOuterRadius, zOffset, phiOffset, iSide, numModules, moduleType, discNumber, -1);
    ringB.preBuild(basics);
    m_ringListB.push_back(ringB);
    m_ringPos.push_back(ringPosition+halfZoffset-ringB.getRingZShift());
    
    rLayerMin = std::min(rLayerMin,ringB.getRingRMin());
    rLayerMax = std::max(rLayerMax,ringB.getRingRMax());
    zLayerMin = std::min(zLayerMin,ringPosition+halfZoffset+ringB.getRingZMin());
    zLayerMax = std::max(zLayerMax,ringPosition+halfZoffset+ringB.getRingZMax());

    // to keep compatibility with Loi+ECring old GeoModel style
    if(m_IDserviceTool&&!m_IDserviceTool->svcRouteAuto()) {
      std::string region = "A";
      std::vector<std::string> svcList;
      std::ostringstream eosName;
      eosName<< "+EOS"<<m_layer;
      std::cout<<"ENDCAP layer : build services "<<region<<" "<<eosName.str()<<std::endl;
      svcList.push_back(eosName.str());    // place EOS services in layer

      double rmin = m_IDserviceTool->computeRmin(region,svcList);
      double rmax = m_IDserviceTool->computeRmax(region,svcList);
      double zmin = m_IDserviceTool->computeZmin(region,svcList);
      double zmax = m_IDserviceTool->computeZmax(region,svcList);
      
      rLayerMin = std::min(rLayerMin,rmin);
      rLayerMax = std::max(rLayerMax,rmax);
      zLayerMin = std::min(zLayerMin,zmin);
      zLayerMax = std::max(zLayerMax,zmax);
    }
  }

  //  std::cout<<"RingLog minmax gbl : "<<layer<<" : "<<zLayerMin<<" "<<zLayerMax<<std::endl;

  m_layerPosition = (zLayerMin+zLayerMax)*.5;
  m_layerZMin = zLayerMin-0.001;
  m_layerZMax = zLayerMax+0.001;
  m_layerRMin = rLayerMin-0.001;
  m_layerRMax = rLayerMax+0.001;

}

GeoVPhysVol* GeoPixelLayerECRingTool::buildLayer(const PixelGeoBuilderBasics* basics, int layer, int side) 
{

 // Check that the prebuild phase is done
  if(layer!=m_layer) preBuild(basics,layer);

  m_layer = layer;
  m_endcapSide = side;

  // Layer data read from xmlReader
  InDet::EndcapLayerTmp* discTmp = m_xmlReader->getPixelEndcapLayerTemplate(m_layer); 

  std::vector<GeoVPhysVol*> ringList_PV;

  std::vector<double> v_ringPosition = discTmp->ringpos;
  std::vector<double> v_ringRadius = discTmp->innerRadius;
  std::vector<double> v_ringOuterRadius = discTmp->outerRadius;
  std::vector<double> v_zOffset = discTmp->zoffset;
  std::vector<int> v_numModules = discTmp->nsectors;
  std::vector<std::string> v_moduleType = discTmp->modtype;

  int nrings = (int)v_ringPosition.size();
  
  // Set numerology
  basics->getDetectorManager()->numerology().setNumRingsForDisk(layer,nrings);

  // Fixme SES
  std::vector<int> v_discNumber;
  std::vector<int> v_discSide;
  v_discSide.push_back(0);
  for(int iRing=0; iRing<nrings; iRing++) v_discNumber.push_back(iRing);

  // Build rings
  for(int iRing=0; iRing<nrings; iRing++){

    //    std::cout<<"**** BUILD ring - physvol "<<iRing<<"/"<<nrings<<"  - layer "<<m_layer<<"   "<<std::endl;
    
    ringList_PV.push_back(m_ringListF[iRing].Build(basics,m_endcapSide));
    ringList_PV.push_back(m_ringListB[iRing].Build(basics,m_endcapSide));

    // Set numerology
    int numModules = getValueFromVector(v_numModules, iRing);
    basics->getDetectorManager()->numerology().setNumPhiModulesForDiskRing(layer,iRing,numModules);

  }

  // Build layer envelope
  const GeoMaterial* air = basics->matMgr()->getMaterial("std::Air");
  double halflength = (m_layerZMax-m_layerZMin)*.5;
  const GeoTube* ecTube = new GeoTube(m_layerRMin,m_layerRMax,halflength);
  GeoLogVol* _ecLog = new GeoLogVol("layer",ecTube,air);
  GeoFullPhysVol* ecPhys = new GeoFullPhysVol(_ecLog);
  
  //  std::cout<<"RingLog minmax : "<<layer<<" : "<<m_layerZMin<<" "<<m_layerZMax<<std::endl;
  double zMiddle = (m_layerZMin+m_layerZMax)*.5;

  // Add ring supports
  PixelRingSupportXMLHelper ringHelper;
  int nbSvcSupport = ringHelper.getNbSupport(m_layer);

  // Place rings
  for(int i=0; i<2*nrings; i++){
    double zPos = m_ringPos[i]-zMiddle;
    //    std::cout<<" -> add ring (minmax): "<<i<<" "<<zPos+zMiddle<<"    vs ring minmax"<<std::endl;
    GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::TranslateZ3D(zPos));

    int tagId = i;
    std::ostringstream ostr; 
    if(i%2==0)
      ostr << "RingF" << i<<"L"<<m_layer;
    else
      ostr << "RingB" << i<<"L"<<m_layer;
    ecPhys->add(new GeoNameTag(ostr.str()));
    ecPhys->add( new GeoIdentifierTag(tagId));
    ecPhys->add(xform);
    ecPhys->add(ringList_PV[i]);

    // Ring supports
    if(i%2==0){
      for(int iSvc=0; iSvc<nbSvcSupport; iSvc++){
	
	double rminSvc = ringHelper.getRingSupportRMin(iSvc);
	double rmaxSvc = ringHelper.getRingSupportRMax(iSvc);
	double thick = ringHelper.getRingSupportThickness(iSvc);
	std::string matName = ringHelper.getRingSupportMaterial(iSvc);
	
	const GeoMaterial* supMat = basics->matMgr()->getMaterial(matName);
	const GeoTube* supTube = new GeoTube(rminSvc,rmaxSvc,thick*.5);
	GeoLogVol* _supLog = new GeoLogVol("supLog",supTube,supMat);
	GeoPhysVol* supPhys = new GeoPhysVol(_supLog);
	
	GeoTransform* xform = new GeoTransform( HepGeom::Translate3D(0., 0., (m_ringPos[i]+m_ringPos[i+1])*.5-zMiddle));
	ecPhys->add(xform);
	ecPhys->add(supPhys);
      }
    }

  }

  // Services
  if(m_IDserviceTool&&!m_IDserviceTool->svcRouteAuto()) {
    std::string region =(side>0)?"C":"A";
    std::vector<std::string> svcList;
    std::ostringstream eosName;
    eosName<< "+EOS"<<m_layer;
    std::cout<<"ENDCAP layer : build services "<<region<<" "<<eosName.str()<<std::endl;

    svcList.push_back(eosName.str());    // place EOS services in layer
    m_IDserviceTool->buildAndPlace(region, ecPhys, m_layerPosition, svcList);

  }

  //  std::cout<<"Layer minmax global : "<<m_layerZMin<<" "<<m_layerZMax<<" / "<<m_layerZMin<<" "<<m_layerZMax<<std::endl;
  return ecPhys;
}




double GeoPixelLayerECRingTool::getValueFromVector(std::vector<double> v, int i)
{
  if(i>(int)v.size()-1) return v[0];
  return v[i];
}

int GeoPixelLayerECRingTool::getValueFromVector(std::vector<int> v, int i)
{
  if(i>(int)v.size()-1) return v[0];
  return v[i];
}

std::string GeoPixelLayerECRingTool::getValueFromVector(std::vector<std::string> v, int i)
{
  if(i>(int)v.size()-1) return v[0];
  return v[i];
}
