/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFONeutralCreatorAlgorithm.h"



#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowTrackClusterLink.h"

#include "xAODPFlow/PFOAuxContainer.h"
#include <algorithm>
#include <cmath>
#include <array>
#include <utility> //for std::pair


PFONeutralCreatorAlgorithm::PFONeutralCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator){
}

StatusCode PFONeutralCreatorAlgorithm::initialize(){
  ATH_CHECK(m_eflowCaloObjectContainerReadHandleKey.initialize());
  ATH_CHECK(m_neutralPFOContainerWriteHandleKey.initialize());
  if(!m_LCMode) {
    ATH_CHECK(m_neutralPFOContainerWriteHandleKey_nonModified.initialize());
  }
  return StatusCode::SUCCESS;
}

StatusCode PFONeutralCreatorAlgorithm::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("Executing");
  /* Create Neutral PFOs from all eflowCaloObjects */
  SG::ReadHandle<eflowCaloObjectContainer> eflowCaloObjectContainerReadHandle(m_eflowCaloObjectContainerReadHandleKey,ctx);
  // Always create at least one PFO container & aux
  auto neutralPFOContainer = std::make_unique<xAOD::PFOContainer>();
  auto neutralPFOContainerAux = std::make_unique<xAOD::PFOAuxContainer>();
  neutralPFOContainer->setStore(neutralPFOContainerAux.get());
  // The non-modified container is only used for LC PFOs
  std::unique_ptr<xAOD::PFOContainer> neutralPFOContainer_nonModified(nullptr);
  std::unique_ptr<xAOD::PFOAuxContainer> neutralPFOContainerAux_nonModified(nullptr);
  if(m_LCMode) {
    neutralPFOContainer_nonModified = std::make_unique<xAOD::PFOContainer>();
    neutralPFOContainerAux_nonModified = std::make_unique<xAOD::PFOAuxContainer>();
    neutralPFOContainer->setStore(neutralPFOContainerAux_nonModified.get());
  }
  ATH_MSG_DEBUG("Looping over eflowCaloObjects");
  // Create PFOs and fill the containers
  for (auto thisEflowCaloObject : *eflowCaloObjectContainerReadHandle) {
    if( createNeutralPFO(*thisEflowCaloObject, neutralPFOContainer.get(), neutralPFOContainer_nonModified.get()).isFailure() ) {
      ATH_MSG_WARNING("Problem encountered while creating neutral PFOs");
      return StatusCode::SUCCESS;
    }
  }
  // Record the output containers
  SG::WriteHandle<xAOD::PFOContainer> neutralPFOContainerWriteHandle(m_neutralPFOContainerWriteHandleKey,ctx);
  std::sort(neutralPFOContainer->begin(), neutralPFOContainer->end(), [] (const xAOD::PFO* pfo1, const xAOD::PFO* pfo2) {return pfo1->pt()>pfo2->pt();});
  ATH_CHECK( neutralPFOContainerWriteHandle.record(std::move(neutralPFOContainer),std::move(neutralPFOContainerAux)) );
  if(m_LCMode) {
    std::sort(neutralPFOContainer_nonModified->begin(), neutralPFOContainer_nonModified->end(), [] (const xAOD::PFO* pfo1, const xAOD::PFO* pfo2) {return pfo1->pt()>pfo2->pt();});
    SG::WriteHandle<xAOD::PFOContainer> neutralPFOContainerWriteHandle_nonModified(m_neutralPFOContainerWriteHandleKey,ctx);
    ATH_CHECK( neutralPFOContainerWriteHandle_nonModified.record(std::move(neutralPFOContainer_nonModified),std::move(neutralPFOContainerAux_nonModified)) );
  }
  return StatusCode::SUCCESS;
}

StatusCode PFONeutralCreatorAlgorithm::finalize(){ return StatusCode::SUCCESS; }

