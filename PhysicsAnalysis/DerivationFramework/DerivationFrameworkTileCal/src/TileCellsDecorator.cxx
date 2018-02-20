///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCellsDecorator.cxx 
// Implementation file for class TileCellsDecorator
/////////////////////////////////////////////////////////////////// 

// PhysicsAnalysis/DerivationFramework/DerivationFrameworkTileCal includes
#include "DerivationFrameworkTileCal/TileCellsDecorator.h"

#include "AthenaKernel/errorcheck.h"

#include "xAODBase/IParticle.h"

// Calo includes
#include "TileEvent/TileCell.h"
#include "TileIdentifier/TileHWID.h"

#include "CaloIdentifier/TileID.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/TileID.h"

// Tile includes


#include <vector>


namespace DerivationFramework {

  static const InterfaceID IID_TileCellsDecorator("TileCellsDecorator", 1, 0);
  const InterfaceID& TileCellsDecorator::interfaceID( ) {  return IID_TileCellsDecorator; }


  TileCellsDecorator::TileCellsDecorator( const std::string& type, const std::string& name, const IInterface* parent )
    : AthAlgTool(type, name, parent)
  {
    declareInterface<TileCellsDecorator>(this);        

    declareProperty( "Prefix", m_prefix = "TCAL1_" );
    declareProperty( "SaveTileCellPmtInfo", m_saveTileCellPmtInfo = true );
    declareProperty( "SaveTileCellPositionAndDimention", m_saveTileCellPositionAndDimention = true );
  }


  StatusCode TileCellsDecorator::initialize() {
    return StatusCode::SUCCESS;
  }

