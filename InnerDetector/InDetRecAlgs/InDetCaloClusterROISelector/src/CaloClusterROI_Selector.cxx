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

#include "InDetRecToolInterfaces/ICaloClusterROI_Builder.h"
//
#include "CaloUtils/CaloCellList.h"



//Cluster cuts
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "egammaInterfaces/IegammaIso.h"
#include "egammaInterfaces/IegammaMiddleShape.h"
#include "TrkCaloClusterROI/CaloClusterROI.h"



// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/MsgStream.h"

#include <algorithm> 
#include <math.h>

//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:

InDet::CaloClusterROI_Selector::CaloClusterROI_Selector(const std::string& name, 
        ISvcLocator* pSvcLocator): 
    AthReentrantAlgorithm(name, pSvcLocator),
    m_inputClusterContainerName("LArClusterEM"),
    m_outputClusterContainerName("CaloClusterROIs"),
    m_cellsName("AllCalo"),
    m_allClusters(0),
    m_selectedClusters(0),
    m_timingProfile(0)
{
    // The following properties are specified at run-time
    // (declared in jobOptions file)
    // input cluster for egamma objects
    declareProperty("InputClusterContainerName",
            m_inputClusterContainerName,
            "Input cluster for egamma objects");

    //Cell container
    declareProperty("CellsName",
            m_cellsName,
            "Names of containers which contain cells ");

    // input cluster for egamma objects
    declareProperty("OutputClusterContainerName",
            m_outputClusterContainerName,
            "Output cluster for egamma objects");
    //
    // Handles of tools
    //Hadronic Isolation
    declareProperty("EMCaloIsolationTool",                    m_emCaloIsolationTool, "Optional tool to check the hadronic energy");
    //Check Fraction 
    declareProperty("egammaCheckEnergyDepositTool",           m_egammaCheckEnergyDepositTool, "Optional tool to check EM samples");
    declareProperty("egammaMiddleShapeTool",                  m_egammaMiddleShape, "Optional tool to check Reta ");
    //
    declareProperty("CaloClusterROIBuilder",                  m_caloClusterROI_Builder,"Handle of the CaloClusterROI_Builder Tool");
    //
    // Other properties.
    //
    // declareProperty("CheckEMSamples",                         m_CheckEMsamples =true);
    // declareProperty("CheckHadronicEnergy",                    m_CheckHadronicEnergy=true);

    //
    declareProperty("HadRatioCut",                            m_HadRatioCut  =0.12,    " Cut on Hadronic Leakage");
    declareProperty("RetaCut",                                m_RetaCut      =0.65,   " Cut on Reta");
    declareProperty("ClusterEtCut",                           m_ClusterEtCut = 0000,  " Cut On Cluster Et");


}

// ================================================================
InDet::CaloClusterROI_Selector::~CaloClusterROI_Selector()
{  
    //
    // destructor
    //
}

