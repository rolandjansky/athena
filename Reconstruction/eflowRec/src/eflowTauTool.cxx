/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowEvent/eflowTauObject.h"
#include "eflowEvent/eflowObjectContainer.h"
#include "eflowEvent/eflowObject.h"
#include "eflowEvent/eflowCaloCluster.h"

#include "eflowRec/eflowTauTool.h"
#include "eflowRec/eflowTrackToCaloTrackExtrapolatorTool.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowTrackClusterMatcher.h"
#include "eflowRec/eflowBinnedParameters.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowCellEOverPTool.h"
#include "eflowRec/eflowCellSubtractionManager.h"
#include "eflowRec/eflowClus.h"
#include "eflowRec/eflowCellList.h"
#include "eflowRec/eflowCellSubtractionFacilitator.h"
#include "eflowRec/eflowUtil.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODPFlow/PFODefs.h"

#include "FourMomUtils/P4DescendingSorters.h"

#include "GaudiKernel/MsgStream.h"

#include <map>
#include <unordered_map>

namespace EFlow_TauFn{ 
  // comparison,  order by Et
  class gtpt
  {
    
  public:
    inline   gtpt () {} ;
    
    inline bool operator () (std::pair<xAOD::PFO*,eflowTrackCaloPoints*> ef1, std::pair<xAOD::PFO*,eflowTrackCaloPoints*> ef2)
    {
      return ef1.first->pt() > ef2.first->pt() ;
    }  
    
  };

  class sortClusters{
  public:
    inline sortClusters() {};
    inline bool operator () (const xAOD::CaloCluster* pf1, const xAOD::CaloCluster* pf2){
      return pf1->pt() > pf2->pt();
    }
  };

}


eflowTauTool::eflowTauTool(const std::string& type,const std::string& name,const IInterface* parent) :
  AthAlgTool( type, name, parent),
  m_recoverIsolatedTracks(true),
  m_doAnnihilation(false),
  m_consistencySigmaCut(1.0),
  m_subtractionSigmaCut(1.28),
  m_theTrackExtrapolatorTool("eflowTrackToCaloTrackExtrapolatorTool",this),
  m_theEOverPTool("eflowCellEOverPTool",this)
{
  declareProperty("RecoverIsolatedTracks",m_recoverIsolatedTracks);
  declareProperty("doAnnihilation",m_doAnnihilation);
  declareProperty("eflowTrackToCaloTrackExtrapolatorTool",m_theTrackExtrapolatorTool,"AlgTool to use TrackToCalo Tool");
  declareProperty("eflowCellEOverPTool", m_theEOverPTool,"Energy Flow E/P Values and Shower Paremeters Tool");
  declareProperty("SubtractionSigmaCut",m_subtractionSigmaCut);
  declareProperty("ConsistencySigmaCut",m_consistencySigmaCut);

  m_integrator = new eflowLayerIntegrator(0.032, 1.0e-3, 3.0);
  m_binnedParameters = new eflowBinnedParameters(0.75,1.0e6);
}

eflowTauTool::~eflowTauTool(){
  delete m_integrator;
  delete m_binnedParameters;
}

