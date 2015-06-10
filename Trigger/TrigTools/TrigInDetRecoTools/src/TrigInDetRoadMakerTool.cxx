/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <iostream>
#include <list>
//#include "GaudiKernel/ToolFactory.h"
#include "IRegionSelector/IRoiDescriptor.h"
//#include "InDetIdentifier/PixelID.h" 
//#include "InDetIdentifier/SCT_ID.h" 
#include "IRegionSelector/IRegSelSvc.h"
#include "TrigInDetPattRecoEvent/TrigInDetRoad.h"
#include "TrigInDetRecoTools/TrigInDetRoadMakerTool.h"



TrigInDetRoadMakerTool::TrigInDetRoadMakerTool(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_numberingTool("TrigL2LayerNumberTool"),
  m_regionSelector("RegSelSvc", n)
{

  declareInterface< ITrigInDetRoadMakerTool >( this );

  // Region Selector usage property
  declareProperty("UseRegSel", m_useRegSel = true);

  // Layer properties
  declareProperty("LayerOneDepth", m_layerOneDepth = 2);
  declareProperty("LayerTwoDepth", m_layerTwoDepth = 1);

  // Kinematic properties
  declareProperty("MinPt", m_minPt = 2000.0);
  declareProperty("MaxD0", m_maxD0 = 3.0);

  // Binning properties
  declareProperty("EtaBin",      m_etaBin = true);
  declareProperty("ZedBin",      m_zedBin = false);
  declareProperty("PhiBin",      m_phiBin = false);
  declareProperty("EtaBinWidth", m_etaBinWidth = 0.2);
  declareProperty("ZedBinWidth", m_zedBinWidth = 100.0);
  declareProperty("PhiBinWidth", m_phiBinWidth = 0.2);

}



TrigInDetRoadMakerTool::~TrigInDetRoadMakerTool() {

}



StatusCode TrigInDetRoadMakerTool::initialize() {

  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());
  //int outputLevel = msgSvc()->outputLevel( name() );

  sc = m_regionSelector.retrieve();
  if(sc.isFailure()) {
    athenaLog << MSG::FATAL << "Unable to retrieve RegionSelector tool " << m_regionSelector.type() << endreq;
    return sc;
  }

  athenaLog << MSG::INFO << "TrigInDetRoadMakerTool initialized" << endreq;

  return sc;

}



StatusCode TrigInDetRoadMakerTool::finalize() {

  StatusCode sc = AthAlgTool::finalize(); 
  return sc;

}



