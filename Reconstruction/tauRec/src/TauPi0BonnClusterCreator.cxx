/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauPi0BonnClusterCreator.cxx
// package:     Reconstruction/tauEvent
// authors:     Benedict Winter, Will Davey
// date:        2012-10-09
//
//-----------------------------------------------------------------------------

#include <vector>

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "FourMomUtils/P4Helpers.h"

#include "tauRec/TauPi0BonnClusterCreator.h"

#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"

using std::vector;
using std::string;

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauPi0BonnClusterCreator::TauPi0BonnClusterCreator( 
    const string& type,
    const string& name,
    const IInterface *parent) 

    : TauToolBase(type, name, parent)
    , m_trackToCaloTool("")
    , m_cellContainerName("TauCommonPi0CellContainer") 
    , m_inputPi0ClusterContainerName("TauPi0BonnSubtractedClusterContainer")
    , m_outputPi0ClusterContainerName("TauPi0BonnClusterContainer")
    , m_neutralPFOContainerName("TauPi0BonnNeutralPFOContainer")
    , m_clusterEtCut(500.)
{
    declareInterface<TauToolBase > (this);

    declareProperty("ExtrapolateToCaloTool",         m_trackToCaloTool);
    declareProperty("CellContainerName",             m_cellContainerName);
    declareProperty("InputPi0ClusterContainerName",  m_inputPi0ClusterContainerName);
    declareProperty("OutputPi0ClusterContainerName", m_outputPi0ClusterContainerName);
    declareProperty("NeutralPFOContainerName",       m_neutralPFOContainerName);
    declareProperty("ClusterEtCut",                  m_clusterEtCut);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

TauPi0BonnClusterCreator::~TauPi0BonnClusterCreator() 
{
}


StatusCode TauPi0BonnClusterCreator::initialize() 
{
    CHECK( m_trackToCaloTool.retrieve() );
    return StatusCode::SUCCESS;
}

StatusCode TauPi0BonnClusterCreator::eventInitialize(TauCandidateData * /*data*/) 
{
    // create new CaloClusterContainer 
    // this container will later persistified
    // so it will get ownership of the objects
    m_pOutputPi0CaloClusterContainer = new xAOD::CaloClusterContainer(SG::OWN_ELEMENTS);
    ATH_MSG_VERBOSE("record container " << m_outputPi0ClusterContainerName);
    //---------------------------------------------------------------------
    // Create container for Pi0
    //---------------------------------------------------------------------
    m_pOutputPi0CaloClusterContainer = CaloClusterStoreHelper::makeContainer(&*evtStore(),   
									     m_outputPi0ClusterContainerName,    
									     msg()                  
									     );

    //---------------------------------------------------------------------
    // Create neutral PFO container
    //---------------------------------------------------------------------
    m_neutralPFOContainer = new xAOD::PFOContainer();
    CHECK( evtStore()->record(m_neutralPFOContainer, m_neutralPFOContainerName ) );
    m_neutralPFOAuxStore = new xAOD::PFOAuxContainer();
    CHECK( evtStore()->record( m_neutralPFOAuxStore, m_neutralPFOContainerName + "Aux." ) );
    m_neutralPFOContainer->setStore(m_neutralPFOAuxStore);

    return StatusCode::SUCCESS;
}

StatusCode TauPi0BonnClusterCreator::execute(TauCandidateData *data) 
{
    xAOD::TauJet *pTau = data->xAODTau;

    //---------------------------------------------------------------------
    // only run shower subtraction on 1-5 prong taus 
    //---------------------------------------------------------------------
    if (pTau->nTracks() == 0 || pTau->nTracks() >5 ) {
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("ClusterCreator: new tau. \tpt = " << pTau->pt() << "\teta = " << pTau->eta() << "\tphi = " << pTau->phi() << "\tnprongs = " << pTau->nTracks());

    //---------------------------------------------------------------------
    // get tau tracks
    //---------------------------------------------------------------------
    vector<const xAOD::TrackParticle*> tracks;
    for(unsigned iTrack = 0; iTrack<pTau->nTracks();++iTrack){
      const xAOD::TrackParticle* track = pTau->track(iTrack);
      tracks.push_back(track);
    }

    //---------------------------------------------------------------------
    // retrieve the CaloClusterContainer created by the CaloClusterMaker
    //---------------------------------------------------------------------
    const xAOD::CaloClusterContainer *pPi0ClusterContainer;
    CHECK( evtStore()->retrieve(pPi0ClusterContainer, m_inputPi0ClusterContainerName) );

    //---------------------------------------------------------------------
    // extrapolate track to calo layers 
    //---------------------------------------------------------------------
    vector<vector<float> > tracksEtaAtSampling;
    vector<vector<float> > tracksPhiAtSampling;
    for(unsigned iTrack = 0; iTrack<pTau->nTracks();++iTrack){
      const xAOD::TrackParticle* track = pTau->track(iTrack);
      vector<float> trackEtaAtSampling;
      vector<float> trackPhiAtSampling;
      this->getExtrapolatedPositions(track,trackEtaAtSampling,trackPhiAtSampling);
      tracksEtaAtSampling.push_back(trackEtaAtSampling);
      tracksPhiAtSampling.push_back(trackPhiAtSampling);
    }

    //---------------------------------------------------------------------
    // TODO: May want to use tau vertex in the future to calculate some cluster moments (DELTA_THETA, etc.).
    // Doesn't help now, since all moments are calculated wrt 0.0.0 atm.
    //---------------------------------------------------------------------


    // Retrieve Ecal1 shots and match them to clusters
    //---------------------------------------------------------------------
    std::vector<const xAOD::PFO*> shotVector;
    unsigned nShots = pTau->nShot_PFOs();
    for(unsigned iShot=0;iShot<nShots;++iShot){
        const xAOD::PFO* thisShot = pTau->shot_PFO(iShot);
        shotVector.push_back( thisShot );
    }
    std::map<unsigned, xAOD::CaloCluster*> clusterToShotMap = getClusterToShotMap(shotVector, pPi0ClusterContainer, pTau);

    xAOD::CaloClusterContainer::const_iterator clusterItr   (pPi0ClusterContainer->begin()),
                                               clusterItrEnd(pPi0ClusterContainer->end());
    for (; clusterItr != clusterItrEnd; ++clusterItr){
        
        // selection
        if ((*clusterItr)->pt() < m_clusterEtCut)   continue;
        // Cluster container has clusters for all taus.
        // Only run on clusters that belong to this tau
        if ((*clusterItr)->p4().DeltaR(pTau->p4()) > .4) continue;

        // Get shots in this cluster. Need to use (CaloCluster*) (*clusterItr) 
        // (not a copy!) since the pointer will otherwise be different than in clusterToShotMap
        std::vector<unsigned> shotsInCluster = getShotsMatchedToCluster( shotVector, clusterToShotMap, (xAOD::CaloCluster*) (*clusterItr));

        // Make a copy of the cluster to store in output container.
        xAOD::CaloCluster* pPi0Cluster = new xAOD::CaloCluster( *(*clusterItr) );

        // store pi0 calo cluster in the output container
        m_pOutputPi0CaloClusterContainer->push_back(pPi0Cluster);

        // Calculate input variables for fake supression. 
        // Do this before applying the vertex correction, 
        // since the position of the cluster in the 
        // calorimeter is required.
        float EM1CoreFrac = getEM1CoreFrac(pPi0Cluster);
        float asymmetryInEM1WRTTrk = getAsymmetryInEM1WRTTrk(pPi0Cluster, tracksEtaAtSampling, tracksPhiAtSampling);
        int NHitsInEM1 = getNPhotons(shotVector, shotsInCluster);
        vector<int> NPosECellsInLayer = getNPosECells(pPi0Cluster);
        vector<float> firstEtaWRTClusterPositionInLayer = get1stEtaMomWRTCluster(pPi0Cluster);
        vector<float> secondEtaWRTClusterPositionInLayer = get2ndEtaMomWRTCluster(pPi0Cluster);

        // Retrieve cluster moments that are used for fake supression and that are not stored in AOD
        // for every cluster. Do this after applying the vertex correction, since the moments 
        // (especcially DELTA_PHI and DELTA_THETA) must be calculated WRT the tau vertex
        double CENTER_MAG = 0.0;
        double FIRST_ETA = 0.0;
        double SECOND_R = 0.0;
        double SECOND_LAMBDA = 0.0;
        double DELTA_PHI = 0.0;
        double DELTA_THETA = 0.0;
        double CENTER_LAMBDA = 0.0;
        double LATERAL = 0.0;
        double LONGITUDINAL = 0.0;
        double ENG_FRAC_EM = 0.0;
        double ENG_FRAC_MAX = 0.0;
        double ENG_FRAC_CORE = 0.0;
        double SECOND_ENG_DENS = 0.0;

        // TODO: Replace numbers by human readable enums
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 404, CENTER_MAG) ) ATH_MSG_WARNING("Couldn't retrieve CENTER_MAG moment. Set it to 0.");

        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 102, FIRST_ETA) )       ATH_MSG_WARNING("Couldn't retrieve FIRST_ETA moment. Set it to 0.");
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 201, SECOND_R) )        ATH_MSG_WARNING("Couldn't retrieve SECOND_R moment. Set it to 0.");
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 202, SECOND_LAMBDA) )   ATH_MSG_WARNING("Couldn't retrieve SECOND_LAMBDA moment. Set it to 0.");
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 301, DELTA_PHI) )       ATH_MSG_WARNING("Couldn't retrieve DELTA_PHI moment. Set it to 0.");
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 302, DELTA_THETA) )     ATH_MSG_WARNING("Couldn't retrieve DELTA_THETA moment. Set it to 0.");
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 501, CENTER_LAMBDA) )   ATH_MSG_WARNING("Couldn't retrieve CENTER_LAMBDA moment. Set it to 0.");
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 601, LATERAL) )         ATH_MSG_WARNING("Couldn't retrieve LATERAL moment. Set it to 0.");
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 602, LONGITUDINAL) )    ATH_MSG_WARNING("Couldn't retrieve LONGITUDINAL moment. Set it to 0.");
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 701, ENG_FRAC_EM) )     ATH_MSG_WARNING("Couldn't retrieve ENG_FRAC_EM moment. Set it to 0.");
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 702, ENG_FRAC_MAX) )    ATH_MSG_WARNING("Couldn't retrieve ENG_FRAC_MAX moment. Set it to 0.");
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 703, ENG_FRAC_CORE) )   ATH_MSG_WARNING("Couldn't retrieve ENG_FRAC_CORE moment. Set it to 0.");
        if( !pPi0Cluster->retrieveMoment((xAOD::CaloCluster_v1::MomentType) 805, SECOND_ENG_DENS) ) ATH_MSG_WARNING("Couldn't retrieve SECOND_ENG_DENS moment. Set it to 0.");

       	float E_EM1 = pPi0Cluster->eSample(CaloSampling::EMB1) + pPi0Cluster->eSample(CaloSampling::EME1);
	      float E_EM2 = pPi0Cluster->eSample(CaloSampling::EMB2) + pPi0Cluster->eSample(CaloSampling::EME2);
        
        // create neutral PFO. Set BDTScore to dummy value <-1. The BDT score is calculated within TauPi0BonnSelector.cxx.
        xAOD::PFO* neutralPFO = new xAOD::PFO();
        m_neutralPFOContainer->push_back( neutralPFO );

        // Create element link from tau to neutral PFO
        ElementLink<xAOD::PFOContainer> PFOElementLink;
        PFOElementLink.toContainedElement( *m_neutralPFOContainer, neutralPFO );
        pTau->addCellBased_Neutral_PFOLink( PFOElementLink );

        // Set PFO variables
        ElementLink<xAOD::CaloClusterContainer> clusElementLink;
        clusElementLink.toContainedElement( *m_pOutputPi0CaloClusterContainer, pPi0Cluster );
        neutralPFO->setClusterLink( clusElementLink );
        
        neutralPFO->setP4( (float) pPi0Cluster->pt(), (float) pPi0Cluster->eta(), (float) pPi0Cluster->phi(), (float) pPi0Cluster->m());
        neutralPFO->setBDTPi0Score( (float) -9999. );
        neutralPFO->setCharge( 0. );

        neutralPFO->setCenterMag( (float) CENTER_MAG);

        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0, -1);
        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, -1);

        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_FIRST_ETA,       (float) FIRST_ETA);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_SECOND_R,        (float) SECOND_R);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_SECOND_LAMBDA,   (float) SECOND_LAMBDA);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_DELTA_PHI,       (float) DELTA_PHI);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_DELTA_THETA,     (float) DELTA_THETA);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_CENTER_LAMBDA,   (float) CENTER_LAMBDA);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_LATERAL,         (float) LATERAL);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_LONGITUDINAL,    (float) LONGITUDINAL);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_ENG_FRAC_EM,     (float) ENG_FRAC_EM);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_ENG_FRAC_MAX,    (float) ENG_FRAC_MAX);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_ENG_FRAC_CORE,   (float) ENG_FRAC_CORE);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_SECOND_ENG_DENS, (float) SECOND_ENG_DENS);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_energy_EM1,      (float) E_EM1);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_energy_EM2,      (float) E_EM2);


        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_EM1CoreFrac, EM1CoreFrac);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_asymmetryInEM1WRTTrk, asymmetryInEM1WRTTrk);
        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NHitsInEM1, NHitsInEM1);
        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NPosECells_PS,  NPosECellsInLayer.at(0));
        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NPosECells_EM1, NPosECellsInLayer.at(1));
        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NPosECells_EM2, NPosECellsInLayer.at(2));
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_firstEtaWRTClusterPosition_EM1, firstEtaWRTClusterPositionInLayer.at(1));
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_firstEtaWRTClusterPosition_EM2, firstEtaWRTClusterPositionInLayer.at(2));
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_secondEtaWRTClusterPosition_EM1, secondEtaWRTClusterPositionInLayer.at(1));
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_secondEtaWRTClusterPosition_EM2, secondEtaWRTClusterPositionInLayer.at(2));

        // Store shot element links in neutral PFO
        std::vector<ElementLink<xAOD::IParticleContainer> > shotlinks;
        for(unsigned iShot = 0;iShot<shotsInCluster.size();++iShot){
            ElementLink<xAOD::PFOContainer> shotPFOElementLink = pTau->shot_PFOLinks().at(shotsInCluster.at(iShot));
            ElementLink<xAOD::IParticleContainer> shotElementLink;
            shotPFOElementLink.toPersistent();
            shotElementLink.resetWithKeyAndIndex( shotPFOElementLink.persKey(), shotPFOElementLink.persIndex() ); 
            if (!shotElementLink.isValid()) ATH_MSG_WARNING("Created an invalid element link to xAOD track particle");
            shotlinks.push_back(shotElementLink);
        }
        if(!neutralPFO->setAssociatedParticleLinks( xAOD::PFODetails::TauShot,shotlinks)) 
            ATH_MSG_WARNING("Couldn't add shot links to neutral PFO!");
    }
    return StatusCode::SUCCESS;
}


