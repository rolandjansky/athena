/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************

NAME:     eflowCellLevelSubtractionTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  25th January, 2005

********************************************************************/

#include "eflowRec/eflowCellLevelSubtractionTool.h"

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
#include "eflowRec/cycle.h"
#include "eflowRec/eflowRingSubtractionManager.h"
#include "eflowRec/eflowCellSubtractionFacilitator.h"
#include "eflowRec/eflowSubtractor.h"
#include "eflowRec/eflowRingThicknesses.h"
#include "eflowRec/eflowCellOrderingParameters.h"

#include "CaloEvent/CaloCluster.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "xAODPFlow/PFO.h"

#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowCaloObjectMaker.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/ListItem.h"

#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>


using namespace eflowSubtract;

eflowCellLevelSubtractionTool::eflowCellLevelSubtractionTool(const std::string& type,const std::string& name,const IInterface* parent) :
  AthAlgTool( type, name, parent),
  m_eflowCaloObjectContainer(0),
  m_eflowTrackContainer(0),
  m_eflowClusterContainer(0),
  m_matchingTool("PFTrackClusterMatchingTool/CalObjBldMatchingTool", this),
  m_matchingToolForPull_015("PFTrackClusterMatchingTool/PFPullMatchingTool_015", this),
  m_matchingToolForPull_02("PFTrackClusterMatchingTool/PFPullMatchingTool_02", this),
  m_binnedParameters(0),
  m_integrator(0),
  m_theEOverPTool("eflowCellEOverPTool",this),
  //m_rCell(0.75),
  m_subtractionSigmaCut(1.5),
  m_consistencySigmaCut(1.0),
  m_calcEOverP(true),
  m_goldenModeString(""),
  m_nMatchesInCellLevelSubtraction(1),
  m_useUpdated2015ChargedShowerSubtraction(true)

{ 
  declareProperty("PFTrackClusterMatchingTool", m_matchingTool, "The track-cluster matching tool");
  declareProperty("PFTrackClusterMatchingTool_015", m_matchingToolForPull_015, "The 0.15 track-cluster matching tool to calculate the pull");
  declareProperty("PFTrackClusterMatchingTool_02", m_matchingToolForPull_02, "The 0.2 track-cluster matching tool to calculate the pull");
  declareProperty("eflowCellEOverPTool", m_theEOverPTool, "Energy Flow E/P Values and Shower Paremeters Tool");
  declareProperty("SubtractionSigmaCut",m_subtractionSigmaCut);
  declareProperty("ConsistencySigmaCut",m_consistencySigmaCut);
  declareProperty("CalcEOverP",m_calcEOverP,"Whether to disable energy flow");
  declareProperty("goldenModeString",m_goldenModeString,"run in golden match mode only?");
  declareProperty("nMatchesInCellLevelSubtraction",m_nMatchesInCellLevelSubtraction,"Number of clusters to match");
  declareProperty("useUpdated2015ChargedShowerSubtraction",m_useUpdated2015ChargedShowerSubtraction,"Toggle whether to use updated 2015 charged shower subtraction, which disables the shower subtraction in high calorimeter energy density region");
}

eflowCellLevelSubtractionTool::~eflowCellLevelSubtractionTool() {
  delete m_integrator;
  delete m_binnedParameters;
}

StatusCode eflowCellLevelSubtractionTool::initialize(){
  StatusCode sc;

  /* tool service */
  IToolSvc* myToolSvc;
  sc = service("ToolSvc", myToolSvc);

  if (m_matchingTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot find PFTrackClusterMatchingTool" << endmsg;
  }

  if (m_matchingToolForPull_015.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot find PFTrackClusterMatchingTool_2" << endmsg;
  }

  if (m_matchingToolForPull_02.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot find PFTrackClusterMatchingTool_2" << endmsg;
  }

  m_integrator = new eflowLayerIntegrator(0.032, 1.0e-3, 3.0);
  m_binnedParameters = new eflowEEtaBinnedParameters();

  sc = m_theEOverPTool->execute(m_binnedParameters);

  if (sc.isFailure()) {
    msg(MSG::WARNING) << "Could not execute eflowCellEOverPTool " << endmsg;
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;

}

void eflowCellLevelSubtractionTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, eflowRecTrackContainer* recTrackContainer, eflowRecClusterContainer* recClusterContainer) {

  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Executing eflowCellLevelSubtractionTool" << endmsg;

  m_eflowCaloObjectContainer = theEflowCaloObjectContainer;
  m_eflowTrackContainer = recTrackContainer;
  m_eflowClusterContainer = recClusterContainer;

  /* Add each track to its best matching cluster's eflowCaloObject */
  matchAndCreateEflowCaloObj(m_nMatchesInCellLevelSubtraction);

  int debug = 0;
  if (1 == debug) {
    printAllClusters(recClusterContainer);
  }

//   std::cout<<"E/p mode is"<<m_calcEOverP<<std::endl;
  /* Check e/p mode - only perform subtraction if not in this mode */
//   if (!m_calcEOverP) performSubtraction();
//    performSubtraction();
   
// Check e/p mode - only perform radial profiles calculations if in this mode
//     std::cout<<"Before calculating radial energy profile function"<<std::endl;
//   if (m_calcEOverP) 
std::cout<<"hello before cREP"<<std::endl;
calculateRadialEnergyProfiles();
std::cout<<"hello after cREP"<<std::endl;
//     std::cout<<"After calculating radial energy profile function"<<std::endl;

}

int eflowCellLevelSubtractionTool::matchAndCreateEflowCaloObj(int n) {
  int nMatches(0);

  /* Create eflowTrackClusterLink after matching */
  unsigned int nTrack = m_eflowTrackContainer->size();
  for (unsigned int iTrack=0; iTrack<nTrack; ++iTrack) {
    eflowRecTrack *thisEfRecTrack = static_cast<eflowRecTrack*>(m_eflowTrackContainer->at(iTrack));

     /* Add cluster matches needed for pull calculation*/
    std::vector<eflowRecCluster*> bestClusters_015 = m_matchingToolForPull_015->doMatches(thisEfRecTrack, m_eflowClusterContainer, -1);
    std::vector<eflowRecCluster*> bestClusters_02 = m_matchingToolForPull_02->doMatches(thisEfRecTrack, m_eflowClusterContainer, -1);

    for (unsigned int imatch=0; imatch < bestClusters_015.size(); ++imatch) {
      eflowTrackClusterLink* trackClusterLink = eflowTrackClusterLink::getInstance(thisEfRecTrack, bestClusters_015.at(imatch));
      thisEfRecTrack->addAlternativeClusterMatch(trackClusterLink,"cone_015");    
    }

    for (unsigned int imatch=0; imatch < bestClusters_02.size(); ++imatch) {
      eflowTrackClusterLink* trackClusterLink = eflowTrackClusterLink::getInstance(thisEfRecTrack, bestClusters_02.at(imatch));
      thisEfRecTrack->addAlternativeClusterMatch(trackClusterLink,"cone_02");    
    }

    std::vector<eflowRecCluster*> bestClusters = m_matchingTool->doMatches(thisEfRecTrack, m_eflowClusterContainer, n);
    if (bestClusters.empty()) { continue; }

    /* Matched cluster: create TrackClusterLink and add it to both the track and the cluster (eflowCaloObject will be created later) */
    nMatches++;
    for (unsigned int imatch=0; imatch < bestClusters.size(); ++imatch) {
    eflowTrackClusterLink* trackClusterLink = eflowTrackClusterLink::getInstance(thisEfRecTrack, bestClusters.at(imatch));
    thisEfRecTrack->addClusterMatch(trackClusterLink);
    bestClusters.at(imatch)->addTrackMatch(trackClusterLink);
    
    }
  
  }

  /* Create 3 types eflowCaloObjects: track-only, cluster-only, track-cluster-link */
  eflowCaloObjectMaker makeCaloObject;
  int nCaloObjects = makeCaloObject.makeTrkCluCaloObjects(m_eflowTrackContainer, m_eflowClusterContainer, m_eflowCaloObjectContainer);
  msg(MSG::DEBUG)  << "eflowCellLevelSubtractionTool created total " << nCaloObjects << " CaloObjects." << endmsg;

  /* integrate cells; determine FLI; eoverp */
  for (unsigned int iCalo=0; iCalo<m_eflowCaloObjectContainer->size(); ++iCalo) {
    eflowCaloObject *thisEflowCaloObject = static_cast<eflowCaloObject*>(m_eflowCaloObjectContainer->at(iCalo));

    thisEflowCaloObject->simulateShower(m_integrator, m_binnedParameters, m_useUpdated2015ChargedShowerSubtraction);
  }

  return nMatches;
}

