/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaSuperClusterBuilder.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "egammaTopoClusterMap.h"
#include "xAODEgamma/Egamma.h"

#include "CandidateMatchHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "CaloUtils/CaloCellList.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellDetPos.h"

#include "CaloEvent/CaloClusterCellLink.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/MsgStream.h"

#include <algorithm>

bool SortByDescendingEnergy(std::pair<const xAOD::CaloCluster*, std::vector<const xAOD::TrackParticle*> > clusTracks1,
			    std::pair<const xAOD::CaloCluster*, std::vector<const xAOD::TrackParticle*> > clusTracks2)
{
  return (clusTracks1.first->e() > clusTracks2.first->e());
}

//Used for checking if a cluster has associated tracks.
//IS THERE A BETTER WAY TO DO THIS?
class ClusCheck
{
public:
  ClusCheck(const xAOD::CaloCluster *clus) : m_clus(clus) {}
  bool operator() (egammaRec           *match) const {return match->caloCluster()->e() == m_clus->e();};
private:
  const xAOD::CaloCluster *m_clus;
};

//////////////////////////////////////////////////////////////////////////////
//Athena interfaces.
//////////////////////////////////////////////////////////////////////////////

//Constructor.
egammaSuperClusterBuilder::egammaSuperClusterBuilder(const std::string& type,
						     const std::string& name,
						     const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_caloCellDetPos(0),
  m_storeGate(0),
  m_ShowerShape("egammaShowerShape/egammashowershape")

{
  // Declare interface.
  declareInterface<IegammaSuperClusterBuilder>(this);

  m_caloCellDetPos = new CaloCellDetPos();

}

//Destructor.
egammaSuperClusterBuilder::~egammaSuperClusterBuilder() {}

StatusCode egammaSuperClusterBuilder::initialize() {
  ATH_MSG_INFO(" Initializing egammaSuperClusterBuilder");

  // retrieve egammaShowerShape Tool
  if(m_ShowerShape.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_ShowerShape);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_INFO("Retrieved shower shape tool " << m_ShowerShape);

  if (service("StoreGateSvc", m_storeGate).isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode egammaSuperClusterBuilder::finalize() {
  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
//Functional routines.
//////////////////////////////////////////////////////////////////////////////

StatusCode egammaSuperClusterBuilder::execute()
					      
{

  const xAOD::CaloClusterContainer* clusters = 0;
  StatusCode sc = evtStore()->retrieve(clusters,  "EMTopoCluster430");
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("No input EM Cluster container found EMTopoCluster430");
    return StatusCode::SUCCESS;
  }
  
  //Retrieve calo cell container.
  if( evtStore()->contains<CaloCellContainer>("AllCalo")) {  
    sc = evtStore()->retrieve(m_cellcoll, "AllCalo") ; 
    if(sc.isFailure() || !m_cellcoll) {
      ATH_MSG_WARNING("newEgammaBuilder::PassesF1Cut: no Calo Cell Container");
      return StatusCode::SUCCESS;
    }
  }


  //Get list of refitted tracks.
  const xAOD::TrackParticleContainer  *refittedTracks = 0;
  sc=evtStore()->retrieve(refittedTracks, "GSFTrackParticles" );
  if(sc.isFailure()) {
    ATH_MSG_INFO("Failed to retrieve GSFTrackParticles!");
    return StatusCode::FAILURE;
  }

  //Get single topocluster egammaRec container.
  EgammaRecContainer *egammaRecs = 0;
  sc=evtStore()->retrieve(egammaRecs, "TopoTrackClusterMatches" );
  if(sc.isFailure()) {
    ATH_MSG_INFO("Failed to retrieve TopoTrackClusterMatches!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Executing superclustering routine");
  
  //Clear out old list of used clusters.
  usedClusters.clear();

  //Have to register cluster container in order to properly get cluster links.
  xAOD::CaloClusterContainer *outputClusterContainer = CaloClusterStoreHelper::makeContainer(m_storeGate, 
  											     "egammaClusters", 
  											     msg());
  if (!outputClusterContainer) {
    ATH_MSG_INFO("Could not make supercluster container!");
  }
  
  EgammaRecContainer *newEgammaRecs = new EgammaRecContainer();
  if (evtStore()->record(newEgammaRecs, "egammaSuperRecCollection").isFailure()) {
    ATH_MSG_ERROR("Could not record egammaRecContainer");
    return StatusCode::FAILURE;
  }
  
  //Loop over input egammaRec objects, build superclusters.
  for (auto egRec : *egammaRecs) {
    
    if (ClusterIsUsed(egRec->caloCluster())) {
      ATH_MSG_INFO("Cluster already used - skip it!");
      continue;
    } else {
      ATH_MSG_INFO("Cluster hasn't been used yet!");
    }
    
    //If the cluster is above the Pt threshold, get nearby secondary EM clusters.
    secondaryTracks.clear();

    //First add the good tracks.
    for (unsigned int i = 0; i < egRec->getNumberOfTrackParticles(); i++)
      secondaryTracks.insert(secondaryTracks.end(), egRec->trackParticle(i));

    //Search for secondary clusters.
    secondaryClusters.clear();
    if (egRec->caloCluster()->pt() > 7E3) {
      usedClusters.push_back(egRec->caloCluster());
      secondaryClusters = SearchForSecondaryClusters(egRec->caloCluster(),
						     clusters,
						     egammaRecs);
    }
    
    ATH_MSG_INFO("Size of secondary clusters container: " << int(secondaryClusters.size()));

    //Merge tracks together for clusters matched to tracks.
    std::vector<const xAOD::CaloCluster*>::iterator secIter = secondaryClusters.begin();

    for (; secIter != secondaryClusters.end(); secIter++) {

      EgammaRecContainer::const_iterator     loc = std::find_if(egammaRecs->begin(), egammaRecs->end(), ClusCheck((*secIter)));

      //First check to see if a topocluster is track-matched.
      //If so, keep the tracks.
      for (unsigned int i = 0; i < (*loc)->getNumberOfTrackParticles(); i++)
	secondaryTracks.insert(secondaryTracks.end(), (*loc)->trackParticle(i));

    }

    //Take the list of secondaries and make a super cluster.
    //secondaryClusters.push_back((*iter).first);
    if (secondaryTracks.size()>0) {
      secondaryClusters.insert(secondaryClusters.begin(), egRec->caloCluster());

      std::vector< ElementLink<xAOD::TrackParticleContainer> > elTracks;
      for (unsigned int i = 0; i < secondaryTracks.size(); i++)
	elTracks.push_back(GetTrackParticleLink(refittedTracks,secondaryTracks[i]));

      xAOD::CaloCluster *newClus = AddTopoClusters(secondaryClusters);

      //Fill some relevant calibration variables.
      if (newClus) {
	fillPositionsInCalo(newClus);
	newClus->setRawE   (newClus->calE());
	newClus->setRawEta (newClus->calEta());
	newClus->setRawPhi (newClus->calPhi());

	ATH_MSG_INFO("Filled positions in calo");

	static SG::AuxElement::Decorator<int> nClusters ("nClusters");
	nClusters(*newClus) = secondaryClusters.size();

	//Push it back into the output container.
	if (outputClusterContainer)
	  outputClusterContainer->push_back(newClus);

	ElementLink< xAOD::CaloClusterContainer > clusterLink(newClus, *outputClusterContainer);
	std::vector< ElementLink<xAOD::CaloClusterContainer> > elClusters {clusterLink};

	//Make egammaRec object, and push it back into output container.
	egammaRec *newEgRec = new egammaRec();
	if (newEgRec && newEgammaRecs) {
	  newEgRec->setCaloClusters  (elClusters);
	  newEgRec->setTrackParticles(elTracks);
	  newEgammaRecs->push_back(newEgRec);
	  ATH_MSG_INFO("Made new egammaRec object");
	}
      }
    }

  } //End look on egammaRecs

  return StatusCode::SUCCESS;

}

std::vector<const xAOD::CaloCluster*> egammaSuperClusterBuilder::SearchForSecondaryClusters(const xAOD::CaloCluster          *cluster,
											    const xAOD::CaloClusterContainer *clusters,
											    EgammaRecContainer               *electrons)
{

  std::vector<const xAOD::CaloCluster*> secondaryClusters;

  xAOD::CaloClusterContainer::const_iterator iter    = clusters->begin();
  xAOD::CaloClusterContainer::const_iterator iterEnd = clusters->end();

  const xAOD::CaloCluster *clus = 0;
  int nClusters(0);

  for (; iter != iterEnd; iter++) {

    clus = (*iter);
   
    //Ignore cluster if it's already been used.
    if (ClusterIsUsed(clus)) continue;

    EgammaRecContainer::iterator orig = std::find_if(electrons->begin(), electrons->end(), ClusCheck(cluster));
    EgammaRecContainer::iterator loc  = std::find_if(electrons->begin(), electrons->end(), ClusCheck(clus));

    int matchIndex(-1);
    if (loc != electrons->end() && loc != orig) 
      matchIndex = std::distance(electrons->begin(), loc);

    //Check that the track doesn't already belong to the higher Pt cluster.
    bool trackAlreadyUsed(false);
    if (matchIndex>-1) {
      if (electrons->at(matchIndex)->trackParticle(0)) {
	if (std::find(secondaryTracks.begin(),
		      secondaryTracks.end(),
		      electrons->at(matchIndex)->trackParticle(0)) != secondaryTracks.end())
	  trackAlreadyUsed = true;
      }
    }
    
    uint8_t bLayerHits(0);
    if (matchIndex > -1 && !trackAlreadyUsed) {
      ATH_MSG_INFO("Cluster has a matching track!");
      if (electrons->at(matchIndex)->trackParticle(0)) {
	ATH_MSG_INFO("Track has b-layer hits - ignore it!");
	bLayerHits = (electrons->at(matchIndex))->trackParticle(0)->summaryValue(bLayerHits, xAOD::numberOfBLayerHits);
      }
    }

    if (bLayerHits > 0)   continue;
    if (clus->e() < 400.) continue;
    if (clus->e() > 15E3) continue;

    //Check on dEta, dPhi of cluster w.r.t. primary.
    float prim_pt(cluster->pt());
    
    CaloPhiRange m_phiHelper;
    float dPhi(m_phiHelper.diff(clus->phi(), cluster->phi()));
    
    if (prim_pt < 15E3) {
      if (fabs(clus->eta()-cluster->eta()) > 0.2) continue;
      if (fabs(dPhi) > 0.1)                       continue;
    }
    else if (prim_pt < 50E3) {
      if (fabs(clus->eta()-cluster->eta()) > 0.2) continue;
      if (fabs(dPhi) > 0.2)                       continue;
    }
    else {
      if (fabs(clus->eta()-cluster->eta()) > 0.2) continue;
      if (fabs(dPhi) > 0.3)                       continue;
    }

    //Shower shape plots show Ecore tops out at 20E3 for hadrons.
    if (CalculateShowerShape("ecore",clus) < 20E3) continue;

    //Add it to the list of secondary clusters.
    secondaryClusters.push_back(clus); 
    usedClusters.push_back(clus);
    nClusters++;

    if (nClusters>0) break;

  }
    
  return secondaryClusters;

}

xAOD::CaloCluster *egammaSuperClusterBuilder::AddTopoClusters(const std::vector <const xAOD::CaloCluster*> &clusters)
{

  if( clusters.size()  ==  0 )  return 0;

  xAOD::CaloCluster  deref_clus(*(clusters[0]));
  xAOD::CaloCluster* myCluster = new xAOD::CaloCluster(deref_clus);
  ATH_MSG_INFO("Made initial cluster with energy " << myCluster->e());
  
  if (clusters.size()==1)
    return myCluster;

  ATH_MSG_INFO( "Building cluster from "<< clusters.size() <<  " topoclusters");

  std::vector<const xAOD::CaloCluster*>::const_iterator  matchedClusters    = clusters.begin();
  std::vector<const xAOD::CaloCluster*>::const_iterator  matchedClustersEnd = clusters.end();
  matchedClusters++;

  for (; matchedClusters!=matchedClustersEnd; matchedClusters++) {
    const xAOD::CaloCluster *tmpCluster = *matchedClusters;

    ATH_MSG_INFO("Adding cells from cluster with energy " << tmpCluster->e());

    xAOD::CaloCluster::const_cell_iterator cell_itr = tmpCluster->begin();
    xAOD::CaloCluster::const_cell_iterator cell_end = tmpCluster->end();

    for (; cell_itr != cell_end; cell_itr++) {
      myCluster->addCell(cell_itr.index(), cell_itr.weight());
    }
    
  }
  
  //Update cluster kinematics.
  ATH_MSG_INFO("Added cells, recalculating kinematics");
  CaloClusterKineHelper::calculateKine(myCluster,true,true);

  ATH_MSG_INFO("Filling positions in calo");



  ATH_MSG_INFO("Made supercluster with energy " << myCluster->e());

  return myCluster;

}
  
float egammaSuperClusterBuilder::CalculateShowerShape(std::string name, const xAOD::CaloCluster *cluster)
{

  StatusCode sct = m_ShowerShape->execute(cluster, m_cellcoll);
  if (sct.isFailure())
    return -999.;

  if (name == "f1")
    return m_ShowerShape->f1();
  else if (name == "e033")
    return m_ShowerShape->e033();
  else if (name == "etot")
    return m_ShowerShape->etot();
  else if (name == "e1152")
    return m_ShowerShape->e1152();
  else if (name == "wtot")
    return m_ShowerShape->wtot();
  else if (name == "f1core")
    return m_ShowerShape->f1core();
  else if (name == "esec1")
    return m_ShowerShape->esec1();
  else if (name == "emin")
    return m_ShowerShape->emin();
  else if (name == "emax")
    return m_ShowerShape->emax();
  else if (name == "fracm")
    return m_ShowerShape->fracm();
  else if (name == "e233")
    return m_ShowerShape->e233();
  else if (name == "e235")
    return m_ShowerShape->e235();
  else if (name == "e237")
    return m_ShowerShape->e237();
  else if (name == "e255")
    return m_ShowerShape->e255();
  else if (name == "e277")
    return m_ShowerShape->e277();
  else if (name == "width")
    return m_ShowerShape->width();
  else if (name == "f3core")
    return m_ShowerShape->f3core();
  else if (name == "ecore")
    return m_ShowerShape->ecore();

  return -999.;

}

void egammaSuperClusterBuilder::fillPositionsInCalo(xAOD::CaloCluster* cluster) {

  bool isBarrel = xAOD::EgammaHelpers::isBarrel(cluster);
  CaloCell_ID::CaloSample sample = isBarrel ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;

  // eta and phi of the cluster in the calorimeter frame
  double eta, phi;
  m_caloCellDetPos->getDetPosition(sample, cluster->eta(), cluster->phi0(), eta, phi); 

  static SG::AuxElement::Decorator<float> etaCalo ("etaCalo");
  static SG::AuxElement::Decorator<float> phiCalo ("phiCalo");

  etaCalo(*cluster) = eta;
  phiCalo(*cluster) = phi;

  // eta and phi of the cluster in the calorimeter frame
  m_caloCellDetPos->getDetPosition(sample, cluster->eta(), cluster->phi(), eta, phi); 
  cluster->insertMoment(xAOD::CaloCluster::ETACALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHICALOFRAME,phi);

  //  eta in the second sampling
  m_caloCellDetPos->getDetPosition(sample, cluster->etaBE(2), cluster->phiBE(2), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA2CALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI2CALOFRAME,phi);
  //  eta in the first sampling 
  sample = isBarrel ? CaloCell_ID::EMB1 : CaloCell_ID::EME1;
  m_caloCellDetPos->getDetPosition(sample, cluster->etaBE(1), cluster->phiBE(1),eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA1CALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI1CALOFRAME,phi);

}

ElementLink< xAOD::TrackParticleContainer > egammaSuperClusterBuilder::GetTrackParticleLink(const xAOD::TrackParticleContainer* tracks,
											    const xAOD::TrackParticle* track)
{

  int nTrk(-1);
  
  for (const auto& egTrack : *tracks) {
    nTrk++;
    if (egTrack->perigeeParameters().position().perp() == track->perigeeParameters().position().perp()) {
      ATH_MSG_INFO("Got track element link!");
      ElementLink< xAOD::TrackParticleContainer > trackLink( *tracks, nTrk );
      return trackLink;
    }
  }

  ElementLink< xAOD::TrackParticleContainer > trackLink;
  ATH_MSG_INFO("TRACK ELEMENT LINK IS NULL!");
  return trackLink;
  
}