StatusCode eflowTauTool::initialize(){

  StatusCode sc = m_theTrackExtrapolatorTool.retrieve();

  if ( sc.isFailure() ) {
    msg(MSG::WARNING) << "Cannot find eflowTrackToCaloExtrapolatroTool " << endreq;
    return StatusCode::SUCCESS;
  }

  sc = m_theEOverPTool.retrieve();

  if (sc.isFailure()){
    msg(MSG::WARNING) << "Cannot find eflowEOverPTool" << endreq;
    return StatusCode::SUCCESS;
  }

  sc = m_theEOverPTool->execute(m_binnedParameters);

  if (sc.isFailure()){
    msg(MSG::WARNING) << "Cannot execute eflowEOverPTool" << endreq;
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

StatusCode eflowTauTool::execute(const eflowTauObject& eflowTauObject){

  int debug = 0;

  const ElementLinkVector<xAOD::TrackParticleContainer>*  theTrackContainer = eflowTauObject.getTrackContainer();

  const int numProng = eflowTauObject.getNumInnerTracks();

  ElementLinkVector<xAOD::TrackParticleContainer>::const_iterator firstTrack = theTrackContainer->begin();
  ElementLinkVector<xAOD::TrackParticleContainer>::const_iterator lastTrack = theTrackContainer->end();

  xAOD::PFOContainer* thePFOContainer = eflowTauObject.getPFOContainer();
  ElementLinkVector<xAOD::CaloClusterContainer>* theClusterContainer = eflowTauObject.getClusterContainer();

  //calculate HAD energy in this tau
  double hadronicEnergy = this->getHadronicEnergy(theClusterContainer);

  if (1 == debug) std::cout << "This tau has hadronicE of " << hadronicEnergy << std::endl;

  xAOD::CaloClusterContainer* theFinalClusterContainer = eflowTauObject.getFinalClusterContainer();

  ElementLinkVector<xAOD::CaloClusterContainer>::iterator firstCluster = theClusterContainer->begin();
  ElementLinkVector<xAOD::CaloClusterContainer>::iterator lastCluster = theClusterContainer->end();

  std::unordered_map<const xAOD::CaloCluster*,size_t> isClusterMatchedMap;
  std::vector<std::pair<ElementLink<xAOD::CaloClusterContainer>,bool> > isClusterMatchedVec;
  for (; firstCluster != lastCluster; ++firstCluster) {
    isClusterMatchedMap[**firstCluster] = isClusterMatchedVec.size();
    isClusterMatchedVec.emplace_back (*firstCluster, false);
  }
  firstCluster = theClusterContainer->begin();

  std::map<std::pair<ElementLink<xAOD::CaloClusterContainer>,xAOD::PFO*>, size_t> matchingMap;
  std::vector<std::pair<xAOD::PFO*, eflowTrackCaloPoints*> > matchingVec;

  std::vector<std::pair<xAOD::PFO*,eflowTrackCaloPoints*> > 
    isolatedChargedPFOs;

  for (; firstTrack != lastTrack; ++firstTrack){

    ElementLink<xAOD::TrackParticleContainer> theTrackLink = *firstTrack;
    const xAOD::TrackParticle* theTrack(NULL);
    if (theTrackLink.isValid()) theTrack = *theTrackLink;
    else{
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not dereference track element link: its invalid " << endreq;
      continue;
    }

    xAOD::PFO* chargedPFO = new xAOD::PFO();
    thePFOContainer->push_back(chargedPFO);
    chargedPFO->setAttribute<bool>("PassEOverPCheck",true); // by default should be true

    ElementLink<xAOD::TrackParticleContainer> myTrackLink = *firstTrack;
    bool isSet = chargedPFO->setTrackLink(myTrackLink);

    if (false == isSet) msg(MSG::WARNING) << MSG::WARNING << " Could not add Track to PFO " << endreq;

    chargedPFO->setCharge(theTrack->charge());
    chargedPFO->setP4(theTrack->pt(),theTrack->eta(),theTrack->phi(),theTrack->m());

    MsgStream log( msgSvc(), name() );
    eflowTrackCaloPoints *trackCalo = new eflowTrackCaloPoints(m_theTrackExtrapolatorTool, theTrack);

    const eflowEtaPhiPosition& trackEM2etaPhi = trackCalo->getEM2etaPhiPos();

    double closestDeltaR = 10.0;
    const xAOD::CaloCluster* closestCluster(0);

    for (; firstCluster != lastCluster; ++firstCluster){
      
      const xAOD::CaloCluster* theClus = **firstCluster;
      eflowTrackClusterMatcher thisMatcher;
      thisMatcher.setCluster(theClus);
      double deltaR = thisMatcher.match(trackEM2etaPhi);

      if (deltaR && deltaR < closestDeltaR){
        closestDeltaR = deltaR;
        closestCluster = theClus;
      }

    }//cluster loop
    firstCluster = theClusterContainer->begin();

    if (closestCluster){
      if (1 == debug) std::cout << "Have matched cluster to this charged PFO - matched cluster has E, eta and phi of " << closestCluster->e() << ", " << closestCluster->eta() << " and " << closestCluster->phi() << std::endl;
      size_t iclus = isClusterMatchedMap[closestCluster];
      std::pair<ElementLink<xAOD::CaloClusterContainer>, bool>& thisPair = 
        isClusterMatchedVec[iclus];

      thisPair.second = true;

      ElementLink<xAOD::CaloClusterContainer> thisClusterLink;
      thisClusterLink.setElement(*thisPair.first);
      thisClusterLink.setStorableObject(*theFinalClusterContainer);

      bool isSet = chargedPFO->setClusterLink(thisClusterLink);

      if (false == isSet) msg(MSG::WARNING) << MSG::WARNING << " Could not add Cluster to PFO " << endreq;

      std::pair<ElementLink<xAOD::CaloClusterContainer>,xAOD::PFO*> anotherPair(thisClusterLink,chargedPFO);
      matchingMap[anotherPair] = matchingVec.size();
      matchingVec.emplace_back (chargedPFO, trackCalo);
    }//if found matched cluster
    else {
      if (1 == debug) std::cout << "Add isolated charged PFO to iso map with E, eta and phi " << chargedPFO->e() << ", " << chargedPFO->eta() << " and " << chargedPFO->phi() << std::endl;
      isolatedChargedPFOs.emplace_back (chargedPFO, trackCalo);
    }
  }//track loop
    
  //create neutral PFO from all unmatched clusters

  for (auto& p : isClusterMatchedVec) {
    //if cluster was not matched to any track this bool will be false
    if (false == p.second){
      
      xAOD::PFO* neutralPFO = new xAOD::PFO();
      thePFOContainer->push_back(neutralPFO);
      neutralPFO->setCharge(0.0);
      const xAOD::CaloCluster* thisCluster = *(p.first);
      neutralPFO->setP4(thisCluster->pt(),thisCluster->eta(),thisCluster->phi(),thisCluster->m());

      ElementLink<xAOD::CaloClusterContainer> theClusterLink = p.first;
      bool isSet = neutralPFO->setClusterLink(theClusterLink);
      
      if (false == isSet) msg(MSG::WARNING) << MSG::WARNING << " Could not add Cluster to PFO " << endreq;

      if (1 == debug) std::cout << " Create isolated neutral PFO with E,eta and phi of " << thisCluster->e() << ", " << thisCluster->eta() << " and " << thisCluster->phi() << std::endl;

    }
  }

  //now loop over matched tracks and do the subtraction

  std::map<const xAOD::CaloCluster*,eflowClus*> clusMap;

  for (auto& p : matchingVec) {

    xAOD::PFO* theChargedPFO = p.first;
    const xAOD::CaloCluster* matchedCluster = theChargedPFO->cluster(0);

    if (matchedCluster){

      std::map<const xAOD::CaloCluster*,eflowClus*>::iterator firstClusInMap = clusMap.begin();
      std::map<const xAOD::CaloCluster*,eflowClus*>::iterator lastClusInMap = clusMap.end();

      bool newCluster = true;
      const xAOD::CaloCluster* referenceClusterPointer(0);
      for (; firstClusInMap != lastClusInMap; ++firstClusInMap){
        const xAOD::CaloCluster* thisClus = (*firstClusInMap).first;
        if ( fabs(thisClus->e() - matchedCluster->e()) < 0.00001 ) {
          newCluster = false;
          referenceClusterPointer = thisClus;
        }
      }

      if (newCluster){
        eflowClus* myClus = new eflowClus();
        myClus->setCluster(matchedCluster);

	eflowTrackCaloPoints* trackCalo = p.second;

        m_integrator->measureNewClus(matchedCluster, *trackCalo);
        
	eflowFirstIntENUM j1st = m_integrator->getFirstIntLayer();
        eflowCellSubtractionManager ranking;

        const std::pair<double, double> trackExtrapEM2EtaPhi = trackCalo->getEM2etaPhi();
        
        double trackE = theChargedPFO->e();

        m_binnedParameters->getOrdering(ranking, trackE, trackExtrapEM2EtaPhi.first, j1st);
        const double eExpect = ranking.fudgeMean() * trackE;
        const double sigmaSqEExpect = ((ranking.fudgeStdDev() * trackE)*(ranking.fudgeStdDev() * trackE));

        myClus->addObject(eExpect,sigmaSqEExpect);

        clusMap[matchedCluster] = myClus;

      }
      else{
	eflowTrackCaloPoints* trackCalo = p.second;
	m_integrator->measureNewClus(matchedCluster, *trackCalo);
	
        eflowFirstIntENUM j1st = m_integrator->getFirstIntLayer();
	eflowCellSubtractionManager ranking;
	
	const std::pair<double, double> trackExtrapEM2EtaPhi = trackCalo->getEM2etaPhi();
	
	double trackE = theChargedPFO->e();
	m_binnedParameters->getOrdering(ranking, trackE, trackExtrapEM2EtaPhi.first, j1st);
	const double eExpect = ranking.fudgeMean() * trackE;
	const double sigmaEExpect = ((ranking.fudgeStdDev() * trackE)*(ranking.fudgeStdDev() * trackE));
        clusMap[referenceClusterPointer]->addObject(eExpect,sigmaEExpect);
      }
    }
  }//loop over the matched tracks

  for (auto& p : matchingVec) {
    xAOD::PFO* theChargedPFO = p.first;
    eflowTrackCaloPoints* trackCalo = p.second;

    double trackE = theChargedPFO->e();
    double track_eta = theChargedPFO->eta();

    if (1 == debug) std::cout << " now considering charged PFO with E,eta and phi " << trackE << ", " << track_eta << " and " << theChargedPFO->phi() << std::endl;

    if (m_binnedParameters->binExists(trackE, track_eta)){

      if (1 == debug) std::cout << "Found bin for this charged PFO " << std::endl;

      const xAOD::CaloCluster* constCluster = theChargedPFO->cluster(0);
      xAOD::CaloCluster* matchedCluster = const_cast<xAOD::CaloCluster*>(constCluster);

      if (matchedCluster){

        m_integrator->measureNewClus(matchedCluster, *trackCalo);
        
	eflowFirstIntENUM j1st = m_integrator->getFirstIntLayer();
	eflowCellSubtractionManager tempRanking;

	const std::pair<double, double> trackExtrapEM2EtaPhi = trackCalo->getEM2etaPhi();

        m_binnedParameters->getOrdering(tempRanking, trackE, trackExtrapEM2EtaPhi.first, j1st);
        const double eExpect = tempRanking.fudgeMean() * trackE;
        const double sigmaEExpect = ((tempRanking.fudgeStdDev() * trackE)*(tempRanking.fudgeStdDev() * trackE));
        
        if (eExpect < hadronicEnergy){
          if (j1st == eflowFirstIntRegions::EMB1 || j1st == eflowFirstIntRegions::EMB2) j1st = eflowFirstIntRegions::Tile;
          if (j1st == eflowFirstIntRegions::EME1 || j1st == eflowFirstIntRegions::EME2) j1st = eflowFirstIntRegions::HEC;
        }

	eflowCellSubtractionManager ranking;
        m_binnedParameters->getOrdering(ranking, trackE, trackExtrapEM2EtaPhi.first, j1st);

        if (1 == debug) std::cout << "Expected mean and sigma are " << eExpect << " and " << sqrt(sigmaEExpect) << std::endl;
        if (1 == debug) std::cout << "j1st is " << j1st << std::endl;

        bool doSplitShowers = false;

        std::map<const xAOD::CaloCluster*,eflowClus*>::iterator testExistence = clusMap.find(matchedCluster);

	if (testExistence!= clusMap.end()) doSplitShowers = clusMap[matchedCluster]->checkEOverP(m_consistencySigmaCut);
        else{
          std::map<const xAOD::CaloCluster*,eflowClus*>::iterator firstClusInMap = clusMap.begin();
          std::map<const xAOD::CaloCluster*,eflowClus*>::iterator lastClusInMap = clusMap.end();

          for (; firstClusInMap != lastClusInMap; ++firstClusInMap){

            const xAOD::CaloCluster* thisClus = (*firstClusInMap).first;
            if ( fabs(thisClus->e() - matchedCluster->e()) < 0.00001 ) {
              doSplitShowers = (*firstClusInMap).second->checkEOverP(m_consistencySigmaCut);
              break;
            }
          }
        }
	
	if (1 == debug) std::cout << "useCluster is " << doSplitShowers << std::endl;

        if (false == doSplitShowers){
           eflowCellList orderedCells;

           orderedCells.setNewExtrapolatedTrack(trackCalo);

	   std::vector<xAOD::CaloCluster*> tracksClusVec;
           tracksClusVec.push_back(matchedCluster);
           eflowCellSubtractionFacilitator facilitator;

	   //layers for debugging
           if (1 == debug){
             double EMB_E = matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EMB1) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EMB2) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EMB3);
             double EME_E = matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EME1) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EME2) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EME3);
             double HEC_E = matchedCluster->eSample(xAOD::CaloCluster::CaloSample::HEC0) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::HEC1) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::HEC2);
             double Tile_E = matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileBar0) +  matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileBar1) +  matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileBar2) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileGap1) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileGap2) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileGap3) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileExt0) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileExt1) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileExt2);

             std::cout << "before subtraction layer energies are EMB, EME, HEC and Tile of " << EMB_E << ", " << EME_E << ", " << HEC_E << " and " << Tile_E << std::endl;
           }
	   
	   bool doSubtraction = true;

           if (1 == numProng){
             
	     double hadronicE_inClusters  = this->getHadronicEnergy(tracksClusVec);

	     //at this point we will reset fudgeMean by knowing that
             //given expectE = E/P * trackE can work out that new fudgemean given by E/P = (expectE - hadCalE)/trackE

             double updatedE = trackE*ranking.fudgeMean() - hadronicE_inClusters;
             if (1 == debug) std::cout << "updatedE is " << updatedE << std::endl;
             if (updatedE < 0) {
               updatedE = 0;
               doSubtraction = false;
             }
             double updatedFudgeMean = updatedE/trackE;
             ranking.setFudgeMean(updatedFudgeMean);

	     std::vector<xAOD::CaloCluster*>::iterator firstClusterA = tracksClusVec.begin();
             std::vector<xAOD::CaloCluster*>::iterator lastClusterA = tracksClusVec.end();
             
	     //isnt this vector always of size 1? MH
	     //could simpify this by combining this loop to one one cluster and then also combine with add cells to ordered cells? MH
             for (; firstClusterA != lastClusterA; ++firstClusterA){

	       xAOD::CaloCluster* theCluster = *firstClusterA;
	       
	       const CaloClusterCellLink* theCellLink = theCluster->getCellLinks();
	       if (theCellLink){
		 CaloClusterCellLink* theCellLink_nonConst = const_cast<CaloClusterCellLink*>(theCellLink);
		 CaloClusterCellLink::iterator theFirstCell = theCellLink_nonConst->begin();
		 CaloClusterCellLink::iterator theLastCell = theCellLink_nonConst->end();

		 for (; theFirstCell != theLastCell; ){

		   const CaloCell* thisCell = *theFirstCell;
		   const CaloDetDescrElement* descr = thisCell->caloDDE();
		   CaloCell_ID::CaloSample thisSample = descr->getSampling();
		   if (CaloCell_ID::HEC0 == thisSample || CaloCell_ID::HEC1 == thisSample || CaloCell_ID::HEC2 == thisSample || CaloCell_ID::HEC3 == thisSample || CaloCell_ID::TileBar0 == thisSample || CaloCell_ID::TileBar1 == thisSample || CaloCell_ID::TileBar2 == thisSample || CaloCell_ID::TileGap1 == thisSample ||  CaloCell_ID::TileGap2 == thisSample ||  CaloCell_ID::TileGap3 == thisSample ||  CaloCell_ID::TileExt0 == thisSample ||  CaloCell_ID::TileExt1 == thisSample ||  CaloCell_ID::TileExt0 == thisSample) {
		     theFirstCell = theCellLink_nonConst->removeCell(theFirstCell);
		     theLastCell = theCellLink_nonConst->end();
		   }
		   else ++theFirstCell;
		 }//cell loop
		 CaloClusterKineHelper::calculateKine(theCluster,true, true);
	       }//if cell link is valid
	       else msg(MSG::WARNING) << MSG::WARNING << " Invalid cell link from this cluster" << endreq;
	     }//cluster loop
	   }//one prong	     

	   const CaloClusterCellLink* theCellLink = matchedCluster->getCellLinks();
	   CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
	   CaloClusterCellLink::const_iterator lastCell = theCellLink->end();
	   
	   for (; firstCell != lastCell; firstCell++) {
	     std::pair<CaloCell*,int> myPair(const_cast<CaloCell*>(*firstCell),0);//0 is dummy. dont need it                                                              
	     orderedCells.addCell(myPair);
	   }
	   
	   //double sigmaEExpect = 0.0;
	   
	   //We will not subtract any energy if the hadronic energy constraint means the expected deposit is zero energy - flagged with doSubtraction
	   if (true == doSubtraction) /*sigmaEExpect =*/ facilitator.subtractCells(ranking,trackE,tracksClusVec,orderedCells);
	   
	   if (1 == debug) {
	     std::cout << "After subtraction have cluster with E, eta and phi of " << matchedCluster->e() << ", " << matchedCluster->eta() << " and " << matchedCluster->phi() << std::endl;
	     double EMB_E = matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EMB1) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EMB2) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EMB3);
	     double EME_E = matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EME1) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EME2) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::EME3);
	     double HEC_E = matchedCluster->eSample(xAOD::CaloCluster::CaloSample::HEC0) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::HEC1) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::HEC2);
	     double Tile_E = matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileBar0) +  matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileBar1) +  matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileBar2) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileGap1) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileGap2) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileGap3) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileExt0) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileExt1) + matchedCluster->eSample(xAOD::CaloCluster::CaloSample::TileExt2);
	     std::cout << "before subtraction layer energies are EMB, EME, HEC and Tile of " << EMB_E << ", " << EME_E << ", " << HEC_E << " and " << Tile_E << std::endl;
	   }
	     
	   bool annflag = false;

	   if (true == m_doAnnihilation){

	     //retrieve information on whether cluster contains additional shower
	     bool annflag = facilitator.annFlag();
	   
	     if (1 == debug) std::cout << "Initially annflag is " << annflag << std::endl;
	   
	     
	     if ( matchedCluster->e() < m_subtractionSigmaCut * sigmaEExpect ) {
	       this->annihilateCluster(matchedCluster);
	       orderedCells.eraseList();
	       annflag = true;
	     }//if should annihilate
	   
	   }//if option to decide to annihilate is on
	   
	   if (1 == debug) std::cout << "Now annflag is " << annflag << std::endl;
	   
	   if (false == annflag) {
	     xAOD::PFO* neutralPFO = new xAOD::PFO();
	     thePFOContainer->push_back(neutralPFO);
	     neutralPFO->setCharge(0.0);
	     
	     xAOD::CaloCluster *newCluster = new xAOD::CaloCluster(*matchedCluster);
	     theFinalClusterContainer->push_back(newCluster);
	     
	     ElementLink<xAOD::CaloClusterContainer> thisClusterLink;
	     thisClusterLink.setElement(newCluster);
	     thisClusterLink.setStorableObject(*theFinalClusterContainer);
	     
	     bool isSet = neutralPFO->setClusterLink(thisClusterLink);
	     if (false == isSet) msg(MSG::WARNING) << MSG::WARNING << " Could not add Cluster to PFO " << endreq;
	     neutralPFO->setP4(newCluster->pt(),newCluster->eta(),newCluster->phi(),newCluster->m());
	     
	     if (1 == debug) std::cout << "Creating left over EFO with E, eta and phi of " << matchedCluster->e() << ", " << matchedCluster->eta() << " and " << matchedCluster->phi() << std::endl;
	     
	   }
	}//Cluster was consistent with expected E/P so subtracted only from that cluster
        else{
	  theChargedPFO->setAttribute<bool>("PassEOverPCheck", false);
        }//ok we had a split shower and have flagged this track as such to be looked at in next stage
      }//matched cluster
      else msg(MSG::WARNING) << MSG::WARNING << " Invalid pointer to matched cluster " << endreq;
    }//bin exists
  }//loop on matched tracks
  
  //now finally recover the split showers and isolated tracks                                                                                                                                                      
  //First add tracks not matched to clusters in list to recover (if option is on)                                                                                                                                  

  std::vector<std::pair<xAOD::PFO*,eflowTrackCaloPoints*> > finalMap;

  if (true == m_recoverIsolatedTracks){
    finalMap.insert (finalMap.end(),
                     isolatedChargedPFOs.begin(),
                     isolatedChargedPFOs.end());
  }//recover isolated tracks

  //Secondly add tracks that had a matched cluster with significantly energy than expected energy deposit (always on)                                                                                              

  for (auto& p : matchingVec) {
    xAOD::PFO* theChargedPFO = p.first;
    eflowTrackCaloPoints* trackCalo = p.second;

    bool PassEOverPCheck = true;
    bool isAvailable = theChargedPFO->attribute<bool>("PassEOverPCheck", PassEOverPCheck);
    if (true == isAvailable){
      if (false == PassEOverPCheck) {
	std::pair<xAOD::PFO*,eflowTrackCaloPoints*> myPair(theChargedPFO,trackCalo);
	finalMap.push_back(myPair);
      }
      else delete trackCalo;
    }//if variable is in the store
  }//loop on matched track list

  //Now build list of unique clusters to consider - more than one PFO could have the same cluster linked, so need to be careful                                                                                                                          
         
  xAOD::PFOContainer::iterator firstPFO = thePFOContainer->begin();
  xAOD::PFOContainer::iterator lastPFO = thePFOContainer->end();

  std::vector<xAOD::PFO*> uniquePFO;

  for (; firstPFO != lastPFO; ++firstPFO){
    xAOD::PFO* PFO = *firstPFO;
    if (1 == debug) std::cout << "considering if PFO with charge, e, eta and phi of " << PFO->charge() << ", " << PFO->e() << ", " << PFO->eta() << " and " << PFO->phi() << " is unique " << std::endl;

    const xAOD::CaloCluster* thisClus = PFO->cluster(0);

    if (thisClus){

        std::vector<xAOD::PFO*>::iterator firstUniquePFO =  uniquePFO.begin();
        std::vector<xAOD::PFO*>::iterator lastUniquePFO =  uniquePFO.end();
        bool isUnique = true;

        for (; firstUniquePFO != lastUniquePFO; ++firstUniquePFO){

          xAOD::PFO* PFO_B = *firstUniquePFO;
	  const xAOD::CaloCluster* thisClus_B = PFO_B->cluster(0);
	  if (thisClus_B){
	    if ( fabs(thisClus->e() - thisClus_B->e()) < 0.0001) isUnique = false;
	  }//if thisclusB                                 

	}//unique loop                         
	if (true == isUnique) {

	  uniquePFO.push_back(PFO);
	  if (1 == debug) std::cout << "PFO with charge, e, eta and phi of " << PFO->charge() << ", " << PFO->e() << ", " << PFO->eta() << " and " << PFO->phi() << " is unique " << std::endl;

	}//unique PFO/cluster
    }//if thisclus is valid pointer  
  }//loop over all of the PFO

  //first check we have not pushed back such a cluster   

  std::sort(finalMap.begin(),finalMap.end(),EFlow_TauFn::gtpt());

  std::vector<std::pair<xAOD::PFO*,eflowTrackCaloPoints*> >::iterator firstPair = finalMap.begin();
  std::vector<std::pair<xAOD::PFO*,eflowTrackCaloPoints*> >::iterator lastPair = finalMap.end();

  for (; firstPair != lastPair; ++firstPair){

    xAOD::PFO* theChargedPFO = (*firstPair).first;

    if (1 == debug) std::cout << "Starting recovery for charged PFO with E, eta and phi " << theChargedPFO->e() << ", " << theChargedPFO->eta() << " and " << theChargedPFO->phi() << std::endl;

    //This vector will store all clusters to subtract from
        
    std::vector<xAOD::CaloCluster*> matchedClusters;

    eflowTrackCaloPoints* trackCalo = (*firstPair).second;
    const std::pair<double, double> trackEM2etaPhi = trackCalo->getEM2etaPhi();

    if (1 == debug) std::cout << "extrapolated eta and phi are " << trackEM2etaPhi.first << " and " << trackEM2etaPhi.second << std::endl;

    //Now we need to find all clusters within a cone of 0.2

    std::vector<xAOD::PFO*>::iterator firstPFO =  uniquePFO.begin();
    std::vector<xAOD::PFO*>::iterator lastPFO =  uniquePFO.end();

    for (; firstPFO != lastPFO; ++firstPFO){

      xAOD::PFO* thisPFO = *firstPFO;

      const std::pair<double, double> trackExtrapEM2EtaPhi = trackCalo->getEM2etaPhi();

      const xAOD::CaloCluster* thisClus = thisPFO->cluster(0);

      if (thisClus){

	if (1 == debug) std::cout << "trying to match cluster with E, eta and phi " << thisClus->e() << ", " << thisClus->eta() << " and " << thisClus->phi() << std::endl;

	double eta = thisClus->eta();
	double phi = thisClus->phi();

	double etaDiff = eta - trackExtrapEM2EtaPhi.first;
	double phiDiff = phi - trackExtrapEM2EtaPhi.second;

	if (phiDiff > 3.14) phiDiff = 6.28 - phiDiff;

	double deltaR = sqrt( (etaDiff*etaDiff) + (phiDiff*phiDiff) );

	if (deltaR < 0.2){
	  matchedClusters.push_back(const_cast<xAOD::CaloCluster*>(thisClus));
	  if (1 == debug) std::cout << "Have matched cluster with E, eta and phi " << thisClus->e() << ", " << thisClus->eta() << " and " << thisClus->phi() << std::endl;
	}//if within cone of 0.2   


      }//valid cluster pointer
      else msg(MSG::WARNING) << MSG::WARNING << " This matching map has a track without a valid matched cluster pointer " << endreq;

    }//loop over unique PFO vector

    // if we don't use map could avoid all sorts? MH
    std::sort(matchedClusters.begin(),matchedClusters.end(),EFlow_TauFn::sortClusters());

    //now do subtraction               

    m_integrator->measureNewClus(matchedClusters, *trackCalo);

    eflowFirstIntENUM j1st = m_integrator->getFirstIntLayer();

    eflowCellList orderedCells;
    orderedCells.setNewExtrapolatedTrack(trackCalo);
    delete trackCalo;

    double trackE = theChargedPFO->e();
    // get parameters for this eta, j1st 

    eflowCellSubtractionManager tempRanking;
    m_binnedParameters->getOrdering(tempRanking, trackE, trackEM2etaPhi.first, j1st);
    
    const double eExpect = tempRanking.fudgeMean() * trackE;
    const double sigmaEExpect_test = ((tempRanking.fudgeStdDev() * trackE)*(tempRanking.fudgeStdDev() * trackE));

    if (eExpect < hadronicEnergy){
      if (j1st == eflowFirstIntRegions::EMB1 || j1st == eflowFirstIntRegions::EMB2) j1st = eflowFirstIntRegions::Tile;
      if (j1st == eflowFirstIntRegions::EME1 || j1st == eflowFirstIntRegions::EME2) j1st = eflowFirstIntRegions::HEC;
    }

    eflowCellSubtractionManager ranking;

    if (1 == debug) std::cout << "Input E, eta and j1st of " << trackE << " , " << trackEM2etaPhi.first << " and " << j1st << std::endl;
    m_binnedParameters->getOrdering(ranking, trackE, trackEM2etaPhi.first, j1st);

    if (1 == debug) std::cout << "Expected mean and sigma are " << eExpect << " and " << sigmaEExpect_test << std::endl;
    if (1 == debug) std::cout << "j1st is " << j1st << std::endl;

    //do subtraction of cells          

    eflowCellSubtractionFacilitator facilitator;

    if (1 == debug){

      std::vector<xAOD::CaloCluster*>::iterator firstMatchedClus = matchedClusters.begin();
      std::vector<xAOD::CaloCluster*>::iterator lastMatchedClus = matchedClusters.end();

      for (; firstMatchedClus != lastMatchedClus; ++firstMatchedClus) {
        std::cout << "Before subtraction have cluster with E, eta and phi of " << (*firstMatchedClus)->e() << ", " <<  (*firstMatchedClus)->eta() << " and " <<  (*firstMatchedClus)->phi() << std::endl;
        double EMB_E = (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EMB1) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EMB2) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EMB3);
        double EME_E = (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EME1) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EME2) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EME3);
        double HEC_E = (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::HEC0) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::HEC1) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::HEC2);
        double Tile_E = (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileBar0) +  (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileBar1) +  (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileBar2) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileGap1) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileGap2) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileGap3) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileExt0) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileExt1) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileExt2);
        std::cout << "before subtraction layer energies are EMB, EME, HEC and Tile of " << EMB_E << ", " << EME_E << ", " << HEC_E << " and " << Tile_E << std::endl;
      }//matched cluster loop
    }//debug output

    bool doSubtraction = true;

    if (1 == numProng){
      double hadronicE_inClusters = this->getHadronicEnergy(matchedClusters);
      
      //at this point we will reset fudgeMean by knowing that 
      //given expectE = E/P * trackE can work out that new fudgemean given by E/P = (expectE - hadCalE)/trackE 

      double updatedE = trackE*ranking.fudgeMean() - hadronicE_inClusters;
      if (1 == debug) std::cout << "updatedE is " << updatedE << std::endl;
      if (updatedE < 0) {
        updatedE = 0;
        doSubtraction = false;
      }
      double updatedFudgeMean = updatedE/trackE;
      ranking.setFudgeMean(updatedFudgeMean);
      
      //Then remove all cells in hadronic calorimeters
      //Again surely we can combine these loops and be more efficient? MH
      
      std::vector<xAOD::CaloCluster*>::iterator firstClusterA = matchedClusters.begin();
      std::vector<xAOD::CaloCluster*>::iterator lastClusterA = matchedClusters.end();

      for (; firstClusterA != lastClusterA; ++firstClusterA){

	const xAOD::CaloCluster* theConstCluster = *firstClusterA;
	xAOD::CaloCluster* theCluster = const_cast<xAOD::CaloCluster*>(theConstCluster);
	       
	const CaloClusterCellLink* theCellLink = theCluster->getCellLinks();
	if (theCellLink){
	  CaloClusterCellLink* theCellLink_nonConst = const_cast<CaloClusterCellLink*>(theCellLink);
	  CaloClusterCellLink::iterator theFirstCell = theCellLink_nonConst->begin();
	  CaloClusterCellLink::iterator theLastCell = theCellLink_nonConst->end();
		 
	  for (; theFirstCell != theLastCell;){
	    const CaloCell* thisCell = *theFirstCell;
	    const CaloDetDescrElement* descr = thisCell->caloDDE();
	    CaloCell_ID::CaloSample thisSample = descr->getSampling();
	    if (CaloCell_ID::HEC0 == thisSample || CaloCell_ID::HEC1 == thisSample || CaloCell_ID::HEC2 == thisSample || CaloCell_ID::HEC3 == thisSample || CaloCell_ID::TileBar0 == thisSample || CaloCell_ID::TileBar1 == thisSample || CaloCell_ID::TileBar2 == thisSample || CaloCell_ID::TileGap1 == thisSample ||  CaloCell_ID::TileGap2 == thisSample ||  CaloCell_ID::TileGap3 == thisSample ||  CaloCell_ID::TileExt0 == thisSample ||  CaloCell_ID::TileExt1 == thisSample ||  CaloCell_ID::TileExt0 == thisSample) {
	      theFirstCell = theCellLink_nonConst->removeCell(theFirstCell);
	      theLastCell = theCellLink_nonConst->end();
	    }
	    else ++theFirstCell;
	  }//cell loop
	  CaloClusterKineHelper::calculateKine(theCluster,true, true);
	}//if cell link is valid

      }//cluster loop
    }

    std::vector<xAOD::CaloCluster*>::iterator firstMatchedCluster = matchedClusters.begin();
    std::vector<xAOD::CaloCluster*>::iterator lastMatchedCluster = matchedClusters.end();

    int matchedClusterCounter = 0;
    
    for (; firstMatchedCluster != lastMatchedCluster; firstMatchedCluster++) {

      const xAOD::CaloCluster* thisClus = *firstMatchedCluster;
      if (1 == debug) std::cout << "adding cells from cluster  with e, eta and phi of " << thisClus->e() << ", " << thisClus->eta() << " and " << thisClus->phi() << std::endl;
      if (thisClus){
	
	const CaloClusterCellLink* theCellLink = thisClus->getCellLinks();
	CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
	CaloClusterCellLink::const_iterator lastCell = theCellLink->end();
	
	for (; firstCell != lastCell; firstCell++) {
	  std::pair<CaloCell*,int> myPair(const_cast<CaloCell*>(*firstCell),matchedClusterCounter);//0 is dummy. dont need it
	  orderedCells.addCell(myPair);
	}

      }//if valid cluster pointer      
      else {
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get valid cluster pointer in matched selection" << endreq;
      }
      ++matchedClusterCounter;
    }//matched cluster loop      

    double sigmaEExpect = 0.0;
    if (true == doSubtraction) sigmaEExpect = facilitator.subtractCells(ranking,trackE , matchedClusters, orderedCells);

    if (1 == debug){
      std::vector<xAOD::CaloCluster*>::iterator firstMatchedClus = matchedClusters.begin();
      std::vector<xAOD::CaloCluster*>::iterator lastMatchedClus = matchedClusters.end();
      for (; firstMatchedClus != lastMatchedClus; ++firstMatchedClus) {
        std::cout << "After subtraction have cluster with E, eta and phi of " << (*firstMatchedClus)->e() << ", " <<  (*firstMatchedClus)->eta() << " and " <<  (*firstMatchedClus)->phi() << std::endl;
        double EMB_E = (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EMB1) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EMB2) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EMB3);
	double EME_E = (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EME1) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EME2) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::EME3);
        double HEC_E = (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::HEC0) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::HEC1) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::HEC2);
        double Tile_E = (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileBar0) +  (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileBar1) +  (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileBar2) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileGap1) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileGap2) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileGap3) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileExt0) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileExt1) + (*firstMatchedClus)->eSample(xAOD::CaloCluster::CaloSample::TileExt2);
        std::cout << "after subtraction layer energies are EMB, EME, HEC and Tile of " << EMB_E << ", " << EME_E << ", " << HEC_E << " and " << Tile_E << std::endl;
      }//matched cluster loop
    }//debug output

    std::vector<xAOD::CaloCluster*>::iterator firstClus = matchedClusters.begin();
    std::vector<xAOD::CaloCluster*>::iterator lastClus = matchedClusters.end();

    double newClusSumE = 0.0;

    for (; firstClus != lastClus; firstClus++) newClusSumE+=(*firstClus)->e();


    if (true == m_doAnnihilation){
                                       
      if ( newClusSumE < m_subtractionSigmaCut * sigmaEExpect ){  
	if (1 == debug) std::cout << "Annihilate all these clusters " << std::endl;    
	firstClus = matchedClusters.begin();   
	for (; firstClus != lastClus; firstClus++){   
	  this->annihilateCluster(*firstClus);
	  orderedCells.eraseList();        
	}                                
      }//if should annih               
    }//if option to decide on annihilation is on

  }//loop over final map of track-cluster pairs

  //At this point the alert reader might notice we have subtracted cells from neutral EFO in the above split shower recovery loop, without updating the 4-vector of the neutral EFO - this will be done when we calibrated the neutral EFO and reset all neutral EFO 4-vectors to match the calibrated clusters.
  std::map<const xAOD::CaloCluster*,eflowClus*>::iterator firstClusInMap = clusMap.begin();
  std::map<const xAOD::CaloCluster*,eflowClus*>::iterator lastClusInMap = clusMap.end();

  //delete all the eflowClus objects we have created 
  for (; firstClusInMap != lastClusInMap; ++firstClusInMap) delete (*firstClusInMap).second;

  return StatusCode::SUCCESS;
}


