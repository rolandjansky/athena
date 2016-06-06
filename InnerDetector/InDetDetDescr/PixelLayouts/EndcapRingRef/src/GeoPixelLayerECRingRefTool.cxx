/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EndcapRingRef/GeoPixelLayerECRingRefTool.h"
#include "EndcapRingRef/GeoPixelRingECRingRef.h"
#include "EndcapRingRef/PixelRingSupportXMLHelper.h"

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

GeoPixelLayerECRingRefTool::GeoPixelLayerECRingRefTool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_layer(-1),
    m_IDserviceTool("InDetservicesTool/PixelServicesTool"),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{
  declareInterface<IGeoPixelEndcapLayerTool>(this);
}

GeoPixelLayerECRingRefTool::~GeoPixelLayerECRingRefTool()
{
}

//================ Initialisation =================================================

StatusCode GeoPixelLayerECRingRefTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_INFO("GeoPixelLayerECRingRefTool initialize() successful in " << name());

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
  }

  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelLayerECRingRefTool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelLayerECRingRefTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelLayerECRingRefTool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

void GeoPixelLayerECRingRefTool::preBuild(const PixelGeoBuilderBasics* basics, int layer)
{
  
  m_layer = layer;
  m_ringPos.clear();
  m_ringListF.clear();
  m_ringListB.clear();

  InDet::EndcapLayerTmp* discTmp = m_xmlReader->getPixelEndcapLayerTemplate(m_layer); 

  double rLayerMin = 9999999.;
  double rLayerMax = -9999999.;
  double zLayerMin = 9999999.;
  double zLayerMax = -9999999.;

  // Retrieve PixeServiceTool
  StatusCode sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::DEBUG) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }

  // Read and store the ring parameters
  std::vector<double> v_ringPosition = discTmp->ringpos;
  std::vector<double> v_ringRadius = discTmp->innerRadius;
  std::vector<double> v_ringOuterRadius = discTmp->outerRadius;
  std::vector<double> v_zOffset = discTmp->zoffset;
  std::vector<double> v_phiOffset = discTmp->phioffset;
  std::vector<int> v_numModules = discTmp->nsectors;
  std::vector<std::string> v_moduleType = discTmp->modtype;

  int nrings = (int)v_ringPosition.size();

  // Prebuild and store the rings (F:forward / B:backward)
  std::vector<int> v_discNumber;
  std::vector<int> v_discSide;
  v_discSide.push_back(0);
  for(int iRing=0; iRing<nrings; iRing++) v_discNumber.push_back(iRing);

  for(int iRing=0; iRing<nrings; iRing++){

    std::cout<<"**** BUILD - prebuild layer - ring "<<iRing<<"/"<<nrings<<"  - layer "<<m_layer<<"   "<<std::endl;
    
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

    std::cout<<"                            - # modules & module type"<<numModules<<"  -  "<<moduleType<<"   "<<std::endl;
    
    int iSide = getValueFromVector(v_discSide,iRing);

    // Forward ring
    GeoPixelRingECRingRef ringF(m_layer,iRing,ringRadius, ringOuterRadius, zOffset, phiOffset, iSide, numModules, moduleType, discNumber, 1);
    ringF.preBuild(basics);
    m_ringListF.push_back(ringF);
    m_ringPos.push_back(ringPosition-halfZoffset+ringF.getRingZShift());

    rLayerMin = std::min(rLayerMin,ringF.getRingRMin());
    rLayerMax = std::max(rLayerMax,ringF.getRingRMax());
    zLayerMin = std::min(zLayerMin,ringPosition-halfZoffset+ringF.getRingZMin());
    zLayerMax = std::max(zLayerMax,ringPosition-halfZoffset+ringF.getRingZMax());

    // Backward ring
    GeoPixelRingECRingRef ringB(m_layer,iRing,ringRadius, ringOuterRadius, zOffset, phiOffset, iSide, numModules, moduleType, discNumber, -1);
    ringB.preBuild(basics);
    m_ringListB.push_back(ringB);
    m_ringPos.push_back(ringPosition+halfZoffset-ringB.getRingZShift());
    
    rLayerMin = std::min(rLayerMin,ringB.getRingRMin());
    rLayerMax = std::max(rLayerMax,ringB.getRingRMax());
    zLayerMin = std::min(zLayerMin,ringPosition+halfZoffset+ringB.getRingZMin());
    zLayerMax = std::max(zLayerMax,ringPosition+halfZoffset+ringB.getRingZMax());

  }

  // Open link to the xml file that describes the layer services
  PixelRingSupportXMLHelper ringHelper(basics);
  int nbSvcSupport = ringHelper.getNbLayerSupport(m_layer);
  for(int iSvc=0; iSvc<nbSvcSupport; iSvc++){
    std::vector<double> r = ringHelper.getLayerSupportRadius(iSvc);
    std::vector<double> z = ringHelper.getLayerSupportZ(iSvc);
    rLayerMin = std::min(rLayerMin,r[0]);
    rLayerMax = std::max(rLayerMax,r[1]);
    zLayerMin = std::min(zLayerMin,z[0]);
    zLayerMax = std::max(zLayerMax,z[1]);
  }

  // Further layer parameters...
  m_layerPosition = (zLayerMin+zLayerMax)*.5;
  m_layerZMin = zLayerMin-0.001;
  m_layerZMax = zLayerMax+0.001;
  m_layerRMin = rLayerMin-0.001;
  m_layerRMax = rLayerMax+0.001;

    std::cout<<"LAYER svc : "<< m_layerRMin<<" "<<m_layerRMax<<" / "<<m_layerZMin<<" "<<m_layerZMax<<std::endl;

}

