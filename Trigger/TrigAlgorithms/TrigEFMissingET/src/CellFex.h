/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   CellFex
 *
 * @brief Fex class for the basic cell algorithm
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_CELLFEX_H
#define TRIGEFMISSINGET_CELLFEX_H 1

#include "FexBase.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloConditions/CaloNoise.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloIdentifier/CaloCell_ID.h"


namespace HLT { namespace MET {
  /****************************************************************************
   * @class CellFex
   *
   * Class to create output from the cell algorithm
   *
   * cell calculates the MET with a sum over all calorimeter cells. A two-sided
   * noise cut is applied, requiring cells to have |E/S| > T1, E/S > -T2, where
   * T1 and T2 are thresholds, and S is the 1-sigma noise threshold.
   ***************************************************************************/
  class CellFex : public FexBase
  {
    public:
      /// Constructor
      CellFex(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the fex
      virtual StatusCode initialize() override;

    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Input cells
      SG::ReadHandleKey<CaloCellContainer> m_cellsKey{
        this, "CellName", "CaloCells", "Collection containing all input cells"};
      /// Calorimeter noise CDO (conditions data object)
      SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{
        this, "CaloNoiseName","totalNoise","SG Key of CaloNoise data object"};
      /// The threshold on the magnitude of the cell energy
      Gaudi::Property<float> m_absNoiseThreshold{
        this, "AbsoluteNoiseThreshold", 2, "Threshold on the magnitude of the "
          "cell energy (as a multiple of the cell noise level). Selection "
          "will not be applied if value is negative"};
      /// The maximum negative cell energy
      Gaudi::Property<float> m_negNoiseThreshold{
        this, "NegativeNoiseThreshold", 5, "The maximum negative cell energy. "
          "Selection will not be applied if value is negative"};
      /// Use the 'two-gaussian' noise calculation for the TileCal
      Gaudi::Property<bool> m_doTwoGaussianNoise{
        this, "TwoGaussianNoise", true,
          "Whether to use the 'two-Gaussian' noise calculation for the TileCal"};

      /************************************************************************
       * Internal functions
       ***********************************************************************/
      /**
       * @brief Calculate and fill the output MET value
       * @param met The object to fill
       * @param context The event context
       * @param monitors[out] Any extra monitors to fill
       */
      virtual StatusCode fillMET(
          xAOD::TrigMissingET& met,
          const EventContext& context,
          MonGroupBuilder& monitors) const override;

      /************************************************************************
       * Data members
       ***********************************************************************/
      /// Fallback option for calo cells which don't have a detector description
      /// - we can read the information from this object
      const CaloCell_ID* m_caloCellID{nullptr};
  }; //> end class FexBase
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_CELLFEX_H
