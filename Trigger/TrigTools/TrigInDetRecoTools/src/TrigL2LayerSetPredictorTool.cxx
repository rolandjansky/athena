/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <iostream>
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "TrigL2LayerSetPredictorTool.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"

TrigL2LayerSetPredictorTool::TrigL2LayerSetPredictorTool(const std::string& t, 
							 const std::string& n,
							 const IInterface*  p ): 
  AthAlgTool(t,n,p),
  //m_regionSelector("RegSelSvc", n), 
  m_numberingTool("TrigL2LayerNumberTool"),
  m_pLUT(NULL) {

  declareInterface< ITrigL2LayerSetPredictorTool >( this ); 
  declareProperty( "LayerNumberTool", m_numberingTool  );
  declareProperty( "MaxEta", m_maxEta = 2.6  );
  declareProperty( "MaxZ0",  m_maxZ0 =  230.0  );
  declareProperty( "NBinsZ0",  m_nBinsZ =  101  );
  declareProperty( "NBinsTheta",  m_nBinsTheta =  101  );
}

StatusCode TrigL2LayerSetPredictorTool::initialize() {

  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  sc=m_numberingTool.retrieve(); 

  if(sc.isFailure()) {
    athenaLog << MSG::FATAL << "Could not retrieve "<<m_numberingTool<<endreq;
    return StatusCode::FAILURE;
   } 
  /*
  sc = m_regionSelector.retrieve();
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL << "Could not retrieve "<<m_regionSelector.type()<<endreq;
    return StatusCode::FAILURE;
  }
  */
  //Get ID helpers
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    athenaLog << MSG::FATAL << "Could not get AtlasDetectorID helper AtlasID"<<endreq;
    return StatusCode::FAILURE;
  }
  
  if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) {
    athenaLog << MSG::FATAL << "Could not get Pixel ID helper"<<endreq;
    return StatusCode::FAILURE;
  }
  
  if (detStore()->retrieve(m_sctId, "SCT_ID").isFailure()) { 
    athenaLog << MSG::FATAL << "Could not get Pixel ID helper"<<endreq;
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_pixelManager);  
  if( sc.isFailure() ) {
    athenaLog << MSG::FATAL << "Could not retrieve Pixel DetectorManager from detStore."<<endreq; 
    return sc;
  }

  sc = detStore()->retrieve(m_SCT_Manager);
  if( sc.isFailure() ) {
    athenaLog << MSG::ERROR << "Could not retrieve SCT DetectorManager from detStore." << endreq;
    return sc;
  }

  athenaLog << MSG::INFO << "TrigL2LayerSetPredictorTool initialized "<< endreq;
  return sc;
}


StatusCode TrigL2LayerSetPredictorTool::finalize() {

  StatusCode sc = AthAlgTool::finalize(); 
  delete m_pLUT;
  return sc;
}

