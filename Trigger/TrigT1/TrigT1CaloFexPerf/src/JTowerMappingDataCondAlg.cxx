#include "JTowerMappingDataCondAlg.h"
#include "TH1F.h"
#include "TFile.h"
#include <memory>
#include "PathResolver/PathResolver.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

namespace LVL1
{
  JTowerMappingDataCondAlg::JTowerMappingDataCondAlg(const std::string &name, ISvcLocator *pSvcLocator)
      : JGTowerMappingDataCondAlgBase(name, pSvcLocator)
  {
  }

  JTowerMappingDataCondAlg::~JTowerMappingDataCondAlg() {}

  StatusCode JTowerMappingDataCondAlg::initialize()
  {
    ATH_CHECK(JGTowerMappingDataCondAlgBase::initialize());
    ATH_CHECK(detStore()->retrieve(m_jtowerID));

    std::unique_ptr<TFile> fIn(TFile::Open(
        PathResolver::find_calib_file(m_towerAreasFile.value()).c_str()));
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
    m_towerAreas.clear();
    m_towerAreas.reserve(areaHist->GetNbinsX());
    // Normalise the areas so that the first tower has an area of 1
    float scale = 1.0 / areaHist->GetBinContent(1);
    for (int towerIdx = 0; towerIdx < areaHist->GetNbinsX(); ++towerIdx)
      m_towerAreas.push_back(areaHist->GetBinContent(towerIdx + 1) * scale);

    return StatusCode::SUCCESS;
  }

  StatusCode JTowerMappingDataCondAlg::buildForwardMapping(
      JGTowerMappingData &data,
      const CaloSuperCellDetDescrManager *scDetMgr) const
  {
    ATH_MSG_INFO("# central JTowers " << data.size());
    // Create a single JTower per forward supercell
    for (std::size_t scHashIdx = 0; scHashIdx < m_scid->calo_cell_hash_max(); ++scHashIdx)
    {
      Identifier scID = m_scid->cell_id(scHashIdx);
      if (m_scid->is_tile(scID) && m_scid->sampling(scID) != 2)
        continue; // skipping tile SCs for now
      const CaloDetDescrElement *dde = scDetMgr->get_element(scID);
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
      data.push_back(std::move(helper));
    }
    ATH_MSG_INFO("# towers (including forward)");
    return StatusCode::SUCCESS;
  }

  StatusCode JTowerMappingDataCondAlg::loadTowerAreas(JGTowerMappingData &data) const
  {
    if (data.size() != m_towerAreas.size())
    {
      ATH_MSG_ERROR("Number of towers (" << data.size() << ") and number of tower areas (" << m_towerAreas.size() << ") do not match!");
      return StatusCode::FAILURE;
    }

    for (std::size_t idx = 0; idx < m_towerAreas.size(); ++idx)
      data.at(idx).setArea(m_towerAreas.at(idx));
    return StatusCode::SUCCESS;
  }
} // namespace LVL1