StatusCode PFONeutralCreatorAlgorithm::createNeutralPFO(const eflowCaloObject& energyFlowCaloObject, xAOD::PFOContainer* neutralPFOContainer, xAOD::PFOContainer* neutralPFOContainer_nonModified) const {
  unsigned int nClusters = energyFlowCaloObject.nClusters();
  ATH_MSG_VERBOSE("  eflowCaloObject with " << nClusters << " clusters");
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster){
    eflowRecCluster* thisEfRecCluster = energyFlowCaloObject.efRecCluster(iCluster);
    /* Skip empty clusters (presumably subtraction remnants) */
    const CaloClusterCellLink* theCellLink = energyFlowCaloObject.efRecCluster(iCluster)->getCluster()->getCellLinks();
    CaloClusterCellLink::const_iterator it=theCellLink->begin();
    CaloClusterCellLink::const_iterator it_e=theCellLink->end();
    if (it == it_e) {
      continue;
    }
    //this vetoes rare examples where only two cells are left, and they have equal and opposite energy
    if (0.0 == energyFlowCaloObject.efRecCluster(iCluster)->getCluster()->e() ) continue;
    /* Create the efo, add the cluster and set the four-momentum, charge and type */
    ATH_MSG_VERBOSE("  Creating PFO");
    xAOD::PFO* thisPFO = new xAOD::PFO();
    if (m_LCMode) {
      if (thisEfRecCluster->isTouchable()) {
        neutralPFOContainer->push_back(thisPFO);
      } else {
        if(neutralPFOContainer_nonModified) {
          neutralPFOContainer_nonModified->push_back(thisPFO);
        } else {
          ATH_MSG_WARNING("Got a nullptr for non-modified nPFO container!");
          return StatusCode::FAILURE;
        }
      }
    } else {
      neutralPFOContainer->push_back(thisPFO);
    }
    ATH_MSG_VERBOSE("  Get original cluster link");
    ElementLink<xAOD::CaloClusterContainer> theOriginalClusterLink = thisEfRecCluster->getOriginalClusElementLink();
    ATH_MSG_VERBOSE("  Get sister cluster link");
    ElementLink<xAOD::CaloClusterContainer> theSisterClusterLink = (*theOriginalClusterLink)->getSisterClusterLink();
    if(theSisterClusterLink.isValid()) {
      bool isSet = thisPFO->setClusterLink(theSisterClusterLink);
      if (!isSet) { ATH_MSG_WARNING( "Could not set Cluster in PFO " ); }
    } else {
      bool isSet = thisPFO->setClusterLink(theOriginalClusterLink);
      if (!isSet) { ATH_MSG_WARNING( "Could not set Cluster in PFO " ); }
    }
    if (m_addShowerSubtractedClusters) thisPFO->setAttribute("PFOShowerSubtractedClusterLink",thisEfRecCluster->getClusElementLink());
    ATH_MSG_VERBOSE("  Sucessfully set cluster link");
    const xAOD::CaloCluster* cluster = thisEfRecCluster->getCluster();
    ATH_MSG_VERBOSE("  Got CaloCluster from EfRecCluster");
    //be careful here - cluster p4 methods do not store sign. Thus -ve energy clusters have +ve pt and hence +ve energy
    if (!m_LCMode) {
      //in EM->EM/LC mode we use eta,phi at EM scale for both 4-vectors
      thisPFO->setP4(cluster->pt(), cluster->rawEta(), cluster->rawPhi(), cluster->m());
      thisPFO->setP4EM(cluster->rawE()/std::cosh(cluster->rawEta()), cluster->rawEta(),cluster->rawPhi(),cluster->rawM());
    } else {
      //in LC-> mode we use the LC 4-vector for the LC scale
      thisPFO->setP4(cluster->pt(), cluster->eta(), cluster->phi(), cluster->m());
      //we cannot access geometric weights for LC clusters, so we make an approximation of the EM energy by looping over the calocells
      //Then the EM 4-vector uses the energy/pt at this EM scale + eta,phi from LC 4-vector
      const CaloClusterCellLink* theCellLink = cluster->getCellLinks();
      float emPt = 0.0;
      for (auto thisCaloCell : *theCellLink) emPt += thisCaloCell->e()/std::cosh(thisCaloCell->eta());
      thisPFO->setP4EM(emPt,cluster->eta(),cluster->phi(),0.0);//mass is always zero at EM scale
    }
    ATH_MSG_DEBUG("Created neutral PFO with E, pt, eta and phi of " << thisPFO->e() << ", " << thisPFO->pt() << ", " << thisPFO->eta() << " and " << thisPFO->phi());
    thisPFO->setCharge(0);
    //Set the CENTER_MAG moment. This has its own dedicated getter in the PFO EDM, and so is not dealt with by the generic addMoment function.
    double center_mag = 0.0;
    bool isRetrieved = cluster->retrieveMoment(xAOD::CaloCluster::CENTER_MAG, center_mag );
    if (isRetrieved) thisPFO->setCenterMag(center_mag);
    else ATH_MSG_WARNING("Could not retreve CENTER_MAG from xAOD::CaloCluster");
    if(m_doClusterMoments) {
      //definitions to reduce verbosity
      using Cluster = xAOD::CaloCluster;
      using Attributes = xAOD::PFODetails::PFOAttributes;
      //now set the moments for touchable clusters (i.e. ones we modify) in LC mode or all clusters in EM mode
      if ( (m_LCMode && thisEfRecCluster->isTouchable()) || !m_LCMode) {
        //Block of code to set moments/attributes
        auto conciseAddMoment = [&cluster, &thisPFO, this](const Cluster::MomentType & c, const Attributes & a){
          return this->addMoment(c, a, *cluster, *thisPFO);
        };
        const std::array< std::pair<Cluster::MomentType, Attributes>, 11> momentAttributePairs{{ //tricky: one more brace than expected, and is necessary
          {Cluster::SECOND_R, Attributes::eflowRec_SECOND_R},
          {Cluster::CENTER_LAMBDA, Attributes::eflowRec_CENTER_LAMBDA},
          {Cluster::ENG_BAD_CELLS, Attributes::eflowRec_ENG_BAD_CELLS},
          {Cluster::N_BAD_CELLS, Attributes::eflowRec_N_BAD_CELLS},
          {Cluster::BADLARQ_FRAC, Attributes::eflowRec_BADLARQ_FRAC},
          {Cluster::ENG_POS, Attributes::eflowRec_ENG_POS},
          {Cluster::AVG_LAR_Q, Attributes::eflowRec_AVG_LAR_Q},
          {Cluster::AVG_TILE_Q, Attributes::eflowRec_AVG_TILE_Q},
          {Cluster::ISOLATION, Attributes::eflowRec_ISOLATION},
          {Cluster::SECOND_LAMBDA, Attributes::eflowRec_SECOND_LAMBDA},
          {Cluster::EM_PROBABILITY, Attributes::eflowRec_EM_PROBABILITY} 
        }};
        for (const auto & [moment,attribute]:momentAttributePairs) conciseAddMoment(moment, attribute);
        if (m_useCalibHitTruth){
          const std::array< std::pair<Cluster::MomentType, Attributes>, 4> momentAttributeTruthPairs{{
            {Cluster::ENG_CALIB_TOT, Attributes::eflowRec_ENG_CALIB_TOT},
            {Cluster::ENG_CALIB_FRAC_EM, Attributes::eflowRec_ENG_CALIB_FRAC_EM},
            {Cluster::ENG_CALIB_FRAC_HAD, Attributes::eflowRec_ENG_CALIB_FRAC_HAD},
            {Cluster::ENG_CALIB_FRAC_REST, Attributes::eflowRec_ENG_CALIB_FRAC_REST}
          }};
          for (const auto & [moment,attribute]:momentAttributeTruthPairs) conciseAddMoment(moment, attribute);
        }
      }
      //
      //block of code to calculate and set the layer energies
      auto setLayerEnergy = [&cluster, &thisPFO](const Cluster::CaloSample &layer, const Attributes & a){
        const auto layerEnergy{cluster->eSample(layer)};
        thisPFO->setAttribute(a, layerEnergy);
        return layerEnergy;
      };
      enum LayerEnergyIndices{preSamplerB, EMB1, EMB2, EMB3, preSamplerE, EME1, EME2, EME3, HEC0, HEC1, HEC2, HEC3,FCAL0,  NumberOfLayerEnergies};
      std::array<float, NumberOfLayerEnergies> layerEnergies{};
      const std::array< std::pair<Cluster::CaloSample, Attributes>, NumberOfLayerEnergies> sampleAttributePairs{{
        {Cluster::CaloSample::PreSamplerB, Attributes::eflowRec_LAYERENERGY_PreSamplerB},
        {Cluster::CaloSample::EMB1, Attributes::eflowRec_LAYERENERGY_EMB1},
        {Cluster::CaloSample::EMB2, Attributes::eflowRec_LAYERENERGY_EMB2},
        {Cluster::CaloSample::EMB3, Attributes::eflowRec_LAYERENERGY_EMB3},
        {Cluster::CaloSample::PreSamplerE, Attributes::eflowRec_LAYERENERGY_PreSamplerE},
        {Cluster::CaloSample::EME1, Attributes::eflowRec_LAYERENERGY_EME1},
        {Cluster::CaloSample::EME2, Attributes::eflowRec_LAYERENERGY_EME2},
        {Cluster::CaloSample::EME3, Attributes::eflowRec_LAYERENERGY_EME3},
        {Cluster::CaloSample::HEC0, Attributes::eflowRec_LAYERENERGY_HEC0},
        {Cluster::CaloSample::HEC1, Attributes::eflowRec_LAYERENERGY_HEC1},
        {Cluster::CaloSample::HEC2, Attributes::eflowRec_LAYERENERGY_HEC2},
        {Cluster::CaloSample::HEC3, Attributes::eflowRec_LAYERENERGY_HEC3},
        {Cluster::CaloSample::FCAL0, Attributes::eflowRec_LAYERENERGY_FCAL0}
      }};
      size_t layerIndex{0};
      for (const auto & [sample, attribute]: sampleAttributePairs) {
        layerEnergies[layerIndex++] = setLayerEnergy(sample, attribute);
      }
      //
      //block of code purely to set attributes of the PFO
      auto setPfoAttribute = [&thisPFO, &cluster](const Cluster::CaloSample &layer, const Attributes & a){
        thisPFO->setAttribute( a, cluster->eSample(layer));
      };
      //
      constexpr size_t numberOfSampleAttributePairs{15};
      const std::array< std::pair<Cluster::CaloSample, Attributes>, numberOfSampleAttributePairs> sampleAttributePairsTileFcal{{
        {Cluster::CaloSample::TileBar0, Attributes::eflowRec_LAYERENERGY_TileBar0},
        {Cluster::CaloSample::TileBar1, Attributes::eflowRec_LAYERENERGY_TileBar1},
        {Cluster::CaloSample::TileBar2, Attributes::eflowRec_LAYERENERGY_TileBar2},
        {Cluster::CaloSample::TileGap1, Attributes::eflowRec_LAYERENERGY_TileGap1},
        {Cluster::CaloSample::TileGap2, Attributes::eflowRec_LAYERENERGY_TileGap2},
        {Cluster::CaloSample::TileGap3, Attributes::eflowRec_LAYERENERGY_TileGap3},
        {Cluster::CaloSample::TileExt0, Attributes::eflowRec_LAYERENERGY_TileExt0},
        {Cluster::CaloSample::TileExt1, Attributes::eflowRec_LAYERENERGY_TileExt1},
        {Cluster::CaloSample::TileExt2, Attributes::eflowRec_LAYERENERGY_TileExt2},
        {Cluster::CaloSample::FCAL1, Attributes::eflowRec_LAYERENERGY_FCAL1},
        {Cluster::CaloSample::FCAL2, Attributes::eflowRec_LAYERENERGY_FCAL2},
        {Cluster::CaloSample::MINIFCAL0, Attributes::eflowRec_LAYERENERGY_MINIFCAL0},
        {Cluster::CaloSample::MINIFCAL1, Attributes::eflowRec_LAYERENERGY_MINIFCAL1},
        {Cluster::CaloSample::MINIFCAL2, Attributes::eflowRec_LAYERENERGY_MINIFCAL2},
        {Cluster::CaloSample::MINIFCAL3, Attributes::eflowRec_LAYERENERGY_MINIFCAL3}
      }};
      for (const auto & [sample, attribute]: sampleAttributePairsTileFcal){
        setPfoAttribute(sample, attribute);
      }
      //
      //now set the layer energies for EMB3 and Tile0 - these are needed if we want to run a GSC style jet calibration, which is binned in EMB3 and Tile0 layer energies
      float layerEnergy_EM3 = layerEnergies[EMB3] + layerEnergies[EME3];
      thisPFO->setAttribute( Attributes::eflowRec_LAYERENERGY_EM3, layerEnergy_EM3);
      //
      float layerEnergy_TileBar0 = cluster->eSample(Cluster::CaloSample::TileBar0);
      float layerEnergy_TileExt0 = cluster->eSample(Cluster::CaloSample::TileExt0);
      float layerEnergy_Tile0 = layerEnergy_TileBar0 + layerEnergy_TileExt0;
      thisPFO->setAttribute(Attributes::eflowRec_LAYERENERGY_Tile0, layerEnergy_Tile0);
      //
      //now set properties that are required for jet cleaning
      const float layerEnergy_HEC = layerEnergies[HEC0] + layerEnergies[HEC1] + layerEnergies[HEC2] + layerEnergies[HEC3];
      thisPFO->setAttribute(Attributes::eflowRec_LAYERENERGY_HEC, layerEnergy_HEC);
      //
      const float layerEnergy_EM = layerEnergies[preSamplerB] + layerEnergies[preSamplerE] + layerEnergies[EMB1] + layerEnergies[EMB2] + layerEnergies[EMB3] + layerEnergies[EME1] + layerEnergies[EME2] + layerEnergies[EME3] + layerEnergies[FCAL0];
      thisPFO->setAttribute(Attributes::eflowRec_LAYERENERGY_EM, layerEnergy_EM);
      //
      const float clusterTiming = cluster->time();
      thisPFO->setAttribute(Attributes::eflowRec_TIMING, clusterTiming);
    }
  }
  return StatusCode::SUCCESS;
}

void PFONeutralCreatorAlgorithm::addMoment(const xAOD::CaloCluster::MomentType& momentType, const xAOD::PFODetails::PFOAttributes& pfoAttribute, const xAOD::CaloCluster& theCluster, xAOD::PFO& thePFO) const {

  double moment = 0.0;
  bool isRetrieved = theCluster.retrieveMoment(momentType, moment);
  if (isRetrieved) {
    xAOD::PFODetails::PFOAttributes myAttribute = pfoAttribute;
    float float_moment = static_cast<float>(moment);
    thePFO.setAttribute(myAttribute, float_moment);
  }
  else ATH_MSG_WARNING(" Could not retrieve moment from the CaloCluster");

}