// =================================================================
StatusCode InDet::CaloClusterROI_Selector::initialize()
{
    //
    // initialize method
    //

    ATH_MSG_DEBUG("Initializing CaloClusterROI_Selector");

    /* Get the middle shapes Tool*/
    m_CheckReta = !m_egammaMiddleShape.empty();
    m_CheckHadronicEnergy = !m_emCaloIsolationTool.empty();

    if (!m_egammaMiddleShape.empty()) ATH_CHECK( m_egammaMiddleShape.retrieve() );
    else m_egammaMiddleShape.disable();

    if (!m_emCaloIsolationTool.empty()) ATH_CHECK( m_emCaloIsolationTool.retrieve() );
    else m_emCaloIsolationTool.disable();

    if (!m_egammaCheckEnergyDepositTool.empty()) ATH_CHECK( m_egammaCheckEnergyDepositTool.retrieve() );
    else m_egammaCheckEnergyDepositTool.disable();

    ATH_CHECK( m_caloClusterROI_Builder.retrieve() );

    m_timingProfile = 0;
    StatusCode sc = service("ChronoStatSvc",m_timingProfile);
    if(sc.isFailure() || m_timingProfile == 0) {
        ATH_MSG_ERROR("Cannot find the ChronoStatSvc " << m_timingProfile);
    }

    m_allClusters=0;
    m_selectedClusters=0;

    ATH_CHECK(m_outputClusterContainerName.initialize());
    ATH_CHECK(m_inputClusterContainerName.initialize(!m_inputClusterContainerName.key().empty()));
    ATH_CHECK(m_cellsName.initialize(!m_cellsName.key().empty()));

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

    //bool do_trackMatch = true;
    ATH_MSG_DEBUG("Executing CaloClusterROI_Selector");

    // Chrono name for each Tool
    std::string chronoName;

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


    // retrieve Calo Cell Container
    SG::ReadHandle<CaloCellContainer> cellcoll;
    if((m_CheckHadronicEnergy|| m_CheckReta) && !m_cellsName.key().empty()){
        cellcoll=SG::ReadHandle<CaloCellContainer>(m_cellsName,ctx);
        if (!cellcoll.isValid()) {
            return StatusCode::FAILURE;
        }
    }

    // loop over clusters 
    unsigned int all_clusters{};
    unsigned int selected_clusters{};
    for(const xAOD::CaloCluster* cluster : *inputClusterContainer )
    {
        all_clusters++;
        if (PassClusterSelection(cluster , cellcoll.cptr()))
        {
            selected_clusters++;
            ATH_MSG_DEBUG("Pass cluster selection");
            Trk::CaloClusterROI* ccROI = m_caloClusterROI_Builder->buildClusterROI( cluster );
            ccROI_Collection->push_back(ccROI);
        } else {
            ATH_MSG_DEBUG("Fail cluster selection");
        }
    }

    {
        std::lock_guard<std::mutex> lock(m_statMutex);
        m_allClusters+=all_clusters;
        m_selectedClusters += selected_clusters;
    }
    ATH_MSG_DEBUG("execute completed successfully");
    return StatusCode::SUCCESS;
}
// ======================================================================
bool InDet::CaloClusterROI_Selector::PassClusterSelection(const xAOD::CaloCluster* cluster ,  const CaloCellContainer* cellcoll) const
{
    if( !m_egammaCheckEnergyDepositTool.empty() && !m_egammaCheckEnergyDepositTool->checkFractioninSamplingCluster( cluster ) ) {
        ATH_MSG_DEBUG("Cluster failed sample check: dont make ROI");
        return false;
    }

    if((m_CheckHadronicEnergy || m_CheckReta) && cellcoll==0) {
        ATH_MSG_DEBUG("No cell collection: dont make ROI");
        return false;
    }

    // transverse energy in calorimeter (using eta position in second sampling)
    double eta2   = fabs(cluster->etaBE(2));  
    if (eta2>2.47 || eta2<0.) {
        ATH_MSG_DEBUG("Cluster failed acceptance test: dont make ROI");
        return false;
    }

    double et = cosh(eta2)!=0. ? cluster->e()/cosh(eta2) : 0.;

    if ( et < m_ClusterEtCut ){
        ATH_MSG_DEBUG("Cluster failed Energy Cut: dont make ROI");
        return false;
    }

    if(m_CheckReta || m_CheckHadronicEnergy){

        if(m_CheckReta){
            IegammaMiddleShape::Info info;
            StatusCode sc = m_egammaMiddleShape->execute(*cluster,*cellcoll,info);
            if ( sc.isFailure() ) {
                ATH_MSG_WARNING("call to Middle shape returns failure for execute");
                return false;
            }
            double e237   = info.e237; 
            // E(7*7) in 2nd sampling
            double e277   = info.e277; 
            if (e277 != 0. && e237/e277<=m_RetaCut){
                ATH_MSG_DEBUG("Cluster failed Reta test: dont make ROI");
                return false;
            }
        }
        if(m_CheckHadronicEnergy){
            // define a new Calo Cell list corresponding to HAD Calo
            CaloCell_ID::SUBCALO theCalo1 =  static_cast<CaloCell_ID::SUBCALO>(CaloCell_ID::LARHEC) ;
            CaloCell_ID::SUBCALO theCalo2 =  static_cast<CaloCell_ID::SUBCALO>(CaloCell_ID::TILE) ;
            std::vector<CaloCell_ID::SUBCALO> theVecCalo;
            theVecCalo.push_back(theCalo1);
            theVecCalo.push_back(theCalo2);
            // define a new Calo Cell list
            CaloCellList* HADccl = new CaloCellList(cellcoll,theVecCalo);
            IegammaIso::Info info;
            StatusCode sc = m_emCaloIsolationTool->execute(*cluster,*HADccl,info); 
            if ( sc.isFailure() ) {
                ATH_MSG_WARNING("call to Iso returns failure for execute");
                delete HADccl;
                return false;
            }
            delete HADccl;
            double ethad1 = info.ethad1;
            double ethad  = info.ethad;
            double raphad1 = fabs(et) > 0. ? ethad1/et : 0.;
            double raphad = fabs(et) > 0. ? ethad/et : 0.;
            if (eta2 >= 0.8 && eta2 < 1.37){
                if (raphad >m_HadRatioCut){
                    ATH_MSG_DEBUG("Cluster failed Hadronic Leakage test: dont make ROI");
                    return false;
                }
            }
            else if(raphad1 >m_HadRatioCut){
                ATH_MSG_DEBUG("Cluster failed Hadronic Leakage test: dont make ROI");
                return false;
            }
        }
    }
    return true;
}
