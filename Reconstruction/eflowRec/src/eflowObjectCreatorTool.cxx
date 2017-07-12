/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowObjectCreatorTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson (based on eflowObjectBuilder class written by D.Tovey)
CREATED:  15th August, 2005

********************************************************************/

#include "eflowRec/eflowObjectCreatorTool.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowCaloObject.h"

#include "FourMomUtils/P4DescendingSorters.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODPFlow/versions/PFOAttributesAccessor_v1.h"

#include <vector>
#include <iostream>
using namespace std;

namespace EFlow_Fn{ 
  // comparison,  order by Et

  class gtpt_eflow{
  public:
    inline gtpt_eflow() {};
    inline bool operator () (const xAOD::PFO& pf1, const xAOD::PFO& pf2){
      return (pf1.e()/cosh(pf1.eta())) > (pf2.e()/cosh(pf2.eta()));
    }
  };

}

eflowObjectCreatorTool::eflowObjectCreatorTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_PFOOutputName("JetETMiss"),
    m_chargedPFOContainer(nullptr),
    m_neutralPFOContainer(nullptr),
    m_neutralPFOContainer_nonModified(nullptr),
    m_eOverPMode(false),
    m_goldenModeString(""),
    m_LCMode(false),
    m_trackVertexAssociationTool(""),
    m_vertexContainerName("PrimaryVertices"),
    m_useAODReductionMomentList(false)
{
  declareInterface<eflowObjectCreatorTool>(this);
  /* Name of  eflow Container to be created */
  declareProperty("PFOOutputName",m_PFOOutputName);
  declareProperty("EOverPMode", m_eOverPMode);
  declareProperty("goldenModeString",m_goldenModeString,"run in golden match mode only?");
  declareProperty("LCMode", m_LCMode, "Whether we are in LC or EM mode");
  declareProperty("TrackVertexAssociationTool", m_trackVertexAssociationTool);
  declareProperty("UseAODReductionMomentList",m_useAODReductionMomentList);
}

StatusCode eflowObjectCreatorTool::initialize(){

  /* tool service */
  IToolSvc* myToolSvc;
  if ( service("ToolSvc",myToolSvc).isFailure() ) {
    msg(MSG::WARNING) << " Tool Service Not Found" << endmsg;
    return StatusCode::SUCCESS;
  }

  if(! m_trackVertexAssociationTool.empty() ) {
     ATH_MSG_VERBOSE("Intialized using ITrackVertexAssociationTool");
     return m_trackVertexAssociationTool.retrieve();
  }

  return StatusCode::SUCCESS;
}

StatusCode eflowObjectCreatorTool::execute(eflowCaloObject *energyFlowCaloObject){

  msg(MSG::DEBUG) << "Executing eflowObjectCreatorTool " << endmsg;

  if (m_eOverPMode){
    /* Loop over charged efos (in practice nClusters currently is either 0 or 1) */
    createChargedEflowObjects(energyFlowCaloObject, true);
    createNeutralEflowObjects(energyFlowCaloObject);
  } else {
    createChargedEflowObjects(energyFlowCaloObject);
    createNeutralEflowObjects(energyFlowCaloObject);
  }

  const xAOD::VertexContainer* vertexContainer = nullptr;
  StatusCode vertexRetrieved =  evtStore()->retrieve(vertexContainer,m_vertexContainerName);
  if ( vertexRetrieved.isFailure() || vertexContainer==nullptr) {
    ATH_MSG_WARNING("Could not retrieve the VertexContainer from evtStore: " << m_vertexContainerName);
  }
  else{
    addVertexLinksToChargedEflowObjects(vertexContainer);
  }
  
  return StatusCode::SUCCESS;

}

void eflowObjectCreatorTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer) {
  msg(MSG::DEBUG) << "Running eflowObjectCreatorTool on the eflowCaloObjectContainer " << endmsg;

  /* Create PFO containers and register them */
  if( setupPFOContainers().isFailure() ){
    msg(MSG::WARNING) << "Couldn't setup PFO containers. " << endmsg;
    return;
  }

  /* Create PFOs from all eflowCaloObjects */
  unsigned int nEFCaloObs = theEflowCaloObjectContainer->size();
  for (unsigned int iEFCalOb = 0; iEFCalOb < nEFCaloObs; ++iEFCalOb){
    eflowCaloObject* thisEflowCaloObject = theEflowCaloObjectContainer->at(iEFCalOb);
    if (execute(thisEflowCaloObject).isFailure()){
      msg(MSG::WARNING) << "Execute failed for one eflowCaloObject. " << endmsg;
      continue;
    }
  }

  /* Sort clusters and efos by pt TODO: this should be done in eflowObjectCreatorTool */
  //Do we need to sort the objects at all?
  //std::sort(theEflowObjectContainer->begin(),theEflowObjectContainer->end(),EFlow_Fn::gtpt_PFO());
}

StatusCode eflowObjectCreatorTool::finalize(){ return StatusCode::SUCCESS; }

StatusCode eflowObjectCreatorTool::setupPFOContainers() {
  m_chargedPFOContainer = new xAOD::PFOContainer();
  
  xAOD::PFOAuxContainer* chargedPFOAuxContainer = new xAOD::PFOAuxContainer();
  
  if (evtStore()->contains<xAOD::PFOContainer>(m_PFOOutputName + "ChargedParticleFlowObjects")){
    CHECK( evtStore()->overwrite( m_chargedPFOContainer, m_PFOOutputName + "ChargedParticleFlowObjects"  ) );
    CHECK( evtStore()->overwrite( chargedPFOAuxContainer, m_PFOOutputName + "ChargedParticleFlowObjectsAux." ) );
  }
  else{
    CHECK( evtStore()->record( m_chargedPFOContainer, m_PFOOutputName + "ChargedParticleFlowObjects"  ) );
    CHECK( evtStore()->record( chargedPFOAuxContainer, m_PFOOutputName + "ChargedParticleFlowObjectsAux." ) );
  }
  m_chargedPFOContainer->setStore(chargedPFOAuxContainer);
  
  m_neutralPFOContainer = new xAOD::PFOContainer();
  xAOD::PFOAuxContainer* neutralPFOAuxContainer = new xAOD::PFOAuxContainer();
  
  if (evtStore()->contains<xAOD::PFOContainer>(m_PFOOutputName + "NeutralParticleFlowObjects")){
    CHECK( evtStore()->overwrite( m_neutralPFOContainer, m_PFOOutputName + "NeutralParticleFlowObjects"  ) );
    CHECK( evtStore()->overwrite( neutralPFOAuxContainer, m_PFOOutputName + "NeutralParticleFlowObjectsAux." ) );
  }
  else{
    CHECK( evtStore()->record( m_neutralPFOContainer,  m_PFOOutputName + "NeutralParticleFlowObjects"  ) );
    CHECK( evtStore()->record( neutralPFOAuxContainer,  m_PFOOutputName + "NeutralParticleFlowObjectsAux." ) );
  }
  m_neutralPFOContainer->setStore(neutralPFOAuxContainer);
  
  if (true == m_LCMode){
    m_neutralPFOContainer_nonModified = new xAOD::PFOContainer();
    xAOD::PFOAuxContainer* neutralPFOAuxContainer_nonModified = new xAOD::PFOAuxContainer();
    if (evtStore()->contains<xAOD::PFOContainer>(m_PFOOutputName + "NonModifiedNeutralParticleFlowObjects"  )){
      CHECK( evtStore()->overwrite( m_neutralPFOContainer_nonModified, m_PFOOutputName + "NonModifiedNeutralPFO"   ) );
      CHECK( evtStore()->overwrite( neutralPFOAuxContainer_nonModified, m_PFOOutputName + "NonModifiedNeutralParticleFlowObjectsAux." ) );
    }
    else{
      CHECK( evtStore()->record( m_neutralPFOContainer_nonModified, m_PFOOutputName + "NonModifiedNeutralParticleFlowObjects"  ) );
      CHECK( evtStore()->record( neutralPFOAuxContainer_nonModified, m_PFOOutputName + "NonModifiedNeutralParticleFlowObjectsAux." ) );
    }
    m_neutralPFOContainer_nonModified->setStore(neutralPFOAuxContainer_nonModified);
  }
  return StatusCode::SUCCESS;
}

