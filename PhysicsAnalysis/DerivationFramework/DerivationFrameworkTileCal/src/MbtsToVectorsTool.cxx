///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MbtsToVectorsTool.cxx 
// Implementation file for class MbtsToVectorsTool
/////////////////////////////////////////////////////////////////// 

// PhysicsAnalysis/DerivationFramework/DerivationFrameworkTileCal includes
#include "DerivationFrameworkTileCal/MbtsToVectorsTool.h"

// Tile includes
#include "TileEvent/TileCell.h"
#include "TileEvent/TileContainer.h"
#include "TileIdentifier/TileTBID.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/make_unique.h"

#include <vector>

namespace DerivationFramework {

  MbtsToVectorsTool::MbtsToVectorsTool( const std::string& type, const std::string& name, const IInterface* parent )
    : AthAlgTool  ( type, name, parent   )
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty( "Prefix", m_prefix = "mbts" );
    declareProperty( "CellContainer", m_cellContainer = "MbtsContainer" );
    declareProperty( "SaveEtaPhiInfo", m_saveEtaPhi = true );
  }


  StatusCode MbtsToVectorsTool::initialize() {

    m_energy = SG::WriteHandle<std::vector<float> >(m_prefix + "_energy");
    m_time = SG::WriteHandle<std::vector<float> >(m_prefix + "_time");
    m_quality = SG::WriteHandle<std::vector<int> >(m_prefix + "_quality");
    m_type = SG::WriteHandle<std::vector<int> >(m_prefix + "_type");
    m_module = SG::WriteHandle<std::vector<int> >(m_prefix + "_module");
    m_channel = SG::WriteHandle<std::vector<int> >(m_prefix + "_channel");
    
    if (m_saveEtaPhi) {
      m_eta = SG::WriteHandle<std::vector<float> >(m_prefix + "_eta");
      m_phi = SG::WriteHandle<std::vector<float> >(m_prefix + "_phi");
    }

    return StatusCode::SUCCESS;
  }

  StatusCode MbtsToVectorsTool::addBranches() const {

    const TileTBID* tileTBID = 0;
    CHECK( detStore()->retrieve (tileTBID) );

    // Set up the vector
    m_energy = CxxUtils::make_unique<std::vector<float> >();
    m_energy->reserve(32U);

    m_time = CxxUtils::make_unique<std::vector<float> >();
    m_time->reserve(32U);

    m_quality = CxxUtils::make_unique<std::vector<int> >();
    m_quality->reserve(32U);

    m_module = CxxUtils::make_unique<std::vector<int> >();
    m_module->reserve(32U);

    m_type = CxxUtils::make_unique<std::vector<int> >();
    m_type->reserve(32U);

    m_channel = CxxUtils::make_unique<std::vector<int> >();
    m_channel->reserve(32U);


    const TileCellContainer* tileCells;
    CHECK( evtStore()->retrieve(tileCells, m_cellContainer) );
    
    if (m_saveEtaPhi) {
      m_eta = CxxUtils::make_unique<std::vector<float> >();
      m_eta->reserve(32U);

      m_phi = CxxUtils::make_unique<std::vector<float> >();
      m_phi->reserve(32U);

      for ( const TileCell* cell : *tileCells ){
        m_energy->push_back( cell->energy() );
        m_eta->push_back( cell->eta() );
        m_phi->push_back( cell->phi() );
        m_time->push_back( cell->time() );
        m_quality->push_back(cell->quality() );
        m_module->push_back(tileTBID->module( cell->ID() ) );
        m_type->push_back(tileTBID->type( cell->ID() ) );
        m_channel->push_back(tileTBID->channel( cell->ID() ) );
      }

    } else {

      for ( const TileCell* cell : *tileCells ){
        m_energy->push_back( cell->energy() );
        m_time->push_back( cell->time() );
        m_quality->push_back(cell->quality() );
        m_module->push_back(tileTBID->module( cell->ID() ) );
        m_type->push_back(tileTBID->type( cell->ID() ) );
        m_channel->push_back(tileTBID->channel( cell->ID() ) );
      }

    }

    return StatusCode::SUCCESS;
  }

}
