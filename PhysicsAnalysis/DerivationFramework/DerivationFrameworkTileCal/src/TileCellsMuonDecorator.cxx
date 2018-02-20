///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCellsMuonDecorator.cxx 
// Implementation file for class TileCellsMuonDecorator
/////////////////////////////////////////////////////////////////// 

// PhysicsAnalysis/DerivationFramework/DerivationFrameworkTileCal includes
#include "DerivationFrameworkTileCal/TileCellsMuonDecorator.h"
#include "DerivationFrameworkTileCal/TileCellsDecorator.h"

#include "AthenaKernel/errorcheck.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "ParticleCaloExtension/ParticleCellIntersection.h"
#include "ParticleCaloExtension/ParticleCaloAssociation.h"
#include "ParticlesInConeTools/ITrackParticlesInConeTool.h"

#include "xAODMuon/MuonContainer.h"
//#include "xAODCaloEvent/CaloCluster.h"

// Calo includes
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"


// Tile includes


#include <vector>
#include <algorithm>

namespace DerivationFramework {

  TileCellsMuonDecorator::TileCellsMuonDecorator( const std::string& type, const std::string& name, const IInterface* parent )
    : AthAlgTool(type, name, parent)
      //    , m_caloCellAssociationTool("Rec::ParticleCaloCellAssociationTool/ParticleCaloCellAssociationTool")
    , m_trackInCalo("TrackTools")
    , m_tracksInCone("xAOD::TrackParticlesInConeTool/TrackParticlesInConeTool")
    , m_cellsDecorator("DerivationFramework::TileCellsDecorator/TileCellsDecorator")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty( "CellContainer", m_cellContainer = "AODCellContainer" );
    declareProperty( "MuonContainer", m_muonContainer = "Muons" );
    declareProperty( "TracksInConeTool", m_tracksInCone);
    declareProperty( "CellsDecorator", m_cellsDecorator );
    declareProperty( "Prefix", m_prefix = "TCAL1_" );
    declareProperty( "SelectMuons", m_selectMuons = false);
    declareProperty( "MinMuonPt", m_minPt = 10000.0);
    declareProperty( "MaxAbsMuonEta", m_maxAbsEta = 1.7);
    declareProperty( "IsoCone", m_isoCone = 0.4);
    declareProperty( "MaxRelETrkInIsoCone", m_maxRelEtrkInIsoCone = 0.1);
    declareProperty( "SaveTileCellMuonInfo", m_saveTileCellMuonInfo = true );
  }


  StatusCode TileCellsMuonDecorator::initialize() {

    //    CHECK(m_caloCellAssociationTool.retrieve());
    CHECK(m_trackInCalo.retrieve());
    CHECK(m_tracksInCone.retrieve());
    CHECK(m_cellsDecorator.retrieve());

    return StatusCode::SUCCESS;
  }

  StatusCode TileCellsMuonDecorator::addBranches() const {

    //    const CaloCellContainer* calo_cells = 0;
    //    CHECK( evtStore()->retrieve(calo_cells, m_cellContainer) );

    const xAOD::MuonContainer* muons = 0;
    CHECK( evtStore()->retrieve(muons, m_muonContainer) );

    std::vector<const CaloCell*> cells;

    static SG::AuxElement::Decorator<int> selected_mu_decorator(m_prefix + "SelectedMuon");

    for ( const xAOD::Muon* mu : *muons ) {

      std::vector< float > cells_mu_x;
      std::vector< float > cells_mu_y;
      std::vector< float > cells_mu_z;
      std::vector< float > cells_mu_eta;
      std::vector< float > cells_mu_phi;

      std::vector< float > cells_to_mu_dx;
      std::vector< float > cells_to_mu_dy;
      std::vector< float > cells_to_mu_dz;
      std::vector< float > cells_to_mu_deta;
      std::vector< float > cells_to_mu_dphi;
      
      std::vector< float > cells_mu_dx;
      std::vector< float > cells_mu_dedx;


      if (m_selectMuons && 
          (mu->muonType() != xAOD::Muon::Combined 
           || mu->pt() < m_minPt 
           || std::abs(mu->eta()) > m_maxAbsEta)) {

        selected_mu_decorator(*mu) = 0;
        continue;
      }

      const xAOD::TrackParticle* mu_track = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
      const xAOD::CaloCluster* mu_cluster = mu->cluster();
      if (mu_track && mu_cluster && mu_cluster->getCellLinks()) {

        float e_trk_in_isocone(0.0);
        std::vector<const xAOD::TrackParticle*> tracks_in_cone;
        m_tracksInCone->particlesInCone(mu_track->eta(), mu_track->phi(), m_isoCone, tracks_in_cone);
        for (const xAOD::TrackParticle* track : tracks_in_cone) {
          if (track != mu_track) e_trk_in_isocone += track->e();
        }

        static SG::AuxElement::Decorator<float> econe_mu_decorator(m_prefix + "etrkcone" + std::to_string(int(m_isoCone * 100)));
        econe_mu_decorator(*mu) = e_trk_in_isocone;

        if (m_selectMuons && (e_trk_in_isocone > m_maxRelEtrkInIsoCone * mu_track->e())) {
          selected_mu_decorator(*mu) = 0;
          continue;
        }

        selected_mu_decorator(*mu) = 1;

        cells.clear();
        for (const CaloCell* cell : *mu_cluster) {

          const CaloDetDescrElement* cell_dde = cell->caloDDE();
          if (!(cell_dde->is_tile())) continue;

          cells.push_back(cell);
          
          if (m_saveTileCellMuonInfo) {
            std::vector<double> coordinates = m_trackInCalo->getXYZEtaPhiInCellSampling(mu_track, cell);
            
            if (coordinates.size() == 5 ) {
         
              float path_length = m_trackInCalo->getPathInsideCell(mu_track, cell);
              cells_mu_dx.push_back( path_length );
              cells_mu_dedx.push_back( (path_length > 0 ? (cell->energy() / path_length) : -1.0) );
              
              cells_mu_x.push_back(coordinates[0]);
              cells_mu_y.push_back(coordinates[1]);
              cells_mu_z.push_back(coordinates[2]);
              cells_mu_eta.push_back(coordinates[3]);
              cells_mu_phi.push_back(coordinates[4]);
              
              cells_to_mu_dx.push_back(cell->x() - coordinates[0]);
              cells_to_mu_dy.push_back(cell->y() - coordinates[1]);
              cells_to_mu_dz.push_back(cell->z() - coordinates[2]);
              cells_to_mu_deta.push_back(cell->eta() - coordinates[3]);
              cells_to_mu_dphi.push_back( KinematicUtils::deltaPhi(coordinates[4], cell->phi()) );
              
            } else {
              
              cells_mu_dx.push_back( 0.0 );
              cells_mu_dedx.push_back( -2.0 );
              
              cells_mu_x.push_back(0.0);
              cells_mu_y.push_back(0.0);
              cells_mu_z.push_back(0.0);
              cells_mu_eta.push_back(0.0);
              cells_mu_phi.push_back(0.0);
              
              cells_to_mu_dx.push_back(0.0);
              cells_to_mu_dy.push_back(0.0);
              cells_to_mu_dz.push_back(0.0);
              cells_to_mu_deta.push_back(0.0);
              cells_to_mu_dphi.push_back(0.0);
              
            }
            
          }
        }

        CHECK( m_cellsDecorator->decorate(mu, cells) );        

      } else {
        selected_mu_decorator(*mu) = 0;
      }


      if (m_saveTileCellMuonInfo) {

        static SG::AuxElement::Decorator< std::vector<float> > cells_mu_x_decorator(m_prefix + "cells_muon_x");
        static SG::AuxElement::Decorator< std::vector<float> > cells_mu_y_decorator(m_prefix + "cells_muon_y");
        static SG::AuxElement::Decorator< std::vector<float> > cells_mu_z_decorator(m_prefix + "cells_muon_z");
        static SG::AuxElement::Decorator< std::vector<float> > cells_mu_eta_decorator(m_prefix + "cells_muon_eta");
        static SG::AuxElement::Decorator< std::vector<float> > cells_mu_phi_decorator(m_prefix + "cells_muon_phi");
        
        static SG::AuxElement::Decorator< std::vector<float> > cells_to_mu_dx_decorator(m_prefix + "cells_to_muon_dx");
        static SG::AuxElement::Decorator< std::vector<float> > cells_to_mu_dy_decorator(m_prefix + "cells_to_muon_dy");
        static SG::AuxElement::Decorator< std::vector<float> > cells_to_mu_dz_decorator(m_prefix + "cells_to_muon_dz");
        static SG::AuxElement::Decorator< std::vector<float> > cells_to_mu_deta_decorator(m_prefix + "cells_to_muon_deta");
        static SG::AuxElement::Decorator< std::vector<float> > cells_to_mu_dphi_decorator(m_prefix + "cells_to_muon_dphi");
        
        static SG::AuxElement::Decorator< std::vector<float> > cells_mu_dx_decorator(m_prefix + "cells_muon_dx");
        static SG::AuxElement::Decorator< std::vector<float> > cells_mu_dedx_decorator(m_prefix + "cells_muon_dedx");
        
        cells_mu_x_decorator(*mu) = std::move(cells_mu_x);
        cells_mu_y_decorator(*mu) = std::move(cells_mu_y);
        cells_mu_z_decorator(*mu) = std::move(cells_mu_z);
        cells_mu_eta_decorator(*mu) = std::move(cells_mu_eta);
        cells_mu_phi_decorator(*mu) = std::move(cells_mu_phi);
        
        cells_to_mu_dx_decorator(*mu) = std::move(cells_to_mu_dx);
        cells_to_mu_dy_decorator(*mu) = std::move(cells_to_mu_dy);
        cells_to_mu_dz_decorator(*mu) = std::move(cells_to_mu_dz);
        cells_to_mu_deta_decorator(*mu) = std::move(cells_to_mu_deta);
        cells_to_mu_dphi_decorator(*mu) = std::move(cells_to_mu_dphi);
        
        cells_mu_dx_decorator(*mu) = std::move(cells_mu_dx);
        cells_mu_dedx_decorator(*mu) = std::move(cells_mu_dedx);

      }

    }

    return StatusCode::SUCCESS;
  }

}
