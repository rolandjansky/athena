/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 
#include "PixelServicesTool/ServicesTrackerBuilder.h"
#include "PixelServicesTool/ServicesDynTracker.h"

#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "AthenaKernel/MsgStreamMember.h"
#include <string>

#include <iostream>  // for DEBUG only
using namespace std;

ServicesTrackerBuilder::ServicesTrackerBuilder()
  : m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc"),
    m_pixelModuleSvc("PixelModuleSvc","PixelModuleSvc")
{


}

ServicesDynTracker* ServicesTrackerBuilder::buildGeometry(const PixelGeoBuilderBasics* basics, bool bBarrelModuleMaterial) const
{
  // Question - what is difference between using msg<<MSG::DEBUG and msg(MSG::DEBUG)?
  const Athena::MsgStreamMember msg(Athena::Options::Eager,"ServiceTrackerBuilder");

  // retrieve the xml interface 
  bool bXMLReaderDefined = true;
  StatusCode sc = m_xmlReader.retrieve();
  if (sc.isSuccess()){
    msg << MSG::DEBUG<<"ITkXMLReader successfully retrieved " << m_xmlReader << endmsg;
  } else {
    msg<<MSG::WARNING<<"ITkXMLReader: Couldn't retrieve " << m_xmlReader <<endmsg;
    bXMLReaderDefined = false;
  }

  if(!bXMLReaderDefined) return 0;
  
  msg << MSG::INFO << "Entering ServicesTrackerBuilder::buildGeometry" <<endmsg;

  ServicesDynTracker* tracker = new ServicesDynTracker(basics,bBarrelModuleMaterial);
  msg << MSG::DEBUG << "Created new ServicesDynTracker()" << endmsg;

  std::string suffix;
  
  // Build pixel barrel layers
  int numLayers = m_xmlReader->nbOfPixelBarrelLayers();
  msg << MSG::DEBUG << "Created new ServicesDynTracker() # barrel layers " << numLayers << endmsg;

  for (int i = 0; i < numLayers; i++) {

    InDet::BarrelLayerTmp *layerTmp = m_xmlReader->getPixelBarrelLayerTemplate(i);
    //    std::vector<InDet::StaveTmp*> staveTmp = m_xmlReader->getPixelStaveTemplate(i);
    std::vector<InDet::StaveTmp*> staveTmp = m_xmlReader->getPixelStaveTemplate(i);

    // ST in principle stave template are different, build material properties for each
    for (size_t is = 0; is < staveTmp.size(); is++) {

      std::string moduleType = staveTmp[is]->b_type; 
      GeoDetModulePixel* barrelModule = m_pixelModuleSvc->getModule(basics,2,i,moduleType);
      int nChipsPerModule = barrelModule->ChipNumber();
      
      if (i == 0)      suffix = "L0";
      else if (i == 1) suffix = "L1";
      else             suffix = "Outer";
      
      int modulesPerStave = staveTmp[is]->b_modn;
      
      int endcapModuleNumber =  2*staveTmp[is]->alp_pos.size();
      int transModuleNumber = 4*staveTmp[is]->trans_pos.size();     // TODO: 2 transition modules per side ?  
      
      double layerRadius =  layerTmp->radius;
      double layerLength =  staveTmp[is]->support_halflength;
      double staveZOffset = layerTmp->stave_zoffset;
      int nSectors = 1.*layerTmp->stave_n/staveTmp.size();                             // number of sectors built from 1 stave template
      
      msg << MSG::DEBUG << "Created new ServicesDynTracker() : barrel layer " << i<<" "
		        << layerRadius << "with " 
		        << nSectors << " sectors, stave template "<< is 
		        << " # modules "<<modulesPerStave
		        <<"  // length : "<<layerLength
		        << endmsg;
      
      if (endcapModuleNumber==0){
	tracker->constructBarrelLayer( layerRadius, layerLength+2.*basics->epsilon(), staveZOffset,
				       DetTypeDyn::Pixel, i, is, nSectors, suffix,
				       modulesPerStave, nChipsPerModule);
      }
      else {
	std::vector<int> modulesPerStave_vec;
	std::vector<int> nChipsPerModule_vec;
	modulesPerStave_vec.push_back(modulesPerStave);
	if(transModuleNumber>0) modulesPerStave_vec.push_back(transModuleNumber);
	modulesPerStave_vec.push_back(endcapModuleNumber);
	
	nChipsPerModule_vec.push_back(nChipsPerModule);
	if(transModuleNumber>0){
	  std::string moduleType = staveTmp[is]->trans_type;
	  GeoDetModulePixel* module = m_pixelModuleSvc->getModule(basics,2,i,moduleType);
	  int nChipsPerModule = module->ChipNumber();
	  nChipsPerModule_vec.push_back(nChipsPerModule);
	}
	std::string moduleType = staveTmp[is]->alp_type;
	GeoDetModulePixel* module = m_pixelModuleSvc->getModule(basics,2,i,moduleType);
	int nChipsPerModule = module->ChipNumber();
	nChipsPerModule_vec.push_back(nChipsPerModule);
	
	tracker->constructBarrelLayer( layerRadius, layerLength+2.*basics->epsilon(), staveZOffset,
				       DetTypeDyn::Pixel, i, is, nSectors, suffix,
				       modulesPerStave_vec, nChipsPerModule_vec);
      }
    }

  }

  // Build endcap pixel layers

  int m_ndisks = m_xmlReader->nbOfPixelEndcapLayers();
  std::vector<int> ringDisk;
  std::vector<double> ringDiskZpos;
  std::vector<int> ringDiskId;
  std::vector<int> ringId;
  std::vector<int> ringROL;    // readout layer

  msg << MSG::DEBUG << "Created new ServicesDynTracker() # endcap layers " << m_ndisks << endmsg;

  int rolMax = 0;

  for (int iDisc = 0; iDisc < m_ndisks; iDisc++) {

    InDet::EndcapLayerTmp* discTmp = m_xmlReader->getPixelEndcapLayerTemplate(iDisc); 
    bool isDisc = discTmp->isDisc;

    suffix = "Outer"; // endcap pixel disks use same services as outer barrel pixel

    double zPos, rMin, rMax;
    int nbModTot, nbChipTot;

    // one route per disc
    if(isDisc) {
      zPos = 0;
      std::vector<double> zOffset = discTmp->zoffset;
      std::vector<double> ringPos = discTmp->ringpos;
      int numRing = (int)zOffset.size();
      for (int j=0; j<numRing; j++) zPos += (ringPos[j]+zOffset[j]);
      zPos /= (double)(numRing);
      
      rMin = 999999.; rMax = 0.;
      nbChipTot = 0; nbModTot=0;
      std::vector<double> rInner = discTmp->innerRadius;
      std::vector<std::string> modType = discTmp->modtype;
      std::vector<int> nSectors = discTmp->nsectors;
      for(int j=0; j<numRing; j++){
	rMin = std::min(rMin,rInner[j]);
	
	msg<<MSG::DEBUG<<"ServicesTrackerBuilder - build disc : "<<iDisc<<"  "<<modType[j]<<endmsg;
	
	GeoDetModulePixel* endcapModule = m_pixelModuleSvc->getModule(basics,2,iDisc,modType[j]);
	nbModTot += nSectors[j];
	nbChipTot += nSectors[j]*endcapModule->ChipNumber();
	
	rMax=std::max(rMax, ComputeRMax(rInner[j], 0.0001, endcapModule->Length(), endcapModule->Width()));
	
      }
      
      tracker->constructEndcapLayer( zPos, rMin, rMax,
				     DetTypeDyn::Pixel, iDisc,
				     numRing,
				     suffix,
				     (int)nbModTot/numRing, (int)(nbModTot/nbChipTot));   // SES fixme
    }
    else {
      ringDisk.push_back(iDisc);
      std::vector<double> zOffset = discTmp->zoffset;
      std::vector<double> ringPos = discTmp->ringpos;
      std::vector<int> ringReadoutLayer = discTmp->readoutLayer;
      int numRing = (int)ringPos.size();
      for(int j=0; j<numRing; j++){
	ringDiskZpos.push_back(ringPos[j]+zOffset[j]);
	ringDiskId.push_back(iDisc);
	ringId.push_back(j);
        ringROL.push_back(ringReadoutLayer[j]);
        if (ringReadoutLayer[j]>rolMax) rolMax = ringReadoutLayer[j] ;
      }
    }
  }

  // Special case : ring disks. We have to create the disk following their zpos (smaller->higher value)
  // Special case : ring layers. We have to create the disk following their zpos (smaller->higher value)
  
  if (!ringDisk.empty()) {
    //      int numDisk = (int)ringDisk.size();
    std::vector<double>sortedRingDiskZpos(ringDiskZpos);
    std::sort(sortedRingDiskZpos.begin(), sortedRingDiskZpos.end());
    sortedRingDiskZpos.erase( unique( sortedRingDiskZpos.begin(), sortedRingDiskZpos.end() ), sortedRingDiskZpos.end() );
    
    std::vector<std::string> trkLayerNumber;
    for (int iDisc = 0; iDisc < m_ndisks; iDisc++){
      std::ostringstream os;
      os<<"Disc trk layer indices "<<iDisc<<" : ";
      trkLayerNumber.push_back(os.str());
    }

    std::vector<std::string> trkROLayerNumber;
    for (int iROL = 0; iROL <= rolMax; iROL++){
      std::ostringstream os;
      os<<"Disc trk readout layer indices "<<iROL<<" : ";
      trkROLayerNumber.push_back(os.str());
    }
   
    int iCmpt=0;
    // Looping over all the ring defined in the discs
    for(int i=0; i<(int)sortedRingDiskZpos.size(); i++){
      
      // Looking for dic/ring indices corresponding to zPos	double zPos = ringDiskZpos[i];
      double zPos = sortedRingDiskZpos[i];
      std::vector<int> discIndexList;
      std::vector<int> ringIndexList;
      for (int iDisc = 0; iDisc < m_ndisks; iDisc++) {
	for(int k=0; k<(int)ringDiskZpos.size(); k++)
	  if(ringDiskId[k]==iDisc) {
	    double tmp= fabs(zPos-ringDiskZpos[k]);
	    if(tmp<0.001) {
	      discIndexList.push_back(iDisc); 
	      ringIndexList.push_back(ringId[k]);
	    }
	  }
      }
      // repeat operation for readout layers
      std::vector<int> roLayIndexList;
      std::vector<int> roRingIndexList;
      for (int iROL = 0; iROL <= rolMax; iROL++) {
	for(int k=0; k<(int)ringDiskZpos.size(); k++)
	  if(ringROL[k]==iROL) {
	    double tmp= fabs(zPos-ringDiskZpos[k]);
	    if(tmp<0.001) {
	      roLayIndexList.push_back(iROL); 
	      roRingIndexList.push_back(ringId[k]);
	    }
	  }
      }
      
      for(int  iDisc = 0; iDisc < (int)discIndexList.size(); iDisc++) {
	
	int discIndex = discIndexList[iDisc];
	int ringIndex = ringIndexList[iDisc];
	
	InDet::EndcapLayerTmp* discTmp = m_xmlReader->getPixelEndcapLayerTemplate(discIndex); 
	
	std::vector<double> zOffset = discTmp->zoffset;
	std::vector<double> ringPos = discTmp->ringpos;
	std::vector<double> rInner = discTmp->innerRadius;
	std::vector<std::string> modType = discTmp->modtype;
	std::vector<int> nSectors = discTmp->nsectors;
	std::vector<int> roLayer = discTmp->readoutLayer;
	//	int numRing = (int)zOffset.size();
	
	zPos = ringPos[ringIndex]+zOffset[ringIndex];
	double rMin = rInner[ringIndex];
	GeoDetModulePixel* endcapModule = m_pixelModuleSvc->getModule(basics,2,iDisc,modType[ringIndex]);  // same module for a disc
	//	double rMax = rMin+endcapModule->Length();
	double rMax=ComputeRMax(rMin, 0.0001, endcapModule->Length(), endcapModule->Width());
	int nbModTot = nSectors[ringIndex];
	int nbChipPerModule = endcapModule->ChipNumber();

        // ordering readout layer
	std::ostringstream osl;
	osl<<" "<<iCmpt;
	trkROLayerNumber[roLayer[ringIndex]]+=osl.str();
	
        // ordering GM layers
	std::ostringstream os;
	os<<" "<<iCmpt;
	trkLayerNumber[discIndex]+=os.str();
	tracker->constructEndcapLayer( zPos, rMin, rMax,
				       DetTypeDyn::Pixel, iCmpt++,
				       1,
				       suffix,
				       nbModTot, nbChipPerModule);


      }
    }
    
    for (int iDisc = 0; iDisc < m_ndisks; iDisc++) msg<<MSG::DEBUG<<trkLayerNumber[iDisc]<<endmsg;
    
    msg<<MSG::DEBUG<<"Ring assignement to readout layers: "<<endmsg;
    for (int iROL = 0; iROL <=rolMax ; iROL++) msg<<MSG::DEBUG<<trkROLayerNumber[iROL]<<endmsg;
  }
  
	

  /*
  // Build sct barrel layers
  suffix = ""; // strip layers have identical services (so far)
  for (int i = 0; i < geoMgr.sctNumLayers(); i++) {
    DetTypeDyn::Type type;
    if ( geoMgr.sctLayerType(i) == 1)  type = DetTypeDyn::ShortStrip;
    else                               type = DetTypeDyn::LongStrip;

    int modulesPerStave = geoMgr.sctModulesPerLadder(i)/2; // we need only half-stave modules for services

    tracker->constructBarrelLayer( geoMgr.sctLayerRadius(i), 0.5*geoMgr.sctLayerLength(i),
				   type, i, geoMgr.sctNumSectorsForLayer(i), suffix,
				   modulesPerStave, 4);
  }
  // SCT endcap
  for (int i = 0; i < geoMgr.sctNumDisks(); i++) {
    DetTypeDyn::Type type = DetTypeDyn::ShortStrip;  // FIXME: meaningless for a disk, but needed to satisfy interface

    int nModulesPerSector = 16;  // FIXME: hard-wired number!
    int nChipsPerModule = 1;     // FIXME: hard-wired number!

    tracker->constructEndcapLayer( geoMgr.sctDiskZ(i), geoMgr.sctInnerSupport(), 
				   geoMgr.sctDiskRMax(i), type, i,
				   geoMgr.sctEndcapNumSectorsForLayer(i), suffix,
				   nModulesPerSector, nChipsPerModule);
  }

  tracker->setGeoMgr(&geoMgr);
  */
  
  return tracker;
  
}

double ServicesTrackerBuilder::ComputeRMax(double rMin, double safety, double moduleLength, double moduleWidth) const
{
  double xCorner = moduleWidth*.5;
  double yCorner = rMin + moduleLength;
  
  double ringRmax = sqrt(xCorner*xCorner+yCorner*yCorner);
  return ringRmax + std::abs(safety);
}