void eflowCellLevelSubtractionTool::calculateRadialEnergyProfiles(){

  std::cout<<"Accessed radial energy profile function"<<std::endl;

  unsigned int nEFCaloObs = m_eflowCaloObjectContainer->size();
  
  std::cout<<"Before iEFCalOb loop"<<std::endl;
  for (unsigned int iEFCalOb = 0; iEFCalOb < nEFCaloObs; ++iEFCalOb) {
  
    eflowCaloObject* thisEflowCaloObject = m_eflowCaloObjectContainer->at(iEFCalOb);
    std::cout<<"Within iEFCalOb loop"<<std::endl;
    
    unsigned int nClusters = thisEflowCaloObject->nClusters();
    
    if (nClusters < 1) {
      std::cout<<"if nClusters < 1"<<std::endl;
    continue;
    }
    
    std::cout<<"Within iEFCalOb loop"<<std::endl;
    const std::vector<eflowTrackClusterLink*>& matchedTrackList = thisEflowCaloObject->efRecLink();
    std::cout<<"After efRecLink"<<std::endl;

    int nTrackMatches = thisEflowCaloObject->nTracks();
    std::cout<<"nTrackMatches = "<< nTrackMatches <<std::endl;
     for (int iTrack = 0; iTrack < nTrackMatches; ++iTrack) {
        std::cout<<"Within iTrack loop"<<std::endl;

        std::cout<<"Matched Track List["<<iTrack<<"]"<<std::endl;
        
//         CHECK( matchedTrackList[iTrack]->getTrack() );
        std::cout<<"Matched Track List["<<iTrack<<"] is "<< matchedTrackList[iTrack] <<std::endl;
        eflowRecTrack* efRecTrack = matchedTrackList[iTrack]->getTrack();
        
        std::cout<<"After get Track"<<std::endl;
        std::vector<eflowRecCluster*> matchedClusters;
        std::cout<<"After matched Clusters"<<std::endl;
        matchedClusters.clear();
        std::cout<<"After matched Clusters clear"<<std::endl;
        std::vector<eflowTrackClusterLink*> links = efRecTrack->getClusterMatches();
        std::cout<<"After get matched Clusters"<<std::endl;
        std::vector<eflowTrackClusterLink*>::iterator itLink = links.begin();
        std::vector<eflowTrackClusterLink*>::iterator endLink = links.end();
        std::cout<<"After defining links"<<std::endl;
        
        
        for (; itLink != endLink; ++itLink) {
          std::cout<<"Within itLINK loop"<<std::endl;
          matchedClusters.push_back((*itLink)->getCluster());
        }
        std::vector<xAOD::CaloCluster*> clusterSubtractionList;
        std::vector<eflowRecCluster*>::const_iterator itCluster = matchedClusters.begin();
        std::vector<eflowRecCluster*>::const_iterator endCluster = matchedClusters.end();
        for (; itCluster != endCluster; ++itCluster) {
          std::cout<<"Within itCluster loop"<<std::endl; 
          clusterSubtractionList.push_back(
              (*itCluster)->getClusterForModification(eflowCaloObject::getClusterContainerPtr()));
        }

    std::cout<<"before calo cell list"<<std::endl;
	eflowCellList calorimeterCellList;
	Subtractor::makeOrderedCellList(efRecTrack->getTrackCaloPoints(),clusterSubtractionList,calorimeterCellList);
	
    std::cout<<"after calo cell list"<<std::endl;
    
//     std::cout<<"track parameters [0] = "<<efRecTrack->getTrackCaloPoints()->getParameters(0)<<std::endl;
    
//     for loop over each layer? how do I check what the layer of each track is (now different than E/p)? -> do all layers for all tracks!

    
	eflowRingThicknesses ringThicknessGenerator;
// 	double EMB1_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::EMB1);
// 	double EMB2_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::EMB2);
// 	double EMB3_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::EMB3);
// 	double EME1_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::EME1);
// 	double EME2_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::EME2);
// 	double EME3_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::EME3);
// 	double HEC1_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::HEC1);
// 	double HEC2_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::HEC2);
// 	double HEC3_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::HEC3);
// 	double HEC4_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::HEC4);
// 	double Tile1_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::Tile1);
// 	double Tile2_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::Tile2);
// 	double Tile3_ringThickness = ringThicknessGenerator.ringThickness(eflowCaloENUM::Tile3);
// 	
// 	std::cout<<"after ring thicknesses list"<<std::endl;
// 	std::cout<<"eflow layer is "<<eflowCalo::LAYER<<std::endl;
	
	
	//write master loop over all possible eflowCaloENUM::LAYER
	


	
	for (int i=0; i < eflowCalo::nRegions ;i++){
	
        eflowCaloENUM layer = (eflowCaloENUM)i;
        std::cout<<"layer is "<<layer<<std::endl;
        double ringThickness = ringThicknessGenerator.ringThickness((eflowCaloENUM)i);
        std::cout<<"after layer def'n"<<std::endl;
        std::cout<<"ring thickness is "<<ringThickness<<std::endl;
        
        double eta_extr = calorimeterCellList.etaFF(layer);
        std::cout<<"extrapolated eta ["<<layer<<"] is "<<eta_extr<<std::endl;
        double phi_extr = calorimeterCellList.phiFF(layer);
        std::cout<<"extrapolated phi ["<<layer<<"] is "<<phi_extr<<std::endl;
        


//      do dR2 check, i.e. provide eta, phi of calorimeter cell and calculate distance to 
//      extrapolated track position in a given layer? 
//      handled by begin ring?
        xAOD::PFO thiseFlowObject;

        if (i==0){
        
        //loop over ring thicknesses and then ask for iterator corresponding to that loop
	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
	        
//         std::cout<<"after rings"<<std::endl;
          int indexofRing = 0;
          int layerToStore = -999;
          std::vector<int> layerToStoreVector;
          
          std::cout<<"layerToStore vector about to be cleared"<<std::endl;
          layerToStoreVector.clear();
          std::cout<<"layerToStore vector cleared"<<std::endl;
          double radiusToStore = 0;
          double totalEnergyPerCell = 0;
      
          int indexofCell = 0;
          double energyDensityPerCell = -666;
          double totalEnergyPerRing = 0;

          double cellVolume = 0;
          int totalCells = 0;
          double previouseta = 0;
          double previousphi = 0;
              
          int n;
        for (n=1; n<100; n++){
        
        // use calorimeterCellList.
        
        
        
          CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
//           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
          // 
//           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
        // for (;beginRing != endRing; beginRing++){
//         save # of rings 
          indexofRing += 1;
          std::cout<<"Ring Number "<<indexofRing<< std::endl;
          std::cout<<"within ring loop "<< std::endl;
          std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
          std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
          std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
      
          int totalCellsinRing = 0;
          double energyDensityPerRing = 0;
          double averageEnergyDensityPerRing = 0;
      
          for (; firstPair != lastPair; ++firstPair) {
                std::cout<<"within pair loop"<<std::endl;
                const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
                CaloCell_ID::CaloSample sampling = DDE->getSampling();
                
                
                if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
                    break;
                }
                
                
                
                std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
                std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
                
                previouseta = ((*firstPair).first)->eta();
                previousphi = ((*firstPair).first)->phi();
                
                double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
                
                if (celltrackdist > 2){
                    break;
                }
                
                double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
                double hand_dR = sqrt(hand_dR2);
                
                std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
                std::cout << " hand_dR is " << hand_dR <<std::endl;
                std::cout << " cell-track distance is " << celltrackdist <<std::endl;
                
                
                
                totalCells += 1;
                totalCellsinRing += 1;
        
                totalEnergyPerRing += ((*firstPair).first)->energy();
                totalEnergyPerCell = ((*firstPair).first)->energy();
                std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
                std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
        
                //implement track E here and divide cluster E by track E to have energy ratio
        // 	    indexofCell = (*firstPair).second;
        // check that volume units are OK -- volume from database
                cellVolume = DDE->volume();
                std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
        
                energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
                std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
                std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
                energyDensityPerRing += energyDensityPerCell;
                std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
        
        // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
        
                        //where should these go?
                averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
                std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
                std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
                layerToStore = (eflowCaloENUM)i;
                std::cout << " before layervector "<<std::endl;
                layerToStoreVector.push_back(layerToStore);
                std::cout<<"layerToStore is "<< layerToStore << std::endl;
                std::cout<<"layerToStoreVector["<< n <<"] is "<< layerToStoreVector[n] << std::endl;
                radiusToStore = (indexofRing)*ringThickness;
                std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
                efRecTrack->setLayerCellOrder(layerToStore);
                efRecTrack->setRadiusCellOrder(radiusToStore);
                efRecTrack->setAverageEnergyDensity(averageEnergyDensityPerRing);
                
                if (radiusToStore > celltrackdist){
                    std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
                    
                }
                else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
      
              }
      
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
        if (averageEnergyDensityPerRing != 0) {
        std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
        std::cout<<"layerToStore is "<< layerToStore << std::endl;
        std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
        std::cout<<"indexofRing is "<< indexofRing << std::endl;
//         eflowCellOrderingParameters* eflowCOP;
//         eflowCOP->setLayerToStore(layerToStore, radiusToStore, averageEnergyDensityPerRing);
        }
        else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
    }
//     efRecTrack->setLayerCellOrderVector(layerToStoreVector);
//     std::cout<<"LayerCellOrder is " << efRecTrack->getLayerCellOrderVector()[0]<<std::endl;

    }
        if (i==1){
        
        //loop over ring thicknesses and then ask for iterator corresponding to that loop
	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
	        
//         std::cout<<"after rings"<<std::endl;
          int indexofRing = 0;
          int layerToStore = -999;
          double radiusToStore = 0;
          std::vector<int> layerToStoreVector;
          double totalEnergyPerCell = 0;
      
          int indexofCell = 0;
          double energyDensityPerCell = -666;
          double totalEnergyPerRing = 0;

          double cellVolume = 0;
          int totalCells = 0;
          double previouseta = 0;
          double previousphi = 0;
              
          int n;
        for (n=1; n<100; n++){
        
        // use calorimeterCellList.
        
        
        
          CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
//           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
          // 
//           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
        // for (;beginRing != endRing; beginRing++){
//         save # of rings 
          indexofRing += 1;
          std::cout<<"Ring Number "<<indexofRing<< std::endl;
          std::cout<<"within ring loop "<< std::endl;
          std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
          std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
          std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
      
          int totalCellsinRing = 0;
          double energyDensityPerRing = 0;
          double averageEnergyDensityPerRing = 0;
      
          for (; firstPair != lastPair; ++firstPair) {
                std::cout<<"within pair loop"<<std::endl;
                const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
                CaloCell_ID::CaloSample sampling = DDE->getSampling();
                
                
                if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
                    break;
                }
                
                
                
                std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
                std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
                
                previouseta = ((*firstPair).first)->eta();
                previousphi = ((*firstPair).first)->phi();
                
                double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
                
                if (celltrackdist > 2){
                    break;
                }
                
                double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
                double hand_dR = sqrt(hand_dR2);
                
                std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
                std::cout << " hand_dR is " << hand_dR <<std::endl;
                std::cout << " cell-track distance is " << celltrackdist <<std::endl;
                
                
                
                totalCells += 1;
                totalCellsinRing += 1;
        
                totalEnergyPerRing += ((*firstPair).first)->energy();
                totalEnergyPerCell = ((*firstPair).first)->energy();
                std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
                std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
        
                //implement track E here and divide cluster E by track E to have energy ratio
        // 	    indexofCell = (*firstPair).second;
        // check that volume units are OK -- volume from database
                cellVolume = DDE->volume();
                std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
        
                energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
                std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
                std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
                energyDensityPerRing += energyDensityPerCell;
                std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
        
        // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
        
                        //where should these go?
                averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
                std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
                std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
                layerToStore = (eflowCaloENUM)i;
                layerToStoreVector.push_back(layerToStore);
                std::cout<<"layerToStore is "<< layerToStore << std::endl;
                radiusToStore = (indexofRing)*ringThickness;
                std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
                efRecTrack->setLayerCellOrder(layerToStore);
                efRecTrack->setRadiusCellOrder(radiusToStore);
                efRecTrack->setAverageEnergyDensity(averageEnergyDensityPerRing);
                
                if (radiusToStore > celltrackdist){
                    std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
                }
                else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
      
              }
      
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
        if (averageEnergyDensityPerRing != 0) {
        std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
        std::cout<<"layerToStore is "<< layerToStore << std::endl;
        std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
        std::cout<<"indexofRing is "<< indexofRing << std::endl;	
        }
        else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
    }

//     efRecTrack->setLayerCellOrder(layerToStoreVector);
//     layerToStoreVector.clear();
            
  
	  
	    }
        if (i==2){
        
        //loop over ring thicknesses and then ask for iterator corresponding to that loop
	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
	        
//         std::cout<<"after rings"<<std::endl;
          int indexofRing = 0;
          int layerToStore = -999;
          double radiusToStore = 0;
          std::vector<int> layerToStoreVector;
          double totalEnergyPerCell = 0;
      
          int indexofCell = 0;
          double energyDensityPerCell = -666;
          double totalEnergyPerRing = 0;

          double cellVolume = 0;
          int totalCells = 0;
          double previouseta = 0;
          double previousphi = 0;
              
          int n;
        for (n=1; n<100; n++){
        
        // use calorimeterCellList.
        
        
        
          CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
//           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
          // 
//           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
        // for (;beginRing != endRing; beginRing++){
//         save # of rings 
          indexofRing += 1;
          std::cout<<"Ring Number "<<indexofRing<< std::endl;
          std::cout<<"within ring loop "<< std::endl;
          std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
          std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
          std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
      
          int totalCellsinRing = 0;
          double energyDensityPerRing = 0;
          double averageEnergyDensityPerRing = 0;
      
          for (; firstPair != lastPair; ++firstPair) {
                std::cout<<"within pair loop"<<std::endl;
                const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
                CaloCell_ID::CaloSample sampling = DDE->getSampling();
                
                
                if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
                    break;
                }
                
                
                
                std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
                std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
                
                previouseta = ((*firstPair).first)->eta();
                previousphi = ((*firstPair).first)->phi();
                
                double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
                
                if (celltrackdist > 2){
                    break;
                }
                
                double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
                double hand_dR = sqrt(hand_dR2);
                
                std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
                std::cout << " hand_dR is " << hand_dR <<std::endl;
                std::cout << " cell-track distance is " << celltrackdist <<std::endl;
                
                
                
                totalCells += 1;
                totalCellsinRing += 1;
        
                totalEnergyPerRing += ((*firstPair).first)->energy();
                totalEnergyPerCell = ((*firstPair).first)->energy();
                std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
                std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
        
                //implement track E here and divide cluster E by track E to have energy ratio
        // 	    indexofCell = (*firstPair).second;
        // check that volume units are OK -- volume from database
                cellVolume = DDE->volume();
                std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
        
                energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
                std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
                std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
                energyDensityPerRing += energyDensityPerCell;
                std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
        
        // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
        
                        //where should these go?
                averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
                std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
                std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
                layerToStore = (eflowCaloENUM)i;
                layerToStoreVector.push_back(layerToStore);
                std::cout<<"layerToStore is "<< layerToStore << std::endl;
                radiusToStore = (indexofRing)*ringThickness;
                std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
                efRecTrack->setLayerCellOrder(layerToStore);
                efRecTrack->setRadiusCellOrder(radiusToStore);
                efRecTrack->setAverageEnergyDensity(averageEnergyDensityPerRing);
                
                if (radiusToStore > celltrackdist){
                    std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
                }
                else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
      
              }
      
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
        if (averageEnergyDensityPerRing != 0) {
        std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
        std::cout<<"layerToStore is "<< layerToStore << std::endl;
        std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
        std::cout<<"indexofRing is "<< indexofRing << std::endl;	
        }
        else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
    }
//     efRecTrack->setLayerCellOrder(layerToStoreVector);
//     layerToStoreVector.clear();

            
  
	  
	    }
// 	    if (i==3){
//         
//         //loop over ring thicknesses and then ask for iterator corresponding to that loop
// 	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// // 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
// 	        
// //         std::cout<<"after rings"<<std::endl;
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//         for (n=1; n<100; n++){
//         
//         // use calorimeterCellList.
//         
//         
//         
//           CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
// //           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
//           // 
// //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//         // for (;beginRing != endRing; beginRing++){
// //         save # of rings 
//           indexofRing += 1;
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
//           int totalCellsinRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                 if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                     break;
//                 }
//                 
//                 
//                 
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 
//                 previouseta = ((*firstPair).first)->eta();
//                 previousphi = ((*firstPair).first)->phi();
//                 
//                 double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
//                 
//                 if (celltrackdist > 2){
//                     break;
//                 }
//                 
//                 double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
//                 double hand_dR = sqrt(hand_dR2);
//                 
//                 std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
//                 std::cout << " hand_dR is " << hand_dR <<std::endl;
//                 std::cout << " cell-track distance is " << celltrackdist <<std::endl;
//                 
//                 
//                 
//                 totalCells += 1;
//                 totalCellsinRing += 1;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK -- volume from database
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                         //where should these go?
//                 averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                 std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                 std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                 layerToStore = (eflowCaloENUM)i;
//                 std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                 radiusToStore = (indexofRing)*ringThickness;
//                 std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//                 
//                 if (radiusToStore > celltrackdist){
//                     std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
//                 }
//                 else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
//       
//               }
//       
// //         //where should these go?
// //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
// //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         if (averageEnergyDensityPerRing != 0) {
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//         std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//         }
//         else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//     }
// 
//             
//   
// 	  
// 	    }
// 	    if (i==4){
//         
//         //loop over ring thicknesses and then ask for iterator corresponding to that loop
// 	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// // 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
// 	        
// //         std::cout<<"after rings"<<std::endl;
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//         for (n=1; n<100; n++){
//         
//         // use calorimeterCellList.
//         
//         
//         
//           CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
// //           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
//           // 
// //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//         // for (;beginRing != endRing; beginRing++){
// //         save # of rings 
//           indexofRing += 1;
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
//           int totalCellsinRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                 if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                     break;
//                 }
//                 
//                 
//                 
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 
//                 previouseta = ((*firstPair).first)->eta();
//                 previousphi = ((*firstPair).first)->phi();
//                 
//                 double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
//                 
//                 if (celltrackdist > 2){
//                     break;
//                 }
//                 
//                 double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
//                 double hand_dR = sqrt(hand_dR2);
//                 
//                 std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
//                 std::cout << " hand_dR is " << hand_dR <<std::endl;
//                 std::cout << " cell-track distance is " << celltrackdist <<std::endl;
//                 
//                 
//                 
//                 totalCells += 1;
//                 totalCellsinRing += 1;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK -- volume from database
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                         //where should these go?
//                 averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                 std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                 std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                 layerToStore = (eflowCaloENUM)i;
//                 std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                 radiusToStore = (indexofRing)*ringThickness;
//                 std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//                 
//                 if (radiusToStore > celltrackdist){
//                     std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
//                 }
//                 else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
//       
//               }
//       
// //         //where should these go?
// //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
// //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         if (averageEnergyDensityPerRing != 0) {
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//         std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//         }
//         else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//     }
// 
//             
//   
// 	  
// 	    }
// 	    if (i==5){
//         
//         //loop over ring thicknesses and then ask for iterator corresponding to that loop
// 	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// // 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
// 	        
// //         std::cout<<"after rings"<<std::endl;
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//         for (n=1; n<100; n++){
//         
//         // use calorimeterCellList.
//         
//         
//         
//           CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
// //           CellIt beginRing = calorimeterCellList.find((eflowCaloENUM)i, ringThickness*(n-1));
//           // 
// //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//         // for (;beginRing != endRing; beginRing++){
// //         save # of rings 
//           indexofRing += 1;
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           
//           
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::cout<<"after tempVector "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::cout<<"after firstPair"<< std::endl;
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//           std::cout<<"after lastPair "<< std::endl;
//           
//           int totalCellsinRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           std::cout<<"after variable definitions "<< std::endl;
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                 if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                     break;
//                 }
//                 
//                 
//                 
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 
//                 previouseta = ((*firstPair).first)->eta();
//                 previousphi = ((*firstPair).first)->phi();
//                 
//                 double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
//                 
//                 if (celltrackdist > 2){
//                     break;
//                 }
//                 
//                 double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
//                 double hand_dR = sqrt(hand_dR2);
//                 
//                 std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
//                 std::cout << " hand_dR is " << hand_dR <<std::endl;
//                 std::cout << " cell-track distance is " << celltrackdist <<std::endl;
//                 
//                 
//                 
//                 totalCells += 1;
//                 totalCellsinRing += 1;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK -- volume from database
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                         //where should these go?
//                 averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                 std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                 std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                 layerToStore = (eflowCaloENUM)i;
//                 std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                 radiusToStore = (indexofRing)*ringThickness;
//                 std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//                 
//                 if (radiusToStore > celltrackdist){
//                     std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
//                 }
//                 else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
//       
//               }
//       
// //         //where should these go?
// //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
// //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         if (averageEnergyDensityPerRing != 0) {
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//         std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//         }
//         else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//     }
// 
//             
//   
// 	  
// 	    }
//         if (i==6){
//         
//         //loop over ring thicknesses and then ask for iterator corresponding to that loop
// 	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// // 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
// 	        
// //         std::cout<<"after rings"<<std::endl;
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//         for (n=1; n<100; n++){
//         
//         // use calorimeterCellList.
//         
//         
//         
//           CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
// //           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
//           // 
// //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//         // for (;beginRing != endRing; beginRing++){
// //         save # of rings 
//           indexofRing += 1;
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
//           int totalCellsinRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                 if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                     break;
//                 }
//                 
//                 
//                 
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 
//                 previouseta = ((*firstPair).first)->eta();
//                 previousphi = ((*firstPair).first)->phi();
//                 
//                 double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
//                 
//                 if (celltrackdist > 2){
//                     break;
//                 }
//                 
//                 double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
//                 double hand_dR = sqrt(hand_dR2);
//                 
//                 std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
//                 std::cout << " hand_dR is " << hand_dR <<std::endl;
//                 std::cout << " cell-track distance is " << celltrackdist <<std::endl;
//                 
//                 
//                 
//                 totalCells += 1;
//                 totalCellsinRing += 1;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK -- volume from database
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                         //where should these go?
//                 averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                 std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                 std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                 layerToStore = (eflowCaloENUM)i;
//                 std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                 radiusToStore = (indexofRing)*ringThickness;
//                 std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//                 
//                 if (radiusToStore > celltrackdist){
//                     std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
//                 }
//                 else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
//       
//               }
//       
// //         //where should these go?
// //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
// //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         if (averageEnergyDensityPerRing != 0) {
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//         std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//         }
//         else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//     }
// 
//             
//   
// 	  
// 	    }
// 	    if (i==7){
//         
//         //loop over ring thicknesses and then ask for iterator corresponding to that loop
// 	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// // 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
// 	        
// //         std::cout<<"after rings"<<std::endl;
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//         for (n=1; n<100; n++){
//         
//         // use calorimeterCellList.
//         
//         
//         
//           CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
// //           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
//           // 
// //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//         // for (;beginRing != endRing; beginRing++){
// //         save # of rings 
//           indexofRing += 1;
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
//           int totalCellsinRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                 if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                     break;
//                 }
//                 
//                 
//                 
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 
//                 previouseta = ((*firstPair).first)->eta();
//                 previousphi = ((*firstPair).first)->phi();
//                 
//                 double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
//                 
//                 if (celltrackdist > 2){
//                     break;
//                 }
//                 
//                 double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
//                 double hand_dR = sqrt(hand_dR2);
//                 
//                 std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
//                 std::cout << " hand_dR is " << hand_dR <<std::endl;
//                 std::cout << " cell-track distance is " << celltrackdist <<std::endl;
//                 
//                 
//                 
//                 totalCells += 1;
//                 totalCellsinRing += 1;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK -- volume from database
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                         //where should these go?
//                 averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                 std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                 std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                 layerToStore = (eflowCaloENUM)i;
//                 std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                 radiusToStore = (indexofRing)*ringThickness;
//                 std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//                 
//                 if (radiusToStore > celltrackdist){
//                     std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
//                 }
//                 else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
//       
//               }
//       
// //         //where should these go?
// //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
// //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         if (averageEnergyDensityPerRing != 0) {
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//         std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//         }
//         else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//     }
// 
//             
//   
// 	  
// 	    }
// 	    if (i==8){
//         
//         //loop over ring thicknesses and then ask for iterator corresponding to that loop
// 	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// // 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
// 	        
// //         std::cout<<"after rings"<<std::endl;
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//         for (n=1; n<100; n++){
//         
//         // use calorimeterCellList.
//         
//         
//         
//           CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
// //           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
//           // 
// //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//         // for (;beginRing != endRing; beginRing++){
// //         save # of rings 
//           indexofRing += 1;
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
//           int totalCellsinRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                 if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                     break;
//                 }
//                 
//                 
//                 
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 
//                 previouseta = ((*firstPair).first)->eta();
//                 previousphi = ((*firstPair).first)->phi();
//                 
//                 double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
//                 
//                 if (celltrackdist > 2){
//                     break;
//                 }
//                 
//                 double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
//                 double hand_dR = sqrt(hand_dR2);
//                 
//                 std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
//                 std::cout << " hand_dR is " << hand_dR <<std::endl;
//                 std::cout << " cell-track distance is " << celltrackdist <<std::endl;
//                 
//                 
//                 
//                 totalCells += 1;
//                 totalCellsinRing += 1;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK -- volume from database
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                         //where should these go?
//                 averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                 std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                 std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                 layerToStore = (eflowCaloENUM)i;
//                 std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                 radiusToStore = (indexofRing)*ringThickness;
//                 std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//                 
//                 if (radiusToStore > celltrackdist){
//                     std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
//                 }
//                 else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
//       
//               }
//       
// //         //where should these go?
// //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
// //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         if (averageEnergyDensityPerRing != 0) {
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//         std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//         }
//         else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//     }
// 
//             
//   
// 	  
// 	    }
// 	    if (i==9){
//         
//         //loop over ring thicknesses and then ask for iterator corresponding to that loop
// 	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// // 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
// 	        
// //         std::cout<<"after rings"<<std::endl;
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//         for (n=1; n<100; n++){
//         
//         // use calorimeterCellList.
//         
//         
//         
//           CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
// //           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
//           // 
// //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//         // for (;beginRing != endRing; beginRing++){
// //         save # of rings 
//           indexofRing += 1;
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
//           int totalCellsinRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                 if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                     break;
//                 }
//                 
//                 
//                 
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 
//                 previouseta = ((*firstPair).first)->eta();
//                 previousphi = ((*firstPair).first)->phi();
//                 
//                 double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
//                 
//                 if (celltrackdist > 2){
//                     break;
//                 }
//                 
//                 double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
//                 double hand_dR = sqrt(hand_dR2);
//                 
//                 std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
//                 std::cout << " hand_dR is " << hand_dR <<std::endl;
//                 std::cout << " cell-track distance is " << celltrackdist <<std::endl;
//                 
//                 
//                 
//                 totalCells += 1;
//                 totalCellsinRing += 1;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK -- volume from database
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                         //where should these go?
//                 averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                 std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                 std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                 layerToStore = (eflowCaloENUM)i;
//                 std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                 radiusToStore = (indexofRing)*ringThickness;
//                 std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//                 
//                 if (radiusToStore > celltrackdist){
//                     std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
//                 }
//                 else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
//       
//               }
//       
// //         //where should these go?
// //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
// //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         if (averageEnergyDensityPerRing != 0) {
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//         std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//         }
//         else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//     }
// 
//             
//   
// 	  
// 	    }
// 	    if (i==10){
//         
//         //loop over ring thicknesses and then ask for iterator corresponding to that loop
// 	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// // 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
// 	        
// //         std::cout<<"after rings"<<std::endl;
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//         for (n=1; n<100; n++){
//         
//         // use calorimeterCellList.
//         
//         
//         
//           CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
// //           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
//           // 
// //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//         // for (;beginRing != endRing; beginRing++){
// //         save # of rings 
//           indexofRing += 1;
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
//           int totalCellsinRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                 if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                     break;
//                 }
//                 
//                 
//                 
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 
//                 previouseta = ((*firstPair).first)->eta();
//                 previousphi = ((*firstPair).first)->phi();
//                 
//                 double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
//                 
//                 if (celltrackdist > 2){
//                     break;
//                 }
//                 
//                 double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
//                 double hand_dR = sqrt(hand_dR2);
//                 
//                 std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
//                 std::cout << " hand_dR is " << hand_dR <<std::endl;
//                 std::cout << " cell-track distance is " << celltrackdist <<std::endl;
//                 
//                 
//                 
//                 totalCells += 1;
//                 totalCellsinRing += 1;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK -- volume from database
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                         //where should these go?
//                 averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                 std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                 std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                 layerToStore = (eflowCaloENUM)i;
//                 std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                 radiusToStore = (indexofRing)*ringThickness;
//                 std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//                 
//                 if (radiusToStore > celltrackdist){
//                     std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
//                 }
//                 else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
//       
//               }
//       
// //         //where should these go?
// //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
// //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         if (averageEnergyDensityPerRing != 0) {
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//         std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//         }
//         else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//     }
// 
//             
//   
// 	  
// 	    }
// 	    if (i==11){
//         
//         //loop over ring thicknesses and then ask for iterator corresponding to that loop
// 	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// // 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
// 	        
// //         std::cout<<"after rings"<<std::endl;
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//         for (n=1; n<100; n++){
//         
//         // use calorimeterCellList.
//         
//         
//         
//           CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
// //           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
//           // 
// //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//         // for (;beginRing != endRing; beginRing++){
// //         save # of rings 
//           indexofRing += 1;
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
//           int totalCellsinRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                 if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                     break;
//                 }
//                 
//                 
//                 
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 
//                 previouseta = ((*firstPair).first)->eta();
//                 previousphi = ((*firstPair).first)->phi();
//                 
//                 double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
//                 
//                 if (celltrackdist > 2){
//                     break;
//                 }
//                 
//                 double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
//                 double hand_dR = sqrt(hand_dR2);
//                 
//                 std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
//                 std::cout << " hand_dR is " << hand_dR <<std::endl;
//                 std::cout << " cell-track distance is " << celltrackdist <<std::endl;
//                 
//                 
//                 
//                 totalCells += 1;
//                 totalCellsinRing += 1;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK -- volume from database
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                         //where should these go?
//                 averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                 std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                 std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                 layerToStore = (eflowCaloENUM)i;
//                 std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                 radiusToStore = (indexofRing)*ringThickness;
//                 std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//                 
//                 if (radiusToStore > celltrackdist){
//                     std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
//                 }
//                 else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
//       
//               }
//       
// //         //where should these go?
// //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
// //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         if (averageEnergyDensityPerRing != 0) {
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//         std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//         }
//         else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//     }
// 
//             
//   
// 	  
// 	    }
// 	    if (i==12){
//         
//         //loop over ring thicknesses and then ask for iterator corresponding to that loop
// 	        // CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness);
// // 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*2);
// 	        
// //         std::cout<<"after rings"<<std::endl;
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//         for (n=1; n<100; n++){
//         
//         // use calorimeterCellList.
//         
//         
//         
//           CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(n-1));
// //           CellIt beginRing = calorimeterCellList.find(eflowCaloENUM::EMB1, EMB1_ringThickness*n);
//           // 
// //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//         // for (;beginRing != endRing; beginRing++){
// //         save # of rings 
//           indexofRing += 1;
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
//           int totalCellsinRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                 if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                     break;
//                 }
//                 
//                 
//                 
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 
//                 previouseta = ((*firstPair).first)->eta();
//                 previousphi = ((*firstPair).first)->phi();
//                 
//                 double celltrackdist = calorimeterCellList.dR(previouseta,previousphi,layer);
//                 
//                 if (celltrackdist > 2){
//                     break;
//                 }
//                 
//                 double hand_dR2 = (((*firstPair).first)->eta()-eta_extr)*(((*firstPair).first)->eta()-eta_extr) + (((*firstPair).first)->phi()-phi_extr)*(((*firstPair).first)->phi()-phi_extr);
//                 double hand_dR = sqrt(hand_dR2);
//                 
//                 std::cout << " hand dR2 is " << hand_dR2 <<std::endl;
//                 std::cout << " hand_dR is " << hand_dR <<std::endl;
//                 std::cout << " cell-track distance is " << celltrackdist <<std::endl;
//                 
//                 
//                 
//                 totalCells += 1;
//                 totalCellsinRing += 1;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK -- volume from database
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                         //where should these go?
//                 averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                 std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                 std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                 layerToStore = (eflowCaloENUM)i;
//                 std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                 radiusToStore = (indexofRing)*ringThickness;
//                 std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//                 
//                 if (radiusToStore > celltrackdist){
//                     std::cout<<"cell-track distance is smaller than ring radius. GOOD!"<<std::endl;
//                 }
//                 else { std::cout<<"cell-track distance is NOT smaller than ring radius. NOT GOOD!"<<std::endl; ;}
//       
//               }
//       
// //         //where should these go?
// //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
// //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         if (averageEnergyDensityPerRing != 0) {
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//         std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//         }
//         else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//     }
// 
//             
//   
// 	  
// 	    }
	    
	    
	    
	    
	    
	    
// 	    if (i==1){
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//           for (n=1; n<100; n++){
//         
// //             poorly named? this just iterates over the cells -- not creates a ring..?
//               CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB2, EMB2_ringThickness*n);
//               // 
//     //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//             // for (;beginRing != endRing; beginRing++){
//     //         save # of rings 
//               indexofRing += 1;
//               std::cout<<"Ring Number "<<indexofRing<< std::endl;
//               std::cout<<"within ring loop "<< std::endl;
//               std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//               std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//               std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
//               int totalCellsinRing = 0;
//               double energyDensityPerRing = 0;
//               double averageEnergyDensityPerRing = 0;
//       
//               for (; firstPair != lastPair; ++firstPair) {
//                     std::cout<<"within pair loop"<<std::endl;
//                     const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                     CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                     if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                         break;
//                     }
//                 
//                     std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                     previouseta = ((*firstPair).first)->eta();
//                     previousphi = ((*firstPair).first)->phi();
//                     std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                     totalCells += 1;
//                     totalCellsinRing += 1;
//         
//                     totalEnergyPerRing += ((*firstPair).first)->energy();
//                     totalEnergyPerCell = ((*firstPair).first)->energy();
//                     std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                     std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                     //implement track E here and divide cluster E by track E to have energy ratio
//             // 	    indexofCell = (*firstPair).second;
//             // check that volume units are OK
//                     cellVolume = DDE->volume();
//                     std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                     energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                     std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                     std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                     energyDensityPerRing += energyDensityPerCell;
//                     std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//             // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                             //where should these go?
//                     averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                     std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                     std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                     layerToStore = (eflowCaloENUM)i;
//                     std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                     radiusToStore = (indexofRing)*EMB2_ringThickness;
//                     std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//       
//                   }
//       
//     //         //where should these go?
//     //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//     //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//             if (averageEnergyDensityPerRing != 0) {
//             std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//             std::cout<<"layerToStore is "<< layerToStore << std::endl;
//             std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//             std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//             }
//             else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//         }
//     }
// 	    if (i==2){
//           int indexofRing = 0;
//           int layerToStore = -999;
//           double radiusToStore = 0;
//           double totalEnergyPerCell = 0;
//       
//           int indexofCell = 0;
//           double energyDensityPerCell = -666;
//           double totalEnergyPerRing = 0;
// 
//           double cellVolume = 0;
//           int totalCells = 0;
//           double previouseta = 0;
//           double previousphi = 0;
//               
//           int n;
//           for (n=1; n<100; n++){
//         
// //             poorly named? this just iterates over the cells -- not creates a ring..?
//               CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB3, EMB3_ringThickness*n);
//               // 
//     //           CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB1, EMB1_ringThickness*n+1);
//             // for (;beginRing != endRing; beginRing++){
//     //         save # of rings 
//               indexofRing += 1;
//               std::cout<<"Ring Number "<<indexofRing<< std::endl;
//               std::cout<<"within ring loop "<< std::endl;
//               std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//               std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//               std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
//               int totalCellsinRing = 0;
//               double energyDensityPerRing = 0;
//               double averageEnergyDensityPerRing = 0;
//       
//               for (; firstPair != lastPair; ++firstPair) {
//                     std::cout<<"within pair loop"<<std::endl;
//                     const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                     CaloCell_ID::CaloSample sampling = DDE->getSampling();
//                 
//                 
//                     if (previouseta == ((*firstPair).first)->eta() && previousphi == ((*firstPair).first)->phi()){
//                         break;
//                     }
//                 
//                     std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                     previouseta = ((*firstPair).first)->eta();
//                     previousphi = ((*firstPair).first)->phi();
//                     std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                     totalCells += 1;
//                     totalCellsinRing += 1;
//         
//                     totalEnergyPerRing += ((*firstPair).first)->energy();
//                     totalEnergyPerCell = ((*firstPair).first)->energy();
//                     std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                     std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                     //implement track E here and divide cluster E by track E to have energy ratio
//             // 	    indexofCell = (*firstPair).second;
//             // check that volume units are OK
//                     cellVolume = DDE->volume();
//                     std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                     energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                     std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                     std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                     energyDensityPerRing += energyDensityPerCell;
//                     std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//             // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
//                             //where should these go?
//                     averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//                     std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//                     std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//                     layerToStore = (eflowCaloENUM)i;
//                     std::cout<<"layerToStore is "<< layerToStore << std::endl;
//                     radiusToStore = (indexofRing)*EMB3_ringThickness;
//                     std::cout<<"radiusToStore is "<< radiusToStore << std::endl;
//       
//                   }
//       
//     //         //where should these go?
//     //         averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));
//     //         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//             if (averageEnergyDensityPerRing != 0) {
//             std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//             std::cout<<"layerToStore is "<< layerToStore << std::endl;
//             std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//             std::cout<<"indexofRing is "<< indexofRing << std::endl;	
//             }
//             else {std::cout<<"averageEnergyDensityPerRing = 0"<<std::endl;}
//         }
//             
//   
// 	  
// 	    }
// 	    {
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB2, EMB2_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB2, EMB2_ringThickness*2);
// 	        
// 	       std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000000000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000000000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::EMB2;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*EMB2_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
//   
// 	  
// 	}
// 	    }
// 	    if (i==2){
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB3, EMB3_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EMB3, EMB3_ringThickness*100);
// 	        
// 	        	       std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::EMB3;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*EMB3_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
// 	    }
// 	    if (i==3){
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EME1, EME1_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EME1, EME1_ringThickness*100);
// 	        
// 	        std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000000000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000000000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::EME1;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*EME1_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
// 	    }
// 	    if (i==4){
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EME2, EME2_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EME2, EME2_ringThickness*100);
// 	        
// 	        	       std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::EME2;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*EME2_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
// 	    }
// 	    if (i==5){
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EME3, EME3_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::EME3, EME3_ringThickness*100);
// 	        
// 	        	       std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::EME3;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*EME3_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
// 	    }
// 	    if (i==6){
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::HEC1, HEC1_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::HEC1, HEC1_ringThickness*100);
// 	        
// 	        	       std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000000000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000000000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::HEC1;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*HEC1_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
// 	    }
// 	    if (i==7){
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::HEC2, HEC2_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::HEC2, HEC2_ringThickness*100);
// 	        
// 	        	       std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000000000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000000000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::HEC2;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*HEC2_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
// 	    }
// 	    if (i==8){
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::HEC3, HEC3_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::HEC3, HEC3_ringThickness*100);
// 	        
// 	        	       std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000000000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000000000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::HEC3;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*HEC3_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
// 	    }
// 	    if (i==9){
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::HEC4, HEC4_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::HEC4, HEC4_ringThickness*100);
// 	        
// 	        	       std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000000000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000000000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::HEC4;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*HEC4_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
// 	    }
// 	    if (i==10){
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::Tile1, Tile1_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::Tile1, Tile1_ringThickness*100);
// 	        
// 	        	       std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000000000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000000000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::Tile1;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*Tile1_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
// 	    }
// 	    if (i==11){
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::Tile2, Tile2_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::Tile2, Tile2_ringThickness*100);
// 	        
// 	        	       std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000000000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000000000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::Tile2;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*Tile2_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
// 	    }
// 	    if (i==12){
// 	        CellIt beginRing = calorimeterCellList.getLowerBound(eflowCaloENUM::Tile3, Tile3_ringThickness);
// 	        CellIt endRing = calorimeterCellList.getLowerBound(eflowCaloENUM::Tile3, Tile3_ringThickness*100);
// 	        
// 	        	       std::cout<<"after rings"<<std::endl;
// 	        int indexofRing = 0;
// 	        int layerToStore = -999;
// 	        double radiusToStore = 0;
//             double totalEnergyPerCell = 0;
//       
//               
//               double energyDensityPerCell = -666;
//               double cellVolume = 0;
//               int totalCells = 0;
// 
//         for (;beginRing != endRing; beginRing++){
//           indexofRing += 1;
//           
//           double totalEnergyPerRing = 0;
//           double energyDensityPerRing = 0;
//           double averageEnergyDensityPerRing = 0;
//           int indexofCell = 0;
//           
//           std::cout<<"Ring Number "<<indexofRing<< std::endl;
//           std::cout<<"within ring loop "<< std::endl;
//           std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
//           std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
//           std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
//       
// 
//       
//           for (; firstPair != lastPair; ++firstPair) {
//                 indexofCell += 1;
//                 std::cout<<"Cell Number "<<indexofCell<< std::endl;
//                 std::cout<<"within pair loop"<<std::endl;
//                 const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
//                 CaloCell_ID::CaloSample sampling = DDE->getSampling();
//         
//                 std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
//                 std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
//                 totalCells += 1;
//                 std::cout<<"TotalCells "<<totalCells<< std::endl;
//         
//                 totalEnergyPerRing += ((*firstPair).first)->energy();
//                 totalEnergyPerCell = ((*firstPair).first)->energy();
//                 std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
//                 std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
//         
//                 //implement track E here and divide cluster E by track E to have energy ratio
//         // 	    indexofCell = (*firstPair).second;
//         // check that volume units are OK
//                 cellVolume = DDE->volume();
//                 std::cout << " cell volume is " << cellVolume/1000000000.<<std::endl;
//         
//                 energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000000000.);
//                 std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
//                 std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
//                 energyDensityPerRing += energyDensityPerCell;
//                 std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
//         
//         // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
//         
// 
//       
//               }
//       
//         //where should these go?
//         averageEnergyDensityPerRing = energyDensityPerRing/((indexofCell)*(efRecTrack->getTrack()->e()/1000.));
//         std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
//         std::cout << " index of cell is " << indexofCell;
//         std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;
//         layerToStore = eflowCaloENUM::Tile3;
//         std::cout<<"layerToStore is "<< layerToStore << std::endl;
//         radiusToStore = (indexofRing)*Tile3_ringThickness;
//         std::cout<<"radiusToStore is "<< radiusToStore << std::endl;	
//     }
// 	    }
	    
