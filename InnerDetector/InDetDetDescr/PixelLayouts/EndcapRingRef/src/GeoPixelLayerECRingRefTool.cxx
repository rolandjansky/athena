/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "EndcapRingRef/GeoPixelLayerECRingRefTool.h"
#include "EndcapRingRef/GeoPixelRingECRingRef.h"
#include "EndcapRingRef/PixelRingSupportXMLHelper.h"

#include "PixelInterfaces/IPixelServicesTool.h"
#include "PixelInterfaces/IGeoBCMPrimeTool.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"
#include "BCMPrime/BCMPrimeXMLHelper.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"

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
    m_bcmTool("GeoBCMPrimeTool"),
    m_IDserviceTool("InDetservicesTool/PixelServicesTool"),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{
  declareInterface<IGeoPixelEndcapLayerTool>(this);

  declareProperty("GeoBCMPrimeTool",m_bcmTool);
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
  m_ringFBzshift.clear();     

  
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
  std::vector<double> v_rOffset = discTmp->roffset;
  std::vector<double> v_phiOffset = discTmp->phioffset;
  std::vector<int> v_numModules = discTmp->nsectors;
  std::vector<std::string> v_moduleType = discTmp->modtype;
  std::vector<std::string> v_splitMode = discTmp->splitMode;
  std::vector<double> v_splitOffset = discTmp->splitOffset;
  std::vector<double> v_inclination = discTmp->inclination;
  std::vector<std::string> v_readoutRegion = discTmp->readoutRegion;
  std::vector<int> v_readoutLayer = discTmp->readoutLayer;
  std::vector<int> v_readoutEta = discTmp->readoutEta;

  int nrings = (int)v_ringPosition.size();

  // Prebuild and store the rings (F:forward / B:backward)
  std::vector<int> v_discNumber;
  std::vector<int> v_discSide;
  v_discSide.push_back(0);
  for(int iRing=0; iRing<nrings; iRing++) v_discNumber.push_back(iRing);
        
  for(int iRing=0; iRing<nrings; iRing++){
        
    ATH_MSG_DEBUG("**** BUILD - prebuild layer - ring "<<iRing<<"/"<<nrings<<"  - layer "<<m_layer);
    
    //    int ringNumber = iRing;
    int discNumber = getValueFromVector(v_discNumber, iRing);
    double ringPosition = getValueFromVector(v_ringPosition, iRing);
    double ringRadius = getValueFromVector(v_ringRadius, iRing); 
    double ringOuterRadius = getValueFromVector(v_ringOuterRadius, iRing); 
    double zOffset = getValueFromVector(v_zOffset, iRing); 
    double rOffset = getValueFromVector(v_rOffset, iRing); 
    double phiOffset = getValueFromVector(v_phiOffset, iRing);
    int numModules = getValueFromVector(v_numModules, iRing);
    std::string moduleType = getValueFromVector(v_moduleType, iRing);
    double halfZoffset = zOffset*.5;
    double splitOffset   = getValueFromVector(v_splitOffset, iRing);
    double halfSplitOffset = splitOffset*0.5;
    double inclination = getValueFromVector(v_inclination, iRing);
    
    SplitMode splitMode  = getSplitMode(getValueFromVector(v_splitMode,iRing));

    std::string roRegion = getValueFromVector(v_readoutRegion, iRing);
    int roLayer = getValueFromVector(v_readoutLayer, iRing);
    int roEta = getValueFromVector(v_readoutEta, iRing);
	
    ATH_MSG_DEBUG("                            - # modules & module type"<<numModules<<"  -  "<<moduleType<<"   ");
    
    int iSide = getValueFromVector(v_discSide,iRing);


    // Forward ring
    GeoPixelRingECRingRef ringF(m_layer,iRing,ringRadius, ringOuterRadius, zOffset, rOffset,phiOffset, iSide, numModules, moduleType, discNumber, 1, splitMode, inclination);
    ringF.preBuild(basics);
    ringF.readoutId(roRegion,roLayer,roEta);
    m_ringListF.push_back(ringF);
    m_ringPos.push_back(ringPosition-halfZoffset+ringF.getRingZShift());
    m_ringFBzshift.push_back(-halfZoffset+ringF.getRingZShift()); 

    rLayerMin = std::min(rLayerMin,ringF.getRingRMin()-fabs(0.5*rOffset));
    rLayerMax = std::max(rLayerMax,ringF.getRingRMax()+fabs(0.5*rOffset));
    zLayerMin = std::min(zLayerMin,ringPosition-halfZoffset-halfSplitOffset + ringF.getRingZMin()  );
    zLayerMax = std::max(zLayerMax,ringPosition-halfZoffset+halfSplitOffset + ringF.getRingZMax()  );

    // Backward ring
    GeoPixelRingECRingRef ringB(m_layer,iRing,ringRadius, ringOuterRadius, zOffset, rOffset, phiOffset, iSide, numModules, moduleType, discNumber, -1, splitMode, inclination);
    ringB.preBuild(basics);
    ringB.readoutId(roRegion,roLayer,roEta);
    m_ringListB.push_back(ringB);
    m_ringPos.push_back(ringPosition+halfZoffset-ringB.getRingZShift());
    m_ringFBzshift.push_back(+halfZoffset-ringB.getRingZShift()); 

    rLayerMin = std::min(rLayerMin,ringB.getRingRMin()-fabs(0.5*rOffset));
    rLayerMax = std::max(rLayerMax,ringB.getRingRMax()+fabs(0.5*rOffset));
    zLayerMin = std::min(zLayerMin,ringPosition+halfZoffset-halfSplitOffset + ringB.getRingZMin() );
    zLayerMax = std::max(zLayerMax,ringPosition+halfZoffset+halfSplitOffset + ringB.getRingZMax() );
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


  for(int iRing=0; iRing<nrings; iRing++){
    nbSvcSupport = ringHelper.getNbSupport(m_layer,iRing);
    for(int iSvc=0; iSvc<nbSvcSupport; iSvc++){
      double rmin = ringHelper.getRingSupportRMin(iSvc);
      double rmax = ringHelper.getRingSupportRMax(iSvc);
      
      rLayerMin = std::min(rLayerMin,rmin);
      rLayerMax = std::max(rLayerMax,rmax);
    }
  }
  

  // Further layer parameters...
  m_layerPosition = (zLayerMin+zLayerMax)*.5;
  m_layerZMin = zLayerMin-0.001;
  m_layerZMax = zLayerMax+0.001;
  m_layerRMin = rLayerMin-0.001;
  m_layerRMax = rLayerMax+0.001;

  ATH_MSG_DEBUG("LAYER svc : "<< m_layerRMin<<" "<<m_layerRMax<<" / "<<m_layerZMin<<" "<<m_layerZMax);

}

GeoVPhysVol* GeoPixelLayerECRingRefTool::buildLayer(const PixelGeoBuilderBasics* basics, int layer, int side) 
{

 // Check that the prebuild phase is done coherently
  if(layer!=m_layer || m_endcapSide!=side ) preBuild(basics,layer);   

  m_layer = layer;
  m_endcapSide = side;

  InDet::EndcapLayerTmp* discTmp = m_xmlReader->getPixelEndcapLayerTemplate(m_layer); 

  std::vector<GeoFullPhysVol*> ringList_PV;

  std::vector<double> v_ringPosition = discTmp->ringpos;
  int nrings = (int)v_ringPosition.size();
  std::vector<int> v_numModules = discTmp->nsectors;
  std::vector<double> v_rOffset = discTmp->roffset;
  std::vector<std::string> v_splitMode = discTmp->splitMode;
  std::vector<double> v_splitOffset    = discTmp->splitOffset;
  std::vector<double> v_inclination    = discTmp->inclination;
  std::vector<int> v_readoutLayer      = discTmp->readoutLayer;
  std::vector<int> v_readoutEta        = discTmp->readoutEta;  
 
  // Set numerology
  for (auto sublay :  discTmp->readoutSublayers)
    basics->getDetectorManager()->numerology().setNumRingsForDisk(sublay.layer,sublay.rings.size());
	
  // Build rings defined in preBuild function
  for(int iRing=0; iRing<nrings; iRing++){
    
    SplitMode splitMode  = getSplitMode(getValueFromVector(v_splitMode,iRing));
    double inclination = getValueFromVector(v_inclination, iRing);
    double rOffset = getValueFromVector(v_rOffset, iRing); 
    
    if (splitMode==NONE) {
      // strategy for common envelope for inclined F+B+ring support : build it while building inclined Forward ring, pass an an argument 
      ATH_MSG_DEBUG("**** BUILD ENTIRE ring "<<iRing<<"/"<<nrings<<"  - layer "<<m_layer<<"   ");
      double zshift = (inclination !=0) ? m_ringFBzshift[2*iRing] : 0;
      GeoFullPhysVol* envelope = m_ringListF[iRing].Build(basics,m_endcapSide,0,zshift,-0.5*rOffset);
      if (inclination==0) { ringList_PV.push_back(envelope); envelope=0; }
      zshift = (inclination !=0) ? m_ringFBzshift[2*iRing+1] : 0;
      ringList_PV.push_back(m_ringListB[iRing].Build(basics,m_endcapSide, envelope,zshift,0.5*rOffset));
    
    } else {
      if (inclination !=0) {
        ATH_MSG_DEBUG("**** split inclined ring not implemented yet, skip "<<iRing<<"/"<<nrings<<"  - layer "<<m_layer<<"   ");
        continue; 
      }      
      ATH_MSG_DEBUG("**** BUILD SPLIT ring "<<iRing<<"/"<<nrings<<"  - layer "<<m_layer<<"   ");
      std::pair<GeoFullPhysVol*,GeoFullPhysVol*> element_F = m_ringListF[iRing].BuildSplit(basics,m_endcapSide);
      std::pair<GeoFullPhysVol*,GeoFullPhysVol*> element_B = m_ringListB[iRing].BuildSplit(basics,m_endcapSide);
      ringList_PV.push_back(element_F.first);
      ringList_PV.push_back(element_F.second);
      ringList_PV.push_back(element_B.first);
      ringList_PV.push_back(element_B.second);
    
    }
    
    // Set numerology
    //std::string roRegion = getValueFromVector(v_readoutRegion, iRing);
    int roLayer = getValueFromVector(v_readoutLayer, iRing);
    int roEta = getValueFromVector(v_readoutEta, iRing);
    int numModules = getValueFromVector(v_numModules, iRing);
    basics->getDetectorManager()->numerology().setNumPhiModulesForDiskRing(roLayer,roEta,numModules); 
  
  }
  
  
  // Build layer envelope
  const GeoMaterial* air = basics->matMgr()->getMaterial("std::Air");
  
  ATH_MSG_DEBUG("LAYER minmax: "<<m_layerRMin<<" "<<m_layerRMax<<" / "<<m_layerZMin<<" "<<m_layerZMax);

  double halflength = (m_layerZMax-m_layerZMin)*.5;
  const GeoTube* ecTube = new GeoTube(m_layerRMin,m_layerRMax,halflength);
  std::ostringstream laystr;
  laystr << "ECLayer_L"<<m_layer;
  GeoLogVol* _ecLog = new GeoLogVol(laystr.str(),ecTube,air);
  GeoFullPhysVol* ecPhys = new GeoFullPhysVol(_ecLog);
  
  double zMiddle = (m_layerZMin+m_layerZMax)*.5;

  // Read pixel general parameters (needed by BCM')
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML", basics);

  // Read ring supports (defined in xml file)
  PixelRingSupportXMLHelper ringHelper(basics);

  // Place the ring in the layer envelpoe
  // i runs over rings here and for all positions etc vectors, but for phys vol
  
  std::vector<double> v_zOffset = discTmp->zoffset;

  if (v_inclination[0]!=0) {

    for(int i=0; i<nrings; i++) {

      double zPos = getValueFromVector(v_ringPosition, i)-zMiddle;	  
      GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::TranslateZ3D(zPos));
      int tagId = i;
      std::ostringstream ostr;
      ostr << "InclinedRing" << i<<"_L"<<m_layer;
      
      ecPhys->add(new GeoNameTag(ostr.str()));
      ecPhys->add( new GeoIdentifierTag(tagId));
      ecPhys->add(xform);
      ecPhys->add(ringList_PV[i]);
    }  // end loop over ring layers
	  
  } else {
  
    for(int i=0; i<2*nrings; i++)
      {
	// each face of ring has two half-rings
	int ihalfr = 2*i;
	
	double zOffset = getValueFromVector(v_zOffset, int(floor(i/2)));
	int numModules = getValueFromVector(v_numModules, int(floor(i/2)));
	double splitOffset   = getValueFromVector(v_splitOffset, int(floor(i/2)));
	SplitMode splitMode  = getSplitMode(getValueFromVector(v_splitMode,int(floor(i/2))));
	
	double halfSplitOffset = splitOffset*0.5;
	bool halfIsEven = (numModules/2)%2==0;
	
	if (splitMode==NONE) {
	  
	  double zPos = m_ringPos[i]-zMiddle;
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
	  
	} else {
	  
	  for (int iTwdAway=0; iTwdAway<2;++iTwdAway) {
	    ihalfr +=iTwdAway;
	    bool isTwdBSShift=(0==iTwdAway);
	    double zPos = m_ringPos[i]-zMiddle;
          if ( ringHelper.putLHBeforeRHrings())	{
            zPos += (isTwdBSShift) ? halfSplitOffset : -halfSplitOffset;  
          } else {zPos += (isTwdBSShift) ? -halfSplitOffset : halfSplitOffset;}
	    bool swap = false;
	    if (halfIsEven && (splitMode==MIDDLE || splitMode==GOOD) && !isTwdBSShift) 
	      swap = true;
	    else if (!halfIsEven && ((splitMode==MIDDLE && isTwdBSShift) || splitMode==GOOD))
	      swap = true;
	    
	    if (swap)
	      zPos+= i%2==0 ? zOffset : -zOffset;
	    GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::TranslateZ3D(zPos));
	    
	    int tagId = i;
	    std::ostringstream ostr; 
	    std::string sTwdAway=(isTwdBSShift) ? "Twd" : "Away";
	    if(i%2==0)
	      ostr << "RingF" << sTwdAway<<i<<"L"<<m_layer;
	    else
	      ostr << "RingB" << sTwdAway<<i<<"L"<<m_layer;
	    
	    ecPhys->add(new GeoNameTag(ostr.str()));
	    ecPhys->add(new GeoIdentifierTag(tagId));
	    ecPhys->add(xform);
	    ecPhys->add(ringList_PV[ihalfr]);
	  }
      
	}

	int nbSvcSupport = ringHelper.getNbSupport(m_layer,int(floor(i/2)));
	
	if(nbSvcSupport>0){      
	  if (splitMode==NONE && i%2==0) {	  
	    // Add ring supports
	    for (int iSvc = 0; iSvc < nbSvcSupport; iSvc++){
	      double rminSvc = ringHelper.getRingSupportRMin(iSvc);
	      double rmaxSvc = ringHelper.getRingSupportRMax(iSvc);
	      double thick = ringHelper.getRingSupportThickness(iSvc);
	      int nsectors = ringHelper.getRingSupportNSectors(iSvc);
	      double sphiSvc = ringHelper.getRingSupportSPhi(iSvc);
	      double dphiSvc = ringHelper.getRingSupportDPhi(iSvc);
	      std::string matName = ringHelper.getRingSupportMaterial(iSvc);	
	      
	      for (int i_sector = 0; i_sector < nsectors; i_sector++) {
		
		if ((360. / nsectors) < dphiSvc) {
		  ATH_MSG_WARNING("Arms will overlap. Do not implement them.");
		  continue;
		}
		
		double Sphi  = (sphiSvc + 360. / nsectors * i_sector) * CLHEP::deg;
		double Dphi  = dphiSvc * CLHEP::deg;
		
		const GeoShape* supTubs = (nsectors>1) ? dynamic_cast<const GeoShape*> (new GeoTubs(rminSvc,rmaxSvc,thick*.5,Sphi,Dphi)) : dynamic_cast<const GeoShape*> (new GeoTube(rminSvc,rmaxSvc,thick*.5));
		double matVolume = supTubs->volume();
		const GeoMaterial* supMat = basics->matMgr()->getMaterialForVolume(matName,matVolume);
		ATH_MSG_DEBUG("Density = " << supMat->getDensity() << " Mass = " << ( matVolume * supMat->getDensity() ));
		GeoLogVol* _supLog = new GeoLogVol("supLog",supTubs,supMat);
		GeoPhysVol* supPhys = new GeoPhysVol(_supLog);
		GeoTransform* xform = new GeoTransform( HepGeom::Translate3D(0., 0., (m_ringPos[i]+m_ringPos[i+1])*.5-zMiddle));
		ecPhys->add(xform);
		ecPhys->add(supPhys);
	      }
	    }
	  }
	  
	  if (splitMode!=NONE && i%2==0){
	    // here starts if we have the 2 tubs when we split the rings
	    int iSvc = i/2<nbSvcSupport ? i/2 : nbSvcSupport-1;
	    double rminSvc = ringHelper.getRingSupportRMin(iSvc);
	    double rmaxSvc = ringHelper.getRingSupportRMax(iSvc);
	    double thick = ringHelper.getRingSupportThickness(iSvc);
	    std::string matName = ringHelper.getRingSupportMaterial(iSvc);	
	    
	    double SphiTwd  = 90.*CLHEP::deg;
	    double DphiTwd  = 180.*CLHEP::deg;
	    double SphiAway = 270.*CLHEP::deg;
	    double DphiAway = 180.*CLHEP::deg;
	  
	    // adapt the extension of the support to fit the modules if needed
	    const GeoTubs* myhalfTubeTwd = dynamic_cast<const GeoTubs*> (ringList_PV[2*i]->getLogVol()->getShape());
	    if (myhalfTubeTwd) {
	      SphiTwd = myhalfTubeTwd->getSPhi();
	      DphiTwd = myhalfTubeTwd->getDPhi();
	      if (rminSvc>myhalfTubeTwd->getRMin()) rminSvc =  myhalfTubeTwd->getRMin();
	      if (rmaxSvc<myhalfTubeTwd->getRMax()) rmaxSvc =  myhalfTubeTwd->getRMax();
	    }
	    const GeoTubs* myhalfTubeAway = dynamic_cast<const GeoTubs*> (ringList_PV[2*i+1]->getLogVol()->getShape());
	    if (myhalfTubeAway) {
	      SphiAway = myhalfTubeAway->getSPhi();
	      DphiAway = myhalfTubeAway->getDPhi();
	      if (rminSvc>myhalfTubeAway->getRMin()) rminSvc =  myhalfTubeAway->getRMin();
	      if (rmaxSvc<myhalfTubeAway->getRMax()) rmaxSvc =  myhalfTubeAway->getRMax();
	    }
	  
	    // supports half-rings supports with z-gap for services
	    // to accomodate for services, this half-ring will get a z-shift toward beam-spot
	    // NB: geometry has 90deg offset wrt ATLAS coordinate system phi
	    const GeoTubs* supTubsTwdBS = new GeoTubs(rminSvc,rmaxSvc,thick*.5, SphiTwd, DphiTwd);
	    // to accomodate for services, this half-ring will get a z-shift away from beam-spot
	    //GeoTubs* tubs = new GeoTubs(rmin,rmax,halfthick, startangle,delta_angle);
	    const GeoTubs* supTubsAwayBS = new GeoTubs(rminSvc,rmaxSvc,thick*.5, SphiAway, DphiAway);
	    double matVolumeT = supTubsTwdBS->volume();
	    double matVolumeA = supTubsAwayBS->volume();
	    
	    // volume & material of both supports should be the same 
	    const GeoMaterial* supMatT = basics->matMgr()->getMaterialForVolume(matName,matVolumeT);
	    const GeoMaterial* supMatA = basics->matMgr()->getMaterialForVolume(matName,matVolumeA);
	    ATH_MSG_DEBUG("TWD  Density of = " << supMatT->getDensity() << " Mass = " << ( matVolumeT * supMatT->getDensity() ));
	    ATH_MSG_DEBUG("AWAY Density of = " << supMatA->getDensity() << " Mass = " << ( matVolumeA * supMatA->getDensity() ));
	    
	    GeoLogVol* _supLogTwdBS = new GeoLogVol("supLogTwdBS",supTubsTwdBS,supMatT);
	    GeoPhysVol* supPhysTwdBS = new GeoPhysVol(_supLogTwdBS);
	    GeoLogVol* _supLogAwayBS = new GeoLogVol("supLogAwayBS",supTubsAwayBS,supMatA);
	    GeoPhysVol* supPhysAwayBS = new GeoPhysVol(_supLogAwayBS);
	    
	    GeoTransform* xformTwdBS;
            GeoTransform* xformAwayBS;
            if ( ringHelper.putLHBeforeRHrings()) {
                  xformTwdBS = new GeoTransform( HepGeom::Translate3D(0., 0., (m_ringPos[i]+m_ringPos[i+1])*.5-zMiddle+halfSplitOffset));
                  xformAwayBS = new GeoTransform( HepGeom::Translate3D(0., 0., (m_ringPos[i]+m_ringPos[i+1])*.5-zMiddle-halfSplitOffset));
            } else {
                  xformTwdBS = new GeoTransform( HepGeom::Translate3D(0., 0., (m_ringPos[i]+m_ringPos[i+1])*.5-zMiddle-halfSplitOffset));
                  xformAwayBS = new GeoTransform( HepGeom::Translate3D(0., 0., (m_ringPos[i]+m_ringPos[i+1])*.5-zMiddle+halfSplitOffset));

           }
	    ecPhys->add(xformTwdBS);
	    ecPhys->add(supPhysTwdBS);
	    
	    ecPhys->add(xformAwayBS);
	    ecPhys->add(supPhysAwayBS);
	  }
	}

        // Place BCM'

        bool bcmPresent   = genDBHelper.isBCMPrimePresent();

        if (bcmPresent) {

          BCMPrimeXMLHelper BCMPrimeDBHelper(0, basics);
          int bcmRing = BCMPrimeDBHelper.getECRingNumber();
          int bcmRingB = 2*bcmRing;

          if (i==bcmRingB && m_layer==0) {

            int nModules = BCMPrimeDBHelper.getNumberOfModules();
            int uniqueGeoIdentifier = 11950; // Unique geo identifier, same as in Run2 sim
                                              // Do we still need to assign GeoIdentifier 
                                              // tags in the upgrade simulation?

            for (int module = 0; module < nModules; module++) {

              GeoPhysVol* bcmModPhys = m_bcmTool->buildModule( module, basics);
              if (bcmModPhys) {

                double safety = 0.001;
                double ringOffset = m_bcmTool->getRingOffset();
                double bcmZPos = m_ringPos[i]-zMiddle-ringOffset-safety;

                ATH_MSG_DEBUG("Placing BCM' module " << module << ", side " << m_endcapSide);

                CLHEP::Hep3Vector pos(m_bcmTool->getTransX(), m_bcmTool->getTransY(), bcmZPos);
                CLHEP::HepRotation rm;
                rm.rotateY(90*CLHEP::deg);
                rm.rotateX(-m_bcmTool->getTilt()*CLHEP::deg);
                rm.rotateX(m_bcmTool->getRotX()*CLHEP::deg);
                rm.rotateY(m_bcmTool->getRotY()*CLHEP::deg);
                rm.rotateZ((m_bcmTool->getRotZ()+m_bcmTool->getRingRot())*CLHEP::deg);

                int k = 2*module + uniqueGeoIdentifier;
                GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(rm,pos.rotateZ(m_bcmTool->getRingRot()*CLHEP::deg)));
                GeoNameTag *tag = new GeoNameTag("BCM Module");
                ecPhys->add(tag);
                ecPhys->add(new GeoIdentifierTag(k));
                ecPhys->add(xform);
                ecPhys->add(bcmModPhys);

              }
            }
          }
        }  // BCM' placement finished

      }
    }
    
    // Place the layer supports
    std::vector<int> Layers = ringHelper.getNbLayerSupportIndex(m_layer);
    for(unsigned int iSvc=0; iSvc<Layers.size(); iSvc++) {
      
      if (Layers.at(iSvc)<0) continue;

      std::vector<double> r = ringHelper.getLayerSupportRadiusAtIndex(Layers.at(iSvc));
      std::vector<double> z = ringHelper.getLayerSupportZAtIndex(Layers.at(iSvc));
      std::string matName = ringHelper.getLayerSupportMaterialAtIndex(Layers.at(iSvc));

      const GeoTube* supTube = new GeoTube(r[0],r[1],(z[1]-z[0])*.5);
      const GeoMaterial* supMat = basics->matMgr()->getMaterial(matName);
      GeoLogVol* _supLog = new GeoLogVol("supLog",supTube,supMat);
      GeoPhysVol* supPhys = new GeoPhysVol(_supLog);
      
      GeoTransform* xform = new GeoTransform( HepGeom::Translate3D(0., 0., (z[0]+z[1])*0.5-zMiddle));
      ecPhys->add(xform);
      ecPhys->add(supPhys);

      ATH_MSG_DEBUG(" placing layer support for layer:"<<m_layer<<":"<<r[0]<<","<<r[1]<<":"<<z[0]<<":"<<z[1]<<":"<<matName);
    }

    // Place the shell supports
    std::vector<double> rShell = ringHelper.getLayerShellRadius(m_layer);
    if (!rShell.size()) return ecPhys;    // no shell defined
    double sTh =  rShell.size()<2 ? ringHelper.getLayerShellThickness(m_layer) :  rShell[1]-rShell[0];  
    if (sTh<=0) ATH_MSG_WARNING("shell support wrongly defined for layer "<<m_layer<<", thickness ="<<sTh);
    std::vector<double> zShell = ringHelper.getLayerShellZBounds(m_layer);
    if (!zShell.size()) { zShell.push_back(m_layerZMin);  zShell.push_back(m_layerZMax); }
    std::string shellMatName = ringHelper.getLayerShellMaterial(m_layer);
       
    const GeoTube* supTube = new GeoTube(rShell[0]-0.5*sTh,rShell[0]+0.5*sTh,(zShell[1]-zShell[0])*.5);
    const GeoMaterial* supMat = basics->matMgr()->getMaterialForVolume(shellMatName,supTube->volume());
    GeoLogVol* _supLog = new GeoLogVol("supLog",supTube,supMat);
    GeoPhysVol* supPhys = new GeoPhysVol(_supLog);      
    GeoTransform* xform = new GeoTransform( HepGeom::Translate3D(0., 0., (zShell[0]+zShell[1])*0.5-zMiddle));
    ecPhys->add(xform);
    ecPhys->add(supPhys);

    ATH_MSG_DEBUG("placing simple shell support for layer:"<<m_layer<<":"<<rShell[0]<<","<<rShell[1]<<":"<<zShell[0]<<":"<<zShell[1]<<":"<<shellMatName);       
     
    ATH_MSG_DEBUG("Layer minmax global : "<<m_layerZMin<<" "<<m_layerZMax<<" / "<<m_layerZMin<<" "<<m_layerZMax);
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

SplitMode GeoPixelLayerECRingRefTool::getSplitMode(std::string mode) {
  ATH_MSG_DEBUG("Decoding SplitMode for " << mode);

  if (mode == "None")
    return NONE;
  else if (mode == "Default")
    return DEFAULT;
  else if (mode == "Middle")
    return MIDDLE;
  else if (mode == "Good")
    return GOOD;
  else 
    ATH_MSG_WARNING("SplitMode " << mode << " not defined. Check the inputs or updated GeoPixelLayerECRingRefTool::getSplitMode... Returning ring w/o split.");
  
  return NONE;
  
}
