/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaSuperClusterBuilder.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellDetPos.h"
#include "CaloUtils/CaloCellList.h"

#include "CaloEvent/CaloClusterCellLink.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/TrackParticleContainer.h" 

#include "egammaTopoClusterMap.h"
#include "CandidateMatchHelpers.h"

// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/MsgStream.h"

#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

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
  m_tpAndTSOS(0),
  m_delEta(0.025),
  m_delPhi(0.025),
  m_defaultParticleCaloExtensionTool("Trk::ParticleCaloExtensionTool")

{

  declareProperty("InputClusterContainerName",
		  m_inputClusterContainerName = "EMTopoCluster430",
		  "Name of input cluster container");

  declareProperty("KeepAllClusters",
		  m_recordFullClusters = false,
		  "Debugging setting to keep all clusters in a container");

  declareProperty("WindowDelEtaCells", m_delEtaCells = 3);
  m_delEta *= float(m_delEtaCells);

  declareProperty("WindowDelPhiCells", m_delPhiCells = 5);
  m_delPhi *= float(m_delPhiCells);

  declareProperty("DefaultCaloExtentionTool", m_defaultParticleCaloExtensionTool);
  
  declareProperty("UseBremFinder", m_useBremFinder=false);

  //Used for layer calculations.
  m_caloCellDetPos = new CaloCellDetPos();

  // Declare interface & properties.
  declareInterface<IegammaSuperClusterBuilder>(this);

}

//Destructor.
egammaSuperClusterBuilder::~egammaSuperClusterBuilder() {}