// 	    	std::cout<<"after rings"<<std::endl;
// 	
// 	  double totalEnergyPerCell = 0;
// 	  int indexofRing = 0;
// 	  int indexofCell = 0;
// 	  double energyDensityPerCell = -666;
// 	  double totalEnergyPerRing = 0;
// 	  double energyDensityPerRing = 0;
// 	  double averageEnergyDensityPerRing = 0;
// 	  double cellVolume = 0;
// 	  int totalCells = 0;
// 
// 	for (;beginRing != endRing; beginRing++){
// 	  indexofRing += 1;
// 	  std::cout<<"Ring Number "<<indexofRing<< std::endl;
// 	  std::cout<<"within ring loop "<< std::endl;
// 	  std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
// 	  std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
// 	  std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
// 	  
// 
// 	  
// 	  for (; firstPair != lastPair; ++firstPair) {
// 	    std::cout<<"within pair loop"<<std::endl;
// 	    const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
// 	    CaloCell_ID::CaloSample sampling = DDE->getSampling();
// 	    
// 	    std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
// 	    std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
// 	    totalCells += 1;
// 	    
// 	    totalEnergyPerRing += ((*firstPair).first)->energy();
// 	    totalEnergyPerCell = ((*firstPair).first)->energy();
// 	    std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
// 	    std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
// 	    
// 	    //implement track E here and divide cluster E by track E to have energy ratio
// // 	    indexofCell = (*firstPair).second;
// // check that volume units are OK
// 	    cellVolume = DDE->volume();
// 	    std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
// 	    
// 	    energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
// 	    std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
// 	    std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
// 	    energyDensityPerRing += energyDensityPerCell;
// 	    std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
// 	    
// // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
// 	    
// 
// 	  
// 	  }
// 	  
// 	//where should these go?
// 	averageEnergyDensityPerRing = energyDensityPerRing/((totalCells)*(efRecTrack->getTrack()->e()/1000.));
// // 	layerToStore = (layer)
// //  radiusToStore = (indexofRing)*ring_thickness
// 	std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
// 	std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;	  
// 	  
// 	}
// 	    else{
// 	        std::cout<<"eflow calo layer != 0"<<std::endl;
// 	        continue;
//         }	
    }

	
