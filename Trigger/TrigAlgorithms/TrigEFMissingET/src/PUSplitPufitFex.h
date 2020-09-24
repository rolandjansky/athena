/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   PUSplitPufitFex
 *
 * @brief Fex class for the pufit algorithm, where inputs are split into 
 * pile-up categories
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_PUSPLITPUFITFEX_H
#define TRIGEFMISSINGET_PUSPLITPUFITFEX_H

#include "FexBase.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include "xAODBase/IParticleContainer.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "TrigEFMissingET/PUClassification.h"
#include "Gaudi/Property.h"

namespace HLT
{
  namespace MET
  {
    /**************************************************************************
     * @class PUSplitPufitFex
     * 
     * Class to calculate pufit MET using pu-split inputs
     * 
     * Uses a generalised pufit technique. The inputs are classified using the
     * @see PUClassification enum and the algorithm is able to treat them
     * differently
     *************************************************************************/
    class PUSplitPufitFex : public FexBase
    {
    public:
      /// Constructor
      PUSplitPufitFex(const std::string &name, ISvcLocator *pSvcLocator);

      /// Initialize the fex
      virtual StatusCode initialize() override;

    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Input objects
      SG::ReadHandleKey<xAOD::IParticleContainer> m_inputKey{
          this, "InputName", "", "The input particle collection"};
      SG::ReadDecorHandleKey<xAOD::IParticleContainer> m_inputCategoryKey{
          this, "InputCategoryName", "PUCategory",
          "The name of PU classification auxdata"};
      /// The sigma threshold
      Gaudi::Property<float> m_nSigma{
          this, "NSigma", 5, "Set the threshold at mean + NSigma*variance"};
      /// The eta range of the grid
      Gaudi::Property<float> m_maxEta{
          this, "MaxEta", 5, "The maximum eta range"};
      /// The number of bins in eta
      Gaudi::Property<std::size_t> m_nEtaBins{
          this, "NEtaBins", 14, "The number of eta bins"};
      Gaudi::Property<std::size_t> m_nPhiBins{
          this, "NPhiBins", 8, "The number of phi bins"};
      /// The trimming fraction
      Gaudi::Property<float> m_trimFraction{
          this, "TrimFraction", 0.9,
          "The fraction of bins to use when calculating the mean and variance"};
      /// The coefficient of the noise term in the calo resolution estimate
      Gaudi::Property<float> m_caloNoise{
          this, "CaloNoise", 50,
          "The coefficient of the noise term in the calorimeter resolution estimate [MeV]"};
      /// The coefficient of the stochastic term in the calo resolution estimate
      Gaudi::Property<float> m_caloStoch{
          this, "CaloStochastic", 15.81,
          "The coefficient of the stochastic term in the calorimeter resolution estimate [MeV^1/2]"};
      /// The relative constraint importance
      Gaudi::Property<float> m_constraintImportance{
          this, "ConstraintImportance", 1,
          "The relative importance of the two constraints to the fit"};
      /// The neutral threshold mode
      Gaudi::Property<std::size_t> m_neutralThresholdMode{
          this, "NeutralThresholdMode", PUClassification::All,
          "Which towers to use to calculate mean/variance and the masking threshold"};
      /// Whether to remove the cPU component from the tower expectations
      Gaudi::Property<bool> m_subtractCPUFromMean{
          this, "SubtractCPUFromMean", false,
          "Whether to remove the cPU component from the expected pileup value"};
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
          xAOD::TrigMissingET &met,
          const EventContext &context,
          MonGroupBuilder &monitors) const override;

      /**
       * @brief Calculate the estimate on the variance of a tower
       * @param kin The kinematics of the tower
       */
      double getSigma(const SignedKinematics &kin) const;
    }; //> end class PUSplitPufixFex
  }    // namespace MET
} // namespace HLT

#endif //> !TRIGEFMISSINGET_PUSPLITPUFITFEX_H