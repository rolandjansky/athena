/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   CellPufitFex
 *
 * @brief Cell-pufit Fex class. Calculate MET from the negative vector sum over
 * calorimeter cells, applying a pufit-based correction.
 * @author Kenji Hamano, Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_CELLPUFITFEX_H
#define TRIGEFMISSINGET_CELLPUFITFEX_H

#include "FexBase.h"
#include "TrigEFMissingET/PufitGrid.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "CaloEvent/CaloCell.h"
#include "LArRecEvent/LArCell.h"
#include "TileEvent/TileCell.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class CellPufitFex
   *
   * Cell-pufit Fex class. Calculates MET from a sum over calo cells with a
   * pufit based correction.
   ***************************************************************************/
  class CellPufitFex : public FexBase {
    public:
      /// Constructor
      CellPufitFex(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the fex
      virtual HLT::ErrorCode hltInitialize() override;

      /// Finalise the fex
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
      /// How many sigmas over the mean to set the threshold
      double m_nSigma;
      /// The eta range to use
      double m_maxEta;
      /// The number of eta bins
      std::size_t m_nEtaBins;
      /// The number of phi bins
      std::size_t m_nPhiBins;
      /// The trim fraction to use
      double m_trimFraction;
      /// The noise term in the calorimeter noise estimate
      double m_caloNoise;
      /// The stochastic term in the calorimeter noise estimate
      double m_caloStoch;
      /// The relative importance of the different constraints
      double m_constraintImportance;
      /************************************************************************
       * Internals
       ***********************************************************************/
      virtual HLT::ErrorCode fillMET(
          xAOD::TrigMissingET& met,
          const std::vector<std::vector<HLT::TriggerElement*>>& tes_in) override;

      /**
       * @brief Process an individual LAr calorimeter cell
       * @param gridset The pufit grid set to fill
       * @param cell The LAr cell to process
       * @param error The error code from the data access tool
       */
      HLT::ErrorCode processCell(
          PufitGridSet& gridset,
          const LArCell* cell);

      /**
       * @brief Process an individual tile calorimeter cell
       * @param gridset The pufit grid set to fill
       * @param cell The tile cell to process
       * @param error The error code from the data access tool
       */
      HLT::ErrorCode processCell(
          PufitGridSet& gridset,
          const TileCell* cell);

      /**
       * @brief Process an individual calorimeter cell
       * @param gridset The pufit grid set to fill
       * @param cell The cell to process
       * @param isTile Whether or not this is a tile cell
       */
      HLT::ErrorCode processCell(
          PufitGridSet& gridset,
          const CaloCell* cell,
          bool isTile);

  }; //> end class CellPufitFex

} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_CELLPUFITFEX_H