// 	std::cout<<"after rings"<<std::endl;
// 	
// 	  double totalEnergyPerCell = 0;
// 	  int indexofRing = 0;
// 	  int indexofCell = 0;
// 	  double energyDensityPerCell = -666;
// 	  double totalEnergyPerRing = 0;
// 	  double energyDensityPerRing = 0;
// 	  double averageEnergyDensityPerRing = 0;
// 	  double cellVolume = 0;
// 	  int totalCells = 0;
// 
// 	for (;beginRing != endRing; beginRing++){
// 	  indexofRing += 1;
// 	  std::cout<<"Ring Number "<<indexofRing<< std::endl;
// 	  std::cout<<"within ring loop "<< std::endl;
// 	  std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
// 	  std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
// 	  std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
// 	  
// 
// 	  
// 	  for (; firstPair != lastPair; ++firstPair) {
// 	    std::cout<<"within pair loop"<<std::endl;
// 	    const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
// 	    CaloCell_ID::CaloSample sampling = DDE->getSampling();
// 	    
// 	    std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
// 	    std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
// 	    totalCells += 1;
// 	    
// 	    totalEnergyPerRing += ((*firstPair).first)->energy();
// 	    totalEnergyPerCell = ((*firstPair).first)->energy();
// 	    std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
// 	    std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
// 	    
// 	    //implement track E here and divide cluster E by track E to have energy ratio
// // 	    indexofCell = (*firstPair).second;
// // check that volume units are OK
// 	    cellVolume = DDE->volume();
// 	    std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
// 	    
// 	    energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
// 	    std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
// 	    std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
// 	    energyDensityPerRing += energyDensityPerCell;
// 	    std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
// 	    
// // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
// 	    
// 
// 	  
// 	  }
// 	  
// 	//where should these go?
// 	averageEnergyDensityPerRing = energyDensityPerRing/((totalCells)*(efRecTrack->getTrack()->e()/1000.));
// // 	layerToStore = (layer)
// //  radiusToStore = (indexofRing)*ring_thickness
// 	std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
// 	std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;	  
// 	  
// 	}
	

    }

     
  }
  
  }