void eflowObjectCreatorTool::addVertexLinksToChargedEflowObjects(const xAOD::VertexContainer* theVertexContainer){

  if (theVertexContainer){
    //This is a loop on all xAOD::PFO with non-zero charge
    for (auto theChargedPFO : *m_chargedPFOContainer){
      const xAOD::TrackParticle* theTrack = theChargedPFO->track(0);
      if (theTrack){
	ElementLink< xAOD::VertexContainer> theVertexLink = m_trackVertexAssociationTool->getUniqueMatchVertexLink(*theTrack,*theVertexContainer);
  	bool haveSetLink = theChargedPFO->setVertexLink(theVertexLink);
	if (!haveSetLink) msg(MSG::WARNING) << " Could not set vertex link on charged PFO " << endmsg;
      }//if valid pointer to xAOD::TrackParticle
    }
  }

}

void eflowObjectCreatorTool::createChargedEflowObjects(eflowCaloObject* energyFlowCaloObject, bool addClusters){
 
  /* Loop over all tracks in the eflowCaloObject */
  int nTracks = energyFlowCaloObject->nTracks();
  for (int iTrack = 0; iTrack < nTracks; ++iTrack) {

    eflowRecTrack* efRecTrack = energyFlowCaloObject->efRecTrack(iTrack);

    /* Skip tracks that haven't been subtracted */
    if (false == m_eOverPMode){
      if (!efRecTrack->isSubtracted()){ continue; }
    }
    
    /* Create new xAOD::PFO */
    xAOD::PFO* myEflowObject = new xAOD::PFO();
    m_chargedPFOContainer->push_back(myEflowObject);

    /* Get the track elementLink and add it to the xAOD:PFO  */
    ElementLink<xAOD::TrackParticleContainer> theTrackLink = efRecTrack->getTrackElemLink();
    bool isSet = myEflowObject->setTrackLink(theTrackLink);
    if (!isSet) { msg(MSG::WARNING) << "Could not set Track B in PFO " << endmsg; }
    myEflowObject->setCharge(efRecTrack->getTrack()->charge());

    std::pair<double,double> etaPhi(0.0,0.0);

    if (m_eOverPMode){
      /* In EOverPMode want charged eflowObjects to have extrapolated eta,phi as coordinates
       * (needed for analysis of EOverP Data) */
      etaPhi = efRecTrack->getTrackCaloPoints().getEM2etaPhi();
        
      /*add information to xAOD*/
      xAOD::PFODetails::PFOAttributes myAttribute_layerHED = xAOD::PFODetails::PFOAttributes::eflowRec_layerHED;
      myEflowObject->setAttribute<int>(myAttribute_layerHED,efRecTrack->getLayerHED() );
      
      xAOD::PFODetails::PFOAttributes myAttribute_layerVectorCellOrdering = xAOD::PFODetails::PFOAttributes::eflowRec_layerVectorCellOrdering;
      myEflowObject->setAttribute<std::vector<int> >(myAttribute_layerVectorCellOrdering,efRecTrack->getLayerCellOrderVector() );
      
      xAOD::PFODetails::PFOAttributes myAttribute_radiusVectorCellOrdering = xAOD::PFODetails::PFOAttributes::eflowRec_radiusVectorCellOrdering;
      myEflowObject->setAttribute<std::vector<float> >(myAttribute_radiusVectorCellOrdering,efRecTrack->getRadiusCellOrderVector() );
        
      xAOD::PFODetails::PFOAttributes myAttribute_avgEdensityVectorCellOrdering = xAOD::PFODetails::PFOAttributes::eflowRec_avgEdensityVectorCellOrdering;
      myEflowObject->setAttribute<std::vector<float> >(myAttribute_avgEdensityVectorCellOrdering,efRecTrack->getAvgEDensityCellOrderVector() );              
    } else {
      /* In normal mode we want te track eta,phi at the perigee */
      etaPhi.first = efRecTrack->getTrack()->eta();
      etaPhi.second = efRecTrack->getTrack()->phi();
    }
    /* Set the 4-vector of the xAOD::PFO */
    myEflowObject->setP4(efRecTrack->getTrack()->pt(), etaPhi.first, etaPhi.second, efRecTrack->getTrack()->m());

    /* Add the amount of energy the track was expected to deposit in the calorimeter - this is needed to calculate the charged weight in the jet finding */
    xAOD::PFODetails::PFOAttributes myAttribute_tracksExpectedEnergyDeposit = xAOD::PFODetails::PFOAttributes::eflowRec_tracksExpectedEnergyDeposit;
    myEflowObject->setAttribute<float>(myAttribute_tracksExpectedEnergyDeposit,efRecTrack->getEExpect() );
    
    /* Flag if this track was in a dense environment for later checking */
    xAOD::PFODetails::PFOAttributes myAttribute_isInDenseEnvironment = xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment;
    //There is some issue using bools - when written to disk they convert to chars. So lets store the bool as an int.
    myEflowObject->setAttribute<int>(myAttribute_isInDenseEnvironment, (efRecTrack->isInDenseEnvironment()));

    /* Optionally we add the links to clusters to the xAOD::PFO */
    if (true == addClusters){
       unsigned int nClusters = energyFlowCaloObject->nClusters();
       for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster){
	 eflowRecCluster* thisEfRecCluster = energyFlowCaloObject->efRecCluster(iCluster);
	 ElementLink<xAOD::CaloClusterContainer> theClusLink = thisEfRecCluster->getClusElementLink();
	 bool isSet = myEflowObject->addClusterLink(theClusLink);
	 if (!isSet) msg(MSG::WARNING) << "Could not set Cluster in PFO " << endmsg;
       }//cluster loop
    }//addClusters is set to true - so we added the clusters to the xAOD::PFO   

  }//loop over the tracks on the 

}