StatusCode egammaSuperClusterBuilder::initialize() {
  ATH_MSG_INFO(" Initializing egammaSuperClusterBuilder");

  if (service("StoreGateSvc", m_storeGate).isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
    return StatusCode::FAILURE;
  }

  if ( m_defaultParticleCaloExtensionTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Cannot retrieve DefaultParticleCaloExtensionTool" << m_defaultParticleCaloExtensionTool );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_INFO("Retrieved DefaultParticleCaloExtensionTool tool " << m_defaultParticleCaloExtensionTool);
  }
  
  ATH_MSG_INFO("Using fixed window reconstruction of size: " << m_delEtaCells << " x " << m_delPhiCells);

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

  //Retrieve input egammaRec container.
  EgammaRecContainer *egammaRecs = 0;
  StatusCode sc=evtStore()->retrieve(egammaRecs, "TopoTrackClusterMatches" );
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve TopoTrackClusterMatches!");
    return StatusCode::FAILURE;
  }

  //Retrieve input clusters.
  const xAOD::CaloClusterContainer* clusters = 0;
  sc = evtStore()->retrieve(clusters, "EMTopoCluster430");
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("No input EM Cluster container found " << m_inputClusterContainerName);
    return StatusCode::FAILURE;
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //FOR MY OWN USE: Write a container out of ALL topoclusters for later analysis.
  if (m_recordFullClusters) {
    xAOD::CaloClusterContainer *fullClusters = CaloClusterStoreHelper::makeContainer(m_storeGate, 
										     "fullClusters", 
										     msg());
    if (!fullClusters) {
      ATH_MSG_ERROR("Could not make fullClusters container! StoreGate failure ...");
      return StatusCode::FAILURE;
    }

    for (auto fullCl : *clusters)
      fullClusters->push_back(new xAOD::CaloCluster(*fullCl));
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////
  
  //Retrieve calo cell container.
  if( evtStore()->contains<CaloCellContainer>("AllCalo")) {  
    sc = evtStore()->retrieve(m_cellcoll, "AllCalo") ; 
    if(sc.isFailure() || !m_cellcoll) {
      ATH_MSG_ERROR("No Calo Cell Container?");
      return StatusCode::FAILURE;
    }
  }


  //Get list of refitted tracks.
  const xAOD::TrackParticleContainer  *refittedTracks = 0;
  sc=evtStore()->retrieve(refittedTracks, "GSFTrackParticles" );
  //sc=evtStore()->retrieve(refittedTracks, "InDetTrackParticles" );
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve GSFTrackParticles!");
    return StatusCode::FAILURE;
  }
  
  //Clear out old list of used clusters.
  usedClusters.clear();

  //Have to register cluster container in order to properly get cluster links.
  xAOD::CaloClusterContainer *outputClusterContainer = CaloClusterStoreHelper::makeContainer(m_storeGate, 
  											     "egammaClusters", 
  											     msg());
  if (!outputClusterContainer) {
    ATH_MSG_ERROR("Could not make supercluster container! StoreGate failure ...");
    return StatusCode::FAILURE;
  }
  
  EgammaRecContainer *newEgammaRecs = new EgammaRecContainer();
  if (evtStore()->record(newEgammaRecs, "egammaSuperRecCollection").isFailure()) {
    ATH_MSG_ERROR("Could not record egammaRecContainer");
    return StatusCode::FAILURE;
  }
  
  //Loop over input egammaRec objects, build superclusters.
  for (auto egRec : *egammaRecs) {
    
    // if (ClusterIsUsed(egRec->caloCluster())) {
    //   ATH_MSG_INFO("Cluster already used - skip it!");
    //   continue;
    // } else {
    //   ATH_MSG_INFO("Cluster hasn't been used yet!");
    // }
    
    //If the cluster is above the Pt threshold, get nearby secondary EM clusters.
    secondaryTracks.clear();

    //First add the good tracks.
    for (unsigned int i = 0; i < egRec->getNumberOfTrackParticles(); i++)
      secondaryTracks.insert(secondaryTracks.end(), egRec->trackParticle(i));

    //Search for secondary clusters.
    secondaryClusters.clear();
    usedClusters.push_back(egRec->caloCluster());
    secondaryClusters = SearchForSecondaryClusters(egRec->caloCluster(),
						   clusters,
						   egammaRecs);
    
    //ATH_MSG_INFO("Size of secondary clusters container: " << int(secondaryClusters.size()));

    //Merge tracks together for clusters matched to tracks.
    std::vector<const xAOD::CaloCluster*>::iterator secIter = secondaryClusters.begin();

    //ATH_MSG_INFO("Looking for matching egammaRec");
    for (; secIter != secondaryClusters.end(); secIter++) {

      EgammaRecContainer::const_iterator     loc = std::find_if(egammaRecs->begin(), egammaRecs->end(), ClusCheck((*secIter)));
      
      //First check to see if a topocluster is track-matched.
      //If so, keep the tracks.
      if (loc != egammaRecs->end()) {
	for (unsigned int i = 0; i < (*loc)->getNumberOfTrackParticles(); i++)
	  secondaryTracks.insert(secondaryTracks.end(), (*loc)->trackParticle(i));
      }

    }
    //ATH_MSG_INFO("Went through matching egammaRecs");

    //Take the list of secondaries and make a super cluster.
    //secondaryClusters.push_back((*iter).first);
    if (secondaryTracks.size()>0) {

      ATH_MSG_INFO("Creating supercluster egammaRec object");

      secondaryClusters.insert(secondaryClusters.begin(), egRec->caloCluster());

      std::vector< ElementLink<xAOD::TrackParticleContainer> > elTracks;
      for (unsigned int i = 0; i < secondaryTracks.size(); i++)
	elTracks.push_back(GetTrackParticleLink(refittedTracks,secondaryTracks[i]));

      xAOD::CaloCluster *newClus = AddTopoClusters(secondaryClusters);

      //Fill some relevant calibration variables.
      if (newClus) {

	ATH_MSG_INFO("Filling E, eta, phi vars ...");

	newClus->setCalE   (newClus->calE());
	newClus->setCalEta (newClus->calEta());
	newClus->setCalPhi (newClus->calPhi());

	newClus->setAltE   (newClus->calE());
	newClus->setAltEta (newClus->calEta());
	newClus->setAltPhi (newClus->calPhi());

	newClus->setRawE   (newClus->calE());
	newClus->setRawEta (newClus->calEta());
	newClus->setRawPhi (newClus->calPhi());

	fillPositionsInCalo(newClus);

	ATH_MSG_INFO("Done filling E, eta, phi vars");

	//ATH_MSG_INFO("Filled positions in calo");

	static SG::AuxElement::Decorator<int> nClusters ("nClusters");
	nClusters(*newClus) = secondaryClusters.size();

	static SG::AuxElement::Decorator<int> allClusters("allClusters");
	allClusters(*newClus) = clusters->size();

	static SG::AuxElement::Decorator<int> seedIndex("seedIndex");
	static SG::AuxElement::Decorator<float> seedE  ("seedE");
	static SG::AuxElement::Decorator<float> seedPt ("seedPt");

	seedIndex(*newClus) = secondaryClusters[0]->index();
	seedE(*newClus)     = secondaryClusters[0]->e();
	seedPt(*newClus)    = secondaryClusters[0]->pt();

	//Push it back into the output container.
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

  if (!cluster) {
    ATH_MSG_INFO("Input cluster is null? Returning an empty vector ...");
    return secondaryClusters;
  }

  if (!electrons) {
    ATH_MSG_INFO("egammaRec container is null? Returning an empty vector ...");
    return secondaryClusters;
  }

  xAOD::CaloClusterContainer::const_iterator iter    = clusters->begin();
  xAOD::CaloClusterContainer::const_iterator iterEnd = clusters->end();

  const xAOD::CaloCluster *clus = 0;
  int nClusters(0);

  //For brem extrapolation-based cluster finding.
  GetBremExtrapolations(cluster,clusters,electrons);

  //First get all clusters in the fixed window.
  for (; iter != iterEnd; iter++) {

    clus = (*iter);
   
    //Ignore cluster if it's already been used.
    if (ClusterIsUsed(clus)) continue;

    //Basic cuts on energy.
    if (clus->e() < 400.) continue;
    if (clus->e() > cluster->e())
      continue;

    //Check if clusters are nearby enough to form the topo-seeded window.
    bool matchesInWindow(MatchesInWindow(cluster,clus));

    //Now go through to see if any match the brem point extrapolations,
    //(if turned on), or if we have any very nearby clusters
    //matching the same track.
    bool overlapsBremPoint(OverlapsABremPoint(clus));
    
    //Lastly, check if we have any nearby electrons matched to the same track,
    //within e.g. delPhi < 0.1, delEta < 0.05.
    bool matchSameTrack (ClustersMatchSameTrack(cluster,clus,electrons));

    //Add it to the list of secondary clusters if it matches.
    if (matchesInWindow || overlapsBremPoint || matchSameTrack) {
      secondaryClusters.push_back(clus); 
      usedClusters.push_back(clus);
      nClusters++;
    }
    
  }
    
  return secondaryClusters;
  
}

xAOD::CaloCluster *egammaSuperClusterBuilder::AddTopoClusters(const std::vector <const xAOD::CaloCluster*> &clusters)
{

  if( clusters.size()  ==  0 )  return 0;

  xAOD::CaloCluster  deref_clus(*(clusters[0]));
  xAOD::CaloCluster* myCluster = new xAOD::CaloCluster(deref_clus);
  //ATH_MSG_INFO("Made initial cluster with energy " << myCluster->e());

  //Change size setting to 3x5.
  myCluster->setClusterSize(xAOD::CaloCluster::SW_35ele);
  
  if (clusters.size()==1)
    return myCluster;

  //ATH_MSG_INFO( "Building cluster from "<< clusters.size() <<  " topoclusters");

  std::vector<const xAOD::CaloCluster*>::const_iterator  matchedClusters    = clusters.begin();
  std::vector<const xAOD::CaloCluster*>::const_iterator  matchedClustersEnd = clusters.end();
  matchedClusters++;

  int nClusters(-1);
  for (; matchedClusters!=matchedClustersEnd; matchedClusters++) {
    nClusters++;

    //const xAOD::CaloCluster *tmpCluster = *matchedClusters;
    const xAOD::CaloCluster *tmpCluster = *matchedClusters;

    //ATH_MSG_INFO("Adding cells from cluster with energy " << tmpCluster->e());

    xAOD::CaloCluster::const_cell_iterator cell_itr = tmpCluster->begin();
    xAOD::CaloCluster::const_cell_iterator cell_end = tmpCluster->end();

    for (; cell_itr != cell_end; cell_itr++) {
      myCluster->addCell(cell_itr.index(), cell_itr.weight());
    }
    
  }
  
  //Update cluster kinematics.
  //ATH_MSG_INFO("Added cells, recalculating kinematics");
  CaloClusterKineHelper::calculateKine(myCluster,true,true);

  //ATH_MSG_INFO("Made supercluster with energy " << myCluster->e());

  return myCluster;

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
      //ATH_MSG_INFO("Got track element link!");
      ElementLink< xAOD::TrackParticleContainer > trackLink( *tracks, nTrk );
      return trackLink;
    }
  }

  ElementLink< xAOD::TrackParticleContainer > trackLink;
  //ATH_MSG_INFO("TRACK ELEMENT LINK IS NULL!");
  return trackLink;
  
}