StatusCode TauPi0BonnClusterCreator::eventFinalize(TauCandidateData *) 
{
    // pt sort container at the end of the event
    // if(m_pOutputPi0CaloClusterContainer->size()) AnalysisUtils::Sort::pT(m_pOutputPi0CaloClusterContainer);

    //----------------------------------------------------------------------
    // Register cluster container in StoreGate
    //----------------------------------------------------------------------
    CHECK( CaloClusterStoreHelper::finalizeClusters(&(*evtStore()),
                                                    m_pOutputPi0CaloClusterContainer,
                                                    m_outputPi0ClusterContainerName,
                                                    msg()));
  
    return StatusCode::SUCCESS;
}

// Functions used to calculate BDT variables other than those provided by the CaloClusterMomentsMaker
float TauPi0BonnClusterCreator::getEM1CoreFrac(
    const xAOD::CaloCluster* pi0Candidate)
{
    float coreEnergy=0.;
    float sumEPosCellsEM1=0.;

    const CaloClusterCellLink* theCellLink = pi0Candidate->getCellLinks();
    CaloClusterCellLink::const_iterator cellInClusterItr  = theCellLink->begin();
    CaloClusterCellLink::const_iterator cellInClusterItrE = theCellLink->end();
    for(;cellInClusterItr!=cellInClusterItrE;++cellInClusterItr){
        CaloCell* cellInCluster = (CaloCell*) *cellInClusterItr;
        int sampling = cellInCluster->caloDDE()->getSampling();
        if(sampling!=1 && sampling!=5) continue;
        float cellE = cellInCluster->e() * cellInClusterItr.weight();
        if(cellE<=0) continue;
        sumEPosCellsEM1 += cellE;
        float cellEtaWRTCluster = cellInCluster->eta()-pi0Candidate->eta();
        float cellPhiWRTCluster = P4Helpers::deltaPhi(cellInCluster->phi(), pi0Candidate->phi());
        if(fabs(cellPhiWRTCluster) > 0.05 || fabs(cellEtaWRTCluster) > 2 * 0.025/8.) continue;
        coreEnergy+=cellE;
    }
    if(sumEPosCellsEM1<=0.) return 0.;
    return coreEnergy/sumEPosCellsEM1;
}

