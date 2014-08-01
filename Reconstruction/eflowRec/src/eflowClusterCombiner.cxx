/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowClusterCombiner.h"
#include "eflowEvent/eflowCaloCluster.h"

std::map<double,std::vector<int> > eflowClusterCombiner::PermutationsOfEnergies(std::vector<std::pair<int,eflowCaloCluster*> > someClusters){
  
  int numberOfClusters = someClusters.size();

  std::map<double,std::vector<int> > permutationMap;

  for (int x = 0; x < numberOfClusters; x++){

    //e.g. for x =1 want to combine it with combinations of
    //clusters x = 0 and x = 2,3,...,numberOfClusters
    int downCount = x - 1;
    int upCount = x + 1;

    std::vector<std::pair<int,eflowCaloCluster*> > subClusterList;

    for (; x != 0 && downCount >= 0; downCount--) subClusterList.push_back(someClusters[downCount]);

    for (; upCount < numberOfClusters; upCount++) subClusterList.push_back(someClusters[upCount]);

    eflowCaloCluster* thisCluster = someClusters[x].second;
    double thisClusEnergy = thisCluster->e();

    if (1 == subClusterList.size()){
      //if have only 1 cluster then trivially
      //have one combination of two clusters
      
      eflowCaloCluster* subCluster = subClusterList[0].second;

      double thisCombo = thisClusEnergy + subCluster->e();

      std::vector<int> indiceList;
      indiceList.push_back(someClusters[x].first);
      indiceList.push_back(subClusterList[0].first);

      permutationMap[thisCombo] = indiceList;

    }
    else if (0 == subClusterList.size()) {
      //if have zero clusters then clearly only
      //one combination involving 1 cluster
      std::vector<int> indiceList;
      indiceList.push_back(someClusters[x].first);
      permutationMap[thisClusEnergy] = indiceList;
    }
    else{
      //else have multiple combinations of clusters
      //and will need to do recursive call to get all possible
      //combinations
      //e.g for x = 2 can have combinations x=1,2,x=2,3,x=1,2,3,x=1,2,4 etc 
      std::map<double,std::vector<int> > subPermutationList = this->PermutationsOfEnergies(subClusterList);//map is between energy and indice list

      eflowCaloCluster* thisCluster = someClusters[x].second;
      double thisClusEnergy = thisCluster->e();
      
      std::map<double,std::vector<int> >::iterator firstMapElement = subPermutationList.begin();
      std::map<double,std::vector<int> >::iterator lastMapElement = subPermutationList.end();
      
      for (; firstMapElement != lastMapElement; firstMapElement++){

	//add this element of returned map to main map
	//prior to combining energy with other energies
	//to ensure we get all subpermutations
	//e.g. if returns combination x=3,4 must combine with x=2 to get x=2,3,4 combination
	//so first of call add e.g. x=3,4...

	permutationMap[(*firstMapElement).first] = (*firstMapElement).second;

	//then create and add e.g x=2,3,4 combination

	double thisCombo = thisClusEnergy + (*firstMapElement).first;
	std::vector<int> indiceList;
	std::vector<int> returnedIndices = (*firstMapElement).second;

	std::vector<int>::iterator firstIndice = returnedIndices.begin();
	std::vector<int>::iterator lastIndice = returnedIndices.end();

	for (; firstIndice != lastIndice; firstIndice++) {
	  indiceList.push_back(*firstIndice);
	}
	indiceList.push_back(someClusters[x].first);

	permutationMap[thisCombo] = indiceList;
	

      }//loop on map elements

    }//if subcluster list size is != 1

  }//loop over clusters

  std::map<double,std::vector<int> > cleanedMap = this->cleanMap(permutationMap);

  return cleanedMap;

}//PermutationsOfEnergies

std::map<double,std::vector<int> > eflowClusterCombiner::cleanMap(std::map<double,std::vector<int> > thisMap){

  std::map<double,std::vector<int> >::iterator  firstElement = thisMap.begin();
  std::map<double,std::vector<int> >::iterator  lastElement = thisMap.end();

  for (; firstElement != lastElement; firstElement++){

    std::vector<int> theseIndices = (*firstElement).second;

    std::vector<int>::iterator firstIndex = theseIndices.begin();
    std::vector<int>::iterator lastIndex = theseIndices.end();
    
    int numberOfIndices = theseIndices.size();

    std::map<double,std::vector<int> >::iterator  firstElementB = thisMap.begin();
    std::map<double,std::vector<int> >::iterator  lastElementB = thisMap.end();
    
    for (; firstElementB != lastElementB && firstElementB != firstElement; firstElementB++){

      std::vector<int> theseIndicesB = (*firstElementB).second;

      std::vector<int>::iterator firstIndexB = theseIndicesB.begin();
      std::vector<int>::iterator lastIndexB = theseIndicesB.end();

      int numberOfIndicesB = theseIndicesB.size();

      int numMatched  = 0;

      firstIndex = theseIndices.begin();

      for (; firstIndex != lastIndex; firstIndex++){
	int thisIndex = *firstIndex;
	firstIndexB = theseIndicesB.begin();
	for (; firstIndexB != lastIndexB; firstIndexB++){
	  if (thisIndex == *firstIndexB) numMatched++;
	}//loop on indicesB
      }//loop on indices

      //if found n matched indices and n is same as size of both vectors of indices
      //then it is the same combination and we should remove one of them
      if (numMatched == numberOfIndices && numMatched == numberOfIndicesB){
	std::map<double,std::vector<int> >::iterator  thisIterator = firstElementB;
	firstElementB--;
	thisMap.erase(thisIterator);
	//firstElementB--;
      }//erase element

    }//second loop over map elements not corresponding to element in prior loop


  }//loop over all map elements
  
  return thisMap;

}//cleanMap
