/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  TrackParticleCaloCellDecorator.cxx  			  */

/*  Decorates the InDetTrackParticles Container with calorimeter  */
/*  cell information. 						  */

/*  Author: Jochen Jens Heinrich (jochen.jens.heinrich@cern.ch)   */	

#include "DerivationFrameworkSUSY/TrackParticleCaloCellDecorator.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  TrackParticleCaloCellDecorator::TrackParticleCaloCellDecorator( const std::string& t,
      								  const std::string& n,
      								  const IInterface* p) :
    								  AthAlgTool(t,n,p),
    								  m_sgName(""),
    								  m_containerName("") {
	declareInterface<DerivationFramework::IAugmentationTool>(this);
    	declareProperty("DecorationPrefix", m_sgName);
    	declareProperty("ContainerName", m_containerName);
  }

  StatusCode TrackParticleCaloCellDecorator::initialize() {

  	if ( m_sgName == "" ) {
  		ATH_MSG_WARNING("No decoration prefix name provided for the output of TrackParticleCaloCellDecorator!");
   		return StatusCode::SUCCESS;
  	}
  	if ( m_containerName == "") {
		ATH_MSG_WARNING("No TrackParticle collection provided for TrackParticleCaloCellDecorator!");
		return StatusCode::SUCCESS;
  	}
  	return StatusCode::SUCCESS;
  }

  StatusCode TrackParticleCaloCellDecorator::finalize() {
  	return StatusCode::SUCCESS;
  }

  StatusCode TrackParticleCaloCellDecorator::addBranches() const  {

	std::unique_ptr<std::vector<std::vector<float> > > cellEta( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > cellPhi( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > cellR( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > cellDEta( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > cellDPhi( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > cellDR( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > cellX( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > cellY( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > cellZ( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > celldX( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > celldY( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > celldZ( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > cellT( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<float> > > cellE( new std::vector<std::vector<float>>() );
	std::unique_ptr<std::vector<std::vector<unsigned int> > > cellID( new std::vector<std::vector<unsigned int>>() );
	std::unique_ptr<std::vector<std::vector<int> > > cellSampling( new std::vector<std::vector<int>>() );
	std::unique_ptr<std::vector<std::vector<int> > > cellQuality( new std::vector<std::vector<int>>() );
	std::unique_ptr<std::vector<std::vector<int> > > cellProvenance( new std::vector<std::vector<int>>() );
	std::unique_ptr<std::vector<std::vector<int> > > cellGain( new std::vector<std::vector<int>>() );
	std::vector<float> trackCellEta;
	std::vector<float> trackCellPhi;
	std::vector<float> trackCellR;
	std::vector<float> trackCellDEta;
	std::vector<float> trackCellDPhi;
	std::vector<float> trackCellDR;
	std::vector<float> trackCellX;
	std::vector<float> trackCellY;
	std::vector<float> trackCellZ;
	std::vector<float> trackCelldX;
	std::vector<float> trackCelldY;
	std::vector<float> trackCelldZ;
	std::vector<float> trackCellT;
	std::vector<float> trackCellE;
	std::vector<unsigned int> trackCellID;
	std::vector<int> trackCellSampling;
	std::vector<int> trackCellQuality;
	std::vector<int> trackCellProvenance;
	std::vector<int> trackCellGain;
        SG::AuxElement::Decorator< std::vector<float> > dec_cellEta( m_sgName+"_CaloCellEta");
        SG::AuxElement::Decorator< std::vector<float> > dec_cellPhi( m_sgName+"_CaloCellPhi");
        SG::AuxElement::Decorator< std::vector<float> > dec_cellR( m_sgName+"_CaloCellR");
        SG::AuxElement::Decorator< std::vector<float> > dec_cellDEta( m_sgName+"_CaloCelldEta");
        SG::AuxElement::Decorator< std::vector<float> > dec_cellDPhi( m_sgName+"_CaloCelldPhi");
        SG::AuxElement::Decorator< std::vector<float> > dec_cellDR( m_sgName+"_CaloCelldR");
        SG::AuxElement::Decorator< std::vector<float> > dec_cellX( m_sgName+"_CaloCellX");
        SG::AuxElement::Decorator< std::vector<float> > dec_cellY( m_sgName+"_CaloCellY");
        SG::AuxElement::Decorator< std::vector<float> > dec_cellZ( m_sgName+"_CaloCellZ");
        SG::AuxElement::Decorator< std::vector<float> > dec_celldX( m_sgName+"_CaloCelldX");
        SG::AuxElement::Decorator< std::vector<float> > dec_celldY( m_sgName+"_CaloCelldY");
        SG::AuxElement::Decorator< std::vector<float> > dec_celldZ( m_sgName+"_CaloCelldZ");
        SG::AuxElement::Decorator< std::vector<float> > dec_cellT( m_sgName+"_CaloCellTime");
        SG::AuxElement::Decorator< std::vector<float> > dec_cellE( m_sgName+"_CaloCellE");
        SG::AuxElement::Decorator< std::vector<unsigned int> > dec_cellID( m_sgName+"_CaloCellID");
        SG::AuxElement::Decorator< std::vector<int> > dec_cellSampling( m_sgName+"_CaloCellSampling");
        SG::AuxElement::Decorator< std::vector<int> > dec_cellQuality( m_sgName+"_CaloCellQuality");
        SG::AuxElement::Decorator< std::vector<int> > dec_cellProvenance( m_sgName+"_CaloCellProvenance");
        SG::AuxElement::Decorator< std::vector<int> > dec_cellGain( m_sgName+"_CaloCellGain");

  	// retrieve track container
  	const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_containerName );
  	if( ! tracks ) {
		ATH_MSG_WARNING ("Couldn't retrieve TrackParticles with key: " << m_containerName );
        	return StatusCode::SUCCESS;
  	}
  	// Loop over tracks
  	for ( xAOD::TrackParticleContainer::const_iterator track_itr = tracks->begin(); track_itr != tracks->end(); ++track_itr) {
		trackCellEta.clear();
		trackCellPhi.clear();
		trackCellR.clear();
		trackCellDEta.clear();
		trackCellDPhi.clear();
		trackCellDR.clear();
		trackCellX.clear();
		trackCellY.clear();
		trackCellZ.clear();
		trackCelldX.clear();
		trackCelldY.clear();
		trackCelldZ.clear();
		trackCellT.clear();
		trackCellE.clear();
		trackCellID.clear();
		trackCellSampling.clear();
		trackCellQuality.clear();
		trackCellProvenance.clear();
		trackCellGain.clear();
		cellEta->push_back(trackCellEta);
		cellPhi->push_back(trackCellPhi);
		cellR->push_back(trackCellR);
		cellDEta->push_back(trackCellDEta);
		cellDPhi->push_back(trackCellDPhi);
		cellDR->push_back(trackCellDR);
		cellX->push_back(trackCellX);
		cellY->push_back(trackCellY);
		cellZ->push_back(trackCellZ);
		celldX->push_back(trackCelldX);
		celldY->push_back(trackCelldY);
		celldZ->push_back(trackCelldZ);
		cellT->push_back(trackCellT);
		cellE->push_back(trackCellE);
		cellID->push_back(trackCellID);
		cellSampling->push_back(trackCellSampling);
		cellQuality->push_back(trackCellQuality);
		cellProvenance->push_back(trackCellProvenance);
		cellGain->push_back(trackCellGain);
        	dec_cellEta( **track_itr ) = trackCellEta;
        	dec_cellPhi( **track_itr ) = trackCellPhi;
        	dec_cellR( **track_itr ) = trackCellR;
        	dec_cellDEta( **track_itr ) = trackCellDEta;
        	dec_cellDPhi( **track_itr ) = trackCellDPhi;
        	dec_cellDR( **track_itr ) = trackCellDR;
        	dec_cellX( **track_itr ) = trackCellX;
        	dec_cellY( **track_itr ) = trackCellY;
        	dec_cellZ( **track_itr ) = trackCellZ;
        	dec_celldX( **track_itr ) = trackCelldX;
        	dec_celldY( **track_itr ) = trackCelldY;
        	dec_celldZ( **track_itr ) = trackCelldZ;
        	dec_cellT( **track_itr ) = trackCellT;
        	dec_cellE( **track_itr ) = trackCellE;
        	dec_cellID( **track_itr ) = trackCellID;
        	dec_cellSampling( **track_itr ) = trackCellSampling;
        	dec_cellQuality( **track_itr ) = trackCellQuality;
        	dec_cellProvenance( **track_itr ) = trackCellProvenance;
        	dec_cellGain( **track_itr ) = trackCellGain;
  	} // end of loop over tracks                                 


	// Look at associated clusters
	if ( !evtStore()->contains<xAOD::TrackParticleClusterAssociationContainer>( m_containerName+"ClusterAssociations") ) {
                ATH_MSG_WARNING ("Couldn't retrieve TrackParticleClusterAssociations with key: " << m_containerName+"ClusterAssociations" );
                return StatusCode::SUCCESS;
        }

	const xAOD::TrackParticleClusterAssociationContainer* associatedClusters = 0;
 	CHECK( evtStore()->retrieve( associatedClusters, m_containerName+"ClusterAssociations") );
    	//ATH_MSG_INFO ("#(InDetTrackParticlesClusterAssociations) = " << associatedClusters->size());
	for ( const auto* assocClusters : *associatedClusters ) {
      		ATH_MSG_DEBUG ("InDetTrackParticlesClusterAssociations index = " << assocClusters->index());
		// flollow the link to the track particle
      		const xAOD::TrackParticle* trk = 0;
      		if (assocClusters->trackParticleLink().isValid()) {
        		trk = *(assocClusters->trackParticleLink());
		}
		else if ( !assocClusters->trackParticleLink().isValid() ){
			ATH_MSG_ERROR ("trackParticleLink is not valid! " );
		}
		// follow the link to the calorimeter clusters
		//ATH_MSG_INFO ("#(CaloCluster) = " << assocClusters->caloClusterLinks().size());
      		if (assocClusters->caloClusterLinks().size()) {
			for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
				trackCellEta.clear();
				trackCellPhi.clear();
				trackCellR.clear();
				trackCellDEta.clear();
				trackCellDPhi.clear();
				trackCellDR.clear();
				trackCellX.clear();
				trackCellY.clear();
				trackCellZ.clear();
				trackCelldX.clear();
				trackCelldY.clear();
				trackCelldZ.clear();
				trackCellT.clear();
				trackCellE.clear();
				trackCellID.clear();
				trackCellSampling.clear();
				trackCellQuality.clear();
				trackCellProvenance.clear();
				trackCellGain.clear();
          			const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));
				const CaloClusterCellLink* cellLinks = cluster->getCellLinks();
          			if ( !cellLinks ) {
					ATH_MSG_ERROR ("Unable to get cell links!");
					continue;
				}
          			CaloClusterCellLink::const_iterator cellIter = cellLinks->begin();
          			CaloClusterCellLink::const_iterator cellIterEnd = cellLinks->end();
          			for ( ; cellIter != cellIterEnd; ++cellIter ) {
            				const CaloCell* pCell = (*cellIter);
					const CaloDetDescrElement *caloDDE = pCell->caloDDE();
					int sampling = -1;
            				if ( caloDDE ) {
              					//ATH_MSG_INFO ("CaloCell x = " << caloDDE->x());
						sampling = caloDDE->getSampling();
						trackCellEta.push_back(caloDDE->eta());
						trackCellPhi.push_back(caloDDE->phi());
						trackCellR.push_back(caloDDE->r());
						trackCellDEta.push_back(caloDDE->deta());
						trackCellDPhi.push_back(caloDDE->dphi());
						trackCellDR.push_back(caloDDE->dr());
						trackCellX.push_back(caloDDE->x());
						trackCellY.push_back(caloDDE->y());
						trackCellZ.push_back(caloDDE->z());
						trackCelldX.push_back(caloDDE->dx());
						trackCelldY.push_back(caloDDE->dy());
						trackCelldZ.push_back(caloDDE->dz());
						trackCellT.push_back(pCell->time());
						trackCellE.push_back(pCell->e());
						Identifier32 IdOfCell = pCell->ID().get_identifier32();
						trackCellID.push_back(IdOfCell.get_compact());
						trackCellSampling.push_back(sampling);
						trackCellQuality.push_back(pCell->quality());
						trackCellProvenance.push_back(pCell->provenance());
						trackCellGain.push_back(pCell->gain());
					} // if ( caloDDE )
				} // for cellIter
			} // for caloClusterLinks
		} // if caloClusterLinks().size
		cellEta->at(trk->index()) = trackCellEta;
		cellPhi->at(trk->index()) = trackCellPhi;
		cellR->at(trk->index()) = trackCellR;
		cellDEta->at(trk->index()) = trackCellDEta;
		cellDPhi->at(trk->index()) = trackCellDPhi;
		cellDR->at(trk->index()) = trackCellDR;
		cellX->at(trk->index()) = trackCellX;
		cellY->at(trk->index()) = trackCellY;
		cellZ->at(trk->index()) = trackCellZ;
		celldX->at(trk->index()) = trackCelldX;
		celldY->at(trk->index()) = trackCelldY;
		celldZ->at(trk->index()) = trackCelldZ;
		cellT->at(trk->index()) = trackCellT;
		cellE->at(trk->index()) = trackCellE;
		cellID->at(trk->index()) = trackCellID;
		cellSampling->at(trk->index()) = trackCellSampling;
		cellQuality->at(trk->index()) = trackCellQuality;
		cellProvenance->at(trk->index()) = trackCellProvenance;
		cellGain->at(trk->index()) = trackCellGain;
                dec_cellEta( *trk ) = trackCellEta;
                dec_cellPhi( *trk ) = trackCellPhi;
                dec_cellR( *trk ) = trackCellR;
                dec_cellDEta( *trk ) = trackCellDEta;
                dec_cellDPhi( *trk ) = trackCellDPhi;
                dec_cellDR( *trk ) = trackCellDR;
                dec_cellX( *trk ) = trackCellX;
                dec_cellY( *trk ) = trackCellY;
                dec_cellZ( *trk ) = trackCellZ;
                dec_celldX( *trk ) = trackCelldX;
                dec_celldY( *trk ) = trackCelldY;
                dec_celldZ( *trk ) = trackCelldZ;
                dec_cellT( *trk ) = trackCellT;
                dec_cellE( *trk ) = trackCellE;
                dec_cellID( *trk ) = trackCellID;
                dec_cellSampling( *trk ) = trackCellSampling;
                dec_cellQuality( *trk ) = trackCellQuality;
                dec_cellProvenance( *trk ) = trackCellProvenance;
                dec_cellGain( *trk ) = trackCellGain;
	} // for assoc clusters

	CHECK( evtStore()->record( std::move(cellEta), m_sgName+"_CaloCellEta" ));       
	CHECK( evtStore()->record( std::move(cellPhi), m_sgName+"_CaloCellPhi" ));       
	CHECK( evtStore()->record( std::move(cellR), m_sgName+"_CaloCellR" ));       
	CHECK( evtStore()->record( std::move(cellDEta), m_sgName+"_CaloCelldEta" ));       
	CHECK( evtStore()->record( std::move(cellDPhi), m_sgName+"_CaloCelldPhi" ));       
	CHECK( evtStore()->record( std::move(cellDR), m_sgName+"_CaloCelldR" ));       
	CHECK( evtStore()->record( std::move(cellX), m_sgName+"_CaloCellX" ));       
        CHECK( evtStore()->record( std::move(cellY), m_sgName+"_CaloCellY" ));
        CHECK( evtStore()->record( std::move(cellZ), m_sgName+"_CaloCellZ" ));
        CHECK( evtStore()->record( std::move(celldX), m_sgName+"_CaloCelldX" ));
        CHECK( evtStore()->record( std::move(celldY), m_sgName+"_CaloCelldY" ));
        CHECK( evtStore()->record( std::move(celldZ), m_sgName+"_CaloCelldZ" ));
        CHECK( evtStore()->record( std::move(cellT), m_sgName+"_CaloCellT" ));
        CHECK( evtStore()->record( std::move(cellE), m_sgName+"_CaloCellE" ));
        CHECK( evtStore()->record( std::move(cellID), m_sgName+"_CaloCellID" ));
        CHECK( evtStore()->record( std::move(cellSampling), m_sgName+"_CaloCellSampling" ));
        CHECK( evtStore()->record( std::move(cellQuality), m_sgName+"_CaloCellQuality" ));
        CHECK( evtStore()->record( std::move(cellProvenance), m_sgName+"_CaloCellProvenance" ));
        CHECK( evtStore()->record( std::move(cellGain), m_sgName+"_CaloCellGain" ));

  	return StatusCode::SUCCESS;
  } 

}