// Function to determine asymmetry with respect to track. For > 1 track give smallest asymmetry.
float TauPi0BonnClusterCreator::getAsymmetryInEM1WRTTrk(
    const xAOD::CaloCluster* pi0Candidate, 
    const vector<vector<float> > tracksEtaAtSampling,
    const vector<vector<float> > tracksPhiAtSampling)
{
    float minAsymmetryInEM1WRTTrk=2.;  // minimum asymmetryWRT tracks
    vector<vector<float> > asymmetriesInEM1WRTTrks;
    vector<float> asymmetryInEM1WRTTrk;
    asymmetryInEM1WRTTrk.push_back(0.); // here the energy in negative eta direction WRT the track will be stored
    asymmetryInEM1WRTTrk.push_back(0.); // here the energy in positive eta direction WRT the track will be stored
    asymmetryInEM1WRTTrk.push_back(0.); // here the energy in negative phi direction WRT the track will be stored
    asymmetryInEM1WRTTrk.push_back(0.); // here the energy in positive phi direction WRT the track will be stored
    for(unsigned iTrack = 0; iTrack<tracksEtaAtSampling.size();++iTrack){
      asymmetriesInEM1WRTTrks.push_back(asymmetryInEM1WRTTrk);
    }

    float sumEPosCellsEM1=0.;
    const CaloClusterCellLink* theCellLink = pi0Candidate->getCellLinks();
    CaloClusterCellLink::const_iterator cellInClusterItr  = theCellLink->begin();
    CaloClusterCellLink::const_iterator cellInClusterItrE = theCellLink->end();
    for(;cellInClusterItr!=cellInClusterItrE;++cellInClusterItr){
        CaloCell* cellInCluster = (CaloCell*) *cellInClusterItr;
        int sampling = cellInCluster->caloDDE()->getSampling();
        if(sampling!=1 && sampling!=5) continue;
        float cellE = cellInCluster->e() * cellInClusterItr.weight();
        if(cellE<=0) continue;
        sumEPosCellsEM1 += cellE;
        for(unsigned iTrack = 0; iTrack<tracksEtaAtSampling.size();++iTrack){
            float cellEtaWRTTrack = cellInCluster->eta() - tracksEtaAtSampling.at(iTrack).at(sampling);
            float cellPhiWRTTrack = P4Helpers::deltaPhi(cellInCluster->phi(), tracksPhiAtSampling.at(iTrack).at(sampling));

            if(cellEtaWRTTrack<0) asymmetriesInEM1WRTTrks.at(iTrack).at(0)+=cellE;
            else asymmetriesInEM1WRTTrks.at(iTrack).at(1)+=cellE;
            if(cellPhiWRTTrack<0) asymmetriesInEM1WRTTrks.at(iTrack).at(2)+=cellE;
            else asymmetriesInEM1WRTTrks.at(iTrack).at(3)+=cellE;
        }
    }
    if(sumEPosCellsEM1<=0.) return 0.; // default value for clusters with no energy in EM1
    // calculate asymmetry WRT the tracks and determine minimum asymmetry
    for(unsigned iTrack = 0; iTrack<tracksEtaAtSampling.size();++iTrack){
        for(unsigned int iEntry=0;iEntry<asymmetriesInEM1WRTTrks.at(iTrack).size();++iEntry){
            asymmetriesInEM1WRTTrks.at(iTrack).at(iEntry)/=sumEPosCellsEM1;
            if(asymmetriesInEM1WRTTrks.at(iTrack).at(iEntry)>=1.) asymmetriesInEM1WRTTrks.at(iTrack).at(iEntry)=0.999;
        }
        float asymmetryToThisTrack = fabs((asymmetriesInEM1WRTTrks.at(iTrack).at(1)-asymmetriesInEM1WRTTrks.at(iTrack).at(0))*
                                          (asymmetriesInEM1WRTTrks.at(iTrack).at(3)-asymmetriesInEM1WRTTrks.at(iTrack).at(2)));
        if(asymmetryToThisTrack<minAsymmetryInEM1WRTTrk) minAsymmetryInEM1WRTTrk = asymmetryToThisTrack;
    }
    return minAsymmetryInEM1WRTTrk;
}