StatusCode eflowTauTool::finalize(){
   return StatusCode::SUCCESS;
}

double eflowTauTool::getHadronicEnergy(std::vector<xAOD::CaloCluster*> theClusterContainer){

  double hadronicEnergy = 0.0;

  std::vector<xAOD::CaloCluster*>::const_iterator firstTauCluster = theClusterContainer.begin();
  std::vector<xAOD::CaloCluster*>::const_iterator lastTauCluster = theClusterContainer.end();

  for (; firstTauCluster != lastTauCluster; ++firstTauCluster) hadronicEnergy += this->calculateHadronicEnergy(*firstTauCluster);

  return hadronicEnergy;
}

double eflowTauTool::getHadronicEnergy(ElementLinkVector<xAOD::CaloClusterContainer>* theClusterContainer){

  double hadronicEnergy = 0.0;

  ElementLinkVector<xAOD::CaloClusterContainer>::iterator firstTauCluster =  theClusterContainer->begin();
  ElementLinkVector<xAOD::CaloClusterContainer>::iterator lastTauCluster =  theClusterContainer->end();

  for (; firstTauCluster != lastTauCluster; ++firstTauCluster) hadronicEnergy += this->calculateHadronicEnergy(**firstTauCluster);

  return hadronicEnergy;
}