bool egammaSuperClusterBuilder::MatchesInWindow(const xAOD::CaloCluster *ref,
						const xAOD::CaloCluster *clus)
{



  //Get the differences in 2nd layer eta, phi of the
  //satellite and seed for convenience.
  //float dEta(fabs(ref->etaBE(2)-clus->etaBE(2))), dPhi(fabs(m_phiHelper.diff(ref->phiBE(2), clus->phiBE(2))));

  float dEta(fabs(ref->eta()-clus->eta())), dPhi(fabs(m_phiHelper.diff(ref->phi(), clus->phi())));

  //ATH_MSG_INFO("MatchesInWindow: dEta: " << dEta << ", dPhi: " << dPhi);

  if (dEta < (m_delEta/2) && dPhi < (m_delPhi/2))
    return true;

  return false;
}

void egammaSuperClusterBuilder::GetBremExtrapolations(const xAOD::CaloCluster          *cluster,
						      const xAOD::CaloClusterContainer *clusters,
						      EgammaRecContainer               *electrons)
{

  //Some protections.
  // if (!m_useBremFinder) {
  //   ATH_MSG_INFO("Brem finder has been turned off! Exiting ...");
  //   return;
  // }

  if (!m_tpAndTSOS) {
    //ATH_MSG_INFO("Map of tracks to brem points is null! Exiting ...");
    return;
  }

  //ATH_MSG_INFO("Running GetBremExtrapolations ...");
  
  xAOD::CaloClusterContainer::const_iterator iter    = clusters->begin();
  xAOD::CaloClusterContainer::const_iterator iterEnd = clusters->end();
  
  //Some variables to store % energy retained, plus extrapolated eta, phi.
  bpRetainedEnFrac.clear();
  bpExtrapEta.clear();
  bpExtrapPhi.clear();
  
  for (; iter != iterEnd; iter++) {

    //Start using m_tpAndTSOS here to search for track kinks.
    //Then use 'getHackEtaPhiAtCalo' (?) to find impact point on calorimeter.
    //Want to search around this region for secondaries, i.e. if it's within
    //a valid range in (dEta, dPhi).
    
    EgammaRecContainer::iterator seedElIter = std::find_if(electrons->begin(), electrons->end(), ClusCheck(cluster));
    if (seedElIter != electrons->end()) {

      //ATH_MSG_INFO("Got seed electron iterator");
      
      //Get track particle.
      const xAOD::TrackParticle *elTrack = (*seedElIter)->trackParticle();

      if (!elTrack) {
	//ATH_MSG_INFO("No track particle associated to the egammaRec");
	return;
      } else {
	//ATH_MSG_INFO("Perigee eta, phi of seed track: (" << elTrack->eta() << ", " << elTrack->phi() << ")");
      }

      //Get corresponding entry from m_tpAndTSOS.
      std::vector<const Trk::TrackStateOnSurface*> bremPts = (*m_tpAndTSOS)[elTrack];

      for (unsigned int ib(0); ib < bremPts.size(); ib++) {

	const Trk::EstimatedBremOnTrack *estimatedBremOnTrack(0);
	const Trk::TrackParameters      *bpTrackParams(0);

	if (!bremPts[ib]) {
	  ATH_MSG_ERROR("Brem point vector has a null element???");
	  continue;
	}
	
	if (bremPts[ib]->materialEffectsOnTrack()) {
	  //ATH_MSG_INFO("Able to get material effects");
	  estimatedBremOnTrack = (Trk::EstimatedBremOnTrack*)(bremPts[ib]->materialEffectsOnTrack());
	} else {
	  //ATH_MSG_INFO("No material effects???");
	}
	
	if (estimatedBremOnTrack && bremPts[ib]->trackParameters() != 0) {

	  //ATH_MSG_INFO("Estimated retained energy fraction: " << estimatedBremOnTrack->retainedEnFraction());
	  bpTrackParams = bremPts[ib]->trackParameters();

	  //ATH_MSG_INFO("Track eta, phi at surface: (" << bpTrackParams->eta() << ", " << bpTrackParams->parameters()[Trk::phi0] << ")");
	  
	  // Hack to create high Pt track parameters.
	  // const Trk::Surface &surface        = bpTrackParams->associatedSurface();
	  // const Trk::TrackParameters* trkPar = surface.createTrackParameters( bpTrackParams->position(), bpTrackParams->momentum().unit()*1.e9, +1, 0);

	  //ATH_MSG_INFO("New surface eta, phi: (" << trkPar->eta() << ", " << trkPar->parameters()[Trk::phi0] << ")");
	  
	  const Trk::PerigeeSurface*  pSurface = new Trk::PerigeeSurface(bpTrackParams->position());
	  const Trk::TrackParameters* pTrkPar  = pSurface->createTrackParameters( bpTrackParams->position(), bpTrackParams->momentum().unit()*1.e9, +1, 0);

	  //ATH_MSG_INFO("Perigee surface track eta, phi: (" << pTrkPar->eta() << ", " << pTrkPar->parameters()[Trk::phi0] << ")");

	  //Should see here the difference between a perigee surface & actual surface.
	  
	  float extrapEta(-999.), extrapPhi(-999.);

	  ///////////////////////////////////////////////////////////////
	  // *** NEED TO TURN THIS LINE BACK ON!!! ***
	  ///////////////////////////////////////////////////////////////
	  
	  bool hitEM2(getHackEtaPhiAtCalo(pTrkPar,&extrapEta,&extrapPhi));

	  if (hitEM2) {
	    //ATH_MSG_INFO("Extrap. eta, phi from brem surface: (" << extrapEta << ", " << extrapPhi << ")");
	  } else {
	    //ATH_MSG_INFO("Extrapolation to EM2 failed!");
	  }
	  
	  //Save info on % brem loss and extrapolated parameters.
	  bpRetainedEnFrac.push_back(estimatedBremOnTrack->retainedEnFraction());

	  if (hitEM2) {
	    bpExtrapEta     .push_back(extrapEta);
	    bpExtrapPhi     .push_back(extrapPhi);
	  } else {
	    bpExtrapEta     .push_back(-999.);
	    bpExtrapPhi     .push_back(-999.);
	  }
	  
	} else {
	  //ATH_MSG_INFO("Cast to Trk::EstimatedBremOnTrack failed?");
	}
      }
      break;
    }
  }

  

}

