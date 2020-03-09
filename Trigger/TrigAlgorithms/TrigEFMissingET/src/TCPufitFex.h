/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   TCPufitFex
 *
 * @brief Fex class for the (tc) pufit algorithm
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_TCPUFITFEX_H
#define TRIGEFMISSINGET_TCPUFITFEX_H

#include "FexBase.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class TCPufitFex
   *
   * Class to calculate MET using the pufit algorithm
   *
   * pufit calculates the MET with a sum over trigger-level topoclusters,
   * applying a fitting procedure to estimate the contribution from pileup.
   ***************************************************************************/
  class TCPufitFex : public FexBase
  {
    public:
      /// Constructor
      TCPufitFex(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the fex
      virtual StatusCode initialize() override;

    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Input clusters
      SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterKey{
        this, "ClusterName", "clusters", "Input cluster collection"};
      /// The sigma threshold
      Gaudi::Property<float> m_nSigma{
        this, "NSigma", 5, "Set the threshold at mean + NSigma*variance"};
      /// The eta range of the grid
      Gaudi::Property<float> m_maxEta{
        this, "MaxEta", 5, "The maximum eta range"};
      /// The number of bins in eta
      Gaudi::Property<std::size_t> m_nEtaBins{
        this, "NEtaBins", 14, "The number of eta bins"};
      /// The number of bins in phi
      Gaudi::Property<std::size_t> m_nPhiBins{
        this, "NPhiBins", 8, "The number of phi bins"};
      /// The trimming fraction
      Gaudi::Property<float> m_trimFraction{
        this, "TrimFraction", 0.9,
        "The fraction of bins to use when calculating the mean and variance"};
      /// The coefficient of the noise term in the calo resolution estimate
      Gaudi::Property<float> m_caloNoise{
        this, "CaloNoise", 50, "The coefficient of the noise term in the "
          "calorimeter resolution estimate [MeV]"};
      /// The coefficient of the stochastic term in the calo resolution estimate
      Gaudi::Property<float> m_caloStoch{
        this, "CaloStochastic", 15.81, "The coefficient of the stochastic "
          "term in the calorimeter resolution estimate [MeV^1/2]"};
      /// The relative constraint importance
      Gaudi::Property<float> m_constraintImportance{
        this, "ConstraintImportance", 1,
        "The relative importance of the two constraints to the fit"};

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
  }; //> end class TCPufitFex
} } //> end namespace HLT::MET
#endif //> !TRIGEFMISSINGET_TCPUFITFEX_H
