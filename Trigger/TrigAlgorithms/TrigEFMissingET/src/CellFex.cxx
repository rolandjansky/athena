/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "CellFex.h"
#include "TrigEFMissingET/StatusFlags.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include "TrigInterfaces/HLTCheck.h"
#include <numeric>

namespace HLT { namespace MET {
  CellFex::CellFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator),
    m_trigData("TrigDataAccess/TrigDataAccess")
  {
    declareProperty("TrigDataAccess", m_trigData,
        "The tool used to access the calorimeter cells");
    declareProperty("CaloNoiseTool", m_noiseTool,
        "The tool used to access calorimeter noise levels");
    declareProperty("AbsoluteNoiseThreshold", m_absNoiseThreshold=2.0,
        "The threshold on the cell absolute energy");
    declareProperty("NegativeNoiseThreshold", m_negNoiseThreshold=5.0,
        "The maximum negative cell energy");
  }

  CellFex::~CellFex() {}

  HLT::ErrorCode CellFex::hltInitialize()
  {
    if (!m_trigData.retrieve().isSuccess() ) {
      ATH_MSG_ERROR("Failed to retrieve TrigDataAccess");
      return HLT::ERROR;
    }
    if (!m_noiseTool.retrieve().isSuccess() ) {
      ATH_MSG_ERROR("Failed to retrieve Calo noise tool");
      return HLT::ERROR;
    }
    std::vector<std::string> componentNames;
    componentNames.reserve(N_SAMPLES);
    for (std::size_t ii = 0; ii < N_SAMPLES; ++ii)
      componentNames.push_back(CaloSampling::getSamplingName(ii) );
    return initializeBase(
        componentNames,
        std::make_unique<METComponentMonitor>(this) );
  }

  HLT::ErrorCode CellFex::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode CellFex::fillMET(
      xAOD::TrigMissingET& met,
      const std::vector<std::vector<HLT::TriggerElement*>>& tes_in)
  {
    // Verify the inputs
    if (tes_in.size() != 0) {
      ATH_MSG_ERROR("Unexpected number " << tes_in.size() << " of TEs received!");
      return HLT::NAV_ERROR;
    }

    // Make the MET components
    std::array<METComponent, N_SAMPLES> cellSums;

    // Start with the different LAr calorimeters
    for (DETID detectorID : {TTEM, TTHEC, FCALEM, FCALHAD}) {
      LArTT_Selector<LArCellCont>::const_iterator itr;
      LArTT_Selector<LArCellCont>::const_iterator end;
      // retrieve the cells
      if (!m_trigData->LoadFullCollections(itr, end, detectorID).isSuccess() ) {
        // Needs a proper message
        ATH_MSG_ERROR("Failed to retrieve cells!");
        return HLT::ERROR;
      }
      uint32_t errorCode = m_trigData->report_error();
      for (; itr != end; ++itr)
        HLT_CHECK( processCell(cellSums, *itr, errorCode) );
    }

    // Now do the same for Tile
    for (std::size_t ii = 0; ii < m_trigData->TileFullContSize(); ++ii) {
      TileCellCollection::const_iterator itr;
      TileCellCollection::const_iterator end;
      if (!m_trigData->LoadFullCollections(itr, end, ii).isSuccess() ) {
        // Needs a proper message
        ATH_MSG_ERROR("Failed to retrieve cells!");
        return HLT::ERROR;
      }
      uint32_t errorCode = m_trigData->report_error();
      for (; itr != end; ++itr)
        HLT_CHECK( processCell(cellSums, *itr, errorCode) );
    }

    // Save the total
    std::accumulate(cellSums.begin(), cellSums.end(), METComponent{}).fillMET(met);
    // Save each component
    for (std::size_t ii = 0; ii < N_SAMPLES; ++ii)
      cellSums.at(ii).fillMETComponent(ii, met);

    return HLT::OK;
  }

  HLT::ErrorCode CellFex::processCell(
      std::array<METComponent, N_SAMPLES>& components,
      const LArCell* cell,
      uint32_t error)
  {
    return processCell(components, cell, false, error);
  }

  HLT::ErrorCode CellFex::processCell(
      std::array<METComponent, N_SAMPLES>& components,
      const TileCell* cell,
      uint32_t error)
  {
    return processCell(components, cell, true, error);
  }

  HLT::ErrorCode CellFex::processCell(
      std::array<METComponent, N_SAMPLES>& components,
      const CaloCell* cell,
      bool isTile,
      uint32_t error)
  {
    // Make the noise selection. This part looks different between tile and LAr
    // but the two functions do pretty similar things, although without
    // identical results.

    // The noise selection logic is to reject a cell if
    // |E| < m_absNoiseThreshold* sigma or 
    // E < -m_negNoiseThreshold * sigma
    if (isTile) {
      if (!m_noiseTool->isEOverNSigma(
            cell, m_absNoiseThreshold,
            ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
            ICalorimeterNoiseTool::TOTALNOISE) )
        return HLT::OK;
      if (cell->e() < 0 && m_noiseTool->isEOverNSigma(
            cell, m_negNoiseThreshold,
            ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
            ICalorimeterNoiseTool::TOTALNOISE) )
        return HLT::OK;
    }
    else {
      double noise = m_noiseTool->getNoise(cell, ICalorimeterNoiseTool::TOTALNOISE);
      if (fabs(cell->e()) < m_absNoiseThreshold*noise ||
          cell->e() < -m_negNoiseThreshold*noise)
        return HLT::OK;
    }
    
    // Get the right component
    METComponent& component = components.at(cell->caloDDE()->getSampling() );
    // Get the cell kinematics. Note that we can't just use a TLorentz vector
    // here as the Et value from that is positive definite and (at least the old
    // code) allows for negative Et values (i.e sumEt/sumE can go down)
    const CaloDetDescrElement* dde = cell->caloDDE();
    component += SignedKinematics::fromEnergyEtaPhi(
        cell->e(), dde->eta(), dde->phi() );
    if (error)
      // TODO - right now we treat all BSErrors the same. Maybe we want to split
      // our treatment of that
      component.status |= StatusFlag::BSConvError;
    return HLT::OK;
  }
} } //> end namespace HLT::MET