bool egammaSuperClusterBuilder::OverlapsABremPoint(const xAOD::CaloCluster *clus)
{

  // if (bpExtrapEta.size()>0)
  //   ATH_MSG_INFO("Running brem point search over " << bpExtrapEta.size() << " points");

  bool  overlapsABremPoint(false);
  //float retainedEnFrac(0.);

  //A better way to do this would probably be to get the overlapping cluster
  //with minimum eta, phi.
  for (unsigned int ib(0); ib < bpExtrapEta.size(); ib++) {

    if (bpExtrapEta[ib] == -999. || bpExtrapPhi[ib] == -999.) {
      //ATH_MSG_INFO("Eta, phi of extrap. point not found, skipping this one ...");
      continue;
    }
      
    bool matchEta(false); //fabs(bpExtrapEta[ib] - clus->etaBE(2)) < 0.1);
    bool matchPhi(false); //fabs(m_phiHelper.diff(bpExtrapPhi[ib], clus->phiBE(2))) < 0.15);

    if (clus->etaBE(2) > -100. && clus->phiBE(2) > -100.) {
      matchEta = fabs(bpExtrapEta[ib] - clus->etaBE(2)) < 0.1;
      matchPhi = fabs(m_phiHelper.diff(bpExtrapPhi[ib], clus->phiBE(2))) < 0.15;
    } else {
      matchEta = fabs(bpExtrapEta[ib] - clus->eta()) < 0.1;
      matchPhi = fabs(m_phiHelper.diff(bpExtrapPhi[ib], clus->phi())) < 0.15;
    }
    
    overlapsABremPoint = matchEta && matchPhi;

    if (overlapsABremPoint) {

      // retainedEnFrac = bpRetainedEnFrac[ib];
	
      ATH_MSG_INFO("Extrapolated brem point overlaps a potential secondary!");
      ATH_MSG_INFO("Extrap. eta: " << bpExtrapEta[ib] << ", cluster eta:" << clus->etaBE(2));
      ATH_MSG_INFO("Extrap. phi: " << bpExtrapPhi[ib] << ", cluster phi:" << clus->phiBE(2));
      ATH_MSG_INFO("dEta: " << fabs(bpExtrapEta[ib]-clus->etaBE(2)) << ", dPhi: " << fabs(m_phiHelper.diff(bpExtrapPhi[ib], clus->phiBE(2))));

      //Should remove the brem point so we don't repeatedly use it.
      bpExtrapEta     .erase(bpExtrapEta.begin()+ib);
      bpExtrapPhi     .erase(bpExtrapPhi.begin()+ib);
      bpRetainedEnFrac.erase(bpRetainedEnFrac.begin()+ib);
	
      break;
    }else {

      //ATH_MSG_INFO("Extrapolated brem point missed a potential secondary");
      //ATH_MSG_INFO("Extrap. eta: " << bpExtrapEta[ib] << ", cluster eta:" << clus->etaBE(2));
      //ATH_MSG_INFO("Extrap. phi: " << bpExtrapPhi[ib] << ", cluster phi:" << clus->phiBE(2));
      //ATH_MSG_INFO("dEta: " << fabs(bpExtrapEta[ib]-clus->etaBE(2)) << ", dPhi: " << fabs(m_phiHelper.diff(bpExtrapPhi[ib], clus->phiBE(2))));

    }
  }
    
  return overlapsABremPoint;
      
}

