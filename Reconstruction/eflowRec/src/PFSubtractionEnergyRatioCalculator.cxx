#include "eflowRec/PFSubtractionEnergyRatioCalculator.h"

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "AthenaKernel/getMessageSvc.h"

PFSubtractionEnergyRatioCalculator::PFSubtractionEnergyRatioCalculator() : AthMessaging(Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc"),
                  "PFSubtractionEnergyRatioCalculator"){}

void PFSubtractionEnergyRatioCalculator::calculateSubtractedEnergyRatios(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& clusterSubtractionList, std::map<xAOD::CaloCluster*, double>& clusterEnergyMap, std::vector<std::pair<float, float> >& clusterSubtractedEnergyRatios){

  ATH_MSG_DEBUG("Setting subtracted energy ratios here");
  
  for (auto thisCluster: clusterSubtractionList) {
    ATH_MSG_DEBUG("Cluster energies are " << thisCluster.first->e() << " and " << clusterEnergyMap[thisCluster.first]);

    //clusterEnergyMap[thisCluster.first can be zero, but this is only a problem if thisCluster.first.e() < 0 - this can happen if a cluster starts with E =0 
    //from a previous shower subtraction step and then we subtract more such that the new E is < 0. Then the ratio would cause an FPE due to the zero.
    //If both thisCluster.first->e() and clusterEnergyMap[thisCluster.first] are zero we never enter this step.          

    if (std::abs(thisCluster.first->e() - clusterEnergyMap[thisCluster.first]) > 0.0001) {
       if ( clusterEnergyMap[thisCluster.first] > 0) {
        ATH_MSG_DEBUG("Subtracted energy ratio is " << thisCluster.first->e()/clusterEnergyMap[thisCluster.first]);
        clusterSubtractedEnergyRatios.emplace_back(std::pair(thisCluster.first->e()/clusterEnergyMap[thisCluster.first],clusterEnergyMap[thisCluster.first]-thisCluster.first->e()));
       }
       //approximate zero with 0.0001 to avoid FPE and still give a meaningful ratio (e.g -100/0.0001)
       else {
         ATH_MSG_DEBUG("Subtracted energy ratio is " << thisCluster.first->e()/0.0001);
         clusterSubtractedEnergyRatios.emplace_back(std::pair(thisCluster.first->e()/0.0001,clusterEnergyMap[thisCluster.first]-thisCluster.first->e()));
       }
    }
    //else if the cluster enegry did not change then we use NAN to denote that no charged shower subtraction occurred.
    else {
      clusterSubtractedEnergyRatios.emplace_back(NAN,NAN);
      ATH_MSG_DEBUG("Subtracted energy ratio is NAN ");
    }
  }//Loop over clusterSubtractionList

}

void PFSubtractionEnergyRatioCalculator::calculateSubtractedEnergyRatiosForAnnih(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& clusterSubtractionList, std::map<xAOD::CaloCluster*, double>& clusterEnergyMap, std::vector<std::pair<float, float> >& clusterSubtractedEnergyRatios){

  ATH_MSG_DEBUG("Setting subtracted energy ratios for annih here");

  for (auto thisCluster: clusterSubtractionList) {
    ATH_MSG_DEBUG("Cluster energies are " << thisCluster.first->e() << " and " << clusterEnergyMap[thisCluster.first]);

    //The energy can be zero if we previously "annihiliated" this cluster because we will have already set the energy to zero.
    //We don't want to set the energy ratio to zero if we just change the energy from "0" to "0" - in effect in this case
    //we don't subtract anything and we will set the value of the ratio to NAN which denotes that no subtraction was performed.
    //So we build a list of indices of clusters which don't need to have the energy set to zero - we compare a floating point
    //directly to zero, because in a previous annihilation we explicity set the energy to be exactly zero.
    if (0 != clusterEnergyMap[thisCluster.first]) {
      ATH_MSG_DEBUG("Setting cluster energy ratio to zero");
      //We are removing the full energy, so set the subtracted energy to the cluster energy.
      clusterSubtractedEnergyRatios.emplace_back(std::pair(0,clusterEnergyMap[thisCluster.first]));
    }
    else {
      ATH_MSG_DEBUG("Setting cluster energy ratio to NAN");
      clusterSubtractedEnergyRatios.emplace_back(NAN,NAN);
    }

  }

}