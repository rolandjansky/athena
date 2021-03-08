// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/**
 * Name    : EFexEMAlgorithm.cxx
 * PACKAGE : Trigger/TrigT1/TrigT1CaloFexPerf
 * AUTHOR  : Denis Oliveira Damazio
 * PURPOSE : emulate the eFex EM algorithm for phase 1 L1Calo
 */

#include "EFexEMAlgorithm.h"

#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "CaloIdentifier/CaloIdManager.h"

#include <math.h>
#include <string>

LVL1::EFexEMAlgorithm::EFexEMAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
   : AthAlgorithm(name, pSvcLocator)
{
   declareProperty("SuperCellContainer", m_inputCellContainerKey = "SCell");
   declareProperty("TileCellContainer", m_inputTileCellContainerKey = "AllCalo");
   declareProperty("TriggerTowerContainer", m_inputTriggerTowerContainerKey = "xAODTriggerTowers");
   declareProperty("OutputClusterName", m_outputClusterContainerKey = "SCluster_TrigT1CaloEFex");

   declareProperty("UseTileCells", m_use_tileCells = false, "Use Tile cells instead of TriggerTowers");
   declareProperty("EnergyWeightedCluster", m_energyWeightedCluster = false, "Use energy-weighted clustering (needs TrigggerTowers)");
   declareProperty("ApplyBaseLineSelection", m_apply_BaseLineCuts = true, "Apply baseline selection to default clustering");

   declareProperty("TimeThreshold", m_timeThr = 200);
   declareProperty("SeedEnergyThreshold", m_seedE = 100);
   declareProperty("EtaCellFormation", m_deta_cellFormation = 0.08);
   declareProperty("PhiCellFormation", m_dphi_cellFormation = 0.21);
   declareProperty("EtaClusterFormation", m_deta = 0.08);
   declareProperty("PhiClusterFormation", m_dphi = 0.11);
   declareProperty("EtaClusterFormation2", m_deta_clusterFormation_2 = 0.21);
   declareProperty("PhiClusterFormation2", m_dphi_clusterFormation_2 = 0.21);
   declareProperty("MinimumClusterEnergy", m_clusterE_EMB2_EMEC2 = 100);
   declareProperty("LimitClusterEta", m_eta_limit = 2.47);
   declareProperty("CleanCellContainerSkim", m_useProvenanceSkim = false);
   declareProperty("QualBitMask", m_qualBitMask = 0x40);
}

LVL1::EFexEMAlgorithm::~EFexEMAlgorithm() = default;

StatusCode
LVL1::EFexEMAlgorithm::initialize() {

   ATH_CHECK(m_inputCellContainerKey.initialize());
   if(! m_energyWeightedCluster && m_use_tileCells) {
      ATH_CHECK(m_inputTileCellContainerKey.initialize());
   }
   if(!m_use_tileCells) {
      ATH_CHECK(m_inputTriggerTowerContainerKey.initialize());
   }

   ATH_CHECK(m_outputClusterContainerKey.initialize());

   ATH_CHECK(m_eFexTool.retrieve());
   ATH_CHECK(m_eFexAlgTool.retrieve());
   return StatusCode::SUCCESS;
}

StatusCode
LVL1::EFexEMAlgorithm::getSuperCellsContainer(CaloConstCellContainer*& scells) const
{
   SG::ReadHandle<CaloCellContainer> inputSCells(m_inputCellContainerKey);
   ATH_CHECK(inputSCells.isValid());
   if ( !scells ) {
      scells = new CaloConstCellContainer(SG::VIEW_ELEMENTS);
   } else {
      scells->clear();
   }
   if ( m_useProvenanceSkim ) {
      scells->reserve( inputSCells->size() );
      for(auto scell : *inputSCells) {
         if ( scell->provenance() & m_qualBitMask ) scells->push_back( scell );
      }
   }
   else {
      scells->assign(inputSCells->begin(), inputSCells->end());
   }
   return StatusCode::SUCCESS;
}

