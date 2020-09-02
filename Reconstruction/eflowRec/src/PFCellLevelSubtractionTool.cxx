/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFCellLevelSubtractionTool.h"

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowDatabase.h"
#include "eflowRec/eflowDepthCalculator.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/IEFlowCellEOverPTool.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/PFTrackClusterMatchingTool.h"
#include "eflowRec/eflowRingSubtractionManager.h"
#include "eflowRec/eflowCellSubtractionFacilitator.h"
#include "eflowRec/eflowSubtractor.h"
#include "eflowRec/eflowRingThicknesses.h"
#include "eflowRec/PFSubtractionStatusSetter.h"

#include "CaloEvent/CaloCluster.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "xAODPFlow/PFO.h"

#include "eflowRec/eflowCaloObjectMaker.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/ListItem.h"

#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>
#include <map>


using namespace eflowSubtract;

PFCellLevelSubtractionTool::PFCellLevelSubtractionTool(const std::string& type,const std::string& name,const IInterface* parent) :
  base_class( type, name, parent),
  m_binnedParameters(nullptr)
{
}

PFCellLevelSubtractionTool::~PFCellLevelSubtractionTool() {
}

StatusCode PFCellLevelSubtractionTool::initialize(){
  StatusCode sc;

  if (m_matchingTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot find PFTrackClusterMatchingTool" << endmsg;
  }

  if (m_matchingToolForPull_02.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot find PFTrackClusterMatchingTool_2" << endmsg;
  }

  m_binnedParameters = std::make_unique<eflowEEtaBinnedParameters>();
  
  sc = m_theEOverPTool->fillBinnedParameters(m_binnedParameters.get());

  m_runInGoldenMode = ((m_goldenModeString == "golden1") || (m_goldenModeString == "golden2"));

  if (sc.isFailure()) {
    msg(MSG::WARNING) << "Could not execute eflowCellEOverPTool " << endmsg;
    return StatusCode::SUCCESS;
  }

  m_trkpos.reset( dynamic_cast<PFMatch::TrackEtaPhiInFixedLayersProvider*>(PFMatch::TrackPositionFactory::Get("EM2EtaPhi").release()) );
  if(!m_trkpos.get()) {
    ATH_MSG_ERROR("Failed to get TrackPositionProvider for cluster preselection!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

void PFCellLevelSubtractionTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, eflowRecTrackContainer* recTrackContainer, eflowRecClusterContainer* recClusterContainer) const {

  ATH_MSG_VERBOSE("Executing PFCellLevelSubtractionTool");

  eflowData data;
  data.caloObjects = theEflowCaloObjectContainer;
  data.tracks = recTrackContainer;
  data.clusters = recClusterContainer;
  data.clusterLUT.fill(*recClusterContainer);
  
  /* Add each track to its best matching cluster's eflowCaloObject */
  matchAndCreateEflowCaloObj(m_nMatchesInCellLevelSubtraction, data);

  if (msgLvl(MSG::DEBUG)) printAllClusters(*recClusterContainer);
  
  /* Check e/p mode - only perform subtraction if not in this mode */
  if (!m_calcEOverP) {performSubtraction(data);}

  ATH_MSG_DEBUG("Have executed performSubtraction");
  
  /* Check e/p mode - only perform radial profiles calculations if in this mode */
  if (m_calcEOverP) {calculateRadialEnergyProfiles(data);}

  ATH_MSG_DEBUG("Have executed calculateRadialEnergyProfiles");
  
}

unsigned int PFCellLevelSubtractionTool::matchAndCreateEflowCaloObj(unsigned int n, eflowData& data) const {
  unsigned int nMatches(0);

  /* Create eflowTrackClusterLink after matching */
  unsigned int nTrack = data.tracks->size();
  for (unsigned int iTrack=0; iTrack<nTrack; ++iTrack) {
    eflowRecTrack *thisEfRecTrack = static_cast<eflowRecTrack*>(data.tracks->at(iTrack));

    // Preselect clusters in a local cone
    eflowRecMatchTrack matchTrack(thisEfRecTrack);
    PFMatch::EtaPhi etaphi = m_trkpos->getPosition(&matchTrack);
    float tracketa = etaphi.getEta();
    float trackphi = etaphi.getPhiD();
    float dRpresel = 0.4; // Some safety margin, but still << full detector
    std::vector<eflowRecCluster*> nearbyClusters = data.clusterLUT.clustersInCone(tracketa,trackphi,dRpresel);

     /* Add cluster matches needed for pull calculation*/
    std::vector<std::pair<eflowRecCluster*,float> > bestClusters_02 = m_matchingToolForPull_02->doMatches(thisEfRecTrack, data.clusters, -1);

    for (auto& matchpair : bestClusters_02) {
      eflowRecCluster* theCluster = matchpair.first;
      float distancesq = matchpair.second;
      eflowTrackClusterLink* trackClusterLink = eflowTrackClusterLink::getInstance(thisEfRecTrack, theCluster);
      if(distancesq<0.15*0.15) {
	// Narrower cone is a subset of the selected clusters
	// Distance returned is deltaR^2
	thisEfRecTrack->addAlternativeClusterMatch(trackClusterLink,"cone_015");
      }
      thisEfRecTrack->addAlternativeClusterMatch(trackClusterLink,"cone_02");    
    }

    std::vector<std::pair<eflowRecCluster*,float> > bestClusters = m_matchingTool->doMatches(thisEfRecTrack, data.clusters, n);
    if (bestClusters.empty()) { continue; }

    /* Matched cluster: create TrackClusterLink and add it to both the track and the cluster (eflowCaloObject will be created later) */
    nMatches++;
    for (auto& matchpair : bestClusters) {
      eflowRecCluster* theCluster = matchpair.first;
      eflowTrackClusterLink* trackClusterLink = eflowTrackClusterLink::getInstance(thisEfRecTrack, theCluster);
      thisEfRecTrack->addClusterMatch(trackClusterLink);
      theCluster->addTrackMatch(trackClusterLink);
    }
  
  }

  /* Create 3 types eflowCaloObjects: track-only, cluster-only, track-cluster-link */
  eflowCaloObjectMaker makeCaloObject;
  unsigned int nCaloObjects = makeCaloObject.makeTrkCluCaloObjects(data.tracks, data.clusters, data.caloObjects);
  ATH_MSG_DEBUG("PFCellLevelSubtractionTool created total " << nCaloObjects << " CaloObjects.");

  // Should move to a common header or similar, as shared with PFRecoverSplitShowersTool
  const double gaussianRadius = 0.032;
  const double gaussianRadiusError = 1.0e-3;
  const double maximumRadiusSigma = 3.0;

  eflowLayerIntegrator integrator(gaussianRadius, gaussianRadiusError, maximumRadiusSigma, m_isHLLHC);

  /* integrate cells; determine FLI; eoverp */
  for (unsigned int iCalo=0; iCalo<data.caloObjects->size(); ++iCalo) {
    eflowCaloObject *thisEflowCaloObject = static_cast<eflowCaloObject*>(data.caloObjects->at(iCalo));

    thisEflowCaloObject->simulateShower(&integrator, m_binnedParameters.get(), m_useUpdated2015ChargedShowerSubtraction);
  }

  return nMatches;
}

void PFCellLevelSubtractionTool::calculateRadialEnergyProfiles(eflowData& data) const {

  ATH_MSG_DEBUG("Accessed radial energy profile function");

  unsigned int nEFCaloObs = data.caloObjects->size();
  
  for (unsigned int iEFCalOb = 0; iEFCalOb < nEFCaloObs; ++iEFCalOb) {
    
    eflowCaloObject* thisEflowCaloObject = data.caloObjects->at(iEFCalOb);
    
    unsigned int nClusters = thisEflowCaloObject->nClusters();
    
    if (nClusters < 1) {
    continue;
    }
    
    const std::vector<std::pair<eflowTrackClusterLink*,float> >& matchedTrackList = thisEflowCaloObject->efRecLink();

    unsigned int nTrackMatches = thisEflowCaloObject->nTracks();
    
    for (unsigned int iTrack = 0; iTrack < nTrackMatches; ++iTrack) {
      
      eflowRecTrack* efRecTrack = (matchedTrackList[iTrack].first)->getTrack();
      
      std::vector<eflowRecCluster*> matchedClusters;
      matchedClusters.clear();
      std::vector<eflowTrackClusterLink*> links = efRecTrack->getClusterMatches();
      for (auto thisEFlowTrackClusterLink : links) matchedClusters.push_back(thisEFlowTrackClusterLink->getCluster());
      
      std::vector<std::pair<xAOD::CaloCluster*, bool> > clusterSubtractionList;
      for (auto thisEFlowRecCluster : matchedClusters) clusterSubtractionList.push_back(std::pair(thisEFlowRecCluster->getCluster(),false));

      eflowCellList calorimeterCellList;
      Subtractor::makeOrderedCellList(efRecTrack->getTrackCaloPoints(),clusterSubtractionList,calorimeterCellList);
      
      eflowRingThicknesses ringThicknessGenerator;
      
      std::vector<int> layerToStoreVector;
      std::vector<float> radiusToStoreVector;
      std::vector<float> avgEdensityToStoreVector;
      
      for (int i=0; i < eflowCalo::nRegions ;i++){
	
	eflowCaloENUM layer = (eflowCaloENUM)i;
	ATH_MSG_DEBUG("layer is "<<layer);
	double ringThickness = ringThicknessGenerator.ringThickness((eflowCaloENUM)i);
	ATH_MSG_DEBUG("ring thickness is "<<ringThickness);
	
	double eta_extr = calorimeterCellList.etaFF(layer);
	ATH_MSG_DEBUG("extrapolated eta ["<<layer<<"] is "<<eta_extr);
	double phi_extr = calorimeterCellList.phiFF(layer);
	ATH_MSG_DEBUG("extrapolated phi ["<<layer<<"] is "<<phi_extr);
    
	if (eta_extr == -999.0){
	  continue;
	}
        
	int indexofRing = 0;
	int layerToStore = -999;
	
	double radiusToStore = 0;
	double totalEnergyPerCell = 0;
	
	double energyDensityPerCell = -666;
	double totalEnergyPerRing = 0;
	
	double cellVolume = 0;
	int totalCells = 0;
        
	/* 100 is chosen as a number higher than the number of cells found in a normal list */
	bool breakloop = false;
	for (unsigned int n=1; n<100; n++){
	  
	  CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));

	  if(beginRing == calorimeterCellList.end()){
	    break;
	  }
        
	  indexofRing += 1;
	  
	  int totalCellsinRing = 0;
	  double energyDensityPerRing = 0;
	  double averageEnergyDensityPerRing = 0;

	  std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
	  
	  for (auto thisPair : tempVector){
	    const CaloDetDescrElement* DDE = (thisPair.first)->caloDDE();
	    CaloCell_ID::CaloSample sampling = DDE->getSampling();

	    if(eflowCalo::translateSampl(sampling)!=(eflowCaloENUM)i){
	      breakloop = true;
	      break;
	    }
	    
	    ATH_MSG_DEBUG(" cell eta and phi are " << (thisPair.first)->eta() << " and " << (thisPair.first)->phi() << " with index " << thisPair.second << " and sampling of " << sampling);
	    ATH_MSG_DEBUG(" cell energy is " << (thisPair.first)->energy());
            
	    totalCells += 1;
	    totalCellsinRing += 1;
	    
	    totalEnergyPerRing += (thisPair.first)->energy();
	    totalEnergyPerCell = (thisPair.first)->energy();
	    ATH_MSG_DEBUG(" Total E per Cell is " << totalEnergyPerCell);
	    ATH_MSG_DEBUG(" Total E per Ring is " << totalEnergyPerRing);
	    
	    cellVolume = DDE->volume();
	    ATH_MSG_DEBUG(" cell volume is " << cellVolume/1000.);
	    
	    energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
	    ATH_MSG_DEBUG(" E density per Cell is " << energyDensityPerCell);
	    ATH_MSG_DEBUG(" Initial added E density per Cell is " << energyDensityPerRing);
	    energyDensityPerRing += energyDensityPerCell;
	    ATH_MSG_DEBUG(" Final added E density per Cell is " << energyDensityPerRing);
	    averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
	  }
	  
	  ATH_MSG_DEBUG(" track eta is " << efRecTrack->getTrack()->eta());
	  ATH_MSG_DEBUG(" track E is " << efRecTrack->getTrack()->e()/1000.);
	  ATH_MSG_DEBUG(" Average E density per Ring is " << averageEnergyDensityPerRing);
	  
	  if (averageEnergyDensityPerRing != 0){
	    avgEdensityToStoreVector.push_back(averageEnergyDensityPerRing);
	    layerToStore = (eflowCaloENUM)i;
	    ATH_MSG_DEBUG("layerToStore is "<< layerToStore);
	    layerToStoreVector.push_back(layerToStore);
	    radiusToStore = (indexofRing)*ringThickness;
	    ATH_MSG_DEBUG("radiusToStore is "<< radiusToStore);
	    radiusToStoreVector.push_back(radiusToStore);
	  }
	  else {ATH_MSG_DEBUG("averageEnergyDensityPerRing = 0");}
	  if (breakloop) break;
	}//loop on 100 cells
      }//loop on calo regions
	
      efRecTrack->setLayerCellOrderVector(layerToStoreVector);
      efRecTrack->setRadiusCellOrderVector(radiusToStoreVector);
      efRecTrack->setAvgEDensityCellOrderVector(avgEdensityToStoreVector);
      
      layerToStoreVector.clear();
      radiusToStoreVector.clear();
      avgEdensityToStoreVector.clear();
    }//loop on track matches    
  }//loop on eflowCaloObjects
}

