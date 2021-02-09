/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaTopoClusterCopier.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "xAODCore/ShallowCopy.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include <cmath>


/** Helper functor to order xAOD::CaloCluster pointers by EM Et **/
class greater {
public:
  bool operator()(xAOD::CaloCluster const *a, xAOD::CaloCluster const *b) const {
    static const SG::AuxElement::Accessor<float> acc_emfraction("EMFraction");
    const double emfrac_a = acc_emfraction(*a);
    const double emfrac_b = acc_emfraction(*b);
    return (a->et() * emfrac_a) > (b->et() * emfrac_b);
  }
};

egammaTopoClusterCopier::egammaTopoClusterCopier(const std::string& name,
                                                 ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm(name, pSvcLocator),
  m_AllClusters{},
  m_PassPreSelection{},
  m_PassSelection{}
{ }

StatusCode egammaTopoClusterCopier::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  ATH_CHECK(m_inputTopoCollection.initialize());
  ATH_CHECK(m_outputTopoCollection.initialize());
  ATH_CHECK(m_outputTopoCollectionShallow.initialize());

  ATH_MSG_DEBUG("Initialization successful");

  return StatusCode::SUCCESS;
}

StatusCode egammaTopoClusterCopier::finalize() {

  ATH_MSG_INFO( name() << " All Clusters " << m_AllClusters );
  ATH_MSG_INFO( name() << " Pass Preselection Clusters " << m_PassPreSelection );
  ATH_MSG_INFO( name() << " Pass Selection " << m_PassSelection );

  return StatusCode::SUCCESS;
}

StatusCode egammaTopoClusterCopier::execute(const EventContext& ctx) const {

  SG::ReadHandle<xAOD::CaloClusterContainer> inputTopoclusters(m_inputTopoCollection, ctx);
  SG::WriteHandle<xAOD::CaloClusterContainer> outputTopoclustersShallow(m_outputTopoCollectionShallow, ctx);
  SG::WriteHandle<ConstDataVector <xAOD::CaloClusterContainer> > outputTopoclusters(m_outputTopoCollection, ctx);

  /* Create a shallow copy, the elements of this can be modified,
   * but no need to recreate the cluster
   */
  std::pair<xAOD::CaloClusterContainer*, xAOD::ShallowAuxContainer*>
    inputShallowcopy = xAOD::shallowCopyContainer(*inputTopoclusters);

  ATH_CHECK( outputTopoclustersShallow.record(std::unique_ptr<xAOD::CaloClusterContainer>(inputShallowcopy.first),
                                              std::unique_ptr<xAOD::ShallowAuxContainer>(inputShallowcopy.second)) );
  /*
   * Here it just needs to be a view copy,
   * i.e the collection we create does not really
   * own its elements
   */
  auto viewCopy = std::make_unique<ConstDataVector <xAOD::CaloClusterContainer> >(SG::VIEW_ELEMENTS );

  // Special egamma EMFraction which includes presampler and E4 cells
  static const SG::AuxElement::Accessor<float> acc_emfraction("EMFraction");

  auto buff_AllClusters = m_AllClusters.buffer();
  auto buff_PassPreSelection = m_PassPreSelection.buffer();
  auto buff_PassSelection = m_PassSelection.buffer();

  // Loop over the output shallow copy
  for (xAOD::CaloCluster* clus : *outputTopoclustersShallow) {
    ++buff_AllClusters;
    acc_emfraction(*clus) = 0.0;  // Always decorate
    const double clusterE = clus->e();
    const double aeta = std::abs(clus->eta());
    ATH_MSG_DEBUG("->CHECKING Cluster at eta,phi,et " << clus->eta() << " , "
                  << clus->phi() << " , " << clus->et());
    if (aeta > m_etaCut || // if it falls outside eta region
        clusterE < m_ECut // Total energy below threshold
       ) {
      continue;
    }
    ++buff_PassPreSelection;

    // Add the relevant TileGap3/E4 cells
    double eg_tilegap = 0;
    if (aeta > 1.35 && aeta < 1.65 && clusterE > 0) {
      xAOD::CaloCluster::const_cell_iterator cell_itr = clus->cell_cbegin();
      xAOD::CaloCluster::const_cell_iterator cell_end = clus->cell_cend();
      for (; cell_itr != cell_end; ++cell_itr) {
        const CaloCell* cell = *cell_itr;
        if (!cell) { continue; }
        const CaloDetDescrElement *dde = cell->caloDDE();
        if (!dde) { continue; }
        // Add TileGap3. Consider only E4 cell
        if (CaloCell_ID::TileGap3 == dde->getSampling()) {
          if (std::abs(dde->eta_raw()) > 1.4 && std::abs(dde->eta_raw()) < 1.6) {
            eg_tilegap += cell->e() * cell_itr.weight();
          }
        }
      }
    }

    double emfrac= (clus->energyBE(0) + clus->energyBE(1) +
                    clus->energyBE(2) + clus->energyBE(3) + eg_tilegap) / clusterE;

    acc_emfraction(*clus) = emfrac;
    if (emfrac > m_EMFracCut && (clusterE * emfrac) > m_ECut) {
      ++buff_PassSelection;
      ATH_MSG_DEBUG("-->Selected Cluster at eta,phi,et,EMFraction " << clus->eta()
                    << " , "<< clus->phi() << " , " << clus->et()<<" , "<< emfrac);
      viewCopy->push_back(clus);
    }
  } // end loop on clusters

  //sort in descenting em energy
  std::sort(viewCopy->begin(),viewCopy->end(), greater());
  ATH_MSG_DEBUG("Cloned container has size: " << viewCopy->size()<<  " selected out of : "
                << inputTopoclusters->size());
  ATH_CHECK( outputTopoclusters.record(std::move(viewCopy)) );

  return StatusCode::SUCCESS;
}