// void eflowCellLevelSubtractionTool::calculateAverageEnergyDensity() {
// 
// 
// 	  double totalEnergyPerCell = 0;
// 	  
// 	  int indexofCell = 0;
// 	  double energyDensityPerCell = -666;
// 	  double totalEnergyPerRing = 0;
// 	  double energyDensityPerRing = 0;
// 	  double averageEnergyDensityPerRing = 0;
// 	  double cellVolume = 0;
// 	  int totalCells = 0;
// 
// 	for (;beginRing != endRing; beginRing++){
// 	  indexofRing += 1;
// 	  std::cout<<"Ring Number "<<indexofRing<< std::endl;
// 	  std::cout<<"within ring loop "<< std::endl;
// 	  std::vector<std::pair<CaloCell*,int> > tempVector = (*beginRing).second;
// 	  std::vector<std::pair<CaloCell*,int> >::iterator firstPair = tempVector.begin();
// 	  std::vector<std::pair<CaloCell*,int> >::iterator lastPair = tempVector.end();
// 	  
// 
// 	  
// 	  for (; firstPair != lastPair; ++firstPair) {
// 	    std::cout<<"within pair loop"<<std::endl;
// 	    const CaloDetDescrElement* DDE = ((*firstPair).first)->caloDDE();
// 	    CaloCell_ID::CaloSample sampling = DDE->getSampling();
// 	    
// 	    std::cout << " cell eta and phi are " << ((*firstPair).first)->eta() << " and " << ((*firstPair).first)->phi() << " with index " << (*firstPair).second << " and sampling of " << sampling << std::endl;
// 	    std::cout << " cell energy is " << ((*firstPair).first)->energy()<<std::endl;
// 	    totalCells += 1;
// 	    
// 	    totalEnergyPerRing += ((*firstPair).first)->energy();
// 	    totalEnergyPerCell = ((*firstPair).first)->energy();
// 	    std::cout << " Total E per Cell is " << totalEnergyPerCell<<std::endl;
// 	    std::cout << " Total E per Ring is " << totalEnergyPerRing<<std::endl;
// 	    
// 	    //implement track E here and divide cluster E by track E to have energy ratio
// // 	    indexofCell = (*firstPair).second;
// // check that volume units are OK
// 	    cellVolume = DDE->volume();
// 	    std::cout << " cell volume is " << cellVolume/1000.<<std::endl;
// 	    
// 	    energyDensityPerCell = totalEnergyPerCell/(cellVolume/1000.);
// 	    std::cout << " E density per Cell is " << energyDensityPerCell<<std::endl;
// 	    std::cout << " Initial added E density per Cell is " << energyDensityPerRing<<std::endl;
// 	    energyDensityPerRing += energyDensityPerCell;
// 	    std::cout << " Final added E density per Cell is " << energyDensityPerRing<<std::endl;
// 	    
// // 	    std::cout << " Added cell E densities are " << energyDensityPerRing<<std::endl;
// 	    
// 
// 	  
// 	  }
// 	  
// 	//where should these go?
// 	averageEnergyDensityPerRing = energyDensityPerRing/((totalCells)*(efRecTrack->getTrack()->e()/1000.));
// 	std::cout << " track E is " << efRecTrack->getTrack()->e()/1000.;
// 	std::cout << " Average E density per Ring is " << averageEnergyDensityPerRing<<std::endl;	
// }
// 
// }