StatusCode TrigInDetRoadMakerTool::makeRoads(std::vector<TrigInDetRoad>& roads, const IRoiDescriptor* roi) {

  //roads should be empty - check
  // Get log service
  MsgStream athenaLog(msgSvc(), name());
  if (!roads.empty()) {
    athenaLog << MSG::DEBUG << "TrigInDetRoadMakerTool: roads not empty, roads.size() " << roads.size() << endreq;
    roads.clear();
  }

  // Statuscode
  StatusCode sc;

  //int outputLevel = msgSvc()->outputLevel(name());

  // Get minimum and maximum eta from RoI descriptor
  float etaMin, etaMax, phiMin, phiMax, zedMin, zedMax;
  if(roi!=nullptr) {
    etaMin = roi->etaMinus();
    etaMax = roi->etaPlus();
    phiMin = roi->phiMinus();
    phiMax = roi->phiPlus();
    zedMin = roi->zedMinus();
    zedMax = roi->zedPlus();
  } else {
    etaMin = -2.5;
    etaMax = 2.5;
    phiMin = -M_PI;
    phiMax = M_PI;
    zedMin = -300.0;
    zedMax = 300.0;
  }
    
  // LOOP OVER POSSIBLE BINS
  unsigned int roadId=0;

  // Calculate eta bin parameters and default values if no binning has to be applied
  unsigned int nEtaBins=1;
  float etaBinWidth=etaMax-etaMin;
  if(m_etaBin) {
    // Bin parameters
    nEtaBins = ceil((etaMax - etaMin) / m_etaBinWidth);
    etaBinWidth = m_etaBinWidth;
    // Adjusted range (possibly exceeding the original one)
    float newEtaHalfWidth = nEtaBins*m_etaBinWidth/2.0;
    etaMin = (etaMax+etaMin)/2.0 - newEtaHalfWidth;
    etaMax = etaMin + 2.0*newEtaHalfWidth;
  }
  
  // Calculate zed bin parameters and default values if no binning has to be applied
  unsigned int nZedBins=1;
  float zedBinWidth=zedMax-zedMin;
  if(m_zedBin && m_useRegSel) {
    // Bin parameters
    nZedBins = ceil((zedMax - zedMin) / m_zedBinWidth);
    zedBinWidth = m_zedBinWidth;
    // Adjusted range (possibly exceeding the original one)
    float newZedHalfWidth = nZedBins*m_zedBinWidth/2.0;
    zedMin = (zedMax+zedMin)/2.0 - newZedHalfWidth;
    zedMax = zedMin + 2.0*newZedHalfWidth;
  }
  
  // Loop over zed bins
  for(unsigned int loopZed=0; loopZed<nZedBins; loopZed++) {
    
    // Calculate zed bin boundaries
    float loopZedMin = zedMin + loopZed*zedBinWidth;
    float loopZedMax = zedMin + (loopZed+1)*zedBinWidth;
    
    // Loop over eta bins
    for(unsigned int loopEta=0; loopEta<nEtaBins; loopEta++) {
      
      // Calculate bin boundaries
      float loopEtaMin = etaMin + loopEta*etaBinWidth;
      float loopEtaMax = etaMin + (loopEta+1)*etaBinWidth;
      
      // Create roads
      int bcount1, bcount2; // Barrel layer counters
      int ecount1, ecount2; // Endcap layer counters      
      
      // Get list of layers to be used
      std::set<unsigned int> l;
      

      if(m_useRegSel) {
	sc = getRoadLayersRS( TrigRoiDescriptor( 0.5*(loopEtaMin+loopEtaMax), loopEtaMin, loopEtaMax, 
						 0, -0.4, 0.4,
						 0.5*(loopZedMin+loopZedMax), loopZedMin, loopZedMax ), l);
      }    
      else sc = getRoadLayersLUT(loopEtaMin, loopEtaMax, l);
      if(sc.isFailure()) athenaLog << MSG::ERROR << "TrigInDetRoadMakerTool: error retrieving layer set";
      
      // Debug print
      athenaLog << MSG::DEBUG << "TrigInDetRoadMakerTool: starting from layer set ";
      std::set<unsigned int>::iterator lay, layEnd=l.end();
      for(lay=l.begin(); lay!=layEnd; lay++) {
	athenaLog << MSG::DEBUG << (*lay) << " ";
      }
      athenaLog << MSG::DEBUG << endreq;
      
      // Loop on first layer
      unsigned int roadSubId=0;
      std::set<unsigned int>::iterator lay1, lay2, lay3;
      for(bcount1=0, ecount1=0, lay1=l.begin(); (lay1!=l.end())&&(bcount1<m_layerOneDepth||ecount1<m_layerOneDepth); lay1++) {
	if(*lay1<7) if(bcount1<m_layerOneDepth) bcount1++; else continue;
	else if(ecount1<m_layerOneDepth) ecount1++; else continue;
	
	// Loop on second layer
	for(bcount2=0, ecount2=0,lay2=lay1,lay2++; (lay2!=l.end())&&(bcount2<m_layerTwoDepth||ecount2<m_layerTwoDepth); lay2++) {
	  if(*lay2<*lay1) continue;
	  if(*lay2<7) if(bcount2<m_layerTwoDepth) bcount2++; else continue;
	  else if(ecount2<m_layerTwoDepth) ecount2++; else continue;
	  
	  // Create ROAD
	  TrigInDetRoad road(1, m_minPt, m_maxD0);
	  road.m_roadId=roadId;
	  road.m_roadSubId=roadSubId;
	  road.m_eta=(loopEtaMin+loopEtaMax)/2.0; road.m_etaMin=loopEtaMin; road.m_etaMax=loopEtaMax;
	  road.m_phi=(phiMin+phiMax)/2.0; road.m_phiMin=phiMin; road.m_phiMax=phiMax;
	  road.m_z0=(loopZedMin+loopZedMax)/2.0; road.m_z0Min=loopZedMin; road.m_z0Max=loopZedMax;
	  road.m_firstLayer.insert(*lay1);
	  road.m_secondLayer.insert(*lay2);
	  
	  // Third layer
	  for(lay3=lay2, lay3++; lay3!=l.end(); lay3++) {
	    if(*lay3<*lay2) continue;
	    
	    // Insert third layer
	    road.m_thirdLayer.insert(*lay3);
	    
	  }
	  
	  // Save ROAD
	  if(road.m_thirdLayer.size() > 0) {
	    roads.push_back(road);
	    roadSubId++;
	    // Debug print
	    athenaLog << MSG::DEBUG << "TrigInDetRoadMakerTool: road eta/phi/z0/id/subid " <<
	      road.m_eta << " " << road.m_phi << " " << road.m_z0 << " " << road.m_roadId << " " << road.m_roadSubId << ": " <<
	      (*(road.m_firstLayer.begin())) << " --> " << (*(road.m_secondLayer.begin())) << " --> ";
	    std::set<unsigned int>::iterator it, itEnd=road.m_thirdLayer.end();
	    for(it=road.m_thirdLayer.begin(); it!=itEnd; it++) {
	      athenaLog << MSG::DEBUG << (*it) << " ";
	    }
	    athenaLog << MSG::DEBUG << endreq;
	  }

	}
      }
      
      // Increase RoI Id counter
      roadId++;
      
    }
  }

  return sc;

}



