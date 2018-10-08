/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/********************************************************************

NAME:     CaloClusterROI_Selector.cxx
PACKAGE:  offline/Reconstruction/egamma/egammaRec

AUTHORS:  A.Morley,C.A
CREATED:  Nov 2011

PURPOSE:   For each cluster create a new CaloClusterROI object and fills it then in the CaloClusterROI_Collection.


 ********************************************************************/

// INCLUDE HEADER FILES:

#include "InDetCaloClusterROISelector/CaloClusterROI_Selector.h"

//Cluster cuts
#include "TrkCaloClusterROI/CaloClusterROI.h"
#include <stdexcept>

//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:

InDet::CaloClusterROI_Selector::CaloClusterROI_Selector(const std::string& name, 
        ISvcLocator* pSvcLocator): 
    AthReentrantAlgorithm(name, pSvcLocator)
{
}

// ================================================================
InDet::CaloClusterROI_Selector::~CaloClusterROI_Selector()
{  
}

// =================================================================
StatusCode InDet::CaloClusterROI_Selector::initialize()
{
    //
    // initialize method
    //

    ATH_MSG_DEBUG("Initializing CaloClusterROI_Selector");

    if (!m_egammaCheckEnergyDepositTool.empty()) {
      ATH_CHECK( m_egammaCheckEnergyDepositTool.retrieve() );
    } else {
      m_egammaCheckEnergyDepositTool.disable();
    }

    ATH_CHECK( m_caloClusterROI_Builder.retrieve() );

    m_allClusters=0;
    m_selectedClusters=0;

    ATH_CHECK(m_outputClusterContainerName.initialize());
    ATH_CHECK(m_inputClusterContainerName.initialize(!m_inputClusterContainerName.key().empty()));

    ATH_MSG_INFO("Initialization completed successfully");
    return StatusCode::SUCCESS;
}



// ====================================================================
StatusCode InDet::CaloClusterROI_Selector::finalize()
{
    //
    // finalize method
    //
    ATH_MSG_INFO ("AllClusters " << m_allClusters);
    ATH_MSG_INFO ("SelectedClusters " << m_selectedClusters);

    return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode InDet::CaloClusterROI_Selector::execute_r(const EventContext& ctx) const
{
    //
    // athena execute method
    //

    ATH_MSG_DEBUG("Executing CaloClusterROI_Selector");

    // Record output CaloClusterROICollection:

    SG::WriteHandle<CaloClusterROI_Collection>  ccROI_Collection(m_outputClusterContainerName, ctx);
    ATH_CHECK( ccROI_Collection.record( std::make_unique<CaloClusterROI_Collection>() ) );

    if (m_inputClusterContainerName.key().empty()) {
        return StatusCode::SUCCESS;
    }

    // retrieve cluster containers, return `failure' if not existing
    SG::ReadHandle<xAOD::CaloClusterContainer> inputClusterContainer(m_inputClusterContainerName,ctx);
    if (!inputClusterContainer.isValid()) {
        return StatusCode::FAILURE;
    }

    // loop over clusters 
    unsigned int all_clusters{};
    unsigned int selected_clusters{};
    for(const xAOD::CaloCluster* cluster : *inputClusterContainer )
    {
        all_clusters++;
        if (PassClusterSelection(cluster))
        {
            selected_clusters++;
            ATH_MSG_DEBUG("Pass cluster selection");
            Trk::CaloClusterROI* ccROI = m_caloClusterROI_Builder->buildClusterROI( cluster );
            ccROI_Collection->push_back(ccROI);
        } else {
            ATH_MSG_DEBUG("Fail cluster selection");
        }
    }

    m_allClusters += all_clusters;
    m_selectedClusters += selected_clusters;

    ATH_MSG_DEBUG("execute completed successfully");
    return StatusCode::SUCCESS;
}
// ======================================================================
bool InDet::CaloClusterROI_Selector::PassClusterSelection(const xAOD::CaloCluster* cluster) const
{
    if( !m_egammaCheckEnergyDepositTool.empty() && 
	!m_egammaCheckEnergyDepositTool->checkFractioninSamplingCluster( cluster ) ) {
        ATH_MSG_DEBUG("Cluster failed sample check: dont make ROI");
        return false;
    }

    // switch to using cluster properties, not layer 2 properties
    if ( cluster->et() < m_ClusterEtCut ){
        ATH_MSG_DEBUG("Cluster failed Energy Cut: dont make ROI");
        return false;
    }

    static const  SG::AuxElement::ConstAccessor<float> acc("EMFraction");

    double emFrac(0.);
    if (acc.isAvailable(*cluster)) {
      emFrac = acc(*cluster);
    } else if (!cluster->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emFrac)){
      throw std::runtime_error("No EM fraction momement stored");
    }

    if ( emFrac< m_ClusterEMFCut ){
        ATH_MSG_DEBUG("Cluster failed EM Fraction cut: dont make ROI");
        return false;
    }

    if ( cluster->et()*emFrac< m_ClusterEMEtCut ){
        ATH_MSG_DEBUG("Cluster failed EM Energy cut: dont make ROI");
        return false;
    }


    double lateral(0.);
    if (!cluster->retrieveMoment(xAOD::CaloCluster::LATERAL, lateral)){
      throw std::runtime_error("No LATERAL momement stored");
    }

    if ( lateral >  m_ClusterLateralCut ){
      ATH_MSG_DEBUG("Cluster failed LATERAL cut: dont make ROI");
      return false;
    }

    return true;
}