bool egammaSuperClusterBuilder::getHackEtaPhiAtCalo (const Trk::TrackParameters* trkPar, 
						     float *etaAtCalo,
						     float *phiAtCalo) const

{

  CaloExtensionHelpers::LayersToSelect layersToSelect;    
  if ( fabs(trkPar->eta()) < 1.425) {
    // Barrel
    layersToSelect.insert(CaloSampling::EMB2 );  
  } else {
    // Endcap
    layersToSelect.insert(CaloSampling::EME2 );  
  }
  
  const Trk::CaloExtension* extension = 0;      
  extension = m_defaultParticleCaloExtensionTool->caloExtension( *trkPar, Trk::alongMomentum, Trk::muon );

  if(!extension){
    ATH_MSG_WARNING("Could not create an extension from getHackEtaPhiAtCalo ");
    return false;
  } 

  CaloExtensionHelpers::EtaPhiPerLayerVector intersections;
  CaloExtensionHelpers::midPointEtaPhiPerLayerVector( *extension, intersections, &layersToSelect );
  
  bool hitEM2(false);
  for( const auto& p : intersections ){    
    int  i(0); 
    auto sample = std::get<0>(p);
    if ( ( sample == CaloSampling::EME2 || sample == CaloSampling::EMB2 )){
      *etaAtCalo = std::get<1>(p); 
      *phiAtCalo = std::get<2>(p); 
      hitEM2=true;
      ++i;
      ATH_MSG_INFO("getMatchAtCalo: i, eta, phi : "  
		    << i << " " << std::get<1>(p) << " " << std::get<2>(p) ); 
      
    }
  }

  return hitEM2;   

}

