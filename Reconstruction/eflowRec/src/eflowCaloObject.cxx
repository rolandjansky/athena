/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowCaloObject.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R.Tovey, M.Hodgkinson and T.Velz
CREATED:  22nd November, 2004

********************************************************************/

#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowRingSubtractionManager.h"

eflowCaloObject::~eflowCaloObject() { }


 double eflowCaloObject::getExpectedEnergy() const {
  double expectedEnergy(0.0);
  for(unsigned iTrack=0; iTrack < nTracks(); ++iTrack) {
    if (!efRecTrack(iTrack)->isInDenseEnvironment()) expectedEnergy += efRecTrack(iTrack)->getEExpect();
  }
  return expectedEnergy;
}

double eflowCaloObject::getExpectedVariance() const {
  double expectedVariance(0.0);
  for(unsigned iTrack=0; iTrack < nTracks(); ++iTrack) {
    if (!efRecTrack(iTrack)->isInDenseEnvironment()) expectedVariance += efRecTrack(iTrack)->getVarEExpect();
  }
  return expectedVariance;
}

double eflowCaloObject::getClusterEnergy() const {
  double clusterEnergy(0.0);
  for(unsigned iCluster=0; iCluster < nClusters(); ++iCluster) {
    clusterEnergy  += efRecCluster(iCluster)->getCluster()->e();
  }
  return clusterEnergy;
}

void eflowCaloObject::simulateShower(eflowLayerIntegrator *integrator, eflowEEtaBinnedParameters* binnedParameters, bool useUpdated2015ChargedShowerSubtraction){

  for (unsigned int iTrack = 0; iTrack < m_eflowRecTracks.size(); ++iTrack) {

    eflowRecTrack* thisEfRecTrack = m_eflowRecTracks.at(iTrack);

    std::vector<eflowRecCluster*> matchedClusters;
    matchedClusters.clear();
    std::vector<eflowTrackClusterLink*> links = thisEfRecTrack->getClusterMatches();
    std::vector<eflowTrackClusterLink*>::iterator itLink = links.begin();
    std::vector<eflowTrackClusterLink*>::iterator endLink = links.end();
    for (; itLink != endLink; ++itLink) {
      matchedClusters.push_back((*itLink)->getCluster());
    }

    double trackEM1eta = thisEfRecTrack->getTrackCaloPoints().getEM1eta();
    /* if a track is in the forward EM (2.5 < eta < 3.2) then there is no EM1 -> need to use EM2 */
    if(trackEM1eta<-998.) trackEM1eta = thisEfRecTrack->getTrackCaloPoints().getEM2eta();
    /* if a track is not in the EM region (3.2 < eta < 4.0) then should use FCAL0 */
    if(trackEM1eta<-998.) trackEM1eta = thisEfRecTrack->getTrackCaloPoints().getFCAL0eta();

    double trackE = thisEfRecTrack->getTrack()->e();

    if (!binnedParameters->binExists(trackE, trackEM1eta)) {
      thisEfRecTrack->setHasBin(false);
      return;
    }

    /* Determine the LFI */
    integrator->measureNewClus(matchedClusters, thisEfRecTrack);
    eflowFirstIntENUM j1st = integrator->getFirstIntLayer();
    
    /*Save j1st info */
    thisEfRecTrack->setLayerHED(j1st);

    /* Get parameters for j1st */
    eflowRingSubtractionManager& cellSubtractionManager = thisEfRecTrack->getCellSubtractionManager();
    cellSubtractionManager.getOrdering(binnedParameters, trackE, trackEM1eta, j1st);

    /* Set expected energy in the eflowRecTrack object */
    const double expectedEnergy = cellSubtractionManager.fudgeMean() * thisEfRecTrack->getTrack()->e();
    const double expectedEnergySigma = fabs(cellSubtractionManager.fudgeStdDev() * thisEfRecTrack->getTrack()->e());

    const std::vector<eflowTrackClusterLink*>* bestClusters_015 = thisEfRecTrack->getAlternativeClusterMatches("cone_015");
    const std::vector<eflowTrackClusterLink*>* bestClusters_02 = thisEfRecTrack->getAlternativeClusterMatches("cone_02");

    //First we calculate how much energy is found in a cone of 0.15
    float totalE_015 = 0.0;
    
    //This pointer can be a nullptr if no clusters were matched to a track in dR < 0.15
    if (bestClusters_015){
      for (eflowTrackClusterLink* thisLink : *bestClusters_015){
	eflowRecCluster* thisBestCluster = thisLink->getCluster();
	if (thisBestCluster){      
	  const xAOD::CaloCluster* theCluster =  thisBestCluster->getCluster();
	  if (theCluster) {
	    if (theCluster->e()>0.0){
	      totalE_015 += theCluster->e();
	    }
	  }
	}
      }
    }//if vector of 0.15 clusters exists
    
    double pull_015 = NAN;
    if (expectedEnergySigma > 1e-6 ) pull_015 = (totalE_015-expectedEnergy)/expectedEnergySigma;
    thisEfRecTrack->setpull15(pull_015);

    double trackPt = thisEfRecTrack->getTrack()->pt();
    //If the looked up expected energy deposit was 0.0, then pull_015 is NAN. In that case we should not try to apply the 2D cut described below.
    if (!std::isnan(pull_015)){
      //We use a 2D cut in the pull_015 and log10 of track pt plane to define a dense environment - if too dense then we disable the charged shower subtraction
      if (pull_015 > 0.0 + (log10(40000)-log10(trackPt))*33.2 && 0.0 != expectedEnergySigma && bestClusters_015 && bestClusters_02 && useUpdated2015ChargedShowerSubtraction){
        thisEfRecTrack->setSubtracted(); //this tricks eflowRec into thinking this track was subtracted, and hence no further subtraction will be done
        thisEfRecTrack->setIsInDenseEnvironment();
        //recalculate the LHED and the ordering  and find the new  expected E + sigma of expected E (the new LHED can change the latter two values we find in the look up tables)
        //we use a larger cone of 0.2 for this
        std::vector<eflowRecCluster*> theBestEfRecClusters_02;
        for (eflowTrackClusterLink* thisLink : *bestClusters_02) if (thisLink->getCluster()->getCluster()->e() > 0.0) theBestEfRecClusters_02.push_back(thisLink->getCluster());
        integrator->measureNewClus(theBestEfRecClusters_02, thisEfRecTrack);
        j1st = integrator->getFirstIntLayer();
        cellSubtractionManager.getOrdering(binnedParameters, trackE, trackEM1eta, j1st);
        thisEfRecTrack->setEExpect(cellSubtractionManager.fudgeMean() * trackE, fabs(cellSubtractionManager.fudgeStdDev()*trackE)*fabs(cellSubtractionManager.fudgeStdDev()*trackE));
      }
      else {
        thisEfRecTrack->setEExpect(expectedEnergy, expectedEnergySigma*expectedEnergySigma);
      }//ok to do subtraction, and so we just set the usual expected E + sigma of expected E needed for subtraction	
    }
  }
}


