 /*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TrigEgammaTLAPhotonReAlgo.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"


class ISvcLocator;
 // helpers functions, if needed
struct DescendingEt{

  bool operator()(const xAOD::Photon* l, const xAOD::Photon* r)  const {
    return l->p4().Et() > r->p4().Et();
  }

};

struct HasPtAboveThreshold {

  HasPtAboveThreshold(double thresholdPt) : thresholdPt(thresholdPt) {}
  double thresholdPt;

  bool operator()(const xAOD::Photon* myphoton)  const {
    return myphoton->p4().Pt() > thresholdPt;
  }

 };
 

TrigEgammaTLAPhotonReAlgo::TrigEgammaTLAPhotonReAlgo(const std::string & name, ISvcLocator* pSvcLocator)
 : AthAlgorithm (name, pSvcLocator)
 {

 }

StatusCode TrigEgammaTLAPhotonReAlgo::initialize()
{

  // here we simply check that the class properties have been correctly initialized at the config stage
  ATH_MSG_DEBUG( "Initializing " << name() << " ...") ;
  ATH_MSG_DEBUG( "pT threshold for saving Photons to TLA Collection: " << m_photonPtThreshold) ;
  ATH_MSG_DEBUG( "Maximum number of Photons to be saved: " << m_maxNPhotons );

  // check if the data handles have been initialized
  ATH_CHECK(m_inputPhotonsKeys.initialize());
  ATH_CHECK(m_TLAOutPhotonsKey.initialize());


  if (static_cast<int>(m_maxNPhotons) == 0){ // allow m_maxNPhotons == -1 as flag to save all Photons
    ATH_MSG_ERROR("This algorithm is initialized to save NO (0) Photons, so it shouldn't be running at all!");
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaTLAPhotonReAlgo::execute()
{
  using namespace xAOD;

  ATH_MSG_DEBUG("Executing " << name() << " ...");


  auto ctx = getContext();

  SG::ReadHandle<PhotonContainer> h_inputPhotons = SG::makeHandle(m_inputPhotonsKeys, ctx);
  SG::WriteHandle<PhotonContainer> h_TLAPhotons = SG::makeHandle(m_TLAOutPhotonsKey, ctx);

  // effectively make the TLA Photon Container

  ATH_MSG_DEBUG("Retrieving Photons from " << h_inputPhotons.key() );
  ATH_MSG_DEBUG("Placing <selected> Photons in " << h_TLAPhotons.key() );

  //ATH_CHECK(h_TLAPhotons.record (std::make_unique<xAOD::PhotonContainer>(),
    //          std::make_unique<PhotonAuxContainer>()) );  


  const xAOD::PhotonContainer* inputPhotons = h_inputPhotons.get();
  std::vector<const xAOD::Photon*> originalPhotons(inputPhotons->begin(), inputPhotons->end());

  // define the maximum number of photons we care about: either equivalent to m_maxNPhotons if smaller than size of vector, or keep all photons (in case of negative value)
  std::vector<const xAOD::Photon*>::iterator it_maxPhotonBound;

  int maxNPhotons = static_cast<int>(m_maxNPhotons);
  int sizeOfOriginalPhotonContainer = static_cast<int>(originalPhotons.size());

  // take the largest between the number of photons requested and the size of the vector, in iterator form
  if (m_maxNPhotons > 0) it_maxPhotonBound = maxNPhotons < sizeOfOriginalPhotonContainer ? originalPhotons.begin() + maxNPhotons : originalPhotons.end();
  else it_maxPhotonBound = originalPhotons.end();

  // check the sort order of the input container is ok
  // use a partial sort to save some time
  std::partial_sort (originalPhotons.begin(), it_maxPhotonBound, originalPhotons.end(), DescendingEt());

  // get an iterator to the last element above the pT threshold (because we ordered the photons, this is the last one we want)
  std::vector<const xAOD::Photon*>::iterator it_ptThresholdBound;
  it_ptThresholdBound = std::partition(originalPhotons.begin(), it_maxPhotonBound, HasPtAboveThreshold(static_cast<float>(m_photonPtThreshold)));

  //make the output photon container
  ATH_CHECK(h_TLAPhotons.record (std::make_unique<xAOD::PhotonContainer>(),
              std::make_unique<PhotonAuxContainer>()) );   // in FastPhotonFex this was TrigEMClusterAuxContainer


  //loop on all the photons from the beginning to the last photon we want, and put them the output photon collection
  //also, set a link to their parent photon
  for( auto it_ph=originalPhotons.begin(); it_ph!=it_ptThresholdBound; ++it_ph ) {
    xAOD::Photon* copiedPhoton = new xAOD::Photon(*(*it_ph));

    ATH_CHECK(setOriginalObjectLink(*(*it_ph),*copiedPhoton));

    h_TLAPhotons->push_back(copiedPhoton);
    ATH_MSG_DEBUG("Selected photon pT: " << copiedPhoton->pt());

  }


  return StatusCode::SUCCESS;
}
