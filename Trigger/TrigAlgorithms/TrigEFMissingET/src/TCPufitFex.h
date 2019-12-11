/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   TCPufitFex
 *
 * @author Bob Kowalewski, Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_TCPUFITFEX_H
#define TRIGEFMISSINGET_TCPUFITFEX_H

#include "FexBase.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class TCPufitFex
   *
   * pufit FEX class. Calculates MET using topoclusters applying a fit to remove
   * pileup contributions.
   ***************************************************************************/
  class TCPufitFex : public FexBase {
    public:
      /// Constructor
      TCPufitFex(const std::string& name, ISvcLocator* pSvcLocator);
      /// Destructor
      virtual ~TCPufitFex() override;

      /// Initialize the fex
      virtual HLT::ErrorCode hltInitialize() override;

      /// Finalize the fex
      virtual HLT::ErrorCode hltFinalize() override;
    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
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
  }; //> end class TCPufitFex
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_TCPUFITFEX_H