// Do cluster to shot matching. 
// A cluster is matched to a shot if the seed cell of the shot is in the cluster
std::map<unsigned, xAOD::CaloCluster*> TauPi0BonnClusterCreator::getClusterToShotMap(
    const std::vector<const xAOD::PFO*> shotVector, 
    const xAOD::CaloClusterContainer* pPi0ClusterContainer,
    xAOD::TauJet *pTau)
{
    std::map<unsigned, xAOD::CaloCluster*> clusterToShotMap;
    for(unsigned iShot = 0;iShot<shotVector.size();++iShot){
        int seedHash_int = -1;
        if( shotVector.at(iShot)->attribute(xAOD::PFODetails::PFOAttributes::tauShots_seedHash, seedHash_int) == false) {
            std::cout << "WARNING: Couldn't find seed hash. Set it to -1, no cluster will be associated to shot." << std::endl;
        }
        const IdentifierHash seedHash = (const IdentifierHash) seedHash_int; 
        xAOD::CaloClusterContainer::const_iterator clusterItr   (pPi0ClusterContainer->begin()),
                                                   clusterItrEnd(pPi0ClusterContainer->end());
        float weightInCluster=-1.;
        float weightInPreviousCluster=-1;
        for (; clusterItr != clusterItrEnd; ++clusterItr){
            xAOD::CaloCluster* cluster = (xAOD::CaloCluster*) (*clusterItr);
            weightInCluster=-1.;
            if (cluster->p4().Et() < m_clusterEtCut) continue; // Not interested in clusters that fail the Et cut
            // Cluster container has clusters for all taus.
            // Only run on clusters that belong to this tau
            if (cluster->p4().DeltaR(pTau->p4()) > .4)  continue;
            const CaloClusterCellLink* theCellLink = cluster->getCellLinks();
            CaloClusterCellLink::const_iterator cellItr  = theCellLink->begin();
            CaloClusterCellLink::const_iterator cellItrE = theCellLink->end();
            for(;cellItr!=cellItrE; ++cellItr){
                CaloCell* cellInCluster = (CaloCell*) *cellItr;
                // Check if seed cell is in cluster.
                if(cellInCluster->caloDDE()->calo_hash()!=seedHash) continue;
                weightInCluster = cellItr.weight();
                // found cell, no need to loop over other cells
                break;
            }
            if(weightInCluster<0) continue;
            // Check if cell was already found in a previous cluster
            if(weightInPreviousCluster<0){
                // Cell not found in a previous cluster. 
                // Have to check whether cell is shared with other cluster
                clusterToShotMap[iShot] = cluster;
                weightInPreviousCluster = weightInCluster;
            }
            else{
                // Cell has been found in a previous cluster
                // assign shot to this cluster if it has larger weight for the cell
                // otherwise the shots keeps assigned to the previous cluster
                if(weightInCluster>weightInPreviousCluster){
                    clusterToShotMap[iShot] = cluster;
                }
                // No need to loop over other clusters as cells can not be shared by more than two clusters
                break;
            }
        }
    }
    return clusterToShotMap;
}