StatusCode TrigInDetRoadMakerTool::getRoadLayersRS( const IRoiDescriptor& roi, std::set<unsigned int>& l) {

  // Get log service
  MsgStream athenaLog(msgSvc(), name());

  athenaLog << MSG::DEBUG << "Created RoI with eta/min/max " << roi << endreq;

  for(long i=0; i<m_numberingTool->offsetBarrelSCT(); i++) {
    std::vector<IdentifierHash> idList;
    m_regionSelector->DetHashIDList(PIXEL, 1000+i, roi, idList);
    //athenaLog << MSG::DEBUG << "Size for layer " << i << " is " << idList.size() << endreq;
    if(idList.size()) l.insert(i);
  }
  
  for(long i=0; i<m_numberingTool->offsetEndcapSCT()-m_numberingTool->offsetEndcapPixels(); i++) {
    std::vector<IdentifierHash> idList;
    m_regionSelector->DetHashIDList(PIXEL, 1200+i, roi, idList);
    //athenaLog << MSG::DEBUG << "Size for layer " << i+7 << " is " << idList.size() << endreq;
    if(idList.size()) l.insert(i+m_numberingTool->offsetEndcapPixels());
  }

  for(long i=0; i<m_numberingTool->offsetEndcapPixels()-m_numberingTool->offsetBarrelSCT(); i++) {
    std::vector<IdentifierHash> idList;
    m_regionSelector->DetHashIDList(SCT, 1000+i, roi, idList);
    //athenaLog << MSG::DEBUG << "Size for layer " << i+3 << " is " << idList.size() << endreq;
    if(idList.size()) l.insert(i+m_numberingTool->offsetBarrelSCT());
  }
  
  for(long i=0; i<m_numberingTool->maxSiliconLayerNum()-m_numberingTool->offsetEndcapSCT(); i++) {
    std::vector<IdentifierHash> idList;
    m_regionSelector->DetHashIDList(SCT, 1200+i, roi, idList);
    //athenaLog << MSG::DEBUG << "Size for layer " << i+10 << " is " << idList.size() << endreq;
    if(idList.size()) l.insert(i+m_numberingTool->offsetEndcapSCT());
  }

  return StatusCode::SUCCESS;

}