double eflowTauTool::calculateHadronicEnergy(const xAOD::CaloCluster* theCluster){
 
  double hadronicEnergy = 0.0;
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::HEC0);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::HEC1);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::HEC2);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::HEC3);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::TileBar0);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::TileBar1);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::TileBar2);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::TileGap1);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::TileGap2);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::TileGap3);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::TileExt0);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::TileExt1);
  hadronicEnergy += theCluster->eSample(xAOD::CaloCluster::CaloSample::TileExt2);

  return hadronicEnergy;

}

void eflowTauTool::annihilateCluster(const xAOD::CaloCluster* theCluster){

  const CaloClusterCellLink* theCellLink = theCluster->getCellLinks();
  CaloClusterCellLink* theCellLink_nonConst = const_cast<CaloClusterCellLink*>(theCellLink);
  CaloClusterCellLink::iterator theFirstCell = theCellLink_nonConst->begin();
  CaloClusterCellLink::iterator theLastCell = theCellLink_nonConst->end();
  
  xAOD::CaloCluster* nonConstCluster = const_cast<xAOD::CaloCluster*>(theCluster);

  for (; theFirstCell != theLastCell; ){
    theFirstCell = theCellLink_nonConst->removeCell(theFirstCell);
    theLastCell = theCellLink_nonConst->end();
  }

  nonConstCluster->setCalE(0.0);
  nonConstCluster->setRawE(0.0);

  CaloClusterKineHelper::calculateKine(nonConstCluster,true, true);

}