std::vector<unsigned> TauPi0BonnClusterCreator::getShotsMatchedToCluster(
    const std::vector<const xAOD::PFO*> shotVector,
    std::map<unsigned, xAOD::CaloCluster*> clusterToShotMap, 
    xAOD::CaloCluster* pPi0Cluster)
{
    std::vector<unsigned> shotsMatchedToCluster;
    for(unsigned iShot = 0;iShot<shotVector.size();++iShot){
        std::map<unsigned, xAOD::CaloCluster*>::iterator itr = clusterToShotMap.find(iShot);
        if(itr==clusterToShotMap.end()) continue;
        if(itr->second!=pPi0Cluster) continue;
        shotsMatchedToCluster.push_back(iShot);
    }
    return shotsMatchedToCluster;
}

int TauPi0BonnClusterCreator::getNPhotons(
    const std::vector<const xAOD::PFO*> shotVector,
    std::vector<unsigned> shotsInCluster
    )
{
    int nPhotons = 0;
    for(unsigned iShot = 0;iShot<shotsInCluster.size();++iShot){
        int curNPhotons=0;
        if(shotVector.at(shotsInCluster.at(iShot))->attribute(xAOD::PFODetails::PFOAttributes::tauShots_nPhotons,curNPhotons) == false)
            ATH_MSG_WARNING("Can't find NHitsInEM1. Set it to 0.");
        nPhotons+=curNPhotons;
    }
    return nPhotons;
}

