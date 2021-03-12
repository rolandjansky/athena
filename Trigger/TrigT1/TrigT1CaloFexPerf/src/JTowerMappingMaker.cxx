/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JTowerMappingMaker.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "TFile.h"
#include "TH1F.h"
#include <memory>
#include "PathResolver/PathResolver.h"

namespace LVL1
{
  JTowerMappingMaker::JTowerMappingMaker(const std::string &type, const std::string &name, const IInterface *parent)
      : JGTowerMappingMakerBase(type, name, parent)
  {
    declareProperty("TowerAreasFile", m_towerAreasFile = "Run3L1CaloSimulation/Noise/jTowerCorrection.20210308.r12406.root"); 
  }

  JTowerMappingMaker::~JTowerMappingMaker() {}

  StatusCode JTowerMappingMaker::initialize()
  {
    ATH_CHECK(JGTowerMappingMakerBase::initialize());
    ATH_CHECK(detStore()->retrieve(m_jTowerId));
    ATH_CHECK(buildMapping(m_jTowerId));
    return StatusCode::SUCCESS;
  }

  StatusCode JTowerMappingMaker::buildForwardMapping()
  {
    // Create a single JTower per forward supercell
    for (std::size_t scHashIdx = 0; scHashIdx < m_scid->calo_cell_hash_max(); ++scHashIdx)
    {
      Identifier scID = m_scid->cell_id(scHashIdx);
      if (m_scid->is_tile(scID) && m_scid->sampling(scID) != 2)
        continue; // skipping tile SCs for now
      const CaloDetDescrElement *dde = m_scDetMgr->get_element(scID);
      if (!dde)
      {
        ATH_MSG_ERROR("Error loading CaloDetDescrElement");
        return StatusCode::FAILURE;
      }
      float superCellEta = dde->eta_raw();
      float superCellPhi = dde->phi_raw();
      if (std::abs(superCellEta) < 3.1)
        continue;
      float superCellDEta = dde->deta();
      float superCellDPhi = dde->dphi();
      JGTowerHelper helper(superCellEta, superCellDEta, superCellPhi, superCellDPhi);
      helper.SetSCIndices(scHashIdx);

      std::string detName = CaloSampling::getSamplingName(m_ccid->calo_sample(scID));
      if (detName.find("FCAL0") != std::string::npos)
        helper.SetSampling(2);
      else if (detName.find("FCAL1") != std::string::npos)
        helper.SetSampling(3);
      else if (detName.find("FCAL2") != std::string::npos)
        helper.SetSampling(4);
      else
        helper.SetSampling(0);
      m_mapping.push_back(std::move(helper));
    }

    return StatusCode::SUCCESS;
  }

  StatusCode JTowerMappingMaker::loadTowerAreas()
  {

    std::unique_ptr<TFile> fIn(TFile::Open(PathResolver::find_calib_file(m_towerAreasFile).c_str()));
    if (!fIn || fIn->IsZombie())
    {
      ATH_MSG_ERROR("Failed to open tower areas file " << m_towerAreasFile);
      return StatusCode::FAILURE;
    }
    TH1F *areaHist = (TH1F *)fIn->Get("jTowerArea_final_hist");
    if (!areaHist)
    {
      ATH_MSG_ERROR("Failed to read area histogram!");
      return StatusCode::FAILURE;
    }
    if (std::size_t(areaHist->GetNbinsX()) != m_mapping.size())
    {
      ATH_MSG_WARNING("Number of area values does not match the expected number of towers");
      ATH_MSG_WARNING("Missing values will be replaced with " << 1e9 << " but even present ones may be incorrect");
    }
    // Normalise the areas so that the first tower has an area of 1
    float scale = 1.0 / areaHist->GetBinContent(1);
    for (std::size_t towerIdx = 0; towerIdx < m_mapping.size(); ++towerIdx)
    {
      if (towerIdx < std::size_t(areaHist->GetNbinsX()))
        m_mapping.at(towerIdx).setArea(areaHist->GetBinContent(towerIdx + 1) * scale);
      else
        // Area is mostly used as a way to downweight contributions, so we set a high number
        // meaning that where we don't have data we don't use that value
        m_mapping.at(towerIdx).setArea(9e9);
    }
    return StatusCode::SUCCESS;
  }
} // namespace LVL1
