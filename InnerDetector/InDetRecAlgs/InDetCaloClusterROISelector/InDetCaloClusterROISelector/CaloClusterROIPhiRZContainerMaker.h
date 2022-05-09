/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_CALOCLUSTERROIPHIRZCOLLECTIONMAKER_H
#define INDET_CALOCLUSTERROIPHIRZCOLLECTIONMAKER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrkCaloClusterROI/ROIPhiRZContainer.h"

#include "egammaInterfaces/IegammaCaloClusterSelector.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"

#include "StoreGate/ReadCondHandleKey.h"

#include "TrkEventPrimitives/LocalParameters.h"


#include <atomic>

namespace InDet {

   /** @brief Algorithm to create a eta ordered ROIs in the eta phi plane.
    * The algorithm creates ROIs ordered by eta in the eta phi plane from 
    * selected calorimer clusters.
    */
class CaloClusterROIPhiRZContainerMaker : public AthReentrantAlgorithm
{
 public:

   CaloClusterROIPhiRZContainerMaker(const std::string& name, ISvcLocator* pSvcLocator);

  ~CaloClusterROIPhiRZContainerMaker();

  StatusCode initialize() override;
  StatusCode finalize() override;
  StatusCode execute(const EventContext& ctx) const override;

 private:

  const Trk::Surface*getCaloSurface(const xAOD::CaloCluster &cluster,
                                    const CaloDetDescrManager &caloDDMgr) const
   {
      // Determine if we want to extrapolate to the barrel or endcap.  If in the
      // crack choose the detector with largest amount of energy in the second
      // sampling layer
      if (xAOD::EgammaHelpers::isBarrel(&cluster)) {
         return m_calosurf->CreateUserSurface(CaloCell_ID::EMB2, 0., cluster.eta(), &caloDDMgr);
      } else {
         return m_calosurf->CreateUserSurface(CaloCell_ID::EME2, 0., cluster.eta(), &caloDDMgr);
      }
   }

   Trk::LocalParameters getClusterLocalParameters(const xAOD::CaloCluster &cluster,
                                                  const Trk::Surface &surf) const;

   void addROI( const xAOD::CaloCluster &cluster,
                const CaloDetDescrManager &caloDDMgr,
                ROIPhiRZContainer &output_rois,
                std::vector<uint_fast8_t> &max_output,
                std::vector<unsigned int> &n_rois) const;

   /** @brief Name of the cluster intput collection*/
   SG::ReadHandleKey<xAOD::CaloClusterContainer>   m_inputClusterContainerName
      {this, "InputClusterContainerName", "egammaTopoCluster", "Input cluster for egamma objects"};

   /** @brief Name of the ROI output collection*/
   SG::WriteHandleKeyArray<ROIPhiRZContainer>  m_outputClusterContainerName
      {this, "OutputROIContainerName", {}, "Output collection of eta ordered ROIs"};

   /**
    * @brief Name of the CaloDetDescrManager condition object
    */
   SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey
      { this,"CaloDetDescrManager", "CaloDetDescrManager", "SG Key for CaloDetDescrManager in the Condition Store" };

   /** @brief Tool to build calorimeter layer surfaces */
   ToolHandle<ICaloSurfaceBuilder> m_calosurf
      { this, "CaloSurfaceBuilder", "CaloSurfaceBuilder", "Tool to build calorimeter layer surfaces"};

   /** @brief Tool to filter the calo clusters */
   ToolHandle<IegammaCaloClusterSelector> m_egammaCaloClusterSelector
      {this, "egammaCaloClusterSelector", "egammaCaloClusterSelector","Tool that makes the cluster selection"};

   Gaudi::Property< bool > m_EMEnergyOnly
      {this, "EMEnergyOnly", false, "Only use EM energy as the ROI energy" };

   Gaudi::Property< std::vector<float> >  m_phiWidth
      {this, "phiWidth", {} };

   Gaudi::Property< std::vector<float> >  m_minPtEm
      {this, "minPt", {}, "Minimum pt of cluster ROIs (EMFraction corrected if EMEnerygOnly is set)."};

   std::vector<unsigned int> m_outputIndex;
   std::vector<unsigned int> m_outputSorted;
   std::vector<unsigned int> m_outputUnsorted;
   std::vector<float>        m_sortedMinPtEm;
   float                     m_maxPhiWidth;

   mutable std::atomic_uint m_allClusters{0};
   mutable std::atomic_uint m_selectedClusters{0};
   mutable std::atomic_uint m_duplicateROI{0};
   mutable std::atomic_uint m_maxNROIs{0};
};

} //End namespace 
#endif
