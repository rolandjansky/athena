/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelLayoutLoI/GeoPixelDiskLoI.h"
#include "PixelLayoutLoI/GeoPixelRingLoI.h"
#include "PixelLayoutLoI/PixelDiscSupportXMLHelper.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
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

#include "PathResolver/PathResolver.h"

#include <sstream>


GeoPixelDiskLoI::GeoPixelDiskLoI(const PixelGeoBuilderBasics* basics, int endcapSide)
  : PixelGeoBuilder(basics), 
    m_endcapSide(endcapSide),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{
  preBuild();
}

GeoPixelDiskLoI::~GeoPixelDiskLoI()
{

}

void GeoPixelDiskLoI::preBuild()
{
  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
  }
}


GeoVPhysVol* GeoPixelDiskLoI::Build( ) 
{

  InDet::EndcapLayerTmp* discTmp = m_xmlReader->getPixelEndcapLayerTemplate(m_disk); 

  double rDiskMin = 9999999.;
  double rDiskMax = -9999999.;
  double zDiskMin = 9999999.;
  double zDiskMax = -9999999.;

  std::vector<GeoVPhysVol*> ringList;
  std::vector<double> ringPos;
  std::vector<int> ringSide;

  std::vector<double> v_ringPosition = discTmp->ringpos;
  std::vector<double> v_ringRadius = discTmp->innerRadius;
  std::vector<double> v_zOffset = discTmp->zoffset;
  std::vector<int> v_numModules = discTmp->nsectors;
  std::vector<std::string> v_moduleType = discTmp->modtype;

  int nrings = (int)v_ringPosition.size();

  double zMidRing= 0.;
  for(int i=0; i<nrings; i++)zMidRing+=v_ringPosition[i];
  zMidRing = zMidRing/(double)nrings;

  // Set numerology
  getBasics()->getDetectorManager()->numerology().setNumRingsForDisk(m_disk,nrings);

  // Loop over rings for a given disc
  for(int iRing=0; iRing<nrings; iRing++){

    std::cout<<"**** BUILD ring "<<iRing<<"/"<<nrings<<"  - disc "<<m_disk<<"   "<<std::endl;
    
    //    int ringNumber = v_ringNumber[iRing];
    double ringPosition = v_ringPosition[iRing];
    double ringRadius = v_ringRadius[iRing]; 
    double zOffset = v_zOffset[iRing]; 
    int numModules = v_numModules[iRing];
    std::string moduleType = v_moduleType[iRing];
    
    // Set numerology
    getBasics()->getDetectorManager()->numerology().setNumPhiModulesForDiskRing(m_disk,iRing,numModules);
    
    double halfOffset = zOffset*.5;
    int iSide=0;
    if(ringPosition + halfOffset <zMidRing)iSide =1;
    else if(ringPosition - halfOffset > zMidRing)iSide=-1;
    ringSide.push_back(iSide);

    std::cout<<"BUILD ring : "<<ringPosition<<" "<<zMidRing<<" -> "<<iSide<<std::endl;

    // Forward ring
    GeoPixelRingLoI ringF(getBasics(),m_endcapSide,m_disk,iRing,ringRadius, zOffset, iSide, numModules, moduleType, 1);
    ringList.push_back(ringF.Build());
    ringPos.push_back(ringPosition-halfOffset);
    
    rDiskMin = std::min(rDiskMin,ringF.getRingRMin());
    rDiskMax = std::max(rDiskMax,ringF.getRingRMax());
    zDiskMin = std::min(zDiskMin,ringPosition-zOffset+ringF.getRingZMin());
    zDiskMax = std::max(zDiskMax,ringPosition-zOffset+ringF.getRingZMax());

    // Backward ring
    GeoPixelRingLoI ringB(getBasics(),m_endcapSide,m_disk,iRing,ringRadius, zOffset, iSide, numModules, moduleType, -1);
    ringList.push_back(ringB.Build());
    ringPos.push_back(ringPosition+halfOffset);
    
    rDiskMin = std::min(rDiskMin,ringB.getRingRMin());
    rDiskMax = std::max(rDiskMax,ringB.getRingRMax());
    zDiskMin = std::min(zDiskMin,ringPosition+zOffset+ringB.getRingZMin());
    zDiskMax = std::max(zDiskMax,ringPosition+zOffset+ringB.getRingZMax());


  }

  // Read services from xml file
  PixelDiscSupportXMLHelper discHelper;
  int nbSvcSupport = discHelper.getNbSupport(m_disk);
  for(int iSvc=0; iSvc<nbSvcSupport; iSvc++){
    
    double rmin = discHelper.getRingSupportRMin(iSvc);
    double rmax = discHelper.getRingSupportRMax(iSvc);
    //    double thick = discHelper.getRingSupportThickness(iSvc);

    rDiskMin = std::min(rDiskMin,rmin);
    rDiskMax = std::max(rDiskMax,rmax);
  }


  // Build the endcap disc envelope
  m_diskPosition = (zDiskMin+zDiskMax)*.5;

  const GeoMaterial* air = matMgr()->getMaterial("std::Air");  
  double halflength = (zDiskMax-zDiskMin)*.5;
  const GeoTube* ecTube = new GeoTube(rDiskMin,rDiskMax,halflength);
  GeoLogVol* _ecLog = new GeoLogVol("ecLog",ecTube,air);
  GeoFullPhysVol* ecPhys = new GeoFullPhysVol(_ecLog);
  
  // Place the rings
  for(int i=0; i<2*nrings; i++){

    double zPos = ringPos[i]-m_diskPosition;
    GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::TranslateZ3D(zPos));
    
    int tagId = i;
    std::ostringstream ostr; 
    ostr << "PixelECRing";
    ecPhys->add(new GeoNameTag(ostr.str()));
    ecPhys->add( new GeoIdentifierTag(tagId));
    ecPhys->add(xform);
    ecPhys->add(ringList[i]);

  }

  // Add disc support (read from xml file)
  for(int iSvc=0; iSvc<nbSvcSupport; iSvc++){
    
    double rmin = discHelper.getRingSupportRMin(iSvc);
    double rmax = discHelper.getRingSupportRMax(iSvc);
    double thick = discHelper.getRingSupportThickness(iSvc);
    std::string matName = discHelper.getRingSupportMaterial(iSvc);

    const GeoMaterial* supMat = matMgr()->getMaterial(matName);
    const GeoTube* supTube = new GeoTube(rmin,rmax,thick*.5);
    GeoLogVol* _supLog = new GeoLogVol("supLog",supTube,supMat);
    GeoPhysVol* supPhys = new GeoPhysVol(_supLog);
    
    GeoTransform* xform = new GeoTransform( HepGeom::Translate3D(0., 0., 0.));
    ecPhys->add(xform);
    ecPhys->add(supPhys);
  }

  return ecPhys;

}
