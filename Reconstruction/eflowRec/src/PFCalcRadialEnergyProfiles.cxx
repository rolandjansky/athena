#include "eflowRec/PFCalcRadialEnergyProfiles.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowCellList.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowRingThicknesses.h"
#include "eflowRec/eflowSubtractor.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "xAODCaloEvent/CaloCluster.h"

void PFCalcRadialEnergyProfiles::calculate(const PFData& data){

  ATH_MSG_DEBUG("Accessed radial energy profile function");

  for (auto thisEflowCaloObject : *data.caloObjects){

    //If there are no clusters available then we cannot calculate any calorimeter shower profiles
    if (thisEflowCaloObject->nClusters() < 1 ) continue;

    const std::vector<std::pair<eflowTrackClusterLink*,std::pair<float,float> > > matchedTrackList = thisEflowCaloObject->efRecLink();

    for( auto track: matchedTrackList){

      eflowRecTrack* efRecTrack = (track.first)->getTrack();
          
      std::vector<eflowRecCluster*> matchedClusters;
      matchedClusters.clear();
      std::vector<eflowTrackClusterLink*> links = efRecTrack->getClusterMatches();
      for (auto thisEFlowTrackClusterLink : links) matchedClusters.push_back(thisEFlowTrackClusterLink->getCluster());

      std::vector<std::pair<xAOD::CaloCluster*, bool> > clusterSubtractionList;
      for (auto thisEFlowRecCluster : matchedClusters) clusterSubtractionList.push_back(std::pair(thisEFlowRecCluster->getCluster(),false));

      eflowCellList calorimeterCellList;
      eflowSubtract::Subtractor::makeOrderedCellList(efRecTrack->getTrackCaloPoints(),clusterSubtractionList,calorimeterCellList);
      
      eflowRingThicknesses ringThicknessGenerator;
      
      std::vector<int> layerToStoreVector;
      std::vector<float> radiusToStoreVector;
      std::vector<float> avgEdensityToStoreVector;

      //Loop over calorimeter layers and in each layer we will calculate radial energy profiles.
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
        		
        //Loop over rings of calorimeter cells going out in increasing radius from the track impact point in this calorimeter layer
        for (unsigned  int indexOfRing = 0; indexOfRing < 100; indexOfRing++){

          CellIt beginRing = calorimeterCellList.getLowerBound((eflowCaloENUM)i, ringThickness*(indexOfRing));
          if(beginRing == calorimeterCellList.end()) break;

          int totalCellsinRing = 0;
          double totalEnergyPerRing = 0;
      	  double energyDensityPerRing = 0;
      	  double averageEnergyDensityPerRing = 0;

          //Get the list of calorimeter cells in this cell ring
      	  std::vector<std::pair<const CaloCell*,int> > tempVector = (*beginRing).second;

          //Loop over the calorimeter cells in this cell ring and calculate total and average energy densities in this cell ring.
          for (auto thisPair : tempVector){
      	    const CaloDetDescrElement* DDE = (thisPair.first)->caloDDE();
	          CaloCell_ID::CaloSample sampling = DDE->getSampling();

            if(eflowCalo::translateSampl(sampling)!=(eflowCaloENUM)i) break;

            ATH_MSG_DEBUG(" cell eta and phi are " << (thisPair.first)->eta() << " and " << (thisPair.first)->phi() << " with index " << thisPair.second << " and sampling of " << sampling);
	          ATH_MSG_DEBUG(" cell energy is " << (thisPair.first)->energy());
            
      	    totalCellsinRing += 1;
	    
      	    totalEnergyPerRing += (thisPair.first)->energy();
      	    double totalEnergyCell = (thisPair.first)->energy();
      	    ATH_MSG_DEBUG(" Total E per Cell is " << totalEnergyCell);
      	    ATH_MSG_DEBUG(" Total E per Ring is " << totalEnergyPerRing);
	    
      	    double cellVolume = DDE->volume();
      	    ATH_MSG_DEBUG(" cell volume is " << cellVolume/1000.);
	    
      	    double energyDensityCell = totalEnergyCell/(cellVolume/1000.);
      	    ATH_MSG_DEBUG(" E density per Cell is " << energyDensityCell);
      	    ATH_MSG_DEBUG(" Initial added E density per Cell is " << energyDensityPerRing);
      	    energyDensityPerRing += energyDensityCell;
      	    ATH_MSG_DEBUG(" Final added E density per Cell is " << energyDensityPerRing);
      	    averageEnergyDensityPerRing = energyDensityPerRing/((totalCellsinRing)*(efRecTrack->getTrack()->e()/1000.));

          }

          ATH_MSG_DEBUG(" track eta is " << efRecTrack->getTrack()->eta());
      	  ATH_MSG_DEBUG(" track E is " << efRecTrack->getTrack()->e()/1000.);
      	  ATH_MSG_DEBUG(" Average E density per Ring is " << averageEnergyDensityPerRing);
	  
          //Fill up the vectors with energy density, layer and ring radii
      	  if (averageEnergyDensityPerRing != 0){
      	    avgEdensityToStoreVector.push_back(averageEnergyDensityPerRing);
      	    int layerToStore = (eflowCaloENUM)i;
      	    ATH_MSG_DEBUG("layerToStore is "<< layerToStore);
      	    layerToStoreVector.push_back(layerToStore);
      	    double radiusToStore = (indexOfRing)*ringThickness;
      	    ATH_MSG_DEBUG("radiusToStore is "<< radiusToStore);
      	    radiusToStoreVector.push_back(radiusToStore);
      	  }
      	  else {ATH_MSG_DEBUG("averageEnergyDensityPerRing = 0");}

        }//loop over rings of calorimeter cells

      }//loop over the calorimeter layers

      //Add the vectors with energy density, layer and ring radii to this eflowRecTrack
      efRecTrack->setLayerCellOrderVector(layerToStoreVector);
      efRecTrack->setRadiusCellOrderVector(radiusToStoreVector);
      efRecTrack->setAvgEDensityCellOrderVector(avgEdensityToStoreVector);

    }//loop over tracks matched to clusters

  }//loop on eflowCaloObjectContainer

}
