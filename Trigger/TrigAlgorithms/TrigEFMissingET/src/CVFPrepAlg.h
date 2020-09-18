/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   CVFPrepAlg
 *
 * @brief Algorithm to prepare clusters for the PUSplit pufit alg
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_CVFPREPALG_H
#define TRIGEFMISSINGET_CVFPREPALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "Gaudi/Property.h"

namespace HLT
{
  namespace MET
  {
    /**************************************************************************
     * @class PFOPrepAlg
     * 
     * Class to prepare the clusters for the PUSplit pufit alg.
     * 
     * This just uses the CVF value per cluster to add the necessary
     * classification decoration
     **************************************************************************/
    class CVFPrepAlg : public AthReentrantAlgorithm
    {
    public:
      /// Constructor
      CVFPrepAlg(const std::string &name, ISvcLocator *pSvcLocator);

      virtual StatusCode initialize() override;

      virtual StatusCode execute(const EventContext &ctx) const override;

    private:
      /// The input container
      SG::ReadHandleKey<xAOD::CaloClusterContainer> m_inputClusterKey{
          this, "InputClusterKey", "", "Input cluster container"};
      /// The input CVF key
      SG::ReadDecorHandleKey<xAOD::CaloClusterContainer> m_inputCVFKey{
          this, "InputCVFKey", "", "Input CVF key"};
      /// The output classifcation
      SG::WriteDecorHandleKey<xAOD::CaloClusterContainer> m_outputCategoryKey{
          this, "OutputCategoryKey", "", "Output category name"};
      /// The CVF threshold
      Gaudi::Property<float> m_cvfThreshold{
          this, "CVFThreshold", 0.5, "The CVF threshold to apply"};
    }; //> end class CVFPrepAlg
  }    // namespace MET
} // namespace HLT

#endif //> !TRIGEFMISSINGET_CVFPREPALG_H