void PFCellLevelSubtractionTool::performSubtraction(eflowData& data) const {

  ATH_MSG_DEBUG("In performSubtraction");

  PFSubtractionStatusSetter pfSubtractionStatusSetter;
  
  unsigned int nEFCaloObs = data.caloObjects->size();
  for (unsigned int iEFCalOb = 0; iEFCalOb < nEFCaloObs; ++iEFCalOb) {
    eflowCaloObject* thisEflowCaloObject = data.caloObjects->at(iEFCalOb);

    unsigned int nClusters = thisEflowCaloObject->nClusters();

    ATH_MSG_DEBUG("Have got an eflowCaloObject with " << nClusters << " clusters");
    
    if (nClusters < 1) {
      continue;
    }
    
    /* Get matched cluster via Links */

    double expectedEnergy = thisEflowCaloObject->getExpectedEnergy();
    double clusterEnergy = thisEflowCaloObject->getClusterEnergy();
    double expectedSigma = sqrt(thisEflowCaloObject->getExpectedVariance());
 
    ATH_MSG_DEBUG("Have got reference values for eflowCaloObject");
    
    /* Check e/p */
    if (isEOverPFail(expectedEnergy, expectedSigma, clusterEnergy, m_consistencySigmaCut,
                     m_runInGoldenMode)
        || (m_runInGoldenMode && thisEflowCaloObject->nTracks() > 1)) {
      continue;      
    }    

    ATH_MSG_DEBUG("eflowCaloObject passed eOverP checks");
    
    /* Do subtraction */

    unsigned int nTrackMatches = thisEflowCaloObject->nTracks();
    if (nTrackMatches == 0 || m_runInGoldenMode) {
      continue;
    }

    ATH_MSG_DEBUG("About to perform subtraction for this eflowCaloObject");

    const std::vector<std::pair<eflowTrackClusterLink*, float> >& matchedTrackList = thisEflowCaloObject->efRecLink();

    /* Check if we can annihilate right away */
    if (canAnnihilated(expectedEnergy, expectedSigma, clusterEnergy)) {
      
      std::vector<std::pair<xAOD::CaloCluster*, bool> > clusterList;
      unsigned nCluster = thisEflowCaloObject->nClusters();
      for (unsigned iCluster = 0; iCluster < nCluster; ++iCluster) clusterList.push_back(std::pair(thisEflowCaloObject->efRecCluster(iCluster)->getCluster(),false));

      Subtractor::annihilateClusters(clusterList);      

      //Now we should mark all of these clusters as being subtracted
      pfSubtractionStatusSetter.markAnnihStatus(*thisEflowCaloObject);

    } else {

      /* Subtract the track from all matched clusters */
      
      for (unsigned int iTrack = 0; iTrack < nTrackMatches; ++iTrack) {
	      eflowRecTrack* efRecTrack = (matchedTrackList[iTrack].first)->getTrack();
	
	      ATH_MSG_DEBUG("Have got eflowRecTrack number " << iTrack << " for this eflowCaloObject");
	
	      /* Can't subtract without e/p */
	      if (!efRecTrack->hasBin()) {
	        continue;
	      }
     
	      if (efRecTrack->isInDenseEnvironment()) continue;

	      ATH_MSG_DEBUG("Have bin and am not in dense environment for this eflowCaloObject");
      
	      std::vector<eflowRecCluster*> matchedClusters;
	      matchedClusters.clear();
	      std::vector<eflowTrackClusterLink*> links = efRecTrack->getClusterMatches();
	      for (auto thisEFlowTrackClusterLink : links) matchedClusters.push_back(thisEFlowTrackClusterLink->getCluster());

	      ATH_MSG_DEBUG("Have filled matchedClusters list for this eflowCaloObject");
	
	      std::vector<std::pair<xAOD::CaloCluster*, bool> > clusterSubtractionList;
        std::map<xAOD::CaloCluster*, double> clusterEnergyMap;
	      for (auto thisEFlowRecCluster : matchedClusters) {
          xAOD::CaloCluster* thisCluster = thisEFlowRecCluster->getCluster();
          clusterSubtractionList.push_back(std::pair(thisCluster,false));
          clusterEnergyMap[thisCluster] = thisCluster->e();
        }

	      ATH_MSG_DEBUG("Have filled clusterSubtractionList for this eflowCaloObject");
      
	      Subtractor::subtractTracksFromClusters(efRecTrack, clusterSubtractionList);
	
	      ATH_MSG_DEBUG("Have performed subtraction for this eflowCaloObject");
      
	      /* Annihilate the cluster(s) if the remnant is small (i.e. below k*sigma) */
	      if (canAnnihilated(0, expectedSigma, clusterEnergy)) {
	        Subtractor::annihilateClusters(clusterSubtractionList);
	        //Now we should mark all of these clusters as being subtracted
          pfSubtractionStatusSetter.markAnnihStatus(*thisEflowCaloObject);	       
	      }
        else{
          std::vector<float> clusterSubtractedEnergyRatios;
          for (auto thisCluster: clusterSubtractionList) {
            if (fabs(thisCluster.first->e() - clusterEnergyMap[thisCluster.first]) > 0.0001) clusterSubtractedEnergyRatios.push_back(thisCluster.first->e()/clusterEnergyMap[thisCluster.first]);
            else clusterSubtractedEnergyRatios.push_back(NAN);
          }

	        pfSubtractionStatusSetter.markSubtractionStatus(clusterSubtractionList, clusterSubtractedEnergyRatios, *thisEflowCaloObject);
	
        } 
	
	      ATH_MSG_DEBUG("Have checked if can annihilate clusters for this eflowCaloOject");
      
      }
    }
    
    /* Flag tracks as subtracted */
    for (unsigned int iTrack = 0; iTrack < thisEflowCaloObject->nTracks(); ++iTrack) {
      if (!thisEflowCaloObject->efRecTrack(iTrack)->isInDenseEnvironment()) thisEflowCaloObject->efRecTrack(iTrack)->setSubtracted();
    }
  }
}