StatusCode TrigL2LayerSetPredictorTool::createLUT() {

  StatusCode sc(StatusCode::SUCCESS);
  MsgStream athenaLog(msgSvc(), name());

  athenaLog << MSG::INFO <<"creating LUT for layer set prediction"<<endreq;

  int maxLayer = m_numberingTool->maxSiliconLayerNum();

  std::map<int, TRIG_L2_LAYER_DESCRIPTOR> layerMap;

  for(int layerId=0;layerId<maxLayer;layerId++) {
    int type = (layerId>=m_numberingTool->offsetEndcapPixels()) ? 1 : 0;
    layerMap.insert(std::pair<int,TRIG_L2_LAYER_DESCRIPTOR>(layerId,TRIG_L2_LAYER_DESCRIPTOR(type)));
    if(type==1) {
      layerMap.insert(std::pair<int,TRIG_L2_LAYER_DESCRIPTOR>(-layerId,TRIG_L2_LAYER_DESCRIPTOR(type)));
    }
  }

  for(int SubDetId=0;SubDetId<2;SubDetId++) {
    
    const InDetDD::SiNumerology& siNumerology = (SubDetId==0) ? m_pixelManager->numerology() : m_SCT_Manager->numerology(); 
    int nSides = 1;
    if(SubDetId==1) nSides = 2;

    for (int iBarrelIndex = 0; iBarrelIndex < siNumerology.numBarrels(); iBarrelIndex++) {
      int iBarrel = siNumerology.barrelId(iBarrelIndex);
      for (int iLayer = 0; iLayer < siNumerology.numLayers(); iLayer++) {
	for (int iPhi = 0; iPhi < siNumerology.numPhiModulesForLayer(iLayer); iPhi++) {
	  for (int iEta = siNumerology.beginEtaModuleForLayer(iLayer); iEta < siNumerology.endEtaModuleForLayer(iLayer); iEta++) {
	    if (!iEta && siNumerology.skipEtaZeroForLayer(iLayer)) continue;
	    for (int iSide = 0; iSide < nSides; iSide++) {
	      Identifier id = (SubDetId==0) ? m_pixelId->wafer_id(iBarrel,iLayer,iPhi,iEta) :
		m_sctId->wafer_id(iBarrel,iLayer,iPhi,iEta,iSide);
	      const InDetDD::SiDetectorElement* element = (SubDetId==0) ? m_pixelManager->getDetectorElement(id) : m_SCT_Manager->getDetectorElement(id);

	      if(element==0) {
		athenaLog << MSG::INFO<<"Barrel element not found: DetId="<<SubDetId<<" iB="<<iBarrel<<" iL="<<iLayer<<" iPhi="<<iPhi<<" iEta="<<iEta<<endreq;
		if (SubDetId==0) athenaLog << MSG::INFO<<"element id="<<id<<" : "<<m_pixelId->print_to_string(id)<<endreq;
		continue;
	      }
	      int layerId = (SubDetId==0) ? iLayer : iLayer+m_numberingTool->offsetBarrelSCT();

	      if(layerMap.find(layerId)==layerMap.end()) {
		std::cout<<"ERROR: layer Id="<<layerId<<" not found in the map"<<std::endl;
	      }
	      else {
		(*layerMap.find(layerId)).second.m_nElements++;
		(*layerMap.find(layerId)).second.m_refCoordinate += element->rMin();
		(*layerMap.find(layerId)).second.m_refCoordinate += element->rMax();
		double& lMin = (*layerMap.find(layerId)).second.m_minCoordinate;
		if(lMin>element->zMin()) lMin=element->zMin();
		double& lMax = (*layerMap.find(layerId)).second.m_maxCoordinate;
		if(lMax<element->zMax()) lMax=element->zMax();
	      } 
	      // For extra safety in case some strip modules do not have two sides (eg in future geometries) one could add.
	      if (!element->otherSide()) iSide++;     
	    } // iSide
	  } // iEta
	} //iPhi
      } //iLayer
    } // Barrel
    // Endcap
    for (int iEndcapIndex = 0; iEndcapIndex < siNumerology.numEndcaps(); iEndcapIndex++) {
      int iEndcap = siNumerology.endcapId(iEndcapIndex);
      for (int iDisk = 0; iDisk < siNumerology.numDisks(); iDisk++) {
       for (int iEta = 0; iEta < siNumerology.numRingsForDisk(iDisk); iEta++) {
         for (int iPhi = 0; iPhi < siNumerology.numPhiModulesForDiskRing(iDisk,iEta); iPhi++) {
           for (int iSide = 0; iSide < nSides; iSide++) {
	     Identifier id = (SubDetId==0) ? m_pixelId->wafer_id(iEndcap,iDisk,iPhi,iEta) :
	       m_sctId->wafer_id(iEndcap,iDisk,iPhi,iEta,iSide);
             const InDetDD::SiDetectorElement* element = (SubDetId==0) ? m_pixelManager->getDetectorElement(id) : m_SCT_Manager->getDetectorElement(id);

	     if(element==0) {
	       continue;
	     }

	     int layerId = (SubDetId==0) ? iDisk+m_numberingTool->offsetEndcapPixels() : iDisk+m_numberingTool->offsetEndcapSCT();
	     if(iEndcap<0) layerId=-layerId;
	     if(layerMap.find(layerId)==layerMap.end()) {
		std::cout<<"ERROR: layer Id="<<layerId<<" not found in the map"<<std::endl;
	      }
	      else {
		(*layerMap.find(layerId)).second.m_nElements++;
		(*layerMap.find(layerId)).second.m_refCoordinate += element->zMin();
		(*layerMap.find(layerId)).second.m_refCoordinate += element->zMax();
		double& lMin = (*layerMap.find(layerId)).second.m_minCoordinate;
		if(lMin>element->rMin()) lMin=element->rMin();
		double& lMax = (*layerMap.find(layerId)).second.m_maxCoordinate;
		if(lMax<element->rMax()) lMax=element->rMax();
	      }
             // For extra safety in case some strip modules do not have two sides (eg in future geometries) one could add.
	     if (!element->otherSide()) iSide++;   
	   } // iSide
	 } // iEta
       } //iPhi
      } //iDisk
    } // Endcap;
  }

  for(int layerId=-maxLayer+1;layerId<maxLayer;layerId++) {
    if(layerMap.find(layerId) == layerMap.end()) continue;
    (*layerMap.find(layerId)).second.m_refCoordinate/=2.0*(*layerMap.find(layerId)).second.m_nElements;
    //std::cout<<"Layer "<<layerId<<" has "<<(*layerMap.find(layerId)).second.m_nElements<<" elements"<<std::endl;
    //std::cout<<"ref coord="<<(*layerMap.find(layerId)).second.m_refCoordinate<<" min="<<(*layerMap.find(layerId)).second.m_minCoordinate<<
    //  " max="<<(*layerMap.find(layerId)).second.m_maxCoordinate<<std::endl;
  }

  //now building the LUT 

  double thetaMax = 2.0*atan(exp(-m_maxEta));
  double tauMax = cos(thetaMax)/sin(thetaMax);
  int nbT = (m_nBinsTheta-1)/2;
  double dTau = 2*tauMax/m_nBinsTheta;

  int nbZ = (m_nBinsZ-1)/2;
  double dZ = 2*m_maxZ0/m_nBinsZ;

  std::vector<LAYER_IMPACT_POINT>* vArray = new std::vector<LAYER_IMPACT_POINT>[m_nBinsZ*m_nBinsTheta];
  for(int i=0;i<m_nBinsZ;i++) {
    for(int j=0;j<m_nBinsTheta;j++) {
      int idx = j + i*m_nBinsTheta;
      vArray[idx].clear();
    }
  }

  for(int layerId=-maxLayer+1;layerId<maxLayer;layerId++) {
    const std::map<int, TRIG_L2_LAYER_DESCRIPTOR>::iterator lIt = layerMap.find(layerId);
    if(lIt == layerMap.end()) continue;

    if((*lIt).second.m_type==0) { // barrel
      for(int j=-nbT;j<=nbT;j++) {
	double tau = j*dTau;
	double lx = (*lIt).second.m_refCoordinate*sqrt(1.0+tau*tau);
	double z0max = (*lIt).second.m_maxCoordinate - tau*(*lIt).second.m_refCoordinate;
	double z0min = (*lIt).second.m_minCoordinate - tau*(*lIt).second.m_refCoordinate;
	int maxBin = (fabs(z0max)<=dZ) ? 0 : 1+(int)(fabs(z0max)/dZ-0.5);
	if(z0max<0.0) maxBin = -maxBin;
	int minBin = (fabs(z0min)<=dZ) ? 0 : 1+(int)(fabs(z0min)/dZ-0.5); 
	if(z0min<0.0) minBin = -minBin;
	if(maxBin>nbZ) maxBin = nbZ;
	if(minBin<-nbZ) minBin = -nbZ;
	for(int i=minBin;i<=maxBin;i++) {
	  int idx = (j+nbT) + (i+nbZ)*m_nBinsTheta;
	  vArray[idx].push_back(LAYER_IMPACT_POINT(layerId,lx));
	}
      }
    }
    else { //endcap
      for(int i=-nbZ;i<=nbZ;i++) {
	double z0 = i*dZ;
	double tauMin = ((*lIt).second.m_refCoordinate-z0)/(*lIt).second.m_maxCoordinate;
	double tauMax = ((*lIt).second.m_refCoordinate-z0)/(*lIt).second.m_minCoordinate;
	if(tauMin>tauMax) {
	  double tmp=tauMin;tauMin=tauMax;tauMax=tmp;
	}
	int maxBin = (fabs(tauMax)<=dTau) ? 0 : 1+(int)(fabs(tauMax)/dTau-0.5);
	if (tauMax<0.0) maxBin = -maxBin;
	int minBin = (fabs(tauMin)<=dTau) ? 0 : 1+(int)(fabs(tauMin)/dTau-0.5);
	if (tauMin<0.0) minBin = -minBin;
	if(maxBin>nbT) maxBin = nbT;
	if(minBin<-nbT) minBin = -nbT;
	for(int j=minBin;j<=maxBin;j++) {
	  int idx = (j+nbT) + (i+nbZ)*m_nBinsTheta;
	  double tau = j*dTau;
	  if(j==0) {
	    std::cout<<"layer zk="<<(*lIt).second.m_refCoordinate<<" z0="<<z0<<" zero tau !"<<std::endl;
	  }
	  double lx = sqrt(1.0+tau*tau)*((*lIt).second.m_refCoordinate-z0)/tau;
	  vArray[idx].push_back(LAYER_IMPACT_POINT(layerId,lx));
	}
      }
    }
  }

  unsigned int nLmax=0;

  for(int i=0;i<m_nBinsZ;i++) {
    for(int j=0;j<m_nBinsTheta;j++) {
      int idx = j + i*m_nBinsTheta;
      std::sort(vArray[idx].begin(), vArray[idx].end(), LAYER_IMPACT_POINT::compare());
      if(nLmax<vArray[idx].size()) nLmax = vArray[idx].size();
    }
  }

  //quick test
  /*
  const std::vector<LAYER_IMPACT_POINT>& v0 = vArray[nbT + nbZ*m_nBinsTheta];

  for(std::vector<LAYER_IMPACT_POINT>::const_iterator lIt = v0.begin();lIt!= v0.end();++lIt) {
    std::cout<<"L"<<(*lIt).m_layerId<<" l="<<(*lIt).m_L<<std::endl;
  }

  int D=4;

  const std::vector<LAYER_IMPACT_POINT>& vm = vArray[D + nbZ*m_nBinsTheta];
  std::cout<<"tau="<<(dTau*(-nbT+D))<<std::endl;

  for(std::vector<LAYER_IMPACT_POINT>::const_iterator lIt = vm.begin();lIt!= vm.end();++lIt) {
    std::cout<<"L"<<(*lIt).m_layerId<<" l="<<(*lIt).m_L<<std::endl;
  }

  const std::vector<LAYER_IMPACT_POINT>& vp = vArray[m_nBinsTheta-1-D + nbZ*m_nBinsTheta];
  std::cout<<"tau="<<(dTau*(nbT-D))<<std::endl;

  for(std::vector<LAYER_IMPACT_POINT>::const_iterator lIt = vp.begin();lIt!= vp.end();++lIt) {
    std::cout<<"L"<<(*lIt).m_layerId<<" l="<<(*lIt).m_L<<std::endl;
  }
  */

  // filling LUT structure
  
  m_pLUT = new TrigL2LayerSetLUT(m_maxZ0, tauMax, m_nBinsZ, m_nBinsTheta, nLmax);

  short* stack = new short[nLmax];

  for(int i=-nbZ;i<=nbZ;i++) {
    double z0 = i*dZ;
    for(int j=-nbT;j<=nbT;j++) {
      double tau = j*dTau;
      int idx = (j+nbT) + (i+nbZ)*m_nBinsTheta;
      const std::vector<LAYER_IMPACT_POINT>& v = vArray[idx];
      int k=0;
      for(std::vector<LAYER_IMPACT_POINT>::const_iterator lIt = v.begin();lIt!= v.end();++lIt) {
	stack[k++] = (*lIt).m_layerId;
      }
      for(;k<(int)nLmax;k++) stack[k]=999;//guard value
      if(!m_pLUT->addSet(stack[0],z0,tau)) {
	std::cout<<"cannot add z0="<<z0<<" tau="<<tau<<" to Layer LUT"<<std::endl;
      }
    }
  }

  delete[] stack;
  
  delete[] vArray;

  //repeat quick test
  /*
  double z0 = 0.0;
  double tau = 0.0;

  const short* pS = m_pLUT->getSet(z0,tau);
  if(pS!=NULL) {
    for(int i=0;i<m_pLUT->m_nLayers;i++) {
      std::cout<<pS[i]<<" ";
    }
    std::cout<<std::endl;
  }

  tau = dTau*(-nbT+D);

  pS = m_pLUT->getSet(z0,tau);
  if(pS!=NULL) {
    for(int i=0;i<m_pLUT->m_nLayers;i++) {
      std::cout<<pS[i]<<" ";
    }
    std::cout<<std::endl;
  }

  tau = dTau*(nbT-D);

  pS = m_pLUT->getSet(z0,tau);
  if(pS!=NULL) {
    for(int i=0;i<m_pLUT->m_nLayers;i++) {
      std::cout<<pS[i]<<" ";
    }
    std::cout<<std::endl;
  }
  */
  return sc;
}