void eflowCellLevelSubtractionTool::performSubtraction() {

  std::cout<<"performing subtraction"<<std::endl;
  unsigned int nEFCaloObs = m_eflowCaloObjectContainer->size();
  for (unsigned int iEFCalOb = 0; iEFCalOb < nEFCaloObs; ++iEFCalOb) {
    eflowCaloObject* thisEflowCaloObject = m_eflowCaloObjectContainer->at(iEFCalOb);

    unsigned int nClusters = thisEflowCaloObject->nClusters();
    if (nClusters < 1) {
      std::cout<<"if nClusters < 1"<<std::endl;
    continue;
    }

    /* Get matched cluster via Links */

    double expectedEnergy = thisEflowCaloObject->getExpectedEnergy();
    double clusterEnergy = thisEflowCaloObject->getClusterEnergy();
    double expectedSigma = sqrt(thisEflowCaloObject->getExpectedVariance());
    
    /* Check e/p */
    if (isEOverPFail(expectedEnergy, expectedSigma, clusterEnergy, m_consistencySigmaCut,
                     runInGoldenMode())
        || (runInGoldenMode() && thisEflowCaloObject->nTracks() > 1)) {
      std::cout<<"if isEOverPFail"<<std::endl;
      continue;
      
    }    

    /* Do subtraction */

    int nTrackMatches = thisEflowCaloObject->nTracks();
    if (nTrackMatches == 0 || runInGoldenMode()) {
      std::cout<<"if nTrackMatches is 0"<<std::endl;
      continue;
    }


      /* Subtract the track from all matched clusters */
      const std::vector<eflowTrackClusterLink*>& matchedTrackList =
          thisEflowCaloObject->efRecLink();

      std::cout<<"before itrack loop"<<std::endl;
      
      for (int iTrack = 0; iTrack < nTrackMatches; ++iTrack) {
        eflowRecTrack* efRecTrack = matchedTrackList[iTrack]->getTrack();
        
        std::cout<<"After get Track in perform subtraction"<<std::endl; 
        /* Can't subtract without e/p */
        if (!efRecTrack->hasBin()) {
          continue;
        }
     std::cout<<"after itrack loop"<<std::endl;
     
	if (efRecTrack->isInDenseEnvironment()) continue;

        std::vector<eflowRecCluster*> matchedClusters;
        matchedClusters.clear();
        std::vector<eflowTrackClusterLink*> links = efRecTrack->getClusterMatches();
        std::vector<eflowTrackClusterLink*>::iterator itLink = links.begin();
        std::vector<eflowTrackClusterLink*>::iterator endLink = links.end();
        for (; itLink != endLink; ++itLink) {
          matchedClusters.push_back((*itLink)->getCluster());
        }
        std::vector<xAOD::CaloCluster*> clusterSubtractionList;
        std::vector<eflowRecCluster*>::const_iterator itCluster = matchedClusters.begin();
        std::vector<eflowRecCluster*>::const_iterator endCluster = matchedClusters.end();
        for (; itCluster != endCluster; ++itCluster) {
          clusterSubtractionList.push_back(
              (*itCluster)->getClusterForModification(eflowCaloObject::getClusterContainerPtr()));
        }

        Subtractor::subtractTracksFromClusters(efRecTrack, clusterSubtractionList);

        /* Annihilate the cluster(s) if the remnant is small (i.e. below k*sigma) */
        if (canAnnihilated(0, expectedSigma, clusterEnergy)) {
          Subtractor::annihilateClusters(clusterSubtractionList);
        }
      }
    


    if (canAnnihilated(expectedEnergy, expectedSigma, clusterEnergy)) {
      /* Check if we can annihilate right away */
      std::cout<<"if can annihilated"<<std::endl;
      std::vector<xAOD::CaloCluster*> clusterList;
      unsigned nCluster = thisEflowCaloObject->nClusters();
      for (unsigned iCluster = 0; iCluster < nCluster; ++iCluster) {
        clusterList.push_back(
            thisEflowCaloObject->efRecCluster(iCluster)->getClusterForModification(
                eflowCaloObject::getClusterContainerPtr()));
      }
      Subtractor::annihilateClusters(clusterList);
    } 
//     else {}

    /* Flag tracks as subtracted */
    for (unsigned int iTrack = 0; iTrack < thisEflowCaloObject->nTracks(); ++iTrack) {
      if (!thisEflowCaloObject->efRecTrack(iTrack)->isInDenseEnvironment()) thisEflowCaloObject->efRecTrack(iTrack)->setSubtracted();
    }
  }
}

