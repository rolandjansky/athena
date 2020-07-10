//eflowRec includes
#include "eflowRec/PFChargedFlowElementCreatorAlgorithm.h"
#include "eflowRec/eflowRecTrack.h"

//EDM includes
#include "xAODBase/IParticleContainer.h"
#include "xAODPFlow/FlowElementAuxContainer.h"

PFChargedFlowElementCreatorAlgorithm::PFChargedFlowElementCreatorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name,pSvcLocator)
{
}

StatusCode PFChargedFlowElementCreatorAlgorithm::initialize(){

  ATH_CHECK(m_eflowCaloObjectContainerReadHandleKey.initialize());

  ATH_CHECK(m_chargedFlowElementContainerWriteHandleKey.initialize());
  
  return StatusCode::SUCCESS;
}

StatusCode PFChargedFlowElementCreatorAlgorithm::execute(const EventContext& ctx) const {

  ATH_MSG_DEBUG("Starting PFOChargedCreatorAlgorithm::execute");

  SG::WriteHandle<xAOD::FlowElementContainer> chargedFlowElementContainerWriteHandle(m_chargedFlowElementContainerWriteHandleKey,ctx);
  ATH_CHECK(chargedFlowElementContainerWriteHandle.record(std::make_unique<xAOD::FlowElementContainer>(),std::make_unique<xAOD::FlowElementAuxContainer>()));
  
  /* Create Charged FlowElements from all eflowCaloObjects */
  SG::ReadHandle<eflowCaloObjectContainer> eflowCaloObjectContainerReadHandle(m_eflowCaloObjectContainerReadHandleKey,ctx);
  for (auto thisEflowCaloObject : *eflowCaloObjectContainerReadHandle) createChargedFlowElements(*thisEflowCaloObject,true,chargedFlowElementContainerWriteHandle);

  std::sort(chargedFlowElementContainerWriteHandle->begin(), chargedFlowElementContainerWriteHandle->end(), [] (const xAOD::FlowElement* flowElement1, const xAOD::FlowElement* flowElement2) {return flowElement1->pt()>flowElement2->pt();});

  return StatusCode::SUCCESS;  

}

StatusCode PFChargedFlowElementCreatorAlgorithm::finalize(){ return StatusCode::SUCCESS; }

void PFChargedFlowElementCreatorAlgorithm::createChargedFlowElements(const eflowCaloObject& energyFlowCaloObject, bool addClusters, SG::WriteHandle<xAOD::FlowElementContainer>& chargedFlowElementContainerWriteHandle) const {

  /* Loop over all tracks in the eflowCaloObject */
  int nTracks = energyFlowCaloObject.nTracks();
  for (int iTrack = 0; iTrack < nTracks; ++iTrack) {

    eflowRecTrack* efRecTrack = energyFlowCaloObject.efRecTrack(iTrack);

    /* Skip tracks that haven't been subtracted */
    if (false == m_eOverPMode){
      if (!efRecTrack->isSubtracted()){ continue; }
    }

    /* Create new xAOD::FlowElement and set the type to charged PFlow */
    xAOD::FlowElement* thisFE = new xAOD::FlowElement();
    thisFE->setSignalType(xAOD::FlowElement::SignalType::ChargedPFlow);
    chargedFlowElementContainerWriteHandle->push_back(thisFE);

    /* Get the track elementLink and add it to the xAOD:FE. 
    Note we first have to convert it to an IParticle ElementLink. */
    ElementLink<xAOD::TrackParticleContainer> theTrackLink = efRecTrack->getTrackElemLink();
    ElementLink< xAOD::IParticleContainer > theIParticleTrackLink; 
    theIParticleTrackLink.resetWithKeyAndIndex(theTrackLink.persKey(),theTrackLink.persIndex() ); 
    std::vector<ElementLink<xAOD::IParticleContainer> > vecIParticleTrackLinkContainer;
    vecIParticleTrackLinkContainer.push_back(theIParticleTrackLink);
    thisFE->setChargedObjectLinks(vecIParticleTrackLinkContainer);

    //Now set the charge
    thisFE->setCharge(efRecTrack->getTrack()->charge());

    std::pair<double,double> etaPhi(0.0,0.0);

    if (m_eOverPMode){
      /* In EOverPMode want charged eflowObjects to have extrapolated eta,phi as coordinates
       * (needed for analysis of EOverP Data) */
      etaPhi = efRecTrack->getTrackCaloPoints().getEM2etaPhi();
        
    } else {
      /* In normal mode we want the track eta,phi at the perigee */
      etaPhi.first = efRecTrack->getTrack()->eta();
      etaPhi.second = efRecTrack->getTrack()->phi();
    }

    /* Set the 4-vector of the xAOD::PFO */
    thisFE->setP4(efRecTrack->getTrack()->pt(), etaPhi.first, etaPhi.second, efRecTrack->getTrack()->m());

  }//loop over eflowRecTracks

}
