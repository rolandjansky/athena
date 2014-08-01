/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCLUSTERCOMBINER_H
#define EFLOWCLUSTERCOMBINER_H

#include <vector>
#include <map>

class eflowCaloCluster;

class eflowClusterCombiner {

 public:
   /** This function finds all permutations of energies of eflowCaloClusters and returns a map of these energies and the 
       list of cluster indices used to form the energy combination*/
  std::map<double,std::vector<int> > PermutationsOfEnergies(std::vector<std::pair<int,eflowCaloCluster*> > someClusters);

 private:
  /** This function checks for duplicate entries in map
      e.g for four doubles order we enter them in changes for example third decimal place
      and hence get duplicate entries in map with similar keys (e.g. 53.9 and 53.8)*/
  std::map<double,std::vector<int> > cleanMap(std::map<double,std::vector<int> > thisMap);

};
#endif
