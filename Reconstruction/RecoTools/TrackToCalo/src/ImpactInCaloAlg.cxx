/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ImpactInCaloAlg.cxx
//   Implementation file for class ImpactInCaloAlg
///////////////////////////////////////////////////////////////////

#include "TrackToCalo/ImpactInCaloAlg.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
//#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/StoreGateSvc.h"

// Tracking includes
#include "TrkParameters/TrackParameters.h"
#include <vector>
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"

#include "TrackToCalo/ImpactInCalo.h"

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/ICaloCoordinateTool.h"
#include "CaloDetDescr/CaloDepthTool.h"

#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "TrackToCalo/ImpactInCaloCollection.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

// Constructor with parameters:
ImpactInCaloAlg::ImpactInCaloAlg(const std::string &name,
                                 ISvcLocator *pSvcLocator) :
        AthAlgorithm(name,pSvcLocator),
        m_TrackName("Tracks"),
        m_tracks(0),
	m_particle(0),
	m_calo_id(0),
	m_calo_dd(0),
        m_toCalo("ExtrapolateToCaloTool"),
        m_trackSelectorTool(""),
        m_extrapolateInBothDirections(false),
        m_compareImpWithCluster(false),
        m_printCellsCrossed(false) {
    // Get parameter values from jobOptions file
    declareProperty("TrackName", m_TrackName);
    declareProperty("TrackParticleName", m_TrackParticleName);
    declareProperty("ClusterContainerName", m_cluster_container);
    declareProperty("CaloCellContainerName", m_cell_container);
    declareProperty("ImpactInCaloContainerName", m_ImpactInCalosOutputName);
    declareProperty("TrackInputType", m_trkinput);
    declareProperty("ExtrapolateToCaloTool", m_toCalo, "the extrapolate to calo tool");
    declareProperty("TrackSelectorTool", m_trackSelectorTool, "Tool handle to TrackSelectorTool (if no tool given: use all tracks)");
    declareProperty("CompareImpactWithCluster", m_compareImpWithCluster, "compare impact with clusters?");
    declareProperty("PrintCellsCrossed", m_printCellsCrossed, "print all cells crossed by tracks?");
    declareProperty("ExtrapolateInBothDirections", m_extrapolateInBothDirections, "extrapolate each track in both directions (along and opposite momentum)?");
}

