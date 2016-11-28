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

#include "CaloEvent/CaloCluster.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

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
  m_calcEOverP(false),
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

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Executing eflowCellLevelSubtractionTool" << endmsg;

  m_eflowCaloObjectContainer = theEflowCaloObjectContainer;
  m_eflowTrackContainer = recTrackContainer;
  m_eflowClusterContainer = recClusterContainer;

  /* Add each track to its best matching cluster's eflowCaloObject */
  matchAndCreateEflowCaloObj(m_nMatchesInCellLevelSubtraction);

  int debug = 0;
  if (1 == debug) {
    printAllClusters(recClusterContainer);
  }

  performSubtraction();
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

void eflowCellLevelSubtractionTool::performSubtraction() {
  unsigned int nEFCaloObs = m_eflowCaloObjectContainer->size();
  for (unsigned int iEFCalOb = 0; iEFCalOb < nEFCaloObs; ++iEFCalOb) {
    eflowCaloObject* thisEflowCaloObject = m_eflowCaloObjectContainer->at(iEFCalOb);
    /* Check e/p mode */
    if (m_calcEOverP) {
      continue;
    }
    unsigned int nClusters = thisEflowCaloObject->nClusters();
    if (nClusters < 1) {
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
      continue;
    }    

    /* Do subtraction */

    int nTrackMatches = thisEflowCaloObject->nTracks();
    if (nTrackMatches == 0 || runInGoldenMode()) {
      continue;
    }


    if (canAnnihilated(expectedEnergy, expectedSigma, clusterEnergy)) {
      /* Check if we can annihilate right away */
      std::vector<xAOD::CaloCluster*> clusterList;
      unsigned nCluster = thisEflowCaloObject->nClusters();
      for (unsigned iCluster = 0; iCluster < nCluster; ++iCluster) {
        clusterList.push_back(
            thisEflowCaloObject->efRecCluster(iCluster)->getClusterForModification(
                eflowCaloObject::getClusterContainerPtr()));
      }
      Subtractor::annihilateClusters(clusterList);
    } else {
      /* Subtract the track from all matched clusters */
      const std::vector<eflowTrackClusterLink*>& matchedTrackList =
          thisEflowCaloObject->efRecLink();


      for (int iTrack = 0; iTrack < nTrackMatches; ++iTrack) {
        eflowRecTrack* efRecTrack = matchedTrackList[iTrack]->getTrack();
        /* Can't subtract without e/p */
        if (!efRecTrack->hasBin()) {
          continue;
        }

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
    }

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
