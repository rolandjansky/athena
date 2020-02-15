/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

/********************************************************************

NAME:     egammaBuilder.cxx
PACKAGE:  offline/Reconstruction/egamma/egammaRec

AUTHORS:  H. Ma MW, SR , FD , JH , KT , CA, BL
CREATED:  Nov 2000 
CHANGES:  Mar 2014 (CA / BL) xAOD migration

PURPOSE:  Algorithm which makes a egammaObjectCollection. For each cluster 
create a new egamma object and fills it then in the egammaContainer.
The algorithm takes care to make the objects containing shower 
shape variables, leakage into had. calo, objects for track matching 
and eventually conversions.
 ********************************************************************/

// INCLUDE HEADER FILES:

#include "egammaBuilder.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IToolSvc.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "egammaRecEvent/egammaRecContainer.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"


#include "egammaUtils/egammaDuplicateRemoval.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

// INCLUDE GAUDI HEADER FILES:
#include <algorithm> 
#include <math.h>


//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:

egammaBuilder::egammaBuilder(const std::string& name, 
        ISvcLocator* pSvcLocator): 
    AthAlgorithm(name, pSvcLocator),
    m_timingProfile(nullptr)
{
}

// ================================================================
egammaBuilder::~egammaBuilder()
{  
    //
    // destructor
    //
}

// =================================================================
StatusCode egammaBuilder::initialize()
{
    //
    // initialize method
    //

    ATH_MSG_DEBUG("Initializing egammaBuilder");

    // the keys
    ATH_CHECK(m_electronOutputKey.initialize());
    ATH_CHECK(m_photonOutputKey.initialize());
    ATH_CHECK(m_inputClusterContainerKey.initialize());
    ATH_CHECK(m_topoSeededClusterContainerKey.initialize(m_doTopoSeededPhotons));
    ATH_CHECK(m_egammaRecContainerKey.initialize());

    // retrieve track match builder
    CHECK( RetrieveEMTrackMatchBuilder() );

    // retrieve conversion builder
    CHECK(  RetrieveEMConversionBuilder() );

    // retrieve EM Cluster Tool
    ATH_CHECK( m_clusterTool.retrieve() );

    // retrieve ambiguity tool
    ATH_CHECK( m_ambiguityTool.retrieve() );

    //retrieve shower builder
    ATH_CHECK( m_ShowerTool.retrieve() );
    
    ATH_MSG_INFO("Retrieving " << m_egammaTools.size() << " tools for egamma objects");
    ATH_CHECK( m_egammaTools.retrieve() );

    ATH_MSG_INFO("Retrieving " << m_electronTools.size() << " tools for electrons");
    ATH_CHECK( m_electronTools.retrieve() );

    ATH_MSG_INFO("Retrieving " << m_photonTools.size() << " tools for photons");
    ATH_CHECK( m_photonTools.retrieve() );

    // retrieve timing profile
    CHECK( service("ChronoStatSvc",m_timingProfile) );

    ATH_MSG_INFO("Initialization completed successfully");
    return StatusCode::SUCCESS;
}