GeoVPhysVol* GeoPixelLayerECRingRefTool::buildLayer(const PixelGeoBuilderBasics* basics, int layer, int side) 
{

 // Check that the prebuild phase is done
  if(layer!=m_layer) preBuild(basics,layer);

  m_layer = layer;
  m_endcapSide = side;


  InDet::EndcapLayerTmp* discTmp = m_xmlReader->getPixelEndcapLayerTemplate(m_layer); 

  std::vector<GeoVPhysVol*> ringList_PV;

  std::vector<double> v_ringPosition = discTmp->ringpos;
  int nrings = (int)v_ringPosition.size();
  std::vector<int> v_numModules = discTmp->nsectors;
   //   std::vector<double> v_ringRadius = discTmp->innerRadius;
   //   std::vector<double> v_ringOuterRadius = discTmp->outerRadius;
   //   std::vector<double> v_zOffset = discTmp->zoffset;
   //   std::vector<std::string> v_moduleType = discTmp->modtype;
  
  // Set numerology
  basics->getDetectorManager()->numerology().setNumRingsForDisk(layer,nrings);

  
//   std::vector<int> v_discNumber;
//   std::vector<int> v_discSide;
//   v_discSide.push_back(0);
//   for(int iRing=0; iRing<nrings; iRing++) v_discNumber.push_back(iRing);

  // Build rings defined in preBuild function
  for(int iRing=0; iRing<nrings; iRing++){

    std::cout<<"**** BUILD ring "<<iRing<<"/"<<nrings<<"  - layer "<<m_layer<<"   "<<std::endl;
    
    ringList_PV.push_back(m_ringListF[iRing].Build(basics,m_endcapSide));
    ringList_PV.push_back(m_ringListB[iRing].Build(basics,m_endcapSide));

    // Set numerology
    int numModules = getValueFromVector(v_numModules, iRing);
    basics->getDetectorManager()->numerology().setNumPhiModulesForDiskRing(layer,iRing,numModules);

  }

  // Build layer envelope
  const GeoMaterial* air = basics->matMgr()->getMaterial("std::Air");
  
  std::cout<<"LAYER minmax: "<<m_layerRMin<<" "<<m_layerRMax<<" / "<<m_layerZMin<<" "<<m_layerZMax<<std::endl;

  double halflength = (m_layerZMax-m_layerZMin)*.5;
  const GeoTube* ecTube = new GeoTube(m_layerRMin,m_layerRMax,halflength);
  GeoLogVol* _ecLog = new GeoLogVol("layer",ecTube,air);
  GeoFullPhysVol* ecPhys = new GeoFullPhysVol(_ecLog);
  
  double zMiddle = (m_layerZMin+m_layerZMax)*.5;

  // Read ring supports (defined in xml file)
  PixelRingSupportXMLHelper ringHelper(basics);
  int nbSvcSupport = ringHelper.getNbSupport(m_layer);

  // Place the ring in the layer envelpoe
  for(int i=0; i<2*nrings; i++)
    {
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
      
      if(i%2==0){
	// Add ring supports
	for(int iSvc=0; iSvc<nbSvcSupport; iSvc++){
	  
	  double rminSvc = ringHelper.getRingSupportRMin(iSvc);
	  double rmaxSvc = ringHelper.getRingSupportRMax(iSvc);
	  double thick = ringHelper.getRingSupportThickness(iSvc);
	  std::string matName = ringHelper.getRingSupportMaterial(iSvc);

	  const GeoTube* supTube = new GeoTube(rminSvc,rmaxSvc,thick*.5);
	  double matVolume = supTube->volume();

	  //std::cout << "Ring support?: matName = " << matName << " matVolume = " << matVolume << " rminSvc = " << rminSvc << " rmaxSvc = " << rmaxSvc << " thick = " << thick << std::endl;

	  const GeoMaterial* supMat = basics->matMgr()->getMaterialForVolume(matName,matVolume);
	  std::cout << "Density = " << supMat->getDensity() << " Mass = " << ( matVolume * supMat->getDensity() ) << std::endl;
	  GeoLogVol* _supLog = new GeoLogVol("supLog",supTube,supMat);
	  GeoPhysVol* supPhys = new GeoPhysVol(_supLog);
	  
	  GeoTransform* xform = new GeoTransform( HepGeom::Translate3D(0., 0., (m_ringPos[i]+m_ringPos[i+1])*.5-zMiddle));
	  ecPhys->add(xform);
	  ecPhys->add(supPhys);
	}
      }
    }

  // Place the layer services
  nbSvcSupport = ringHelper.getNbLayerSupport(m_layer);
  for(int iSvc=0; iSvc<nbSvcSupport; iSvc++)
    {
      std::vector<double> r = ringHelper.getLayerSupportRadius(iSvc);
      std::vector<double> z = ringHelper.getLayerSupportZ(iSvc);
      std::string matName = ringHelper.getLayerSupportMaterial(iSvc);

      const GeoTube* supTube = new GeoTube(r[0],r[1],(z[1]-z[0])*.5);
      //double matVolume = supTube->volume();

      //std::cout << "Ring layer support?: matName = " << matName << " matVolume = " << matVolume << " r[0] = " << r[0] << " r[1] = " << r[1] << " z[0] = " << z[0] << " z[1] = " << z[1] << std::endl;

      const GeoMaterial* supMat = basics->matMgr()->getMaterial(matName);
      GeoLogVol* _supLog = new GeoLogVol("supLog",supTube,supMat);
      GeoPhysVol* supPhys = new GeoPhysVol(_supLog);
      
      GeoTransform* xform = new GeoTransform( HepGeom::Translate3D(0., 0., 0.));
      ecPhys->add(xform);
      ecPhys->add(supPhys);

    }

  std::cout<<"Layer minmax global : "<<m_layerZMin<<" "<<m_layerZMax<<" / "<<m_layerZMin<<" "<<m_layerZMax<<std::endl;

  return ecPhys;
}




double GeoPixelLayerECRingRefTool::getValueFromVector(std::vector<double> v, int i)
{
  if(i>(int)v.size()-1) return v[0];
  return v[i];
}

int GeoPixelLayerECRingRefTool::getValueFromVector(std::vector<int> v, int i)
{
  if(i>(int)v.size()-1) return v[0];
  return v[i];
}

std::string GeoPixelLayerECRingRefTool::getValueFromVector(std::vector<std::string> v, int i)
{
  if(i>(int)v.size()-1) return v[0];
  return v[i];
}