bool egammaSuperClusterBuilder::ClustersMatchSameTrack(const xAOD::CaloCluster *seed,
						       const xAOD::CaloCluster *sec,
						       EgammaRecContainer      *electrons)
{

  bool matchesSameTrack(false);

  //First check that the clusters overlap sufficiently in dEta and dPhi.
  float dEta(fabs(seed->eta() - sec->eta())), dPhi(fabs(m_phiHelper.diff(seed->phi(), sec->phi())));

  if (dEta > 0.075 || dPhi > 0.15)
    return false;

  //ATH_MSG_INFO("Eta, phi check is OK");
  
  //See if we have "electrons" corresponding both to the seed and secondary.
  EgammaRecContainer::iterator seedElIter = std::find_if(electrons->begin(), electrons->end(), ClusCheck(seed));
  EgammaRecContainer::iterator secElIter  = std::find_if(electrons->begin(), electrons->end(), ClusCheck(sec));

  if (seedElIter != electrons->end() && secElIter != electrons->end()) {

    //ATH_MSG_INFO("Got track matches for each cluster");
      
    //Get track particle for each.
    const xAOD::TrackParticle *seedTrack = (*seedElIter)->trackParticle();
    const xAOD::TrackParticle *secTrack  = (*secElIter) ->trackParticle();

    if (seedTrack && secTrack) {

      //Check that the tracks are the same.
      if (fabs(seedTrack->pt() - secTrack->pt()) < 0.001) {
	ATH_MSG_INFO(Form("Track Pts match! Clusters matched to same track."));
	matchesSameTrack = true;
      } else {
	ATH_MSG_INFO(Form("Track Pts don't match: %f vs %f",seedTrack->pt(),secTrack->pt()));
      }
    }

  }

  if (matchesSameTrack)
    ATH_MSG_INFO("Clusters match the same track!");

  // else
  //   ATH_MSG_INFO("Clusters don't match the same track!");

  return matchesSameTrack;

}