vector<int> TauPi0BonnClusterCreator::getNPosECells(
    const xAOD::CaloCluster* pi0Candidate)
{
    vector<int> nPosECellsInLayer(3,0); // 3 layers initialised with 0 +ve cells

    const CaloClusterCellLink* theCellLink = pi0Candidate->getCellLinks();
    CaloClusterCellLink::const_iterator cellInClusterItr  = theCellLink->begin();
    CaloClusterCellLink::const_iterator cellInClusterItrE = theCellLink->end();

    for(;cellInClusterItr!=cellInClusterItrE; ++cellInClusterItr){
        CaloCell* cellInCluster = (CaloCell*) *cellInClusterItr;
        int sampling = cellInCluster->caloDDE()->getSampling();
        // Get cell layer: PSB and PSE belong to layer 0,  
        // EMB1 and EME1 to layer 1, EMB2 and EME2 to layer 2. 
        // Cells in EMB3 and EME3 have already been removed 
        // during subtraction.
        int cellLayer = sampling%4;  
        if(cellInCluster->e() > 0) nPosECellsInLayer[cellLayer]++;
    }
    return nPosECellsInLayer;
}


vector<float> TauPi0BonnClusterCreator::get1stEtaMomWRTCluster(
    const xAOD::CaloCluster* pi0Candidate)
{
    vector<float> firstEtaWRTClusterPositionInLayer (4, 0.);  //init with 0. for 0-3 layers
    vector<float> sumEInLayer (4, 0.); //init with 0. for 0-3 layers

    const CaloClusterCellLink* theCellLink = pi0Candidate->getCellLinks();
    CaloClusterCellLink::const_iterator cellInClusterItr  = theCellLink->begin();
    CaloClusterCellLink::const_iterator cellInClusterItrE = theCellLink->end();

    for(;cellInClusterItr!=cellInClusterItrE;++cellInClusterItr){
        CaloCell* cellInCluster = (CaloCell*) *cellInClusterItr;
        int sampling = cellInCluster->caloDDE()->getSampling();
        // Get cell layer: PSB and PSE belong to layer 0,  
        // EMB1 and EME1 to layer 1, EMB2 and EME2 to layer 2. 
        // Cells in EMB3 and EME3 (layer 3) have already been removed 
        // during subtraction.
        int cellLayer = sampling%4;
        
        float cellEtaWRTClusterPos=cellInCluster->eta()-pi0Candidate->eta();
        float cellE=cellInCluster->e();
        if(cellE<=0) continue;
        firstEtaWRTClusterPositionInLayer[cellLayer]+=cellEtaWRTClusterPos*cellE;
        sumEInLayer[cellLayer]+=cellE;
    }

    for(int iLayer=0;iLayer<4;++iLayer){
        if(sumEInLayer[iLayer]!=0) 
            firstEtaWRTClusterPositionInLayer[iLayer]/=fabs(sumEInLayer[iLayer]);
        else firstEtaWRTClusterPositionInLayer[iLayer]=0.;
    }
    return firstEtaWRTClusterPositionInLayer;
}

