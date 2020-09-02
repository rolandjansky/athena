/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   MHTPufitFex
 *
 * @brief Fex class for the mhtpufit algorithm, using the pufit procedure to
 * correct the MET produced by a sum over jets
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_MHTPUFITFEX_H
#define TRIGEFMISSINGET_MHTPUFITFEX_H

#include "FexBase.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODEventShape/EventShape.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace HLT
{
  namespace MET
  {
    /**************************************************************************
     * @class MHTPufitFex
     * 
     * Class to calculate MET from a sum over jets using a correction derived
     * with the pufit technique
     *************************************************************************/
    class MHTPufitFex : public FexBase
    {
    public:
      /// Constructor
      MHTPufitFex(const std::string &name, ISvcLocator *pSvcLocator);

      /// Initialize the fex
      virtual StatusCode initialize() override;

    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Input objects
      SG::ReadHandleKey<xAOD::JetContainer> m_inputJetsKey{
          this, "InputJetsName", "", "The input jet container"};
      /// The input JVT decoration
      SG::ReadDecorHandleKey<xAOD::JetContainer> m_inputJvtKey{
          this, "InputJvtName", "Jvt", "The input JVT name"};
      /// The input clusters or PFOs
      SG::ReadHandleKey<xAOD::IParticleContainer> m_inputKey{
          this, "InputName", "", "The input clusters or PFOs"};
      Gaudi::Property<bool> m_jetCalibIncludesAreaSub{
          this, "JetCalibIncludesAreaSub", true,
          "Whether the calibration applied to the jets includes area subtraction"};
      SG::ReadHandleKey<xAOD::EventShape> m_rhoKey{
          this, "JetEventShapeName", "",
          "The name of the event shape container for the area correction"};
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
      // Properties relating to jet selection
      Gaudi::Property<float> m_jvtCut{
          this, "JvtCut", 0.59, "The JVT selection in the central region"};
      Gaudi::Property<float> m_minPt{
          this, "MinPt", 20*Gaudi::Units::GeV, "The minimum pT (in the central region)"};
      Gaudi::Property<float> m_maxPt{
          this, "MaxPt", 120*Gaudi::Units::GeV,
          "The maximum pT (in the central region), above which the JVT selection is not applied"};
      Gaudi::Property<float> m_forwardPt{
          this, "ForwardPt", 30*Gaudi::Units::GeV, "The minimum pt in the forward region"};
      Gaudi::Property<float> m_centralEta{
          this, "CentralEta", 2.4, "The boundary between the central and border regions"};
      Gaudi::Property<bool> m_useDetectorEta{
          this, "UseDetectorEta", true, "Whether to use the 'DectectorEta' value to select central/forward jets"};
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
    }; //> end class MHTPufitFex
  }    // namespace MET
} // namespace HLT

#endif //> !TRIGEFMISSINGET_MHTPUFITFEX_H