void eflowObjectCreatorTool::createNeutralEflowObjects(eflowCaloObject* energyFlowCaloObject) {
  unsigned int nClusters = energyFlowCaloObject->nClusters();
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster){
    eflowRecCluster* thisEfRecCluster = energyFlowCaloObject->efRecCluster(iCluster);

    /* Skip empty clusters (presumably subtraction remnants) */
    const CaloClusterCellLink* theCellLink = energyFlowCaloObject->efRecCluster(iCluster)->getCluster()->getCellLinks();
    CaloClusterCellLink::const_iterator it=theCellLink->begin();
    CaloClusterCellLink::const_iterator it_e=theCellLink->end();
    if (it == it_e) {
      continue;
    }
    //this vetoes rare examples where only two cells are left, and they have equal and opposite energy
    if (0.0 == energyFlowCaloObject->efRecCluster(iCluster)->getCluster()->e() ) continue;

    /* Create the efo, add the cluster and set the four-momentum, charge and type */

    xAOD::PFO* thisEflowObject = new xAOD::PFO();
    if (m_LCMode) {
      if (thisEfRecCluster->isTouchable()) {
        m_neutralPFOContainer->push_back(thisEflowObject);
      } else {
       m_neutralPFOContainer_nonModified->push_back(thisEflowObject);
      }
    } else {
      m_neutralPFOContainer->push_back(thisEflowObject);
    }

    ElementLink<xAOD::CaloClusterContainer> theClusterLink = thisEfRecCluster->getClusElementLink();
    bool isSet = thisEflowObject->setClusterLink(theClusterLink);
    if (!isSet) { msg(MSG::WARNING) << "Could not set Cluster in PFO " << endmsg; }

    const xAOD::CaloCluster* cluster = thisEfRecCluster->getCluster();
    //be careful here - cluster p4 methods do not store sign. Thus -ve energy clusters have +ve pt and hence +ve energy
    if (!m_LCMode) {
      //in EM->EM/LC mode we use eta,phi at EM scale for both 4-vectors
      thisEflowObject->setP4(cluster->pt(), cluster->rawEta(), cluster->rawPhi(), cluster->m());
      thisEflowObject->setP4EM(cluster->rawE()/cosh(cluster->rawEta()), cluster->rawEta(),cluster->rawPhi(),cluster->rawM());
    }
    else{
      //in LC-> mode we use the LC 4-vector for the LC scale
      thisEflowObject->setP4(cluster->pt(), cluster->eta(), cluster->phi(), cluster->m());
      //we cannot access geometric weights for LC clusters, so we make an approximation of the EM energy by looping over the calocells
      //Then the EM 4-vector uses the energy/pt at this EM scale + eta,phi from LC 4-vector
      const CaloClusterCellLink* theCellLink = cluster->getCellLinks();
      auto theFirstCell = theCellLink->begin();
      auto theLastCell = theCellLink->end();
      float emPt = 0.0;
      for (; theFirstCell != theLastCell; ++theFirstCell){
	const CaloCell* thisCell = *theFirstCell;
	emPt += thisCell->e()/cosh(thisCell->eta());
      }
      thisEflowObject->setP4EM(emPt,cluster->eta(),cluster->phi(),0.0);//mass is always zero at EM scale

    }

    thisEflowObject->setCharge(0);

    //now set the moments for touchable clusters (i.e. ones we modify) in LC mode or all clusters in EM mode
    if ( (m_LCMode && thisEfRecCluster->isTouchable()) || !m_LCMode) {
      this->addMoment(xAOD::CaloCluster::SECOND_R,xAOD::PFODetails::PFOAttributes::eflowRec_SECOND_R,cluster, thisEflowObject);
      this->addMoment(xAOD::CaloCluster::CENTER_LAMBDA,xAOD::PFODetails::PFOAttributes::eflowRec_CENTER_LAMBDA,cluster, thisEflowObject);
      this->addMoment(xAOD::CaloCluster::ENG_BAD_CELLS,xAOD::PFODetails::PFOAttributes::eflowRec_ENG_BAD_CELLS,cluster, thisEflowObject);
      this->addMoment(xAOD::CaloCluster::N_BAD_CELLS,xAOD::PFODetails::PFOAttributes::eflowRec_N_BAD_CELLS,cluster, thisEflowObject);
      this->addMoment(xAOD::CaloCluster::BADLARQ_FRAC,xAOD::PFODetails::PFOAttributes::eflowRec_BADLARQ_FRAC,cluster, thisEflowObject);
      this->addMoment(xAOD::CaloCluster::ENG_POS,xAOD::PFODetails::PFOAttributes::eflowRec_ENG_POS,cluster, thisEflowObject);
      this->addMoment(xAOD::CaloCluster::AVG_LAR_Q,xAOD::PFODetails::PFOAttributes::eflowRec_AVG_LAR_Q,cluster, thisEflowObject);
      this->addMoment(xAOD::CaloCluster::AVG_TILE_Q,xAOD::PFODetails::PFOAttributes::eflowRec_AVG_TILE_Q,cluster, thisEflowObject);
      this->addMoment(xAOD::CaloCluster::ISOLATION,xAOD::PFODetails::PFOAttributes::eflowRec_ISOLATION,cluster, thisEflowObject);

      if (false == m_useAODReductionMomentList){
	 this->addMoment(xAOD::CaloCluster::LATERAL,xAOD::PFODetails::PFOAttributes::eflowRec_LATERAL,cluster, thisEflowObject);
	 this->addMoment(xAOD::CaloCluster::LONGITUDINAL,xAOD::PFODetails::PFOAttributes::eflowRec_LONGITUDINAL,cluster, thisEflowObject);
	 this->addMoment(xAOD::CaloCluster::FIRST_ENG_DENS,xAOD::PFODetails::PFOAttributes::eflowRec_FIRST_ENG_DENS,cluster, thisEflowObject);
	 this->addMoment(xAOD::CaloCluster::ENG_FRAC_MAX,xAOD::PFODetails::PFOAttributes::eflowRec_ENG_FRAC_MAX,cluster, thisEflowObject);
	 this->addMoment(xAOD::CaloCluster::SIGNIFICANCE,xAOD::PFODetails::PFOAttributes::eflowRec_SIGNIFICANCE,cluster, thisEflowObject);
      }
      if (true == m_useAODReductionMomentList){
	this->addMoment(xAOD::CaloCluster::SECOND_LAMBDA,xAOD::PFODetails::PFOAttributes::eflowRec_SECOND_LAMBDA,cluster, thisEflowObject);
	this->addMoment(xAOD::CaloCluster::EM_PROBABILITY,xAOD::PFODetails::PFOAttributes::eflowRec_EM_PROBABILITY,cluster, thisEflowObject);
      }
      
    }

    //First set all the layer energies
    float layerEnergy_preSamplerB = cluster->eSample(xAOD::CaloCluster::CaloSample::PreSamplerB);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_PreSamplerB = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_PreSamplerB;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_PreSamplerB, layerEnergy_preSamplerB);

    float layerEnergy_EMB1 = cluster->eSample(xAOD::CaloCluster::CaloSample::EMB1);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EMB1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EMB1;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_EMB1, layerEnergy_EMB1);

    float layerEnergy_EMB2 = cluster->eSample(xAOD::CaloCluster::CaloSample::EMB2);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EMB2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EMB2;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_EMB2, layerEnergy_EMB2);

    float layerEnergy_EMB3 = cluster->eSample(xAOD::CaloCluster::CaloSample::EMB3);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EMB3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EMB3;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_EMB3, layerEnergy_EMB3);

    float layerEnergy_preSamplerE = cluster->eSample(xAOD::CaloCluster::CaloSample::PreSamplerE);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_PreSamplerE = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_PreSamplerE;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_PreSamplerE, layerEnergy_preSamplerE);

    float layerEnergy_EME1 = cluster->eSample(xAOD::CaloCluster::CaloSample::EME1);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EME1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EME1;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_EME1, layerEnergy_EME1);

    float layerEnergy_EME2 = cluster->eSample(xAOD::CaloCluster::CaloSample::EME2);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EME2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EME2;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_EME2, layerEnergy_EME2);

    float layerEnergy_EME3 = cluster->eSample(xAOD::CaloCluster::CaloSample::EME3);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EME3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EME3;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_EME3, layerEnergy_EME3);

    float layerEnergy_HEC0 = cluster->eSample(xAOD::CaloCluster::CaloSample::HEC0);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_HEC0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC0;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_HEC0, layerEnergy_HEC0);

    float layerEnergy_HEC1 = cluster->eSample(xAOD::CaloCluster::CaloSample::HEC1);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_HEC1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC1;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_HEC1, layerEnergy_HEC1);

    float layerEnergy_HEC2 = cluster->eSample(xAOD::CaloCluster::CaloSample::HEC2);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_HEC2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC2;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_HEC2, layerEnergy_HEC2);

    float layerEnergy_HEC3 = cluster->eSample(xAOD::CaloCluster::CaloSample::HEC3);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_HEC3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC3;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_HEC3, layerEnergy_HEC3);

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileBar0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileBar0;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_TileBar0, cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar0));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileBar1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileBar1;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_TileBar1, cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar1));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileBar2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileBar2;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_TileBar2, cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar2));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileGap1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileGap1;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_TileGap1, cluster->eSample(xAOD::CaloCluster::CaloSample::TileGap1));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileGap2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileGap2;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_TileGap2, cluster->eSample(xAOD::CaloCluster::CaloSample::TileGap2));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileGap3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileGap3;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_TileGap3, cluster->eSample(xAOD::CaloCluster::CaloSample::TileGap3));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileExt0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileExt0;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_TileExt0, cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt0));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileExt1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileExt1;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_TileExt1, cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt1));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileExt2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileExt2;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_TileExt2, cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt2));

    float layerEnergy_FCAL0 = cluster->eSample(xAOD::CaloCluster::CaloSample::FCAL0);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_FCAL0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_FCAL0;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_FCAL0, layerEnergy_FCAL0);

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_FCAL1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_FCAL1;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_FCAL1, cluster->eSample(xAOD::CaloCluster::CaloSample::FCAL1));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_FCAL2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_FCAL2;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_FCAL2, cluster->eSample(xAOD::CaloCluster::CaloSample::FCAL2));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_MINIFCAL0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL0;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_MINIFCAL0, cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL0));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_MINIFCAL1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL1;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_MINIFCAL1, cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL1));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_MINIFCAL2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL2;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_MINIFCAL2, cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL2));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_MINIFCAL3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL3;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_MINIFCAL3, cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL3));

    //now set the layer energies for EMB3 and Tile0 - these are needed if we want to run a GSC style jet calibration, which is binned in EMB3 and Tile0 layer energies
    
    float layerEnergy_EM3 = layerEnergy_EMB3 + layerEnergy_EME3;

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EM3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EM3;
    thisEflowObject->setAttribute( myAttribute_layerEnergy_EM3, layerEnergy_EM3);

    float layerEnergy_TileBar0 = cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar0);
    float layerEnergy_TileExt0 = cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt0);
    float layerEnergy_Tile0 = layerEnergy_TileBar0 + layerEnergy_TileExt0;

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_Tile0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_Tile0;
    thisEflowObject->setAttribute(myAttribute_layerEnergy_Tile0, layerEnergy_Tile0);

    //now set properties that are required for jet cleaning
    float layerEnergy_HEC = layerEnergy_HEC0 + layerEnergy_HEC1 + layerEnergy_HEC2 + layerEnergy_HEC3;

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_HEC = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC;
    thisEflowObject->setAttribute(myAttribute_layerEnergy_HEC, layerEnergy_HEC);

    float layerEnergy_EM = layerEnergy_preSamplerB + layerEnergy_preSamplerE + layerEnergy_EMB1 + layerEnergy_EMB2 + layerEnergy_EMB3 + layerEnergy_EME1 + layerEnergy_EME2 + layerEnergy_EME3 + layerEnergy_FCAL0;
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EM = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EM;
    thisEflowObject->setAttribute(myAttribute_layerEnergy_EM, layerEnergy_EM);
    
    float clusterTiming = cluster->time();

    xAOD::PFODetails::PFOAttributes myAttribute_TIMING = xAOD::PFODetails::PFOAttributes::eflowRec_TIMING;
    thisEflowObject->setAttribute(myAttribute_TIMING, clusterTiming);

    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Created neutral EFO with E, eta and phi of " << thisEflowObject->e() << ", " << thisEflowObject->eta() << " and " << thisEflowObject->phi() << std::endl;

  }
}

void eflowObjectCreatorTool::addMoment(xAOD::CaloCluster::MomentType momentType, xAOD::PFODetails::PFOAttributes pfoAttribute, const xAOD::CaloCluster* theCluster, xAOD::PFO* thePFO){
  
  double moment = 0.0;
  bool isRetrieved = theCluster->retrieveMoment(momentType, moment);
  if (true == isRetrieved) {
    xAOD::PFODetails::PFOAttributes myAttribute = pfoAttribute;
    float float_moment = static_cast<float>(moment);
    thePFO->setAttribute(myAttribute, float_moment);
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Could not retrieve moment from the CaloCluster " << endmsg;
  
}