StatusCode eflowCellLevelSubtractionTool::finalize(){

  return StatusCode::SUCCESS;
}


bool eflowCellLevelSubtractionTool::isEOverPFail(double expectedEnergy, double sigma, double clusterEnergy, bool consistencySigmaCut, bool useGoldenMode) {
  if ((expectedEnergy == 0) && (clusterEnergy > 0)) {
    return false;
  }

  bool result = useGoldenMode ? fabs(clusterEnergy - expectedEnergy) > consistencySigmaCut*sigma
                              : clusterEnergy < expectedEnergy - consistencySigmaCut*sigma;
  return result;
}

bool eflowCellLevelSubtractionTool::canAnnihilated(double expectedEnergy, double sigma, double clusterEnergy) {
   return  clusterEnergy - expectedEnergy < m_subtractionSigmaCut * sigma;
}

std::string eflowCellLevelSubtractionTool::printTrack(const xAOD::TrackParticle* track) {
  std::stringstream result;
  result << " track with E, eta and phi "<< track->e() << ", " << track->eta() << " and " << track->phi();
  return result.str();
}

std::string eflowCellLevelSubtractionTool::printCluster(const xAOD::CaloCluster* cluster) {
  std::stringstream result;
  result << " cluster with E, eta and phi of " << cluster->e() << ", " << cluster->eta() << " and " << cluster->phi();
  return result.str();
}

void eflowCellLevelSubtractionTool::printAllClusters(eflowRecClusterContainer* recClusterContainer) {
    unsigned int nClusters = recClusterContainer->size();
    for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
      /* Create the eflowCaloObject and put it in the container */
      eflowRecCluster* thisEFRecCluster = recClusterContainer->at(iCluster);
    if (thisEFRecCluster->getTrackMatches().empty()) {
      std::cout << "Isolated" << printCluster(thisEFRecCluster->getCluster()) << std::endl;
    } else {
      std::cout << "Matched" << printCluster(thisEFRecCluster->getCluster()) << std::endl;
      int nTrackMatches = thisEFRecCluster->getNTracks();
      std::vector<eflowTrackClusterLink*> theTrackLinks = thisEFRecCluster->getTrackMatches();
      for (int iTrackMatch = 0; iTrackMatch < nTrackMatches; ++iTrackMatch) {
        std::cout << "Matched" << printTrack(theTrackLinks[iTrackMatch]->getTrack()->getTrack()) << std::endl;
      }
    }
  }
}
