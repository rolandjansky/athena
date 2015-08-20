/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


  /********************************************************************

NAME:     eflowTauProcessingTools.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  May 2013

********************************************************************/

#include "eflowRec/eflowTauProcessingTools.h"
#include "eflowRec/eflowPi0Cuts.h"

#include "eflowEvent/eflowTauObject.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODJet/JetConstituentVector.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"

#include "xAODBase/ObjectType.h"


eflowTauProcessingTools::eflowTauProcessingTools(const std::string& name, ISvcLocator* pSvcLocator): 
  eflowBaseAlg(name, pSvcLocator),
  m_PFOOutputName("TauPFO_eflowRec"),
  m_eflowClustersOutputName("eflowClusters_tauMode"),
  m_topoClusterCandsName("CaloTopoCluster"),
  m_tauRecCandsName("TauJets"),
  m_storeGate(nullptr),
  m_tools(this)
{
  //Tool configuration
  declareProperty( "PrivateToolList",  m_tools, "List of Private AlgTools" );
  //Names of containers
  declareProperty("PFOOutputName",m_PFOOutputName);
  declareProperty("EflowClustersOutputName",m_PFOOutputName);
  declareProperty("EMScaleClustersInputName", m_topoClusterCandsName);
  declareProperty("TauCandidatesInputName",m_tauRecCandsName);
  
  m_tools.push_back( "eflowTauBaseAlgTool/eflowTauTool" );

}

eflowTauProcessingTools::~eflowTauProcessingTools() {}

StatusCode eflowTauProcessingTools::initialize(){

  if ( m_tools.retrieve().isFailure() ) {
    msg(MSG::WARNING) << MSG::WARNING << "Failed to retreive " << m_tools << endreq;
    return StatusCode::SUCCESS;
  } else {
    msg(MSG::INFO) << MSG::INFO << "Retrieved " << m_tools << endreq;
  }

  ToolHandleArray< eflowTauBaseAlgTool >::iterator firstTool = m_tools.begin();
  ToolHandleArray< eflowTauBaseAlgTool >::iterator lastTool  = m_tools.end();
  msg(MSG::INFO) << MSG::INFO << " " << endreq;
  msg(MSG::INFO) << MSG::INFO << "List of tools in execution sequence of eflowTauProcessingTools:" << endreq;
  msg(MSG::INFO) << MSG::INFO << "------------------------------------" << endreq;
  msg(MSG::INFO) << MSG::INFO << " " << endreq;

  unsigned int toolCtr = 0;
  for ( ; firstTool != lastTool; firstTool++ ){
    toolCtr++;
    msg(MSG::INFO) << MSG::INFO 
	<< std::setw(2)     << std::setiosflags(std::ios_base::right)
	<< toolCtr << ".) " << std::resetiosflags(std::ios_base::right)
	<< std::setw(36) << std::setfill('.') 
	<< std::setiosflags(std::ios_base::left) << (*firstTool)->type()
	<< std::setfill('.')
	<< (*firstTool)->name()
	<< std::setfill(' ')
	<< endreq;
  }
  msg(MSG::INFO) << MSG::INFO << " " << endreq;
  msg(MSG::INFO) << MSG::INFO << "------------------------------------" << endreq;

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())
  {
    msg(MSG::WARNING) << MSG::WARNING 
	<< "Unable to retrieve pointer to StoreGateSvc"
	<< endreq;
    return StatusCode::SUCCESS;
  } 

  return StatusCode::SUCCESS;
}