// Initialize method:
StatusCode ImpactInCaloAlg::initialize() {

    // Get the messaging service, print where you are
    ATH_MSG_DEBUG( "ImpactInCaloAlg::initialize()" );

    //StatusCode sc;

    m_calo_dd = CaloDetDescrManager::instance();
    m_calo_id = m_calo_dd->getCaloCell_ID();

    if (m_toCalo.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve " << m_toCalo );
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "ImpactInCaloAlg initialisation OK" );
    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode ImpactInCaloAlg::execute() {
    // Get the messaging service, print where you are
    //StatusCode sc;
    //ATH_MSG_DEBUG( "ImpactInCaloAlg::execute()" );

    // Create the impacts :
    StatusCode sc = CreateTrkImpactInCalo();
    if (sc.isFailure())
        return StatusCode::SUCCESS;

    // Example 1 : you want to read the impacts and compare to clusters
    if (m_compareImpWithCluster) {
        sc = PrintImpact();
        if (sc.isFailure())
	  return StatusCode::SUCCESS;
        sc = CompareImpactWithCluster();
        if (sc.isFailure())
	  return StatusCode::SUCCESS;
    }

    // Example 2 : you want to know the list of cells crossed by the track
    if (m_printCellsCrossed) {
        sc = PrintCellsCrossed();
        if (sc.isFailure())
	  return StatusCode::SUCCESS;
    }

    return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode ImpactInCaloAlg::finalize() {
    ATH_MSG_DEBUG( "ImpactInCaloAlg::finalize()" );

    return StatusCode::SUCCESS;
}


// Loop on Tracks/TrackParticles and create ImpactInCaloCollection
StatusCode ImpactInCaloAlg::CreateTrkImpactInCalo() {
    //StatusCode sc;
    ATH_MSG_DEBUG( "ImpactInCaloAlg::CreateTrkImpactInCalo()" );

    //create and record new ImpactInCalo container
    ImpactInCaloCollection* outputContainer = new ImpactInCaloCollection();
    StatusCode sc=evtStore()->record(outputContainer,m_ImpactInCalosOutputName,false); //"false" indicates it can't be modified by another algorithm
    if(sc != StatusCode::SUCCESS) {
        ATH_MSG_WARNING(" Could not record ImpactInCaloCollection"
        << m_ImpactInCalosOutputName);
        return StatusCode::FAILURE;
    }

    m_particle = 0;
    m_tracks = 0;

    if( m_trkinput == "TrackParticleCandidates") {
        // use track particle collection
        if (m_TrackParticleName == "") {
            ATH_MSG_WARNING("TrackParticleName not set" );
            return StatusCode::FAILURE;
        }

        sc = evtStore()->retrieve(m_particle, m_TrackParticleName);

        if (sc.isFailure()) {
            ATH_MSG_WARNING("TrackParticle not found: " << m_TrackParticleName );
            return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG("TrackParticle found in StoreGate" );
        // loop over track particles and get impacts in calo:
        for (Rec::TrackParticleContainer::const_iterator itr = (*m_particle).begin(); itr != (*m_particle).end(); itr++) {
            if (!(*itr)) continue;
            if (!m_trackSelectorTool.empty()) {
                if (!m_trackSelectorTool->decision(**itr)) {
                    // track out of selection cuts
                    continue;
                }
            }
            // get impacts from ExtrapolateToCaloTool
            ImpactInCalo* imp = m_toCalo->getImpactInCalo(**itr, Trk::undefined, Trk::alongMomentum);
            if(imp)
                outputContainer->push_back(imp);
            else
                ATH_MSG_DEBUG(" ImpactInCalo pointer not valid for this track");
            if (m_extrapolateInBothDirections) {
                ImpactInCalo* imp = m_toCalo->getImpactInCalo(**itr, Trk::undefined, Trk::oppositeMomentum);
                if (imp) outputContainer->push_back(imp);
            }
        } // end loop over track particles
    } else {
        // use track collection
        if (m_TrackName == "") {
            ATH_MSG_WARNING("TrackName not set" );
            return StatusCode::FAILURE;
        }

        sc = evtStore()->retrieve(m_tracks, m_TrackName);

        if (sc.isFailure()) {
            ATH_MSG_WARNING("Tracks not found: " << m_TrackName );
            return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG("Tracks found in StoreGate" );
        // loop over tracks
        for (TrackCollection::const_iterator itr = (*m_tracks).begin(); itr < (*m_tracks).end(); itr++) {
            if (!(*itr))
                continue;
            if (!m_trackSelectorTool.empty()) {
                if (!m_trackSelectorTool->decision(**itr)) {
                    // track out of selection cuts
                    continue;
                }
            }
            // get impacts from ExtrapolateToCaloTool
            ImpactInCalo* imp = m_toCalo->getImpactInCalo(*(*itr), Trk::undefined, Trk::alongMomentum);
            if(imp)
                outputContainer->push_back(imp);
            else
                ATH_MSG_DEBUG(" ImpactInCalo pointer not valid for this track");
            if (m_extrapolateInBothDirections) {
                ImpactInCalo* imp = m_toCalo->getImpactInCalo(*(*itr), Trk::undefined, Trk::oppositeMomentum);
                if (imp) outputContainer->push_back(imp);
            }
        }  // end loop over tracks
    }
    return  StatusCode::SUCCESS;
}


// Retreive ImpactInCaloCollection and compare to CaloClusters
StatusCode ImpactInCaloAlg::CompareImpactWithCluster() {
    StatusCode sc;

    ATH_MSG_DEBUG( "ImpactInCaloAlg::CompareImpactWithCluster()" );

    // loop on clusters

    const CaloClusterContainer* cluster_container;
    sc=evtStore()->retrieve(cluster_container,m_cluster_container);
    if (sc.isFailure()) {
        ATH_MSG_WARNING( "Cluster Container could not be retrieved: " << m_cluster_container );
        return sc;
    }

    const ImpactInCaloCollection* impact_collection;
    sc=evtStore()->retrieve(impact_collection,m_ImpactInCalosOutputName);
    if (sc.isFailure()) {
        ATH_MSG_WARNING( "Impact collection could not be retrieved: " << m_ImpactInCalosOutputName );
        return sc;
    }

    typedef CaloClusterContainer::const_iterator cluster_iterator;
    cluster_iterator f_clu =cluster_container->begin();
    cluster_iterator l_clu = cluster_container->end();

    typedef ImpactInCaloCollection::const_iterator impact_iterator;
    impact_iterator f_imp = impact_collection->begin();
    impact_iterator l_imp = impact_collection->end();

    for ( ; f_clu!=l_clu; f_clu++) {
        const CaloCluster* cluster = (*f_clu);
        double hecluster = cluster->energy()/1000.;
        double heta = cluster->eta();
        double hphi = cluster->phi();

        ATH_MSG_INFO("Found a cluster : E= " << hecluster
        << "(GeV), etaCaloLocal=" <<  heta
        << ", phiCaloLocal=" << hphi);

        for ( ; f_imp!=l_imp; f_imp++) {
            const ImpactInCalo* impact = (*f_imp);

            ATH_MSG_INFO("==> Comparison between cluster and impact in Middle : deta="
            << heta - impact->etaCaloLocal_2()
            << " , dphi=" << hphi - impact->phiCaloLocal_2());
        }
    }
    return StatusCode::SUCCESS;
}

// Retreive ImpactInCaloCollection and print content
StatusCode ImpactInCaloAlg::PrintImpact() {
    StatusCode sc;


    ATH_MSG_INFO( " Method PrintImpacts : " );

    ATH_MSG_INFO( " " );
    ATH_MSG_INFO( " Start with Impacts : " );

    const ImpactInCaloCollection* impact_collection;
    sc=evtStore()->retrieve(impact_collection,m_ImpactInCalosOutputName);

    if(sc.isFailure()) {
        ATH_MSG_WARNING( "Could not retrieve impact colection: " << m_ImpactInCalosOutputName );
        return sc;
    }

    typedef ImpactInCaloCollection::const_iterator impact_iterator;
    impact_iterator f_imp = impact_collection->begin();
    impact_iterator l_imp = impact_collection->end();

    for ( ; f_imp!=l_imp; f_imp++) {
        const ImpactInCalo* impact = (*f_imp);

        const double impPhi = impact->phiCaloLocal_1();
        const double impeta = impact->etaCaloLocal_1();

        ATH_MSG_INFO("Found an impact in strips : parameters are eta = " << impeta
        << " phi = " << impPhi);

        //impact->print();
    }

    if (m_tracks) {

        ATH_MSG_INFO( " " );
        ATH_MSG_INFO(" Now loop on Trk::Track collection " );
        ATH_MSG_INFO( " " );

        for (TrackCollection::const_iterator itr  =
                    (*m_tracks).begin(); itr < (*m_tracks).end(); itr++) {
            const Trk::Perigee *aPer=(*itr)->perigeeParameters();
            if (!aPer) {
                ATH_MSG_WARNING( "Could not get Trk::Perigee from Track" );
            } else {

                const double trkphi = aPer->parameters()[Trk::phi];
                const double trketa = aPer->eta();

                ATH_MSG_INFO("Found a Trk::Track : parameters are eta = " << trketa
                << " phi = " << trkphi);
            }
        }
    }

    if (m_particle) {

        ATH_MSG_INFO( " " );
        ATH_MSG_INFO(" Now loop on Trk::TrackParticle collection " );
        ATH_MSG_INFO( " " );

        for (Rec::TrackParticleContainer::const_iterator itr = m_particle->begin();
                itr != m_particle->end(); ++itr ) {
            if (!(*itr)->originalTrack()) {
                ATH_MSG_WARNING( "Could not get Track from TrackParticle" );
                continue;
            }
            const Trk::Perigee *aPer=((*itr)->originalTrack())->perigeeParameters();
            if (!aPer) {
                ATH_MSG_WARNING( "Could not get Perigee from TrackParticle" );
            } else {

                const double partphi = aPer->parameters()[Trk::phi];
                const double parteta = aPer->eta();

                ATH_MSG_INFO("Found a trackparticle : parameters are eta = " << parteta
                << " phi = " << partphi);
            }
        }
    }
    return StatusCode::SUCCESS;
}

// List of cells crossed by Trk::Tracks in CaloSample
// ( neta and nphi are the total window width, e.g .5x5 )
CaloCellList* ImpactInCaloAlg::CellsCrossedByTrack(const Trk::Track& trk,
        const CaloCell_ID::CaloSample sam,
        int neta, int nphi) {
    CaloCellList* my_list = 0;

    // Retreive CaloCell's from StoreGate :

    const CaloCellContainer* cell_container;
    StatusCode sc=evtStore()->retrieve(cell_container,m_cell_container);
    if ( sc.isFailure()  )
        return 0;

    // Where is the track shooting ?
    double offset = 0.;

    const Trk::TrackParameters* parametersInCalo = m_toCalo->extrapolate (trk, sam, offset);

    if (!parametersInCalo)
        return 0;

    double eta = parametersInCalo->position().eta();
    double phi = parametersInCalo->position().phi();

    // CaloCellList needs both enums: subCalo and CaloSample
    CaloCell_ID::SUBCALO subcalo;
    bool barrel;
    int sampling_or_module;
    m_calo_dd->decode_sample (subcalo, barrel, sampling_or_module, sam);

    // Get the corresponding granularities : needs to know where you are
    //                  the easiest is to look for the CaloDetDescrElement
    const CaloDetDescrElement* dde =
      m_calo_dd->get_element(subcalo,sampling_or_module,barrel,eta,phi);

    double deta = int(neta/2)*dde->deta();
    double dphi = int(nphi/2)*dde->dphi();

    // Construct the list :
    my_list = new CaloCellList(cell_container,subcalo);
    my_list->select(eta,phi,deta,dphi, (int) sam);

    // cleanup
    delete parametersInCalo;

    return my_list ;
}

StatusCode ImpactInCaloAlg::PrintCellsCrossed() {

    // Get the messaging service, print where you are
    StatusCode sc;
    ATH_MSG_INFO( "ImpactInCaloAlg::PrintCellsCrossed()" );

    // Here we are :
    CaloCell_ID::CaloSample sam = CaloCell_ID::EMB2;
    int neta = 5;
    int nphi = 5;

    // get tracks from TDS
    if (m_TrackName == "") {
        ATH_MSG_WARNING("TrackName not set" );
        return StatusCode::FAILURE;
    }

    sc = evtStore()->retrieve(m_tracks, m_TrackName);
    if (sc.isFailure()) {
        ATH_MSG_WARNING("Tracks not found: " << m_TrackName );
        return sc;
    }
    ATH_MSG_DEBUG("Tracks found in StoreGate" );

    for (TrackCollection::const_iterator itr  = (*m_tracks).begin(); itr != (*m_tracks).end(); itr++) {
        if (!(*itr)) {
            ATH_MSG_INFO( "Trackcollection contains empty entry" );
            continue;
        }
        const Trk::Perigee *aPer= (*itr)->perigeeParameters();
        if (!aPer) {
            ATH_MSG_INFO( "Could not get Trk::Perigee from Track" );
        } else {
            double d0 = aPer->parameters()[Trk::d0];
            double z0 = aPer->parameters()[Trk::z0];
            double phi0 = aPer->parameters()[Trk::phi0];
            double theta = aPer->parameters()[Trk::theta];
            double qOverP = aPer->parameters()[Trk::qOverP];
            ATH_MSG_INFO( " " );
            ATH_MSG_INFO("Found a track: parameters are (" << d0  << ", "
            << z0  << ", " << phi0 << ", " << theta << ", " << qOverP << ")");
        }

        // Warning : if anything fails, CellsCrossedByTrack will
        //           simply return a null pointer
        //           if it works, it does a new CaloCellList
        //           ==> the client has to do the delete !!!!

        CaloCellList* my_list = CellsCrossedByTrack(*(*itr), sam, neta, nphi);

        if (my_list) {

            for ( CaloCellList::list_iterator itr  = my_list->begin(); itr < my_list->end(); itr++) {
                ATH_MSG_INFO("found cell ! eta=" << (*itr)->eta()
                << " phi=" << (*itr)->phi() << " energy=" << (*itr)->energy());
            }
            delete my_list;
        } else
            ATH_MSG_INFO( "could not get cell list " );
    }
    return StatusCode::SUCCESS;
}
