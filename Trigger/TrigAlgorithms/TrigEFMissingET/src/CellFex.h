/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   CellFex
 *
 * @brief Cell Fex class. Calculate MET from the negative vector sum over
 * calorimeter cells.
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_CELLFEX_H
#define TRIGEFMISSINGET_CELLFEX_H

#include "FexBase.h"
#include "TrigEFMissingET/METComponent.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCell.h"
#include "LArRecEvent/LArCell.h"
#include "TileEvent/TileCell.h"

#include <array>

namespace HLT { namespace MET {
  /****************************************************************************
   * @class CellFex
   *
   * Cell Fex class. Calculates MET from a sum over calo cells.
   ***************************************************************************/
  class CellFex : public FexBase {
    public:
      /// Constructor
      CellFex(const std::string& name, ISvcLocator* pSvcLocator);
      /// Destructor
      virtual ~CellFex() override;

      /// Initialize the fex
      virtual HLT::ErrorCode hltInitialize() override;

      /// Finalize the fex
      virtual HLT::ErrorCode hltFinalize() override;
    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Gives access to calo cells
      ToolHandle<ITrigDataAccess> m_trigData;
      /// The calorimeter noise tool
      ToolHandle<ICalorimeterNoiseTool> m_noiseTool;
      /// The absolute threshold
      float m_absNoiseThreshold;
      /// The negative threshold
      float m_negNoiseThreshold;
      /************************************************************************
       * Internals
       ***********************************************************************/
      virtual HLT::ErrorCode fillMET(
          xAOD::TrigMissingET& met,
          const std::vector<std::vector<HLT::TriggerElement*>>& tes_in) override;

      /// The number of samples that we use in this algorithm
      // TrigDataAccess only goes up to FCAL2. The 'mini FCal' samplings aren't
      // used.
      constexpr static std::size_t N_SAMPLES{CaloSampling::FCAL2+1};

      /**
       * @brief Process an individual LAr calorimeter cell
       * @param components The MET components to fill (split by sampling)
       * @param cell The LAr cell to process
       * @param error The error code from the data access tool
       */
      HLT::ErrorCode processCell(
          std::array<METComponent, N_SAMPLES>& components,
          const LArCell* cell,
          uint32_t error);

      /**
       * @brief Process an individual tile calorimeter cell
       * @param components The MET components to fill (split by sampling)
       * @param cell The tile cell to process
       * @param error The error code from the data access tool
       */
      HLT::ErrorCode processCell(
          std::array<METComponent, N_SAMPLES>& components,
          const TileCell* cell,
          uint32_t error);

      /**
       * @brief Process an individual calorimeter cell
       * @param components The MET components to fill (split by sampling)
       * @param cell The cell to process
       * @param isTile Whether or not this is a tile cell
       * @param error The error code from the data access tool
       */
      HLT::ErrorCode processCell(
          std::array<METComponent, N_SAMPLES>& components,
          const CaloCell* cell,
          bool isTile,
          uint32_t error);

  }; //> end class CellFex
} } //> end namespace HLT::MET

#endif //>! TRIGEFMISSINGET_CELLFEX_H