StatusCode eflowTauProcessingTools::execute(){

  eflowPi0Cuts eflowPi0Cuts;

  int debug = 0;

  //The first thing we need to do is load in the input containers - taus and EM scale clusters

  StatusCode sc;

  const xAOD::TauJetContainer* theTauJets(NULL);
  sc = evtStore()->retrieve(theTauJets,m_tauRecCandsName);

  if(sc.isFailure() || !theTauJets)
  {
    msg(MSG::WARNING) << MSG::WARNING   <<" no tauJet container" << endreq; 
    return StatusCode::SUCCESS;
  }//check we got the taus

  const xAOD::CaloClusterContainer* theClusters(NULL);
  sc = evtStore()->retrieve(theClusters,m_topoClusterCandsName);

  if (sc.isFailure() || !theClusters){
    msg(MSG::WARNING) << MSG::WARNING   <<" no CaloCluster container" << endreq; 
    return StatusCode::SUCCESS;
  }//check we got the clusters

  // create PFO containers and register them
  xAOD::PFOContainer* neutralPFOContainer = new xAOD::PFOContainer();
  CHECK( evtStore()->record( neutralPFOContainer, "neutral" + m_PFOOutputName ) );

  xAOD::PFOAuxContainer* neutralPFOAuxContainer = new xAOD::PFOAuxContainer();
  CHECK( evtStore()->record( neutralPFOAuxContainer, "neutral" + m_PFOOutputName + "Aux." ) );
  neutralPFOContainer->setStore(neutralPFOAuxContainer);
 
  xAOD::PFOContainer* chargedPFOContainer = new xAOD::PFOContainer();
  CHECK( evtStore()->record( chargedPFOContainer, "charged" + m_PFOOutputName ) );

  xAOD::PFOAuxContainer* chargedPFOAuxContainer = new xAOD::PFOAuxContainer();
  CHECK( evtStore()->record( chargedPFOAuxContainer, "charged" + m_PFOOutputName + "Aux." ) );
  chargedPFOContainer->setStore(chargedPFOAuxContainer);

  xAOD::TauJetContainer::const_iterator firstTau = theTauJets->begin();
  xAOD::TauJetContainer::const_iterator lastTau = theTauJets->end();

  xAOD::CaloClusterContainer* finalClusContainer = new xAOD::CaloClusterContainer();

  CHECK( evtStore()->record( finalClusContainer, m_eflowClustersOutputName ) );

  xAOD::CaloClusterAuxContainer* auxClusterContainer = new xAOD::CaloClusterAuxContainer();
  CHECK( evtStore()->record( auxClusterContainer, m_eflowClustersOutputName  + "Aux." ) );
  finalClusContainer->setStore( auxClusterContainer );

  for (; firstTau != lastTau; ++firstTau){
    
    ElementLinkVector<xAOD::CaloClusterContainer>* tauClusContainer = new ElementLinkVector<xAOD::CaloClusterContainer>;
    xAOD::PFOContainer* tempPFOContainer = new xAOD::PFOContainer();
    xAOD::PFOAuxContainer* tempPFOAuxContainer = new xAOD::PFOAuxContainer();
    tempPFOContainer->setStore(tempPFOAuxContainer);

    const xAOD::TauJet* theTau = *firstTau;

    if (theTau){

      if (theTau->nTracks() > 5 || 0 == theTau->nTracks() ) {
        delete tauClusContainer;
	delete tempPFOAuxContainer;
        delete tempPFOContainer;
        continue;
      }   

      const ElementLink<xAOD::JetContainer> theJetLink =  theTau->jetLink();
      if (theJetLink.isValid()){
        const xAOD::Jet* theJet = *theJetLink;
        if (theJet){
          xAOD::JetConstituentVector theConstituents = theJet->getConstituents();

          xAOD::JetConstituentVector::iterator firstClus = theConstituents.begin();
          xAOD::JetConstituentVector::iterator lastClus = theConstituents.end();
	  
          for (; firstClus != lastClus; ++firstClus){

            const xAOD::JetConstituent* theLink = *firstClus;
            if (theLink){
              const xAOD::IParticle* theParticle = theLink->rawConstituent() ;
              if (theParticle){
                xAOD::Type::ObjectType theType = theParticle->type() ;
                if (xAOD::Type::CaloCluster == theType){
                  const xAOD::CaloCluster* theCluster = dynamic_cast<const xAOD::CaloCluster*>(theParticle);
                  
                  if (theCluster){
                    xAOD::CaloClusterContainer::const_iterator firstTopoClus = theClusters->begin();
                    xAOD::CaloClusterContainer::const_iterator lastTopoClus = theClusters->end();
		    
                    for (; firstTopoClus != lastTopoClus; ++firstTopoClus){
		      const xAOD::CaloCluster* thisClus = *firstTopoClus;
                      if (thisClus){

                        //This has to be loose because of the lossy precision algorithm - energy of clusters is changed when reading CaloCalTopoCluster comapred to re-recoing CaloTopoCluster
                        //Also use eta-phi to triple check the match
                        if ( fabs(theCluster->rawE() - thisClus->e())/(fabs(theCluster->rawE())) < 0.05){
                          if (fabs(theCluster->eta() - thisClus->eta()) < 0.05 && fabs(theCluster->phi() - thisClus->phi()) < 0.05){

                            xAOD::CaloCluster* eflowCaloClus = new xAOD::CaloCluster(*thisClus);
                            finalClusContainer->push_back(eflowCaloClus);

                            ElementLink<xAOD::CaloClusterContainer> thisClusterLink;
                            thisClusterLink.setElement(eflowCaloClus);
                            thisClusterLink.setStorableObject(*finalClusContainer);

                            bool isDuplicate = this->checkForDuplicates(tauClusContainer,eflowCaloClus);

                            if (false == isDuplicate) tauClusContainer->push_back(thisClusterLink);

                            if (1 == debug) std::cout << "Added cluster with E, eta and phi " << eflowCaloClus->e() << ", " << eflowCaloClus->eta() << " and " << eflowCaloClus->phi() << " to tau cluster container " << std::endl;
                            
                          }//eta-phi match
                        }
                      }//energy match
                    }//topocluster loop
                  }//if cluster pointer is valid
                  else msg(MSG::WARNING) << MSG::WARNING << "The jet constituent of a tauRec candidate is an invalid pointer!" << endreq;
                }
                else msg(MSG::WARNING) << MSG::WARNING << "The tau jet seed has a constituent which is NOT a CaloCluster " << endreq;
              }
              else msg(MSG::WARNING) << MSG::WARNING << " The tau jet seed has an invalid pointer to its constituent " << endreq;
            }
            else msg(MSG::WARNING) << MSG::WARNING << " The tau jet seed has an invalid element link to its constituent " << endreq;

          }
        }
        else msg(MSG::WARNING) << MSG::WARNING << "The jet constituent of a tauRec candidate is an invalid pointer!" << endreq;
      }
      else msg(MSG::WARNING) << MSG::WARNING << "Have invalid link from tau to jet seed " << endreq;

      ElementLinkVector<xAOD::TrackParticleContainer> myTauTracks;

      std::vector< ElementLink< xAOD::TrackParticleContainer > >  theTauTracks = theTau->trackLinks();

      std::vector< ElementLink< xAOD::TrackParticleContainer > >::iterator firstTrack = theTauTracks.begin();
      std::vector< ElementLink< xAOD::TrackParticleContainer > >::iterator lastTrack = theTauTracks.end();

      for (; firstTrack != lastTrack; ++firstTrack) {
        myTauTracks.push_back(*firstTrack);
      }

      std::vector< ElementLink< xAOD::TrackParticleContainer > > wideTrackVector = theTau->wideTrackLinks();

      std::vector< ElementLink< xAOD::TrackParticleContainer > >::iterator firstTrackB = wideTrackVector.begin();
      std::vector< ElementLink< xAOD::TrackParticleContainer > >::iterator lastTrackB = wideTrackVector.end();

      for (; firstTrackB != lastTrackB; ++firstTrackB) {
        myTauTracks.push_back(*firstTrackB);
      }
      
      const ElementLinkVector<xAOD::TrackParticleContainer>* myTauContainerPointer = &myTauTracks;

      //Now we can create the eflowTauObject
      eflowTauObject myTauObject;
      myTauObject.setPFOContainer(tempPFOContainer);
      myTauObject.setNumInnerTracks(theTauTracks.size());
      myTauObject.setTrackContainer(myTauContainerPointer);
      myTauObject.setClusterContainer(tauClusContainer);
      myTauObject.setFinalClusterContainer(finalClusContainer);

      StatusCode processStatus = StatusCode::SUCCESS;

      ToolHandleArray< eflowTauBaseAlgTool >::iterator firstAlgTool = m_tools.begin();
      ToolHandleArray< eflowTauBaseAlgTool >::iterator lastAlgTool  = m_tools.end();

      while ( ! processStatus.isFailure() && firstAlgTool != lastAlgTool ) {
        processStatus = (*firstAlgTool)->execute(myTauObject);
        ++firstAlgTool;
      }
      
      if ( processStatus.isFailure() ) {
        msg(MSG::WARNING) << MSG::WARNING << (*firstAlgTool)->name() << "returned StatusCode FAILURE " << endreq;
        return StatusCode::SUCCESS;
      }
      
    }
    else msg(MSG::WARNING) << MSG::WARNING << "Have invalid pointer to tau seed " << endreq;

    xAOD::PFOContainer::iterator firstPFO = tempPFOContainer->begin();
    xAOD::PFOContainer::iterator lastPFO = tempPFOContainer->end();

    std::vector< ElementLink< xAOD::TrackParticleContainer > > innerTrackContainer = theTau->trackLinks();
    
    for (; firstPFO != lastPFO; ++firstPFO) {


      if (0 == (*firstPFO)->charge()){
	const xAOD::CaloCluster* thisClus = (*firstPFO)->cluster(0);
	if (thisClus){
	  const CaloClusterCellLink* theCellLink = thisClus->getCellLinks();
	  CaloClusterCellLink::const_iterator it=theCellLink->begin();
	  CaloClusterCellLink::const_iterator it_e=theCellLink->end();
	  int numCell = 0;
	  for (;it != it_e ; ++it)  numCell++;
	  //this vetoes two types of PFO
	  //First of all fully subtracted clusters have zero cells
	  //Secondly we have found rare examples where only two cells are left, and they have equal and opposite energy
	  if (0 == numCell || 0.0 == thisClus->e() ) continue;
	}
      }

      xAOD::PFO* thisPFO = new xAOD::PFO(**firstPFO);
      if (0.0 != (*firstPFO)->charge()) {
	chargedPFOContainer->push_back(thisPFO);
	ElementLink<xAOD::PFOContainer> thisChargedLink;
	thisChargedLink.setElement(thisPFO);
	thisChargedLink.setStorableObject(*chargedPFOContainer);
	xAOD::TauJet* theNonConstTau = const_cast<xAOD::TauJet*>(theTau);
	theNonConstTau->addProtoChargedPFOLink(thisChargedLink);
      }
      else {
	neutralPFOContainer->push_back(thisPFO);
	ElementLink<xAOD::PFOContainer> thisNeutralLink;
	thisNeutralLink.setElement(thisPFO);
	thisNeutralLink.setStorableObject(*neutralPFOContainer);
	xAOD::TauJet* theNonConstTau = const_cast<xAOD::TauJet*>(theTau);
	theNonConstTau->addProtoNeutralPFOLink(thisNeutralLink);
	bool result = false;
	eflowPi0Cuts.isPi0(result, thisPFO->eta(), thisPFO->e()/cosh(thisPFO->eta()), thisPFO->bdtPi0Score(), innerTrackContainer.size());
	if (true == result) theNonConstTau->addProtoPi0PFOLink(thisNeutralLink);
      }
    }

    //clean up our memory
    delete tauClusContainer;
    delete tempPFOContainer;
    delete tempPFOAuxContainer;

  }//tau loop



  return StatusCode::SUCCESS;
}

StatusCode eflowTauProcessingTools::finalize(){
  return StatusCode::SUCCESS;
}

bool eflowTauProcessingTools::checkForDuplicates(ElementLinkVector<xAOD::CaloClusterContainer>* clusterContainer, xAOD::CaloCluster* theCluster){

  ElementLinkVector<xAOD::CaloClusterContainer>::iterator firstClus = clusterContainer->begin();
  ElementLinkVector<xAOD::CaloClusterContainer>::iterator lastClus = clusterContainer->end();

  for (; firstClus != lastClus; ++firstClus) if ( fabs( (**firstClus)->e() - theCluster->e()) < 0.0001) return true;


  return false;
}