vector<float> TauPi0BonnClusterCreator::get2ndEtaMomWRTCluster(
    const xAOD::CaloCluster* pi0Candidate)
{
      vector<float> secondEtaWRTClusterPositionInLayer (4, 0.); //init with 0. for 0-3 layers
      vector<float> sumEInLayer (4, 0.); //init with 0. for 0-3 layers

      const CaloClusterCellLink* theCellLink = pi0Candidate->getCellLinks();
      CaloClusterCellLink::const_iterator cellInClusterItr  = theCellLink->begin();
      CaloClusterCellLink::const_iterator cellInClusterItrE = theCellLink->end();

      for(;cellInClusterItr!=cellInClusterItrE;++cellInClusterItr){
            CaloCell* cellInCluster = (CaloCell*) *cellInClusterItr;
            int sampling = cellInCluster->caloDDE()->getSampling();
            // Get cell layer: PSB and PSE belong to layer 0,  
            // EMB1 and EME1 to layer 1, EMB2 and EME2 to layer 2. 
            // Cells in EMB3 and EME3 (layer 3) have already been removed 
            // during subtraction.
            int cellLayer = sampling%4;

            float cellEtaWRTClusterPos=cellInCluster->eta()-pi0Candidate->eta();
            float cellE=cellInCluster->e();
            if(cellE<=0) continue;
            secondEtaWRTClusterPositionInLayer[cellLayer]+=cellEtaWRTClusterPos*cellEtaWRTClusterPos*cellE;
            sumEInLayer[cellLayer]+=cellE;
      }

      for(int iLayer=0;iLayer<4;++iLayer){
            if(sumEInLayer[iLayer]!=0) 
                secondEtaWRTClusterPositionInLayer[iLayer]/=fabs(sumEInLayer[iLayer]);
            else secondEtaWRTClusterPositionInLayer[iLayer]=0.;
      }
      return secondEtaWRTClusterPositionInLayer;
}