StatusCode TrigInDetRoadMakerTool::getRoadLayersLUT(float etaMin, float etaMax, std::set<unsigned int>& l) {

  // Fill layer groups
  std::vector< std::list<unsigned int> > layerGroups;
  std::list<unsigned int> gr1,gr2,gr3,gr4,gr5,gr6,gr7,gr8,gr9; // layer groups as a function of eta

  /* Group definitions
     gr1:         eta<1        0  1  2  3  4  5   6
     gr2:      1<=eta<1.5      0  1  2  3  4  5   6   10  11  12  13
     gr3:    1.5<=eta<1.7      0  1  2  3  4  5   7   10  12  12  13  14
     gr4:    1.7<=eta<1.8      0  1  2  3  4  7   10  11  12  13  14  15
     gr5:    1.8<=eta<1.9      0  1  2  3  7  8   11  12  13  14  15  16
     gr6:    1.9<=eta<2.1      0  1  2  3  7  8   9   11  12  13  14  15  16  17
     gr7:    2.1<=eta<2.2      0  1  2  7  8  9   11  12  13  14  15  16  17
     gr8:    2.2<=eta<2.4      0  1  2  7  8  9   13  14  15  16  17  18
     gr9:         eta>=2.4     0  9  15  16  17  18
  */
 
  gr1.push_back(0); gr1.push_back(1); gr1.push_back(2); gr1.push_back(3); gr1.push_back(4); gr1.push_back(5); gr1.push_back(6);

  gr2.push_back(0); gr2.push_back(1); gr2.push_back(2); gr2.push_back(3); gr2.push_back(4); gr2.push_back(5); gr2.push_back(6);
  gr2.push_back(10); gr2.push_back(11); gr2.push_back(12); gr2.push_back(13);

  gr3.push_back(0); gr3.push_back(7); gr3.push_back(1); gr3.push_back(2);
  gr3.push_back(3); gr3.push_back(4); gr3.push_back(5); gr3.push_back(10); gr3.push_back(11); gr3.push_back(12); gr3.push_back(13); gr3.push_back(14);

  gr4.push_back(0); gr4.push_back(7); gr4.push_back(1); gr4.push_back(2);
  gr4.push_back(3); gr4.push_back(4); gr4.push_back(10); gr4.push_back(11); gr4.push_back(12); gr4.push_back(13); gr4.push_back(14); gr4.push_back(15);

  gr5.push_back(0); gr5.push_back(7); gr5.push_back(1); gr5.push_back(8); gr5.push_back(2);
  gr5.push_back(3); gr5.push_back(11); gr5.push_back(12); gr5.push_back(13); gr5.push_back(14); gr5.push_back(15); gr5.push_back(16);

  gr6.push_back(0); gr6.push_back(7); gr6.push_back(1); gr6.push_back(8);  gr6.push_back(2); gr6.push_back(9);
  gr6.push_back(3); gr6.push_back(11); gr6.push_back(12); gr6.push_back(13); gr6.push_back(14); gr6.push_back(15); gr6.push_back(16); gr6.push_back(17);

  gr7.push_back(0); gr7.push_back(7); gr7.push_back(1); gr7.push_back(8);  gr7.push_back(2); gr7.push_back(9);
  gr7.push_back(11); gr7.push_back(12); gr7.push_back(13); gr7.push_back(14); gr7.push_back(15); gr7.push_back(16); gr7.push_back(17);

  gr8.push_back(0); gr8.push_back(7); gr8.push_back(1); gr8.push_back(8);  gr8.push_back(2); gr8.push_back(9);
  gr8.push_back(13); gr8.push_back(14); gr8.push_back(15); gr8.push_back(16); gr8.push_back(17); gr8.push_back(18);

  gr9.push_back(0); gr9.push_back(9);
  gr9.push_back(15); gr9.push_back(16); gr9.push_back(17); gr9.push_back(18);

  layerGroups.push_back(gr1);
  layerGroups.push_back(gr2);
  layerGroups.push_back(gr3);
  layerGroups.push_back(gr4);
  layerGroups.push_back(gr5);
  layerGroups.push_back(gr6);
  layerGroups.push_back(gr7);
  layerGroups.push_back(gr8);
  layerGroups.push_back(gr9);

  float absetaMin=0, absetaMax=2.5;
  if(etaMin>0)            {absetaMin = etaMin;  absetaMax = etaMax;}
  if(etaMin<=0&&etaMax>0) {absetaMin = 0;       absetaMax = (etaMax>-etaMin) ? etaMax : -etaMin;}
  if(etaMax<=0)           {absetaMin = -etaMax; absetaMax = -etaMin;}
  
  int imin=1,imax=9;
  if (absetaMin<1.)                     imin=1;
  if (absetaMin>=1.  && absetaMin<1.5)  imin=2;
  if (absetaMin>=1.5 && absetaMin<1.7)  imin=3;
  if (absetaMin>=1.7 && absetaMin<1.8)  imin=4;
  if (absetaMin>=1.8 && absetaMin<1.9)  imin=5;
  if (absetaMin>=1.9 && absetaMin<2.1)  imin=6;
  if (absetaMin>=2.1 && absetaMin<2.2)  imin=7;
  if (absetaMin>=2.2 && absetaMin<2.4)  imin=8;
  if (absetaMin>=2.4)                   imin=9;
  if (absetaMax<1.)                     imax=1;
  if (absetaMax>=1.  && absetaMax<1.5)  imax=2;
  if (absetaMax>=1.5 && absetaMax<1.7)  imax=3;
  if (absetaMax>=1.7 && absetaMax<1.8)  imax=4;
  if (absetaMax>=1.8 && absetaMax<1.9)  imax=5;
  if (absetaMax>=1.9 && absetaMax<2.1)  imax=6;
  if (absetaMax>=2.1 && absetaMax<2.2)  imax=7;
  if (absetaMax>=2.2 && absetaMax<2.4)  imax=8;
  if (absetaMax>=2.4)                   imax=9;
    
  // Build list of layers to be combined
  for(int i=imin-1; i<imax;i++) {
    std::list<unsigned int>::iterator it, itEnd=layerGroups[i].end();
    for(it=layerGroups[i].begin(); it!=itEnd; it++)
      l.insert(*it);
  }
    
  return StatusCode::SUCCESS;
  
}

