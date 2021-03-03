// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/**
 * Name    : EFexTauAlgorithm.cxx
 * PACKAGE : Trigger/TrigT1/TrigT1CaloFexPerf
 * AUTHOR  : Denis Oliveira Damazio
 * PURPOSE : emulate the eFex tau algorithm for phase 1 L1Calo
 */

#include "EFexTauAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include <string>
#include "TH2F.h"
#include "TVector2.h"
#include "xAODTrigger/EmTauRoIAuxContainer.h"
#include "xAODTrigger/EmTauRoI.h"
#include "GaudiKernel/ThreadLocalContext.h"

namespace
{
   const static SG::AuxElement::Decorator<float> decR3ClusterET("R3ClusterET");
   const static SG::AuxElement::Decorator<float> decR3OreClusterET("R3_Ore_ClusterET");
   const static SG::AuxElement::Decorator<float> decR3BCClusterET("R3_BC_ClusterET");
   const static SG::AuxElement::Decorator<float> decR3BCClusterIso("R3_BC_ClusterIso");
   const static SG::AuxElement::Decorator<float> decR3OreClusterIso("R3_Ore_ClusterIso");
   const static SG::AuxElement::Decorator<bool> decR3OreIsoPass12("R3_Ore_ClusterIso_12pass");
   const static SG::AuxElement::Decorator<bool> decR3OreIsoPass20("R3_Ore_ClusterIso_20pass");
   const static SG::AuxElement::Decorator<bool> decR3BCIsoPass12("R3_BC_ClusterIso_12pass");
   const static SG::AuxElement::Decorator<bool> decR3BCIsoPass20("R3_BC_ClusterIso_20pass");
   const static SG::AuxElement::Decorator<float> decR3ClusterIso("R3ClusterIso");
} // namespace