StatusCode PFCellLevelSubtractionTool::finalize(){

  return StatusCode::SUCCESS;
}


bool PFCellLevelSubtractionTool::isEOverPFail(double expectedEnergy, double sigma, double clusterEnergy, bool consistencySigmaCut, bool useGoldenMode) const {
  if ((expectedEnergy == 0) && (clusterEnergy > 0)) {
    return false;
  }

  bool result = useGoldenMode ? fabs(clusterEnergy - expectedEnergy) > consistencySigmaCut*sigma
                              : clusterEnergy < expectedEnergy - consistencySigmaCut*sigma;
  return result;
}

bool PFCellLevelSubtractionTool::canAnnihilated(double expectedEnergy, double sigma, double clusterEnergy) const {
   return  clusterEnergy - expectedEnergy < m_subtractionSigmaCut * sigma;
}

std::string PFCellLevelSubtractionTool::printTrack(const xAOD::TrackParticle* track) const {
  std::stringstream result;
  result << " track with E, eta and phi "<< track->e() << ", " << track->eta() << " and " << track->phi();
  return result.str();
}

std::string PFCellLevelSubtractionTool::printCluster(const xAOD::CaloCluster* cluster) const {
  std::stringstream result;
  result << " cluster with E, eta and phi of " << cluster->e() << ", " << cluster->eta() << " and " << cluster->phi();
  return result.str();
}

void PFCellLevelSubtractionTool::printAllClusters(const eflowRecClusterContainer& recClusterContainer) const {
  unsigned int nClusters = recClusterContainer.size();
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
    /* Create the eflowCaloObject and put it in the container */
    const eflowRecCluster* thisEFRecCluster = recClusterContainer.at(iCluster);
    if (thisEFRecCluster->getTrackMatches().empty()) {
      ATH_MSG_DEBUG("Isolated" << printCluster(thisEFRecCluster->getCluster()));
    } else {
      ATH_MSG_DEBUG("Matched" << printCluster(thisEFRecCluster->getCluster()));
      unsigned int nTrackMatches = thisEFRecCluster->getNTracks();
      std::vector<eflowTrackClusterLink*> theTrackLinks = thisEFRecCluster->getTrackMatches();
      for (unsigned int iTrackMatch = 0; iTrackMatch < nTrackMatches; ++iTrackMatch) {
	ATH_MSG_DEBUG("Matched" << printTrack(theTrackLinks[iTrackMatch]->getTrack()->getTrack()));
      }
    }
  }
}