// ====================================================================
StatusCode egammaBuilder::RetrieveEMTrackMatchBuilder(){
    //
    // retrieve EMTrackMatchBuilder tool
    //
    if (!m_doTrackMatching) {
        m_trackMatchBuilder.disable();
        return StatusCode::SUCCESS;
    }

    if (m_trackMatchBuilder.empty()) {
        ATH_MSG_ERROR("EMTrackMatchBuilder is empty, but track matching is enabled");
        return StatusCode::FAILURE;
    } 


    if(m_trackMatchBuilder.retrieve().isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_trackMatchBuilder);
        return StatusCode::FAILURE;
    } 
    else ATH_MSG_DEBUG("Retrieved Tool "<<m_trackMatchBuilder); 

    return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode egammaBuilder::RetrieveEMConversionBuilder(){
    //
    // retrieve EMConversionBuilder tool
    //

    if (!m_doConversions) {
        m_conversionBuilder.disable();
        return StatusCode::SUCCESS;
    }

    if (m_conversionBuilder.empty()) {
        ATH_MSG_ERROR("EMConversionBuilder is empty");
        return StatusCode::FAILURE;
    } 

    if(m_conversionBuilder.retrieve().isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_conversionBuilder);
        return StatusCode::FAILURE;
    } 
    else ATH_MSG_DEBUG("Retrieved Tool "<<m_conversionBuilder); 

    return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode egammaBuilder::finalize(){
    //
    // finalize method
    //
    return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode egammaBuilder::execute(){
    //
    // athena execute method
    //

    ATH_MSG_DEBUG("Executing egammaBuilder");

    // This we can drop once the Alg becomes re-entrant
    const EventContext& ctx = Gaudi::Hive::currentContext();

    // Chrono name for each Tool
    std::string chronoName;

    // create egammaRecContainer, ElectronContainer, PhotonContainer and register them
    //Build the initial egamma Rec objects for all copied Topo Clusters
    SG::WriteHandle<EgammaRecContainer> egammaRecs(m_egammaRecContainerKey);
    ATH_CHECK(egammaRecs.record(std::make_unique<EgammaRecContainer>()));


    SG::WriteHandle<xAOD::ElectronContainer> electronContainer(m_electronOutputKey);
    ATH_CHECK(electronContainer.record(std::make_unique<xAOD::ElectronContainer>(),
                std::make_unique<xAOD::ElectronAuxContainer>()));

    SG::WriteHandle<xAOD::PhotonContainer> photonContainer(m_photonOutputKey);
    ATH_CHECK(photonContainer.record(std::make_unique<xAOD::PhotonContainer>(),
                std::make_unique<xAOD::PhotonAuxContainer>()));

    // Loop over clusters and create egammaRec objects
    ATH_MSG_DEBUG("Creating egammaRec objects");
    SG::ReadHandle<xAOD::CaloClusterContainer> clusters(m_inputClusterContainerKey);
    // only for single-threaded running, remove later
    if (!clusters.isValid()) {
        ATH_MSG_FATAL("Could not retrieve cluster container " << m_inputClusterContainerKey.key());
        return StatusCode::FAILURE;
    }

    for (unsigned int i = 0; i < clusters->size(); ++i) {
        const ElementLink< xAOD::CaloClusterContainer > clusterLink( *clusters, i );
        const std::vector< ElementLink<xAOD::CaloClusterContainer> > elClusters {clusterLink};    
        egammaRec *egRec = new egammaRec();
        egRec->setCaloClusters( elClusters );
        egammaRecs->push_back( egRec );
    }

    //
    if (m_doTrackMatching){    
        ATH_MSG_DEBUG("Running TrackMatchBuilder");  
        //
        std::string chronoName = this->name()+"_"+m_trackMatchBuilder->name();         
        if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
        //
          ATH_CHECK(m_trackMatchBuilder->executeRec(ctx,egammaRecs.ptr()));
        //
        if(m_timingProfile) m_timingProfile->chronoStop(chronoName);
    }
    //
    if (m_doConversions){
        ATH_MSG_DEBUG("Running ConversionBuilder");  
        //
        chronoName = this->name()+"_"+m_conversionBuilder->name();         
        if(m_timingProfile) m_timingProfile->chronoStart(chronoName);
        for (auto egRec : *egammaRecs) {
            if (m_conversionBuilder->executeRec(ctx,egRec).isFailure()){
                ATH_MSG_ERROR("Problem executing " << m_conversionBuilder);
                return StatusCode::FAILURE;  
            }
        }
        if(m_timingProfile) m_timingProfile->chronoStop(chronoName);
    }
    ////////////////////////////

    // Run the ambiguity resolving to decide if we should create electron and/or photon
    static const  SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
    static const SG::AuxElement::Accessor<ElementLink<xAOD::EgammaContainer> > ELink ("ambiguityLink");
    ElementLink<xAOD::EgammaContainer> dummylink;

    for (const auto& egRec : *egammaRecs){

        xAOD::AmbiguityTool::AmbiguityType type= xAOD::AmbiguityTool::unknown;

        ATH_MSG_DEBUG("Running AmbiguityTool");
        unsigned int author = m_ambiguityTool->ambiguityResolve(egRec->caloCluster(),
                egRec->vertex(),
                egRec->trackParticle(),
                type);

        ATH_MSG_DEBUG("...author: " << author);
        if (author == xAOD::EgammaParameters::AuthorUnknown) {
            continue;
        }
        //Electron
        if (author == xAOD::EgammaParameters::AuthorElectron){
            ATH_MSG_DEBUG("getElectron");
            if ( !getElectron(egRec, electronContainer.ptr(), author) ){
                return StatusCode::FAILURE;
            }
            acc(*(electronContainer->back())) = type;
            ELink(*(electronContainer->back()))=dummylink;
        }
        //Photon
        if (author == xAOD::EgammaParameters::AuthorPhoton ){
            ATH_MSG_DEBUG("getPhoton");
            if ( !getPhoton(egRec, photonContainer.ptr(), author) ){
                return StatusCode::FAILURE;
            }
            acc(*(photonContainer->back())) = type;
            ELink(*(photonContainer->back()))=dummylink;
        }
        //Both Electron and Photon
        if(author == xAOD::EgammaParameters::AuthorAmbiguous){

            ATH_MSG_DEBUG("get Electron and Photon");

            if ( !getPhoton(egRec, photonContainer.ptr(), author) || 
                    !getElectron(egRec, electronContainer.ptr(), author)){
                return StatusCode::FAILURE;
            }

            acc(*(electronContainer->back())) = type;
            size_t photonIndex=photonContainer->size()-1;
            ElementLink<xAOD::EgammaContainer> linktoPhoton (*photonContainer,photonIndex);
            ELink(*(electronContainer->back()))=linktoPhoton;

            acc(*(photonContainer->back())) = type;
            size_t electronIndex=electronContainer->size()-1;
            ElementLink<xAOD::EgammaContainer> linktoElectron (*electronContainer,electronIndex);
            ELink(*(photonContainer->back()))=linktoElectron;
        }
    }

    // Add topo-seeded clusters to the photon collection
    if (m_doTopoSeededPhotons) {
        CHECK( addTopoSeededPhotons(photonContainer.ptr(), clusters.ptr()) );
    }
 
    const CaloDetDescrManager* calodetdescrmgr = nullptr;                                                                 
    ATH_CHECK( detStore()->retrieve(calodetdescrmgr,"CaloMgr") );   
    // Call tools

    /*
     * Shower Shapes
     */
    if (electronContainer.ptr()) {
      for (xAOD::Electron* electron : *electronContainer) {
        ATH_CHECK(m_ShowerTool->execute(ctx, *calodetdescrmgr, electron));
      }
    }
    if (photonContainer.ptr()) {
      for (xAOD::Photon* photon : *photonContainer) {
        ATH_CHECK(m_ShowerTool->execute(ctx, *calodetdescrmgr, photon));
      }
    }

    // First the final cluster/calibration
    ATH_MSG_DEBUG("Executing : " << m_clusterTool);  
    if ( m_clusterTool->contExecute(ctx, *calodetdescrmgr ,electronContainer.ptr(), photonContainer.ptr()).isFailure() ){
        ATH_MSG_ERROR("Problem executing the " << m_clusterTool<<" tool");
        return StatusCode::FAILURE;
    }


    for (auto& tool : m_egammaTools)
    {
        CHECK( CallTool(ctx, tool, electronContainer.ptr(), photonContainer.ptr()) );
    }

    for (auto& tool : m_electronTools)
    {
        CHECK( CallTool(ctx, tool, electronContainer.ptr(), nullptr) );
    }

    for (auto& tool : m_photonTools)
    {
        CHECK( CallTool(ctx, tool, nullptr, photonContainer.ptr()) );
    }
    ATH_MSG_DEBUG("execute completed successfully");

    return StatusCode::SUCCESS;
}

// =====================================================
StatusCode egammaBuilder::CallTool(const EventContext& ctx,
        ToolHandle<IegammaBaseTool>& tool, 
        xAOD::ElectronContainer *electronContainer /* = 0*/, 
        xAOD::PhotonContainer *photonContainer /* = 0*/)
{

    std::string chronoName = this->name()+"_"+tool->name();         
    if(m_timingProfile) m_timingProfile->chronoStart(chronoName);

    if (electronContainer)
    {    
        ATH_MSG_DEBUG("Executing tool on electrons: " << tool );
        for (const auto& electron : *electronContainer)
        {
            if (tool->execute(ctx, electron).isFailure() )
            {
                ATH_MSG_ERROR("Problem executing tool on electrons: " << tool);
                return StatusCode::FAILURE;
            }
        }
    }

    if (photonContainer)
    {
        ATH_MSG_DEBUG("Executing tool on photons: " << tool );
        for (const auto& photon : *photonContainer)
        {
            if (tool->execute(ctx, photon).isFailure() )
            {
                ATH_MSG_ERROR("Problem executing tool on photons: " << tool);
                return StatusCode::FAILURE;
            }
        }
    }

    if(m_timingProfile) m_timingProfile->chronoStop(chronoName);
    return StatusCode::SUCCESS;
}
// =====================================================
bool egammaBuilder::getElectron(const egammaRec* egRec, 
        xAOD::ElectronContainer *electronContainer,
        unsigned int author)
{  
    if (!egRec || !electronContainer) return false;

    xAOD::Electron *electron = new xAOD::Electron();
    electronContainer->push_back( electron );
    electron->setAuthor( author );

    std::vector< ElementLink< xAOD::CaloClusterContainer > > clusterLinks;
    for (unsigned int i = 0 ; i < egRec->getNumberOfClusters(); ++i){
        clusterLinks.push_back( egRec->caloClusterElementLink(i) );
    }
    electron->setCaloClusterLinks( clusterLinks );

    std::vector< ElementLink< xAOD::TrackParticleContainer > > trackLinks;
    for (unsigned int i = 0 ; i < egRec->getNumberOfTrackParticles(); ++i){
        trackLinks.push_back( egRec->trackParticleElementLink(i) );
    }
    electron->setTrackParticleLinks( trackLinks );

    electron->setCharge(electron->trackParticle()->charge());

     //Set DeltaEta, DeltaPhi , DeltaPhiRescaled
    std::array<double,4> deltaEta = egRec->deltaEta();
    std::array<double,4> deltaPhi = egRec->deltaPhi();
    std::array<double,4> deltaPhiRescaled = egRec->deltaPhiRescaled();

    electron->setTrackCaloMatchValue(static_cast<float>(deltaEta[0]),xAOD::EgammaParameters::deltaEta0);
    electron->setTrackCaloMatchValue(static_cast<float> (deltaPhi[0]),xAOD::EgammaParameters::deltaPhi0 );
    electron->setTrackCaloMatchValue(static_cast<float>(deltaPhiRescaled[0]), xAOD::EgammaParameters::deltaPhiRescaled0);

    electron->setTrackCaloMatchValue(static_cast<float>(deltaEta[1]), xAOD::EgammaParameters::deltaEta1);
    electron->setTrackCaloMatchValue(static_cast<float> (deltaPhi[1]),xAOD::EgammaParameters::deltaPhi1 );
    electron->setTrackCaloMatchValue(static_cast<float>(deltaPhiRescaled[1]), xAOD::EgammaParameters::deltaPhiRescaled1);

    electron->setTrackCaloMatchValue(static_cast<float>(deltaEta[2]), xAOD::EgammaParameters::deltaEta2);
    electron->setTrackCaloMatchValue(static_cast<float> (deltaPhi[2]),xAOD::EgammaParameters::deltaPhi2 );
    electron->setTrackCaloMatchValue(static_cast<float>(deltaPhiRescaled[2]), xAOD::EgammaParameters::deltaPhiRescaled2);

    electron->setTrackCaloMatchValue(static_cast<float>(deltaEta[3]), xAOD::EgammaParameters::deltaEta3);
    electron->setTrackCaloMatchValue(static_cast<float> (deltaPhi[3]),xAOD::EgammaParameters::deltaPhi3 );
    electron->setTrackCaloMatchValue(static_cast<float>(deltaPhiRescaled[3]), xAOD::EgammaParameters::deltaPhiRescaled3);


    float deltaPhiLast = static_cast<float>(egRec->deltaPhiLast ());
    electron->setTrackCaloMatchValue(deltaPhiLast,xAOD::EgammaParameters::deltaPhiFromLastMeasurement );

    return true;
}

// =====================================================
bool egammaBuilder::getPhoton(const egammaRec* egRec,
        xAOD::PhotonContainer *photonContainer,
        unsigned int author)
{
    if (!egRec || !photonContainer) return false;

    xAOD::Photon *photon = new xAOD::Photon();
    photonContainer->push_back( photon );
    photon->setAuthor( author );

    // Transfer the links to the clusters
    std::vector< ElementLink< xAOD::CaloClusterContainer > > clusterLinks;
    for (unsigned int i = 0 ; i < egRec->getNumberOfClusters(); ++i)
        clusterLinks.push_back( egRec->caloClusterElementLink(i) );
    photon->setCaloClusterLinks( clusterLinks );

    // Transfer the links to the vertices  
    std::vector< ElementLink< xAOD::VertexContainer > > vertexLinks;
    for (unsigned int i = 0 ; i < egRec->getNumberOfVertices(); ++i)
        vertexLinks.push_back( egRec->vertexElementLink(i) );
    photon->setVertexLinks( vertexLinks );

    // Transfer deltaEta/Phi info
    float deltaEta = egRec->deltaEtaVtx();
    float deltaPhi = egRec->deltaPhiVtx();
    if (!photon->setVertexCaloMatchValue( deltaEta,
                xAOD::EgammaParameters::convMatchDeltaEta1) )
    {
        ATH_MSG_WARNING("Could not transfer deltaEta to photon");
        return false;
    }

    if (!photon->setVertexCaloMatchValue( deltaPhi,
                xAOD::EgammaParameters::convMatchDeltaPhi1) )
    {
        ATH_MSG_WARNING("Could not transfer deltaPhi to photon");
        return false;
    }

    return true;
}

// =====================================================
StatusCode egammaBuilder::addTopoSeededPhotons(xAOD::PhotonContainer *photonContainer,
        const xAOD::CaloClusterContainer *clusters)
{
    // Retrieve the cluster container
    SG::ReadHandle<xAOD::CaloClusterContainer> topoSeededClusters(m_topoSeededClusterContainerKey);
    // only for single-threaded running, remove later
    if (!topoSeededClusters.isValid()) {
        ATH_MSG_FATAL("Could not retrieve toposeeded cluster container " << m_topoSeededClusterContainerKey.key());
        return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Number of photons (before topo-clusters): " << photonContainer->size() );
    ATH_MSG_DEBUG("Number of topo-seeded clusters: " << topoSeededClusters->size() );

    // Create photon objects for the clusters that pass minimum Et and eta requirements and
    // do not overlap with egamma clusters
    typedef ElementLink<xAOD::CaloClusterContainer> ClusterLink_t;

    std::vector<const xAOD::CaloCluster* > candidateTopoClusters;
    for (const auto topoCluster : *topoSeededClusters){
        if (topoCluster->et() < m_minEtTopo || 
                topoCluster->et() > m_maxEtTopo ||
                fabs(topoCluster->etaBE(2)) > 2.47 || 
                clustersOverlap(topoCluster, clusters))
            continue;

        candidateTopoClusters.push_back(topoCluster);                  
    }

    std::vector<const xAOD::CaloCluster* > finalTopoClusters= egammaDuplicateRemoval::getClusterDuplicateRemoval(candidateTopoClusters);

    for (const auto topoCluster : finalTopoClusters){
        ATH_MSG_DEBUG("Creating topo-seeded photon, Et: " << topoCluster->et() <<
                " eta: " << topoCluster->eta() << " phi: " << topoCluster->phi());

        xAOD::Photon *photon = new xAOD::Photon();
        photonContainer->push_back(photon);
        photon->setAuthor( xAOD::EgammaParameters::AuthorCaloTopo35 );

        ClusterLink_t link(topoCluster, *topoSeededClusters );
        photon->setCaloClusterLinks( std::vector< ClusterLink_t>{ link } );
        //
        //Add the dummy to all new photons created here
        static const SG::AuxElement::Accessor<ElementLink<xAOD::EgammaContainer> > ELink ("ambiguityLink");
        ElementLink<xAOD::EgammaContainer> dummylink;
        ELink(*photon)=dummylink;
    }
    ATH_MSG_DEBUG("Number of photons (after topo-clusters): " << photonContainer->size() );

    return StatusCode::SUCCESS;
}

// =====================================================
bool egammaBuilder::clustersOverlap(const xAOD::CaloCluster *refCluster, 
        const xAOD::CaloClusterContainer *clusters)
{
    if (!refCluster || !clusters) return false;
    CaloPhiRange phiHelper;

    for (const auto cluster: *clusters)
    {
        if (fabs(refCluster->eta() - cluster->eta()) < m_minDeltaEta &&
                fabs(phiHelper.diff(refCluster->phi(), cluster->phi())) < m_minDeltaPhi)
            return true;
    }
    return false;
}

//  LocalWords:  newClus egc getCellLinks