LVL1::EFexTauAlgorithm::EFexTauAlgorithm(const std::string &name, ISvcLocator *pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
{
   declareProperty("InputSuperCellContainer", m_inputCellContainerKey = "SCell");
   declareProperty("InputTileCellContainer", m_inputTileCellContainerKey = "AllCalo");
   declareProperty("InputTriggerTowerContainer", m_inputTriggerTowerContainerKey = "xAODTriggerTowers");
   declareProperty("OutputClusterName", m_outputClusterName = "SCluster");

   declareProperty("UseTileCells", m_use_tileCells = false, "Use Tile cells instead of TriggerTowers");

   declareProperty("CleanCellContainerSkim", m_useProvenanceSkim = false);
   declareProperty("CleanCellContainer", m_useProvenance = true);
   declareProperty("QualBitMask", m_qualBitMask = 0x40);

   declareProperty("NominalDigitizationValue", m_nominalDigitization = 25.);
   declareProperty("NominalNoiseThreshold", m_nominalNoise_thresh = 100.);
   declareProperty("TimeThreshold", m_timeThr = 200);
}

LVL1::EFexTauAlgorithm::~EFexTauAlgorithm() = default;

StatusCode
LVL1::EFexTauAlgorithm::initialize()
{
   ATH_MSG_DEBUG("initialize");
   ATH_CHECK(m_inputCellContainerKey.initialize());
   if (m_use_tileCells)
   {
      ATH_CHECK(m_inputTileCellContainerKey.initialize());
   }
   else
   {
      ATH_CHECK(m_inputTriggerTowerContainerKey.initialize());
   }
   ATH_CHECK(m_outputClusterName.initialize());
   return StatusCode::SUCCESS;
}

float LVL1::EFexTauAlgorithm::CaloCellET(const CaloCell *const &inputCell, float digitScale, float digitThreshold) const
{
   if (inputCell == nullptr)
      return 0.;
   // Check that timing is correct
   if (m_useProvenance)
   {
      bool correctProv = (inputCell->provenance() & m_qualBitMask);
      if (!correctProv)
         return 0.;
   }
   // Calculates the ET (before digitization)
   float inputCell_energy = inputCell->energy();
   float inputCell_eta = inputCell->eta();
   float inputCell_ET = inputCell_energy / cosh(inputCell_eta);
   // Check to see if negative ET values are allowed
   bool allowNegs = false;
   if (digitScale < 0.)
   {
      digitScale = std::abs(digitScale);
      allowNegs = true;
   }
   if (inputCell_ET == 0)
      return 0.;
   else if (digitScale == 0)
      return inputCell_ET;
   if (allowNegs || inputCell_ET > 0.)
   {
      // Split up ET into magnitude & whether it's positive or negative
      float posOrNeg = inputCell_ET / std::abs(inputCell_ET);
      inputCell_ET = std::abs(inputCell_ET);
      // If no digitisation, return ET following noise cut
      if (digitScale == 0)
      {
         if (inputCell_ET > digitThreshold)
            return inputCell_ET * posOrNeg;
         else
            return 0.;
      }
      // Apply digitization & then noise cut
      else
      {
         float divET = inputCell_ET / digitScale;
         int roundET = divET;
         float result = digitScale * roundET;
         if (digitThreshold == 0)
            return result * posOrNeg;
         else if (result >= digitThreshold)
            return result * posOrNeg;
         else
            return 0;
      }
   }
   else
      return 0.;
}

StatusCode
LVL1::EFexTauAlgorithm::execute()
{
   const EventContext &ctx = Gaudi::Hive::currentContext();

   // supercells are used by both methods
   auto scellsHandle = SG::makeHandle(m_inputCellContainerKey, ctx);
   if (!scellsHandle.isValid())
   {
      ATH_MSG_ERROR("Failed to retrieve " << m_inputCellContainerKey.key());
      return StatusCode::FAILURE;
   }
   CaloConstCellContainer scells(SG::VIEW_ELEMENTS);
   if (m_useProvenanceSkim)
   {
      for (const CaloCell *scell : *scellsHandle)
         if (scell->provenance() & m_qualBitMask)
            scells.push_back(scell);
   }
   else
      scells.assign(scellsHandle->begin(), scellsHandle->end());

   CaloConstCellContainer tileCellCont(SG::VIEW_ELEMENTS);
   const xAOD::TriggerTowerContainer *TTs{nullptr};
   if (m_use_tileCells)
   {
      auto tileCellHandle = SG::makeHandle(m_inputTileCellContainerKey, ctx);
      if (!tileCellHandle.isValid())
      {
         ATH_MSG_ERROR("Failed to retrieve " << m_inputTileCellContainerKey.key());
         return StatusCode::FAILURE;
      }
      tileCellCont.assign(tileCellHandle->begin(), tileCellHandle->end());
   }
   else
   {
      auto triggerTowerHandle = SG::makeHandle(m_inputTriggerTowerContainerKey, ctx);
      if (!triggerTowerHandle.isValid())
      {
         ATH_MSG_ERROR("Failed to retrieve " << m_inputTriggerTowerContainerKey.key());
         return StatusCode::FAILURE;
      }
      TTs = triggerTowerHandle.cptr();
   }
   auto clustersForTau = std::make_unique<xAOD::EmTauRoIContainer>();
   auto auxClustersForTau = std::make_unique<xAOD::EmTauRoIAuxContainer>();
   clustersForTau->setStore(auxClustersForTau.get());

   // prepare all supercells vector in whole ATLAS detector
   std::vector<const CaloCell *> allSuperCells(scells.begin(), scells.end());

   // clear all TH2 histograms for supercell map
   TH2F supercellMapEM0("SupercellMapEM0", "Supercell map of EM0", 98, -4.9, 4.9, 64, 0, 2 * M_PI);
   TH2F supercellMapEM1("SupercellMapEM1", "Supercell map of EM1", 392, -4.9, 4.9, 64, 0, 2 * M_PI);
   TH2F supercellMapEM2("SupercellMapEM2", "Supercell map of EM2", 392, -4.9, 4.9, 64, 0, 2 * M_PI);
   TH2F supercellMapEM1_coarse("SupercellMapEM1_coarse", "Supercell map of EM1 coarse", 196, -4.9, 4.9, 64, 0, 2 * M_PI);
   TH2F supercellMapEM2_coarse("SupercellMapEM2_coarse", "Supercell map of EM2 coarse", 196, -4.9, 4.9, 64, 0, 2 * M_PI);
   TH2F supercellMapEM3("SupercellMapEM3", "Supercell map of EM3", 98, -4.9, 4.9, 64, 0, 2 * M_PI);
   TH2F supercellMapHAD("SupercellMapHAD", "Supercell map of HAD", 98, -4.9, 4.9, 64, 0, 2 * M_PI);
   TH2F supercellMapTWR("SupercellMapTWR", "Supercell map of TWR", 98, -4.9, 4.9, 64, 0, 2 * M_PI);

   int currentSampling = 0;
   float currentEta = 0;
   float currentPhi = 0;
   float currentCellEt = 0;

   // fill energy in all TH2 histograms for supercell map
   for (auto scell : allSuperCells)
   {
      currentSampling = scell->caloDDE()->getSampling();
      currentEta = scell->eta();
      currentPhi = TVector2::Phi_0_2pi(scell->phi());
      currentCellEt = CaloCellET(scell, m_nominalDigitization, m_nominalNoise_thresh);

      // Store maps per layer
      if (currentSampling == 0 || currentSampling == 4)
      {
         supercellMapEM0.Fill(currentEta, currentPhi, currentCellEt);
      }
      else if (currentSampling == 1 || currentSampling == 5)
      {
         supercellMapEM1.Fill(currentEta, currentPhi, currentCellEt);
         supercellMapEM1_coarse.Fill(currentEta, currentPhi, currentCellEt);
      }
      else if (currentSampling == 2 || currentSampling == 6)
      {
         supercellMapEM2.Fill(currentEta, currentPhi, currentCellEt);
         supercellMapEM2_coarse.Fill(currentEta, currentPhi, currentCellEt);
      }
      else if (currentSampling == 3 || currentSampling == 7)
      {
         supercellMapEM3.Fill(currentEta, currentPhi, currentCellEt);
      }
      else
      {
         supercellMapHAD.Fill(currentEta, currentPhi, currentCellEt);
      }

      // Store a map sum of all layers
      supercellMapTWR.Fill(currentEta, currentPhi, currentCellEt);
   }

   // Need to also loop over Run-I towers to get central region hadronic energy
   for (const xAOD::TriggerTower *tt : *TTs)
   {

      // Only use towers within 1.5, and in Tile
      if (tt->sampling() != 1 || std::abs(tt->eta()) > 1.5)
      {
         continue;
      }

      // Conversion into ET
      float cpET = tt->cpET() * 500.; // EM energy scale: 1 unit corresponds to 500 MeV
      if (cpET < 0.)
      {
         cpET = 0;
      }

      // Fill hadronic maps
      supercellMapHAD.Fill(tt->eta(), TVector2::Phi_0_2pi(tt->phi()), cpET);
      supercellMapTWR.Fill(tt->eta(), TVector2::Phi_0_2pi(tt->phi()), cpET);
   }

   // Find local maxima
   std::vector<TLorentzVector> localMaxima;
   localMaxima.reserve(200);

   // X is eta, Y is phi
   for (int i = 0; i < supercellMapTWR.GetNbinsX(); i++)
   {
      for (int j = 0; j < supercellMapTWR.GetNbinsY(); j++)
      {
         // Start by filtering out 'useless towers' (ie: anything less than a GeV)
         double towerET = supercellMapTWR.GetBinContent(i + 1, j + 1);
         if (towerET < 1000.)
            continue;

         // Check if the current tower has the largest ET in this 3x3 window

         // Need to be careful with wrap-around in phi, unfortunately.
         std::vector<double> binsAbove;
         binsAbove.reserve(4);
         std::vector<double> binsBelow;
         binsBelow.reserve(4);

         // Handle wrap-around in phi
         int aboveInPhi = j + 1;
         if (j == supercellMapTWR.GetNbinsY() - 1)
            aboveInPhi = 0;
         int belowInPhi = j - 1;
         if (j == 0)
            belowInPhi = supercellMapTWR.GetNbinsY() - 1;

         // The convention here is arbitrary, but needs to be mirrored
         // Take the cells in the next row in phi, and the one cell above in eta
         binsAbove.push_back(supercellMapTWR.GetBinContent(i, aboveInPhi + 1));
         binsAbove.push_back(supercellMapTWR.GetBinContent(i + 1, aboveInPhi + 1));
         binsAbove.push_back(supercellMapTWR.GetBinContent(i + 2, aboveInPhi + 1));
         binsAbove.push_back(supercellMapTWR.GetBinContent(i + 2, j + 1));

         // Inversely so for the bins below
         binsBelow.push_back(supercellMapTWR.GetBinContent(i, belowInPhi + 1));
         binsBelow.push_back(supercellMapTWR.GetBinContent(i + 1, belowInPhi + 1));
         binsBelow.push_back(supercellMapTWR.GetBinContent(i + 2, belowInPhi + 1));
         binsBelow.push_back(supercellMapTWR.GetBinContent(i, j + 1));

         bool isMax = true;

         // Check if it is a local maximum
         for (unsigned int k = 0; k < binsAbove.size(); k++)
         {
            if (towerET < binsAbove[k])
               isMax = false;
         }
         for (unsigned int k = 0; k < binsBelow.size(); k++)
         {
            if (towerET <= binsBelow[k])
               isMax = false;
         }

         if (isMax)
         {
            TLorentzVector myMaximum;
            myMaximum.SetPtEtaPhiM(towerET, supercellMapTWR.GetXaxis()->GetBinCenter(i + 1), supercellMapTWR.GetYaxis()->GetBinCenter(j + 1), 0);
            localMaxima.push_back(myMaximum);
         }
      }
   }

   if (msgLvl(MSG::DEBUG))
   {
      for (int i = 1; i < supercellMapEM1_coarse.GetNbinsX() + 1; i++)
      {
         for (int j = 1; j < supercellMapEM1_coarse.GetNbinsY() + 1; j++)
         {
            ATH_MSG_DEBUG("supercellMapEM1_coarse.GetBinContent(" << i << "," << j << ") " << supercellMapEM1_coarse.GetBinContent(i, j));
            ATH_MSG_DEBUG("supercellMapEM2_coarse.GetBinContent(" << i << "," << j << ") " << supercellMapEM2_coarse.GetBinContent(i, j));
            ATH_MSG_DEBUG("supercellMapEM2_coarse.GetXaxis()->GetBinCenter(" << i << ") " << supercellMapEM2_coarse.GetXaxis()->GetBinCenter(i));
            ATH_MSG_DEBUG("supercellMapEM2_coarse.GetYaxis()->GetBinCenter(" << j << ") " << supercellMapEM2_coarse.GetYaxis()->GetBinCenter(j));
         }
      }
   }

   // Now loop over local maxima, decide what to do
   for (auto myMaximum : localMaxima)
   {
      // Check eta bounds
      if (std::abs(myMaximum.Eta()) > 2.5)
      {
         continue;
      }
      // Cluster coordinates
      // Get eta coordinate for coarse layers EM0, EM3, HAD, and TWR
      int i = supercellMapTWR.GetXaxis()->FindFixBin(myMaximum.Eta());
      // Careful, ROOT conventions are -pi,pi
      int j = supercellMapTWR.GetYaxis()->FindFixBin(TVector2::Phi_0_2pi(myMaximum.Phi()));
      // Get eta coordinate for fine layers EM1 and EM2
      int i_fine_start = ((i - 1) * 4) + 1;
      int i_offset = 0;
      double i_max = 0;
      for (unsigned int i_off_cand = 0; i_off_cand < 4; i_off_cand++)
      {
         int i_et = supercellMapEM2.GetBinContent(i_fine_start + i_off_cand, j);
         if (i_et > i_max)
         {
            i_max = i_et;
            i_offset = i_off_cand;
         }
      }
      int i_fine = i_fine_start + i_offset;

      // Prepare Phi Wrap-around
      int aboveInPhi = j + 1;
      if (j == supercellMapTWR.GetNbinsY())
      {
         aboveInPhi = 1;
      }
      int belowInPhi = j - 1;
      if (j == 1)
      {
         belowInPhi = supercellMapTWR.GetNbinsY();
      }

      // Start calculating total energy
      // Use Fixed 2x2 cluster, 4 possibilities
      std::vector<double> allET;
      allET.reserve(4);

      double ET;
      // Up and right
      ET = supercellMapTWR.GetBinContent(i, j);
      ET += supercellMapTWR.GetBinContent(i + 1, j);
      ET += supercellMapTWR.GetBinContent(i, aboveInPhi);
      ET += supercellMapTWR.GetBinContent(i + 1, aboveInPhi);
      allET.push_back(ET);

      // Up and left
      ET = supercellMapTWR.GetBinContent(i, j);
      ET += supercellMapTWR.GetBinContent(i - 1, j);
      ET += supercellMapTWR.GetBinContent(i, aboveInPhi);
      ET += supercellMapTWR.GetBinContent(i - 1, aboveInPhi);
      allET.push_back(ET);

      // Down and left
      ET = supercellMapTWR.GetBinContent(i, j);
      ET += supercellMapTWR.GetBinContent(i - 1, j);
      ET += supercellMapTWR.GetBinContent(i, belowInPhi);
      ET += supercellMapTWR.GetBinContent(i - 1, belowInPhi);
      allET.push_back(ET);

      // Down and right
      ET = supercellMapTWR.GetBinContent(i, j);
      ET += supercellMapTWR.GetBinContent(i + 1, j);
      ET += supercellMapTWR.GetBinContent(i, belowInPhi);
      ET += supercellMapTWR.GetBinContent(i + 1, belowInPhi);
      allET.push_back(ET);

      // Pick largest resulting sum
      double eFEXOldCluster = 0;
      for (unsigned int k = 0; k < allET.size(); k++)
      {
         if (allET.at(k) > eFEXOldCluster)
            eFEXOldCluster = allET.at(k);
      }

      // Calculate Oregon algorithm reconstructed ET
      // Determine if the Oregon shapes, which are asymmetric in phi, should be oriented up or down
      bool sumAboveInPhi = true;

      // Sum the cells above in phi for EM1 and EM2
      double abovePhiCellET = supercellMapEM2.GetBinContent(i_fine, aboveInPhi);

      // Sum the cells below in phi for EM1 and EM2
      double belowPhiCellET = supercellMapEM2.GetBinContent(i_fine, belowInPhi);

      // If more energy deposited below in phi, then orient shapes downward
      if (belowPhiCellET > abovePhiCellET)
         sumAboveInPhi = false;

      // Hold the coordinate of the non-central phi row
      int offPhiCoordinate = sumAboveInPhi ? aboveInPhi : belowInPhi;

      // Construct 3x2 EM0 Oregon layer energy
      double em0OregonET = 0;
      em0OregonET += supercellMapEM0.GetBinContent(i, j);
      em0OregonET += supercellMapEM0.GetBinContent(i - 1, j);
      em0OregonET += supercellMapEM0.GetBinContent(i + 1, j);
      em0OregonET += supercellMapEM0.GetBinContent(i, offPhiCoordinate);
      em0OregonET += supercellMapEM0.GetBinContent(i - 1, offPhiCoordinate);
      em0OregonET += supercellMapEM0.GetBinContent(i + 1, offPhiCoordinate);

      // Construct 5x2 EM1 Oregon layer energy
      double em1OregonET = 0;
      em1OregonET += supercellMapEM1.GetBinContent(i_fine, j);
      em1OregonET += supercellMapEM1.GetBinContent(i_fine - 1, j);
      em1OregonET += supercellMapEM1.GetBinContent(i_fine - 2, j);
      em1OregonET += supercellMapEM1.GetBinContent(i_fine + 1, j);
      em1OregonET += supercellMapEM1.GetBinContent(i_fine + 2, j);
      em1OregonET += supercellMapEM1.GetBinContent(i_fine, offPhiCoordinate);
      em1OregonET += supercellMapEM1.GetBinContent(i_fine - 1, offPhiCoordinate);
      em1OregonET += supercellMapEM1.GetBinContent(i_fine - 2, offPhiCoordinate);
      em1OregonET += supercellMapEM1.GetBinContent(i_fine + 1, offPhiCoordinate);
      em1OregonET += supercellMapEM1.GetBinContent(i_fine + 2, offPhiCoordinate);

      // Construct 5x2 EM2 Oregon layer energy
      double em2OregonET = 0;
      em2OregonET += supercellMapEM2.GetBinContent(i_fine, j);
      em2OregonET += supercellMapEM2.GetBinContent(i_fine - 1, j);
      em2OregonET += supercellMapEM2.GetBinContent(i_fine - 2, j);
      em2OregonET += supercellMapEM2.GetBinContent(i_fine + 1, j);
      em2OregonET += supercellMapEM2.GetBinContent(i_fine + 2, j);
      em2OregonET += supercellMapEM2.GetBinContent(i_fine, offPhiCoordinate);
      em2OregonET += supercellMapEM2.GetBinContent(i_fine - 1, offPhiCoordinate);
      em2OregonET += supercellMapEM2.GetBinContent(i_fine - 2, offPhiCoordinate);
      em2OregonET += supercellMapEM2.GetBinContent(i_fine + 1, offPhiCoordinate);
      em2OregonET += supercellMapEM2.GetBinContent(i_fine + 2, offPhiCoordinate);

      // Construct 3x2 EM3 Oregon layer energy
      double em3OregonET = 0;
      em3OregonET += supercellMapEM3.GetBinContent(i, j);
      em3OregonET += supercellMapEM3.GetBinContent(i - 1, j);
      em3OregonET += supercellMapEM3.GetBinContent(i + 1, j);
      em3OregonET += supercellMapEM3.GetBinContent(i, offPhiCoordinate);
      em3OregonET += supercellMapEM3.GetBinContent(i - 1, offPhiCoordinate);
      em3OregonET += supercellMapEM3.GetBinContent(i + 1, offPhiCoordinate);

      // Construct 3x2 HAD Oregon layer energy
      double hadOregonET = 0;
      hadOregonET += supercellMapHAD.GetBinContent(i, j);
      hadOregonET += supercellMapHAD.GetBinContent(i - 1, j);
      hadOregonET += supercellMapHAD.GetBinContent(i + 1, j);
      hadOregonET += supercellMapHAD.GetBinContent(i, offPhiCoordinate);
      hadOregonET += supercellMapHAD.GetBinContent(i - 1, offPhiCoordinate);
      hadOregonET += supercellMapHAD.GetBinContent(i + 1, offPhiCoordinate);

      // Add individual layer energies to get the full Oregon reconstructed energy
      double eFEX_OregonET = em0OregonET + em1OregonET + em2OregonET + em3OregonET + hadOregonET;

      // Calculate Oregon algorithm isolation value
      // Construct inner 3x2 energy
      double oreIsoInnerET = 0;
      oreIsoInnerET += supercellMapEM2.GetBinContent(i_fine, j);
      oreIsoInnerET += supercellMapEM2.GetBinContent(i_fine - 1, j);
      oreIsoInnerET += supercellMapEM2.GetBinContent(i_fine + 1, j);
      oreIsoInnerET += supercellMapEM2.GetBinContent(i_fine, offPhiCoordinate);
      oreIsoInnerET += supercellMapEM2.GetBinContent(i_fine - 1, offPhiCoordinate);
      oreIsoInnerET += supercellMapEM2.GetBinContent(i_fine + 1, offPhiCoordinate);

      // Construct outer 9x2 energy using inner sum plus extra cells
      double oreIsoOuterET = oreIsoInnerET;
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine - 2, j);
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine - 3, j);
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine - 4, j);
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine + 2, j);
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine + 3, j);
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine + 4, j);
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine - 2, offPhiCoordinate);
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine - 3, offPhiCoordinate);
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine - 4, offPhiCoordinate);
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine + 2, offPhiCoordinate);
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine + 3, offPhiCoordinate);
      oreIsoOuterET += supercellMapEM2.GetBinContent(i_fine + 4, offPhiCoordinate);

      // Calculate isolation value as the ratio of inner over outer energies
      double eFEX_OregonIso = oreIsoInnerET / oreIsoOuterET;

      // Calculation of isolation cut values discussed here
      // https://indico.cern.ch/event/867020/contributions/3726146/attachments/2003208/3344698/L1CALOJoint03132020.pdf

      // Set boolean for whether event passes 12 GeV isolation cut, hardcoding isolation threshold for now
      bool eFEX_OregonIso_12pass = true;
      if (10000. < eFEX_OregonET && 15000. > eFEX_OregonET && eFEX_OregonIso < 0.69)
      {
         eFEX_OregonIso_12pass = false;
      }

      // Set boolean for whether event passes 20 GeV isolation cut, hardcoding isolation threshold for now
      bool eFEX_OregonIso_20pass = true;
      if (20000. < eFEX_OregonET && 25000. > eFEX_OregonET && eFEX_OregonIso < 0.61)
      {
         eFEX_OregonIso_20pass = false;
      }

      // Code Oregon cluster later
      // Will need to write a library of shape summation functions
      // EM0: 1x2, 4 possibilities
      // EM1: 5x2
      // EM2: 5x2
      // EM3: 3x2
      // HAD: 3x2

      //Code TLV bigCluster algorithm

      //make vectors with EM0/3/HAD energies in each of 3x3 bins
      //Make a function since this is disgusting but vim does it very fast
      std::vector<double> E_EM0;
      E_EM0.reserve(9);
      E_EM0.push_back(supercellMapEM0.GetBinContent(i, j));
      E_EM0.push_back(supercellMapEM0.GetBinContent(i - 1, j));
      E_EM0.push_back(supercellMapEM0.GetBinContent(i - 1, aboveInPhi));
      E_EM0.push_back(supercellMapEM0.GetBinContent(i, aboveInPhi));
      E_EM0.push_back(supercellMapEM0.GetBinContent(i + 1, aboveInPhi));
      E_EM0.push_back(supercellMapEM0.GetBinContent(i + 1, j));
      E_EM0.push_back(supercellMapEM0.GetBinContent(i + 1, belowInPhi));
      E_EM0.push_back(supercellMapEM0.GetBinContent(i, belowInPhi));
      E_EM0.push_back(supercellMapEM0.GetBinContent(i - 1, belowInPhi));
      std::vector<double> E_EM3;
      E_EM3.reserve(9);
      E_EM3.push_back(supercellMapEM3.GetBinContent(i, j));
      E_EM3.push_back(supercellMapEM3.GetBinContent(i - 1, j));
      E_EM3.push_back(supercellMapEM3.GetBinContent(i - 1, aboveInPhi));
      E_EM3.push_back(supercellMapEM3.GetBinContent(i, aboveInPhi));
      E_EM3.push_back(supercellMapEM3.GetBinContent(i + 1, aboveInPhi));
      E_EM3.push_back(supercellMapEM3.GetBinContent(i + 1, j));
      E_EM3.push_back(supercellMapEM3.GetBinContent(i + 1, belowInPhi));
      E_EM3.push_back(supercellMapEM3.GetBinContent(i, belowInPhi));
      E_EM3.push_back(supercellMapEM3.GetBinContent(i - 1, belowInPhi));
      std::vector<double> E_HAD;
      E_HAD.reserve(9);
      E_HAD.push_back(supercellMapHAD.GetBinContent(i, j));
      E_HAD.push_back(supercellMapHAD.GetBinContent(i - 1, j));
      E_HAD.push_back(supercellMapHAD.GetBinContent(i - 1, aboveInPhi));
      E_HAD.push_back(supercellMapHAD.GetBinContent(i, aboveInPhi));
      E_HAD.push_back(supercellMapHAD.GetBinContent(i + 1, aboveInPhi));
      E_HAD.push_back(supercellMapHAD.GetBinContent(i + 1, j));
      E_HAD.push_back(supercellMapHAD.GetBinContent(i + 1, belowInPhi));
      E_HAD.push_back(supercellMapHAD.GetBinContent(i, belowInPhi));
      E_HAD.push_back(supercellMapHAD.GetBinContent(i - 1, belowInPhi));
      std::vector<double> E_EM12_central;
      E_EM12_central.reserve(5);
      std::vector<double> E_EM12_above;
      E_EM12_above.reserve(6);
      std::vector<double> E_EM12_below;
      E_EM12_below.reserve(6);
      float seedEta = supercellMapTWR.GetXaxis()->GetBinCenter(i);
      int EM2seedBin = supercellMapEM2_coarse.GetXaxis()->FindBin(seedEta - 0.025);

      // Make a vector with the 5 possible energies in the central phi row
      E_EM12_central.push_back(supercellMapEM2_coarse.GetBinContent(EM2seedBin - 2, j) + supercellMapEM2_coarse.GetBinContent(EM2seedBin - 1, j) + supercellMapEM1_coarse.GetBinContent(EM2seedBin - 2, j) + supercellMapEM1_coarse.GetBinContent(EM2seedBin - 1, j));
      E_EM12_central.push_back(supercellMapEM2_coarse.GetBinContent(EM2seedBin - 1, j) + supercellMapEM2_coarse.GetBinContent(EM2seedBin - 0, j) + supercellMapEM1_coarse.GetBinContent(EM2seedBin - 1, j) + supercellMapEM1_coarse.GetBinContent(EM2seedBin - 0, j));
      E_EM12_central.push_back(supercellMapEM2_coarse.GetBinContent(EM2seedBin - 0, j) + supercellMapEM2_coarse.GetBinContent(EM2seedBin + 1, j) + supercellMapEM1_coarse.GetBinContent(EM2seedBin - 0, j) + supercellMapEM1_coarse.GetBinContent(EM2seedBin + 1, j));
      E_EM12_central.push_back(supercellMapEM2_coarse.GetBinContent(EM2seedBin + 1, j) + supercellMapEM2_coarse.GetBinContent(EM2seedBin + 2, j) + supercellMapEM1_coarse.GetBinContent(EM2seedBin + 1, j) + supercellMapEM1_coarse.GetBinContent(EM2seedBin + 2, j));
      E_EM12_central.push_back(supercellMapEM2_coarse.GetBinContent(EM2seedBin + 2, j) + supercellMapEM2_coarse.GetBinContent(EM2seedBin + 3, j) + supercellMapEM1_coarse.GetBinContent(EM2seedBin + 2, j) + supercellMapEM1_coarse.GetBinContent(EM2seedBin + 3, j));
      //For upper and lower phi rows take the bin with highest energy
      for (int k = -2; k <= 3; k++)
      {
         E_EM12_above.push_back(supercellMapEM2_coarse.GetBinContent(EM2seedBin + k, aboveInPhi) + supercellMapEM1_coarse.GetBinContent(EM2seedBin + k, aboveInPhi));
         E_EM12_below.push_back(supercellMapEM2_coarse.GetBinContent(EM2seedBin + k, belowInPhi) + supercellMapEM1_coarse.GetBinContent(EM2seedBin + k, belowInPhi));
      }

      sort(E_EM0.begin(), E_EM0.end());
      sort(E_EM3.begin(), E_EM3.end());
      sort(E_HAD.begin(), E_HAD.end());
      sort(E_EM12_central.begin(), E_EM12_central.end());
      sort(E_EM12_above.begin(), E_EM12_above.end());
      sort(E_EM12_below.begin(), E_EM12_below.end());

      double eFEX_BC = 0;

      //Three hottest bins in EM0
      eFEX_BC = E_EM0.at(E_EM0.size() - 1) + E_EM0.at(E_EM0.size() - 2) + E_EM0.at(E_EM0.size() - 3);
      //Two hottest bins in EM3
      eFEX_BC += E_EM3.at(E_EM3.size() - 1) + E_EM3.at(E_EM3.size() - 2);
      //Three hottest bins in HAD
      eFEX_BC += E_HAD.at(E_HAD.size() - 1) + E_HAD.at(E_HAD.size() - 2) + E_HAD.at(E_HAD.size() - 3);
      eFEX_BC += E_EM12_central.at(E_EM12_central.size() - 1);
      eFEX_BC += E_EM12_above.at(E_EM12_above.size() - 1);
      eFEX_BC += E_EM12_below.at(E_EM12_below.size() - 1);

      //Bigcluster isolation
      float nomeFEX_BCiso = 0;
      nomeFEX_BCiso += E_EM12_central.at(E_EM12_central.size() - 1);
      nomeFEX_BCiso += E_EM12_above.at(E_EM12_above.size() - 1);
      nomeFEX_BCiso += E_EM12_below.at(E_EM12_below.size() - 1);
      nomeFEX_BCiso += E_EM0.at(E_EM0.size() - 1);
      nomeFEX_BCiso += E_EM0.at(E_EM0.size() - 2);
      nomeFEX_BCiso += E_EM0.at(E_EM0.size() - 3);
      nomeFEX_BCiso += E_EM3.at(E_EM3.size() - 1);
      nomeFEX_BCiso += E_EM3.at(E_EM3.size() - 2);

      float denBCiso = 0;
      denBCiso += supercellMapTWR.GetBinContent(i, j);
      denBCiso += supercellMapTWR.GetBinContent(i - 1, j);
      denBCiso += supercellMapTWR.GetBinContent(i + 1, j);
      denBCiso += supercellMapTWR.GetBinContent(i, j + 1);
      denBCiso += supercellMapTWR.GetBinContent(i - 1, j + 1);
      denBCiso += supercellMapTWR.GetBinContent(i + 1, j + 1);
      denBCiso += supercellMapTWR.GetBinContent(i, j - 1);
      denBCiso += supercellMapTWR.GetBinContent(i - 1, j - 1);
      denBCiso += supercellMapTWR.GetBinContent(i + 1, j - 1);

      float eFEX_BCiso = nomeFEX_BCiso / denBCiso;

      // Set boolean for whether event passes 12 GeV isolation cut, hardcoding isolation threshold for now
      bool eFEX_BCiso_12pass = true;
      if (10000. < eFEX_BC && 15000. > eFEX_BC && eFEX_BCiso < 0.38)
      {
         eFEX_BCiso_12pass = false;
      }

      // Set boolean for whether event passes 20 GeV isolation cut, hardcoding isolation threshold for now
      bool eFEX_BCiso_20pass = true;
      if (20000. < eFEX_BC && 25000. > eFEX_BC && eFEX_BCiso < 0.18)
      {
         eFEX_BCiso_20pass = false;
      }

      // Calculate an EM2-based isolation
      // Center in EM2, offset to the right in eta:
      int em2i = supercellMapEM2.GetXaxis()->FindFixBin(myMaximum.Eta() + 0.05);
      // Careful, ROOT conventions are -pi,pi
      int em2j = supercellMapEM2.GetYaxis()->FindFixBin(TVector2::Phi_0_2pi(myMaximum.Phi()));

      float maximumET = supercellMapEM2.GetBinContent(em2i, em2j);
      int maximumCell = em2i;

      // Find maximum in EM2
      for (int k = -2; k < 2; k++)
      {
         float ETvalue = supercellMapEM2.GetBinContent(em2i + k, em2j);
         if (ETvalue > maximumET)
         {
            maximumET = ETvalue;
            maximumCell = em2i + k;
         }
      }

      // Find highest pT nearest cell, but we don't care which it is
      // Note that we don't need to worry about phi wrap-around
      float nextET = supercellMapEM2.GetBinContent(maximumCell + 1, em2j);
      nextET = ((supercellMapEM2.GetBinContent(maximumCell - 1, em2j) > nextET) ? supercellMapEM2.GetBinContent(maximumCell - 1, em2j) : nextET);
      nextET = ((supercellMapEM2.GetBinContent(maximumCell, em2j + 1) > nextET) ? supercellMapEM2.GetBinContent(maximumCell, em2j + 1) : nextET);
      nextET = ((supercellMapEM2.GetBinContent(maximumCell, em2j - 1) > nextET) ? supercellMapEM2.GetBinContent(maximumCell, em2j - 1) : nextET);

      float numerator = maximumET + nextET;

      // And now, run the full sum: avoid phi wrapping by converting back and forth
      float denominator = 0;
      float phicenter = supercellMapEM2.GetYaxis()->GetBinCenter(em2j);
      for (int eta = -6; eta < 6; eta++)
         for (int phi = -1; phi < 2; phi++)
            denominator += supercellMapEM2.GetBinContent(em2i + eta, supercellMapEM2.GetYaxis()->FindFixBin(TVector2::Phi_0_2pi(phicenter + (phi * 0.1))));

      // build cluster for tau
      //xAOD::EmTauRoI_v2* clForTau = new xAOD::EmTauRoI_v2();
      xAOD::EmTauRoI *clForTau = new xAOD::EmTauRoI();
      clustersForTau->push_back(clForTau);
      clForTau->setEta(myMaximum.Eta());
      clForTau->setPhi(myMaximum.Phi());

      decR3ClusterET(*clForTau) = eFEXOldCluster;
      decR3OreClusterET(*clForTau) = eFEX_OregonET;
      decR3BCClusterET(*clForTau) = eFEX_BC;
      decR3BCClusterIso(*clForTau) = denBCiso > 0 ? eFEX_BCiso : 0;
      decR3OreClusterIso(*clForTau) = oreIsoOuterET > 0 ? eFEX_OregonIso : 0;

      decR3OreIsoPass12(*clForTau) = eFEX_OregonIso_12pass;
      decR3OreIsoPass20(*clForTau) = eFEX_OregonIso_20pass;
      decR3BCIsoPass12(*clForTau) = eFEX_BCiso_12pass;
      decR3BCIsoPass20(*clForTau) = eFEX_BCiso_20pass;
      decR3ClusterIso(*clForTau) = denominator > 0 ? numerator / denominator : 0;
   }

   auto writeHandle = SG::makeHandle(m_outputClusterName, ctx);
   ATH_CHECK(writeHandle.record(std::move(clustersForTau), std::move(auxClustersForTau)));
   return StatusCode::SUCCESS;
}