StatusCode
LVL1::EFexEMAlgorithm::getTriggerTowerContainer(const xAOD::TriggerTowerContainer*& TTs) const
{
   SG::ReadHandle<xAOD::TriggerTowerContainer> inputTriggerTowers(m_inputTriggerTowerContainerKey);
   ATH_CHECK(inputTriggerTowers.isValid());
   TTs = inputTriggerTowers.cptr();
   if(TTs==nullptr) {
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}

StatusCode
LVL1::EFexEMAlgorithm::getTileCalCellsContainer(const CaloConstCellContainer*& tileCells) const
{
   SG::ReadHandle<CaloCellContainer> inputTileCells(m_inputTileCellContainerKey);
   ATH_CHECK(inputTileCells.isValid());
   CaloConstCellContainer* x = new CaloConstCellContainer(SG::VIEW_ELEMENTS);
   x->assign(inputTileCells->begin(), inputTileCells->end());
   tileCells = x;
   return StatusCode::SUCCESS;
}

StatusCode
LVL1::EFexEMAlgorithm::getIDHelper(const CaloCell_SuperCell_ID*& idHelper) const
{
   const CaloIdManager* caloMgr(0);
   ATH_CHECK(detStore()->retrieve(caloMgr));
   idHelper = caloMgr->getCaloCell_SuperCell_ID();
   if(!idHelper) {
      return StatusCode::FAILURE;
   }
   else return StatusCode::SUCCESS;
}

StatusCode
LVL1::EFexEMAlgorithm::getTileIDHelper(const TileID*& idHelper) const
{
   const CaloIdManager* caloMgr(0);
   if (detStore()->retrieve(caloMgr).isFailure()){
      return StatusCode::FAILURE;
   }
   idHelper = caloMgr->getTileID();
   if (!idHelper) return StatusCode::FAILURE;
   return StatusCode::SUCCESS;
}

StatusCode
LVL1::EFexEMAlgorithm::execute() {

   // supercells are used by both methods
   CaloConstCellContainer* scells{nullptr};
   ATH_CHECK(getSuperCellsContainer(scells));

   // Prepare output containers
   xAOD::TrigEMClusterContainer* clusters = new xAOD::TrigEMClusterContainer();
   xAOD::TrigEMClusterAuxContainer* auxclusters = new xAOD::TrigEMClusterAuxContainer();
   clusters->setStore(auxclusters);
   clusters->reserve(100);

   // ATH_CHECK(evtStore()->record(clusters, m_outputClusterName));
   // ATH_CHECK(evtStore()->record(auxclusters, m_outputClusterName+"Aux."));

   // standard cluster formation
   if ( ! m_energyWeightedCluster ) {

      const CaloConstCellContainer* tileCellCon{nullptr};
      const xAOD::TriggerTowerContainer* TTs{nullptr};
      const CaloCell_SuperCell_ID* idHelper{nullptr};
      const TileID* tileIDHelper{nullptr};
      if ( m_use_tileCells ) {
         ATH_CHECK(getTileCalCellsContainer(tileCellCon));
         ATH_CHECK(getTileIDHelper(tileIDHelper));
      }
      else {
         ATH_CHECK(getTriggerTowerContainer(TTs));
      }
      ATH_CHECK(getIDHelper(idHelper));

      std::vector<LVL1::EFexEMClusterTool::AlgResult> algResults = m_eFexAlgTool->clusterAlg(m_apply_BaseLineCuts, scells, TTs, idHelper, tileIDHelper, tileCellCon);
      for (const auto & algCl : algResults) {
         auto cl = new xAOD::TrigEMCluster();
         clusters->push_back( cl );
         cl->setEta( algCl.eta );
         cl->setPhi( algCl.phi );
         cl->setEt( algCl.clusterET );
         cl->setWstot( algCl.l1Width );
         if (algCl.hadET > -999) {
            cl->setEhad1( algCl.hadET );
         }
         cl->setE233( algCl.l2ClusterET33 );
         cl->setE237( algCl.l2ClusterET37 );
         SG::AuxElement::Accessor<float>("Run3REta")(*cl) = algCl.rEta;
         SG::AuxElement::Accessor<float>("Run3RHad")(*cl) = algCl.rHad;
         SG::AuxElement::Accessor<float>("Run3REtaL12")(*cl) = algCl.rEtaL12;
         // baseline cuts passed
         SG::AuxElement::Accessor<int>("PassRun3ClusterEnergy")(*cl) = algCl.passClusterEnergy;
         SG::AuxElement::Accessor<int>("PassRun3REta")(*cl) = algCl.passREta;
         SG::AuxElement::Accessor<int>("PassRun3RHad")(*cl) = algCl.passRHad;
         SG::AuxElement::Accessor<int>("PassRun3wstot")(*cl) = algCl.passWstot;

      }
   }
   // energy weighted cluster formation
   else {
      /////////////////////////////////////////////////////////////////
      // Note that there are several additional differences
      // on top of the different cluster formation:
      //   -- Method requires TT, if m_use_tileCells = false do not use
      //      energy weighted cluster formation
      //   -- The energy of the cluster is not given as multiples
      //      of the digitization scale (25 MeV)
      //   -- The cluster sizes differ per default (but can be adjusted)
      //   -- The definition of the lateral isolation wstot differs
      //   -- The isolation variables REta, RHad are not defined
      /////////////////////////////////////////////////////////////////

      const xAOD::TriggerTowerContainer* TTs{nullptr};
      if( !m_use_tileCells ) {
         ATH_CHECK(getTriggerTowerContainer(TTs));
      }

      std::vector<const CaloCell*> cellsAround;
      std::vector<const CaloCell*> cellsAboveThr;
      cellsAround.reserve(200);
      cellsAboveThr.reserve(200);

      // Check cells which are above a user defined threshold (default: 100MeV) in EMB2 or EMEC2
      m_eFexTool->findCellsAbove_EMB2_EMEC2(scells, m_seedE, cellsAboveThr);
      // Loop over possible seed cells
      for ( auto cellAbove : cellsAboveThr ) {
         // Builds a vector with all the cells around the seed cell with the default size (deta,dphi)=(0.08,0.21)
         m_eFexTool->findCellsAround(scells, cellAbove, cellsAround, m_deta_cellFormation, m_dphi_cellFormation);

         // Find cluster center (eta/phiCluster) based on the energy weighted scell position
         float etaCluster{0}, phiCluster{0};
         m_eFexTool->findCluster(cellsAround, etaCluster, phiCluster);
         if ( std::abs(etaCluster) > 998.0 ) {
            continue;
         }

         // Smaller cluster (closer to egamma standard)
         m_eFexTool->findCellsAround(scells, etaCluster, phiCluster, cellsAround, m_deta, m_dphi);
         // SElectron eta should be within the kinematic acceptance, default: Run 2 recommendations
         if ( std::abs(etaCluster) >= m_eta_limit ) {
            continue;
         }

         if ( ! m_eFexTool->isCellEmMaximum ( cellsAround, cellAbove ) ) continue;
         float clusterTime=0;
         float clusterTimeWeight=0;
         for( auto cellAround : cellsAround ) {
            if ( cellAround->et() < m_timeThr ) continue;
            clusterTime+=cellAround->time()*cellAround->et();
            clusterTimeWeight+=cellAround->et();
         }
         if ( std::abs(clusterTimeWeight) > 0.1 ) {
            clusterTime /= clusterTimeWeight;
         } else {
            clusterTime = -999.99;
         }
         ATH_MSG_DEBUG("CELL versus CLUSTER : " << cellAbove->eta() << " " << cellAbove->phi() << " " << etaCluster << " " << phiCluster << " " << cellAbove->eta()-etaCluster << " " << cellAbove->phi()-phiCluster);

         // Other cluster sizes for some of the shower shapes
         std::vector<const CaloCell*> cellsAround2;
         m_eFexTool->findCellsAround(scells, (float)etaCluster, (float)phiCluster, cellsAround2, m_deta_clusterFormation_2, m_dphi_clusterFormation_2);

         // Include TT (for Tile region only)
         std::vector<const xAOD::TriggerTower*> TTsAround;
         m_eFexTool->findTTsAround(TTs, etaCluster, phiCluster, TTsAround);

         float et = m_eFexTool->sumEmCells( cellsAround ) / TMath::CosH(cellAbove->eta());
         float clusterEmEnergy32 = m_eFexTool->sumEmCells2nd( cellsAround2 );
         if ( clusterEmEnergy32 < m_clusterE_EMB2_EMEC2 ) {
            continue;
         }

         float clusterEmEnergy72 = m_eFexTool->sumEmCells2nd( cellsAround );
         float clusterHadEnergy = m_eFexTool->sumHadCells( cellsAround ) + m_eFexTool->sumHadTTs( TTsAround );

         // build the cluster
         xAOD::TrigEMCluster* cl = new xAOD::TrigEMCluster();
         clusters->push_back( cl );
         for ( unsigned int i=0; i<(unsigned int)CaloSampling::CaloSample::Unknown; i++ ) {
            cl->setEnergy((CaloSampling::CaloSample)i,0.0);
         }
         cl->setEnergy( et * TMath::CosH(cellAbove->eta()) );
         cl->setEt( et );
         cl->setEta( cellAbove->eta() );
         cl->setPhi( cellAbove->phi() );
         cl->setE237 ( clusterEmEnergy32 );
         cl->setE277 ( clusterEmEnergy72 );
         cl->setEhad1 ( clusterHadEnergy );
         cl->setE233( clusterTime );
         float wstot=0.;
         float wstot_nor=0.;
         for( auto cellAround : cellsAround ) {
            unsigned int layer = cellAround->caloDDE()->getSampling();
            cl->setEnergy( (CaloSampling::CaloSample)layer, cl->energy( (CaloSampling::CaloSample)layer )+cellAround->energy() );
            if ( (layer==1) || (layer==5) ) {
               if ( cellAround->et() < 10 ) continue;
               wstot+=(cellAround->et()*pow(cellAround->eta()-etaCluster, 2));
               wstot_nor+=(cellAround->et());
            }
         }
         if ( std::abs(wstot_nor) > 0.01 ) wstot = sqrt(wstot/wstot_nor);
         cl->setWstot( wstot );
      }
   }

   SG::WriteHandle<xAOD::TrigEMClusterContainer> writeHandle( m_outputClusterContainerKey );
   ATH_CHECK( writeHandle.record( std::unique_ptr<xAOD::TrigEMClusterContainer>(clusters) ));

   if ( scells ) {
      scells->clear();
      delete scells;
   }
   return StatusCode::SUCCESS;
}