  StatusCode TileCellsDecorator::decorate(const xAOD::IParticle* particle, std::vector<const CaloCell*>& cells) const {

    CHECK( detStore()->retrieve(m_tileID) );
    CHECK( detStore()->retrieve(m_tileHWID) );

    std::vector< float > cells_energy;
    std::vector< float > cells_et;
    std::vector< float > cells_eta;
    std::vector< float > cells_phi;
    
    std::vector< int > cells_gain;
    std::vector< bool > cells_bad;
    std::vector< int > cells_sampling;
    std::vector< float > cells_time;
    std::vector< unsigned short > cells_quality;
    
    std::vector< float > cells_sinTh;
    std::vector< float > cells_cosTh;
    std::vector< float > cells_cotTh;
    std::vector< float > cells_x;
    std::vector< float > cells_y;
    std::vector< float > cells_z;
    
    std::vector< float > cells_r;
    std::vector< float > cells_dx;
    std::vector< float > cells_dy;
    std::vector< float > cells_dz;
    std::vector< float > cells_dr;
    std::vector< float > cells_volume;
    std::vector< float > cells_dphi;
    std::vector< float > cells_deta;
    
    std::vector< int > cells_side;
    std::vector< unsigned short > cells_section;
    std::vector< unsigned short > cells_module;
    std::vector< unsigned short > cells_tower;
    std::vector< unsigned short > cells_sample;
    
    std::vector< unsigned short > cells_pmt1_ros;
    std::vector< unsigned short > cells_pmt2_ros;
    std::vector< unsigned short > cells_pmt1_drawer;
    std::vector< unsigned short > cells_pmt2_drawer;
    std::vector< unsigned short > cells_pmt1_channel;
    std::vector< unsigned short > cells_pmt2_channel;
    
    std::vector< float > cells_pmt1_energy;
    std::vector< float > cells_pmt2_energy;
    std::vector< float > cells_pmt1_time;
    std::vector< float > cells_pmt2_time;
    
    std::vector< unsigned short > cells_pmt1_quality;
    std::vector< unsigned short > cells_pmt2_quality;
    std::vector< unsigned short > cells_pmt1_qbit;
    std::vector< unsigned short > cells_pmt2_qbit;
    
    std::vector< bool > cells_pmt1_bad;
    std::vector< bool > cells_pmt2_bad;
    std::vector< unsigned short > cells_pmt1_gain;
    std::vector< unsigned short > cells_pmt2_gain;
    
    
    for (const CaloCell* cell : cells) {

      const CaloDetDescrElement* cell_dde = cell->caloDDE();
      
      // Cell energy and eta/phi
      cells_energy.push_back( cell->energy() );
      cells_et.push_back( cell->et() );
      cells_eta.push_back( cell->eta() );
      cells_phi.push_back( cell->phi() );
      
      
      cells_gain.push_back( cell->gain() );
      cells_bad.push_back( cell->badcell() );
      cells_sampling.push_back( cell_dde->getSampling() );
      cells_time.push_back( cell->time() );
      cells_quality.push_back( cell->quality() );
      
      
      if (m_saveTileCellPositionAndDimention) {
        
        // Cell positioning
        cells_sinTh.push_back( cell->sinTh() );
        cells_cosTh.push_back( cell->cosTh() );
        cells_cotTh.push_back( cell->cotTh() );
        cells_x.push_back( cell->x() );
        cells_y.push_back( cell->y() );
        cells_z.push_back( cell->z() );
        
        // Cells dimensions
        cells_r.push_back( cell_dde->r() );
        cells_dx.push_back( cell_dde->dx() );
        cells_dy.push_back( cell_dde->dy() );
        cells_dz.push_back( cell_dde->dz() );
        cells_dr.push_back( cell_dde->dr() );
        cells_volume.push_back( cell_dde->volume() );
        cells_dphi.push_back( cell_dde->dphi() );
        cells_deta.push_back( cell_dde->deta() );
        
        cells_side.push_back( m_tileID->side(cell->ID()) );
        cells_section.push_back( m_tileID->section(cell->ID()) );
        cells_module.push_back( m_tileID->module(cell->ID()) );
        cells_tower.push_back( m_tileID->tower(cell->ID()) );
        cells_sample.push_back( m_tileID->sample(cell->ID()) );
        
      }
      
      if (m_saveTileCellPmtInfo) {
        
        // Tile PMT information
        const TileCell* tile_cell = dynamic_cast<const TileCell*> (cell);
        
        // Tile cell positioning
        int ros1 = m_tileHWID->ros(m_tileHWID->adc_id(cell_dde->onl1(), tile_cell->gain1()));
        int ros2 = m_tileHWID->ros(m_tileHWID->adc_id(cell_dde->onl2(), tile_cell->gain2()));
        int drawer1 = m_tileHWID->drawer(m_tileHWID->adc_id(cell_dde->onl1(), tile_cell->gain1()));
        int drawer2 = m_tileHWID->drawer(m_tileHWID->adc_id(cell_dde->onl2(), tile_cell->gain2()));
        int channel1 = m_tileHWID->channel(m_tileHWID->adc_id(cell_dde->onl1(), tile_cell->gain1()));
        int channel2 = m_tileHWID->channel(m_tileHWID->adc_id(cell_dde->onl2(), tile_cell->gain2()));
        
        cells_pmt1_ros.push_back( ros1 );
        cells_pmt2_ros.push_back( ros2 );
        cells_pmt1_drawer.push_back( drawer1 );
        cells_pmt2_drawer.push_back( drawer2 );
        cells_pmt1_channel.push_back( channel1 );
        cells_pmt2_channel.push_back( channel2 );
        
        cells_pmt1_energy.push_back( tile_cell->ene1() );
        cells_pmt2_energy.push_back( tile_cell->ene2() );
        cells_pmt1_time.push_back( tile_cell->time1() );
        cells_pmt2_time.push_back( tile_cell->time2() );
        cells_pmt1_quality.push_back( tile_cell->qual1() );
        cells_pmt2_quality.push_back( tile_cell->qual2() );
        cells_pmt1_qbit.push_back( tile_cell->qbit1() );
        cells_pmt2_qbit.push_back( tile_cell->qbit2() );
        cells_pmt1_bad.push_back( tile_cell->badch1() );
        cells_pmt2_bad.push_back( tile_cell->badch2() );
        cells_pmt1_gain.push_back( tile_cell->gain1() );
        cells_pmt2_gain.push_back( tile_cell->gain2() );
      }
      
    }
    
    
    static SG::AuxElement::Decorator< std::vector<float> > cells_energy_decorator(m_prefix + "cells_energy");
    static SG::AuxElement::Decorator< std::vector<float> > cells_et_decorator(m_prefix + "cells_et");
    static SG::AuxElement::Decorator< std::vector<float> > cells_eta_decorator(m_prefix + "cells_eta");
    static SG::AuxElement::Decorator< std::vector<float> > cells_phi_decorator(m_prefix + "cells_phi");
    
    cells_energy_decorator(*particle) = std::move(cells_energy);
    cells_et_decorator(*particle) = std::move(cells_et);
    cells_eta_decorator(*particle) = std::move(cells_eta);
    cells_phi_decorator(*particle) = std::move(cells_phi);
    
    
    static SG::AuxElement::Decorator< std::vector<int> > cells_gain_decorator(m_prefix + "cells_gain");
    static SG::AuxElement::Decorator< std::vector<bool> > cells_bad_decorator(m_prefix + "cells_bad");
    static SG::AuxElement::Decorator< std::vector<int> > cells_sampling_decorator(m_prefix + "cells_sampling");
    static SG::AuxElement::Decorator< std::vector<float> > cells_time_decorator(m_prefix + "cells_time");
    static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_quality_decorator(m_prefix + "cells_quality");
    
    cells_gain_decorator(*particle) = std::move(cells_gain);
    cells_bad_decorator(*particle) = std::move(cells_bad);
    cells_sampling_decorator(*particle) = std::move(cells_sampling);
    cells_time_decorator(*particle) = std::move(cells_time);
    cells_quality_decorator(*particle) = std::move(cells_quality);
    
    
    if (m_saveTileCellPositionAndDimention) {
      
      static SG::AuxElement::Decorator< std::vector<float> > cells_sinTh_decorator(m_prefix + "cells_sinTh");
      static SG::AuxElement::Decorator< std::vector<float> > cells_cosTh_decorator(m_prefix + "cells_cosTh");
      static SG::AuxElement::Decorator< std::vector<float> > cells_cotTh_decorator(m_prefix + "cells_cotTh");
      static SG::AuxElement::Decorator< std::vector<float> > cells_x_decorator(m_prefix + "cells_x");
      static SG::AuxElement::Decorator< std::vector<float> > cells_y_decorator(m_prefix + "cells_y");
      static SG::AuxElement::Decorator< std::vector<float> > cells_z_decorator(m_prefix + "cells_z");
      
      static SG::AuxElement::Decorator< std::vector<float> > cells_r_decorator(m_prefix + "cells_r");
      static SG::AuxElement::Decorator< std::vector<float> > cells_dx_decorator(m_prefix + "cells_dx");
      static SG::AuxElement::Decorator< std::vector<float> > cells_dy_decorator(m_prefix + "cells_dy");
      static SG::AuxElement::Decorator< std::vector<float> > cells_dz_decorator(m_prefix + "cells_dz");
      static SG::AuxElement::Decorator< std::vector<float> > cells_dr_decorator(m_prefix + "cells_dr");
      static SG::AuxElement::Decorator< std::vector<float> > cells_volume_decorator(m_prefix + "cells_volume");
      static SG::AuxElement::Decorator< std::vector<float> > cells_dphi_decorator(m_prefix + "cells_dphi");
      static SG::AuxElement::Decorator< std::vector<float> > cells_deta_decorator(m_prefix + "cells_deta");
      
      static SG::AuxElement::Decorator< std::vector<int> > cells_side_decorator(m_prefix + "cells_side");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_section_decorator(m_prefix + "cells_section");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_module_decorator(m_prefix + "cells_module");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_tower_decorator(m_prefix + "cells_tower");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_sample_decorator(m_prefix + "cells_sample");
      
      
      cells_sinTh_decorator(*particle) = std::move(cells_sinTh);
      cells_cosTh_decorator(*particle) = std::move(cells_cosTh);
      cells_cotTh_decorator(*particle) = std::move(cells_cotTh);
      cells_x_decorator(*particle) = std::move(cells_x);
      cells_y_decorator(*particle) = std::move(cells_y);
      cells_z_decorator(*particle) = std::move(cells_z);
      
      cells_r_decorator(*particle) = std::move(cells_r);
      cells_dx_decorator(*particle) = std::move(cells_dx);
      cells_dy_decorator(*particle) = std::move(cells_dy);
      cells_dz_decorator(*particle) = std::move(cells_dz);
      cells_dr_decorator(*particle) = std::move(cells_dr);
      cells_volume_decorator(*particle) = std::move(cells_volume);
      cells_dphi_decorator(*particle) = std::move(cells_dphi);
      cells_deta_decorator(*particle) = std::move(cells_deta);
      
      cells_side_decorator(*particle) = std::move(cells_side);
      cells_section_decorator(*particle) = std::move(cells_section);
      cells_module_decorator(*particle) = std::move(cells_module);
      cells_tower_decorator(*particle) = std::move(cells_tower);
      cells_sample_decorator(*particle) = std::move(cells_sample);
      
    }
    
    
    if (m_saveTileCellPmtInfo) {
      
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt1_ros_decorator(m_prefix + "cells_pmt1_ros");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt2_ros_decorator(m_prefix + "cells_pmt2_ros");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt1_drawer_decorator(m_prefix + "cells_pmt1_drawer");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt2_drawer_decorator(m_prefix + "cells_pmt2_drawer");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt1_channel_decorator(m_prefix + "cells_pmt1_channel");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt2_channel_decorator(m_prefix + "cells_pmt2_channel");
      
      static SG::AuxElement::Decorator< std::vector<float> > cells_pmt1_energy_decorator(m_prefix + "cells_pmt1_energy");
      static SG::AuxElement::Decorator< std::vector<float> > cells_pmt2_energy_decorator(m_prefix + "cells_pmt2_energy");
      static SG::AuxElement::Decorator< std::vector<float> > cells_pmt1_time_decorator(m_prefix + "cells_pmt1_time");
      static SG::AuxElement::Decorator< std::vector<float> > cells_pmt2_time_decorator(m_prefix + "cells_pmt2_time");
      
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt1_quality_decorator(m_prefix + "cells_pmt1_quality");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt2_quality_decorator(m_prefix + "cells_pmt2_quality");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt1_qbit_decorator(m_prefix + "cells_pmt1_qbit");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt2_qbit_decorator(m_prefix + "cells_pmt2_qbit");
      
      static SG::AuxElement::Decorator< std::vector<bool> > cells_pmt1_bad_decorator(m_prefix + "cells_pmt1_bad");
      static SG::AuxElement::Decorator< std::vector<bool> > cells_pmt2_bad_decorator(m_prefix + "cells_pmt2_bad");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt1_gain_decorator(m_prefix + "cells_pmt1_gain");
      static SG::AuxElement::Decorator< std::vector<unsigned short> > cells_pmt2_gain_decorator(m_prefix + "cells_pmt2_gain");
      
      
      
      cells_pmt1_ros_decorator(*particle) = std::move(cells_pmt1_ros);
      cells_pmt2_ros_decorator(*particle) = std::move(cells_pmt2_ros);
      cells_pmt1_drawer_decorator(*particle) = std::move(cells_pmt1_drawer);
      cells_pmt2_drawer_decorator(*particle) = std::move(cells_pmt2_drawer);
      cells_pmt1_channel_decorator(*particle) = std::move(cells_pmt1_channel);
      cells_pmt2_channel_decorator(*particle) = std::move(cells_pmt2_channel);
      
      cells_pmt1_energy_decorator(*particle) = std::move(cells_pmt1_energy);
      cells_pmt2_energy_decorator(*particle) = std::move(cells_pmt2_energy);
      cells_pmt1_time_decorator(*particle) = std::move(cells_pmt1_time);
      cells_pmt2_time_decorator(*particle) = std::move(cells_pmt2_time);
      
      cells_pmt1_quality_decorator(*particle) = std::move(cells_pmt1_quality);
      cells_pmt2_quality_decorator(*particle) = std::move(cells_pmt2_quality);
      cells_pmt1_qbit_decorator(*particle) = std::move(cells_pmt1_qbit);
      cells_pmt2_qbit_decorator(*particle) = std::move(cells_pmt2_qbit);
      
      cells_pmt1_bad_decorator(*particle) = std::move(cells_pmt1_bad);
      cells_pmt2_bad_decorator(*particle) = std::move(cells_pmt2_bad);
      cells_pmt1_gain_decorator(*particle) = std::move(cells_pmt1_gain);
      cells_pmt2_gain_decorator(*particle) = std::move(cells_pmt2_gain);
      
    }
    
    
    return StatusCode::SUCCESS;
  }
  
}