void TauPi0BonnClusterCreator::getExtrapolatedPositions(
    const xAOD::TrackParticle * track,
    vector<float> &trackToCaloEta,
    vector<float> &trackToCaloPhi)
{
    for (int layer = 0 ; layer != CaloCell_ID::MINIFCAL0; ++layer) {
        // Only need to extrapolate to Ecal1 (samplings 1 and 5)
        if(layer!=1 && layer!=5){
          trackToCaloEta.push_back(-99999.);
          trackToCaloPhi.push_back(-99999.);
          continue;
        }
        ATH_MSG_DEBUG( "Try extrapolation of track with pt = " << track->pt() << ", eta " << track->eta() << ", phi" << track->phi() << " to layer " << layer);
        // extrapolate track to layer 
        const Trk::TrackParameters* param_at_calo = 0;
        param_at_calo = m_trackToCaloTool->extrapolate(
                *track,
                (CaloCell_ID::CaloSample) layer,
                0.0, Trk::alongMomentum, Trk::pion);

        // store if track extrapolation successful, else use dummy values 
        if(param_at_calo){
            ATH_MSG_DEBUG( "Extrapolated track with eta=" << track->eta()
                            << " phi="<<track->phi()
                            << " to eta=" << param_at_calo->position().eta()
                            << " phi="<<param_at_calo->position().phi() 
                            );
            trackToCaloEta.push_back(param_at_calo->position().eta());
            trackToCaloPhi.push_back(param_at_calo->position().phi());
            delete param_at_calo;
        }
        else {
            ATH_MSG_DEBUG( "Could not extrapolate track with eta = " << track->eta()
                            << " phi=" << track->phi() 
                            );
            trackToCaloEta.push_back(-99999.); //Use something huge to flag the
            trackToCaloPhi.push_back(-99999.); //track as useless
        }
    }
}

