/*                                                                                                                                                                                                                                          
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration                                                                                                                                                                   
*/

#ifndef PFSUBTRACTIONENERGYRATIOCALCULATOR_H
#define PFSUBTRACTIONENERGYRATIOCALCULATOR_H

#include "AthenaBaseComps/AthMessaging.h"

#include "xAODCaloEvent/CaloCluster.h"

#include <vector>
#include <map>
#include <utility>

/** Class to calculate the ratio of new to old energies of CaloClusters after the particle flow
charged shower subtraction has been run **/

class PFSubtractionEnergyRatioCalculator : public AthMessaging {

  public:
  PFSubtractionEnergyRatioCalculator();
  /** For each xAOD::CaloCluster in clusterSubtractionList we calculate the ratio of new to old energy after the charged shower subtraction, which
  is added in clusterSubtractedEnergyRatios. clusterEnergyMap contains the cluster energy prior to the charged shower energy subtraction */
  void calculateSubtractedEnergyRatios(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& clusterSubtractionList, std::map<xAOD::CaloCluster*, double>& clusterEnergyMap, std::vector<std::pair<float, float> >& clusterSubtractedEnergyRatios);
  /** If we have decided to annihiliate all clusters in clusterSubtractionList we use this function to set all ratios to zero in clusterEnergySubtractionRatios.
  If the old energy, in clusterEnergyMap, was zero we instead set the ratio to NAN to denote that no energy was removed from that xAOD::CaloCluster */
  void calculateSubtractedEnergyRatiosForAnnih(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& clusterSubtractionList, std::map<xAOD::CaloCluster*, double>& clusterEnergyMap, std::vector<std::pair<float, float> >& clusterSubtractedEnergyRatios);

};
#endif
