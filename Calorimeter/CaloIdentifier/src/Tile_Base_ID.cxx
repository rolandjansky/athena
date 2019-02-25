/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/src/Tile_Base_ID.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Factor out code common between TileID and Tile_SuperCell_ID.
 */


#include "CaloIdentifier/Tile_Base_ID.h"
#include "IdDict/IdDictDefs.h"
#include "Identifier/IdentifierHash.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "CxxUtils/StrFormat.h"

#include "GaudiKernel/MsgStream.h"

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <stdexcept>

using CxxUtils::strformat;
using namespace LArNeighbours;



Tile_Base_ID::Tile_Base_ID (const std::string& name, bool supercell)
  : CaloIDHelper (name)
    , m_supercell (supercell ? 1 : 0)
    , m_tile_region_index(0)
    , m_SYSTEM_INDEX(999)
    , m_SECTION_INDEX(999)
    , m_SIDE_INDEX(999)
    , m_MODULE_INDEX(999)
    , m_TOWER_INDEX(999)
    , m_SAMPLE_INDEX(999)
    , m_PMT_INDEX(999)
    , m_ADC_INDEX(999)
{
}

//
// Tile_Base_ID methods
//
std::string
Tile_Base_ID::to_string( const Identifier & id, int level ) const
{
  IdContext context;
  switch (level) {
    case 1: case -6:
    case 2: case -5: context = region_context(); break;
    case 3: case -4: context = module_context(); break;
    case 4: case -3: context = tower_context(); break;
    case 5: case -2: context = cell_context(); break;
    case 6: case -1: context = pmt_context(); break;
    case 7: case  0: context = adc_context(); break;
  default:
    return "Tile_Base_ID::to_string - unknown context";
  }    
      
  ExpandedIdentifier exp_id;
  if ( get_expanded_id(id,exp_id,&context) ) {
    return "Tile_Base_ID::to_string - can not expand ID";
  } else {
    return (std::string)(exp_id);
  }  
}

std::string
Tile_Base_ID::to_string( const IdentifierHash & hash_id, int level ) const
{
  IdContext context;
  switch (level) {
    case 1: case -6:
    case 2: case -5: context = region_context(); break;
    case 3: case -4: context = module_context(); break;
    case 4: case -3: context = tower_context(); break;
    case 5: case -2: context = cell_context(); break;
    case 6: case -1: context = pmt_context(); break;
    case 7: case  0: context = adc_context(); break;
  default:
    return "Tile_Base_ID::to_string - unknown context";
  }    

  Identifier id;
  if ( get_id ( hash_id, id, &context ) == 0 ) {
    ExpandedIdentifier exp_id;
    if ( get_expanded_id(id,exp_id,&context) ) {
      return "Tile_Base_ID::to_string - can not expand ID";
    } else {
      return (std::string)(exp_id);
    }
  } else {
    return "Tile_Base_ID::to_string - can not get ID from hash_id";
  }
}

//
// get methods
//
int
Tile_Base_ID::get_field ( const Identifier & id, size_type index ) const
{
  int field = 0;
  if(!dict()->unpack(id, 0, index, m_tile_region_index, field)) {
    return field;
  } else {
    return 0;
  }
}

int
Tile_Base_ID::region ( int section, int side ) const
{
    IdentifierHash hash_id;
    IdContext context = region_context();

    get_hash(region_id(section,side),hash_id,&context);

    return (int)hash_id;
}

int
Tile_Base_ID::region ( const Identifier & id ) const
{
    IdentifierHash hash_id;
    IdContext context = region_context();

    get_hash(region_id(id),hash_id,&context);

    return (int)hash_id;
}

int
Tile_Base_ID::system ( const Identifier & id ) const
{
    return get_field(id,m_SYSTEM_INDEX);
}

int
Tile_Base_ID::section ( const Identifier & id ) const
{
  return decode_section (get_field(id,m_SECTION_INDEX));
}

int
Tile_Base_ID::side ( const Identifier & id ) const
{
    return get_field(id,m_SIDE_INDEX);
}

int
Tile_Base_ID::module ( const Identifier & id ) const
{
    return get_field(id,m_MODULE_INDEX);
}

int
Tile_Base_ID::tower ( const Identifier & id ) const
{
    return get_field(id,m_TOWER_INDEX);
}

int
Tile_Base_ID::sample ( const Identifier & id ) const
{
  if ( section(id) == Tile_Base_ID::AUXDET )
    return Tile_Base_ID::SAMP_X;
  else
    return get_field(id,m_SAMPLE_INDEX);
}

int
Tile_Base_ID::pmt ( const Identifier & id ) const
{
    return get_field(id,m_PMT_INDEX);
}

int
Tile_Base_ID::adc ( const Identifier & id ) const
{
    return get_field(id,m_ADC_INDEX);
}

//
// check methods
//
/*
bool               
Tile_Base_ID::is_tile (const Identifier& id) const
{
    bool result = ( system(id) == tile_field_value() );

    return result;
}
*/

bool               
Tile_Base_ID::is_tile_barrel (const Identifier& id) const
{
    bool result = is_tile(id)
      && ( section(id) == Tile_Base_ID::BARREL );

    return result;
}

bool               
Tile_Base_ID::is_tile_extbarrel (const Identifier& id) const
{
    bool result = is_tile(id)
      && ( section(id) == Tile_Base_ID::EXTBAR );

    return result;
}

bool               
Tile_Base_ID::is_tile_gap (const Identifier& id) const
{
    bool result = is_tile(id)
      && ( section(id) == Tile_Base_ID::GAPDET );

    return result;
}

bool               
Tile_Base_ID::is_tile_aux (const Identifier& id) const
{
    bool result = is_tile(id)
      && ( section(id) == Tile_Base_ID::AUXDET );

    return result;
}

bool               
Tile_Base_ID::is_negative (const Identifier& id) const
{
    bool result = is_tile(id)
      && ( side(id) == Tile_Base_ID::NEGATIVE );

    return result;
}

bool               
Tile_Base_ID::is_positive (const Identifier& id) const
{
    bool result = is_tile(id)
      && ( side(id) == Tile_Base_ID::POSITIVE );

    return result;
}

bool               
Tile_Base_ID::is_twosides (const Identifier& id) const
{
    bool result = is_tile(id)
      && ( side(id) == 0 );

    return result;
}

bool
Tile_Base_ID::is_tile_gapscin (const Identifier& id) const
{
    bool result = is_tile(id)
      && ( sample(id) == Tile_Base_ID::SAMP_E );

    return result;
}

bool
Tile_Base_ID::is_low_gain (const Identifier& id) const
{
    bool result = is_tile(id)
      && ( adc(id) == Tile_Base_ID::LOWGAIN );

    return result;
}

bool
Tile_Base_ID::is_high_gain (const Identifier& id) const
{
    bool result = is_tile(id)
      && ( adc(id) == Tile_Base_ID::HIGHGAIN );

    return result;
}

bool Tile_Base_ID::is_supercell (const Identifier& id) const
{
  return is_tile(id) && (get_field(id,m_SECTION_INDEX) & SUPERCELL_MASK);
}

// Whole TileCal
Identifier
Tile_Base_ID::tile_system     ()      const
{
    Identifier result;

    int id[8];
    id[m_SYSTEM_INDEX] = tile_field_value();
        
    Identifier compactId(0);
    if (!dict()->pack32(id, 0, m_SYSTEM_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }

    return result;
}

// For Tile Barrel ID
Identifier
Tile_Base_ID::tile_barrel     ()      const
{
    Identifier result;

    int id[8];
    id[m_SYSTEM_INDEX] = tile_field_value();
    id[m_SECTION_INDEX] = encode_section (Tile_Base_ID::BARREL);
        
    Identifier compactId(0);
    if (!dict()->pack32(id, 0, m_SECTION_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }

    return result;
}

// For Tile Extended Barrel ID
Identifier
Tile_Base_ID::tile_extbarrel  ()      const
{
    Identifier result;

    int id[8];
    id[m_SYSTEM_INDEX] = tile_field_value();
    id[m_SECTION_INDEX] = encode_section (Tile_Base_ID::EXTBAR);
        
    Identifier compactId(0);
    if (!dict()->pack32(id, 0, m_SECTION_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }

    return result;
}


// For Tile Gap Scintillator ID
Identifier
Tile_Base_ID::tile_gap        ()      const
{
    Identifier result;

    assert (!m_supercell);
    int id[8];
    id[m_SYSTEM_INDEX] = tile_field_value();
    id[m_SECTION_INDEX] = Tile_Base_ID::GAPDET;
        
    Identifier compactId(0);
    if (!dict()->pack32(id, 0, m_SECTION_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }

    return result;
}

// For any Tilecal section
Identifier
Tile_Base_ID::tile_det        ( int section )        const
{
    if(m_do_checks) {
	// Check that id is within allowed range
	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << encode_section (section) ;
    }

    Identifier result;

    int id[8];
    id[m_SYSTEM_INDEX] = tile_field_value();
    id[m_SECTION_INDEX] = encode_section (section);
        
    Identifier compactId(0);
    if (!dict()->pack32(id, 0, m_SECTION_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }

    return result;
}

// single region, module, tower, cell, pmt, adc identifiers
Identifier
Tile_Base_ID::region_id       ( int index )           const
{
    Identifier result;
    IdentifierHash hash_id = index;
    IdContext context = region_context();

    get_id(hash_id,result,&context);

    return result;
}

Identifier
Tile_Base_ID::region_id       ( int section, int side )      const
{
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << encode_section (section) << side;

	if (!m_full_region_range.match(expId)) { 
	    std::string errorMessage = "Tile_Base_ID::region_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_region_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }

    Identifier result;

    int id[8];
    id[m_SYSTEM_INDEX] = tile_field_value();
    id[m_SECTION_INDEX] = encode_section (section);
    id[m_SIDE_INDEX] = side;
        
    Identifier compactId(0);
    if (!dict()->pack32(id, 0, m_SIDE_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }

    return result;
}

Identifier
Tile_Base_ID::region_id       ( const Identifier& any_id )   const
{
  Identifier result;
  //  extract with the dictionary
  Identifier id;
  if(!dict()->copy(any_id, 0, 0, m_SIDE_INDEX, m_tile_region_index, id)) {
    result = id;
  }
  return (result);
}

Identifier
Tile_Base_ID::module_id       ( int section, int side, 
                                int module ) const
{
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << encode_section (section) << side << module;

	if (!m_full_module_range.match(expId)) { 
	    std::string errorMessage = "Tile_Base_ID::module_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_region_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }

    Identifier result;

    int id[8];
    id[m_SYSTEM_INDEX]  = tile_field_value();
    id[m_SECTION_INDEX] = encode_section (section);
    id[m_SIDE_INDEX]    = side;
    id[m_MODULE_INDEX]  = module;
        
    Identifier compactId(0);
    if (!dict()->pack32(id, 0, m_MODULE_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }

    return result;
}

Identifier
Tile_Base_ID::module_id       ( const Identifier& any_id )   const
{
  Identifier result;
  //  extract with the dictionary
  Identifier id;
  if(!dict()->copy(any_id, 0, 0, m_MODULE_INDEX, m_tile_region_index, id)) {
    result = id;
  }
  return (result);
}

Identifier
Tile_Base_ID::tower_id        ( int section, int side,
                                int module,   int tower )     const
{
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << encode_section (section) << side << module << tower;

	if (!m_full_tower_range.match(expId)) { 
	    std::string errorMessage = "Tile_Base_ID::tower_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_region_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }

    Identifier result;

    int id[8];
    id[m_SYSTEM_INDEX]  = tile_field_value();
    id[m_SECTION_INDEX] = encode_section (section);
    id[m_SIDE_INDEX]    = side;
    id[m_MODULE_INDEX]  = module;
    id[m_TOWER_INDEX]   = tower;
        
    Identifier compactId(0);
    if (!dict()->pack32(id, 0, m_TOWER_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }

    return result;
}

Identifier
Tile_Base_ID::tower_id       ( const Identifier& any_id )   const
{
  Identifier result;
  //  extract with the dictionary
  Identifier id;
  if(!dict()->copy(any_id, 0, 0, m_TOWER_INDEX, m_tile_region_index, id)) {
    result = id;
  }
  return (result);
}

Identifier
Tile_Base_ID::cell_id         ( const Identifier & any_id )   const
{
  Identifier result;
  //  extract with the dictionary
  Identifier id;
  if(!dict()->copy(any_id, 0, 0, m_SAMPLE_INDEX, m_tile_region_index, id)) {
    result = id;
  }
  return (result);
}

Identifier
Tile_Base_ID::cell_id         ( int section, int side,
                                int module,   int tower, 
                                int sample )                  const
{
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << encode_section (section) << side << module << tower << sample;

	if (!m_full_cell_range.match(expId)) { 
	    std::string errorMessage = "Tile_Base_ID::cell_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_cell_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }

    Identifier result;

    int id[8];
    id[m_SYSTEM_INDEX]  = tile_field_value();
    id[m_SECTION_INDEX] = encode_section (section);
    id[m_SIDE_INDEX]    = side;
    id[m_MODULE_INDEX]  = module;
    id[m_TOWER_INDEX]   = tower;
    id[m_SAMPLE_INDEX]  = sample;
        
    Identifier compactId(0);
    if (!dict()->pack32(id, 0, m_SAMPLE_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }

    return result;
}

Identifier
Tile_Base_ID::pmt_id          ( const Identifier & any_id )   const
{
  Identifier result;
  //  extract with the dictionary
  Identifier id;
  if(!dict()->copy(any_id, 0, 0, m_PMT_INDEX, m_tile_region_index, id)) {
    result = id;
  }
  return (result);
}

Identifier
Tile_Base_ID::pmt_id          ( const Identifier & cell_id, 
                                int pmt )                     const
{
    Identifier result;

    int id[8];
    id[0] = pmt;
        
    Identifier compactId(cell_id);
    if (!dict()->pack32(id, m_PMT_INDEX, m_PMT_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }
    else {
      std::string errorMessage = "Tile_Base_ID::pmt_id(cell_id,pmt) could not pack ID: "
	+ cell_id.getString() ;
      throw TileID_Exception(errorMessage , 1);
    }

    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId;

	IdContext context = cell_context();
	if (get_expanded_id(result, expId, &context)) {
	    std::string errorMessage = "Tile_Base_ID::pmt_id(cell_id,pmt) result is not OK: ID = "
		+ result.getString() ;
	    throw TileID_Exception(errorMessage , 1);

	    //      region_id.show();
	}

	expId << pmt;

	if (!m_full_pmt_range.match(expId)) { 
	    std::string errorMessage = "Tile_Base_ID::pmt_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_region_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }

    return result;
}

Identifier
Tile_Base_ID::pmt_id          ( int section, int side,
                                int module,   int tower,
                                int sample,   int pmt )       const
{
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << encode_section (section) << side << module << tower << sample << pmt;

	if (!m_full_pmt_range.match(expId)) { 
	    std::string errorMessage = "Tile_Base_ID::pmt_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_region_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }

    Identifier result;

    int id[8];
    id[m_SYSTEM_INDEX]  = tile_field_value();
    id[m_SECTION_INDEX] = encode_section (section);
    id[m_SIDE_INDEX]    = side;
    id[m_MODULE_INDEX]  = module;
    id[m_TOWER_INDEX]   = tower;
    id[m_SAMPLE_INDEX]  = sample;
    id[m_PMT_INDEX]     = pmt;
        
    Identifier compactId(0);
    if (!dict()->pack32(id, 0, m_PMT_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }

    return result;
}

Identifier
Tile_Base_ID::adc_id          ( const Identifier & cell_id,
                                int pmt, int adc )            const
{
    Identifier result;

    int id[8];
    id[0] = pmt;
    id[1] = adc;
        
    Identifier compactId(cell_id);
    if (!dict()->pack32(id, m_PMT_INDEX, m_ADC_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }
    else {
      std::string errorMessage = "Tile_Base_ID::adc_id(cell_id,pmt,adc) could not pack ID: "
	+ cell_id.getString() ;
      throw TileID_Exception(errorMessage , 1);
    }

    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId;

	IdContext context = cell_context();
	if (get_expanded_id(result, expId, &context)) {
	    std::string errorMessage = "Tile_Base_ID::adc_id(cell_id,pmt,adc) result is not OK: ID = "
		+ result.getString() ;
	    throw TileID_Exception(errorMessage , 1);

	    //      region_id.show();
	}

	expId << pmt << adc;

	if (!m_full_adc_range.match(expId)) { 
	    std::string errorMessage = "Tile_Base_ID::adc_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_region_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }

    return result;
}

Identifier
Tile_Base_ID::adc_id          ( const Identifier & pmt_id,
                                int adc )                     const
{
    Identifier result;

    int id[8];
    id[0] = adc;
        
    Identifier compactId(pmt_id);
    if (!dict()->pack32(id, m_ADC_INDEX, m_ADC_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }
    else {
      std::string errorMessage = "Tile_Base_ID::adc_id(pmt_id,adc) could not pack ID: "
	+ pmt_id.getString() ;
      throw TileID_Exception(errorMessage , 1);
    }

    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId;

	IdContext context = pmt_context();
	if (get_expanded_id(result, expId, &context)) {
	    std::string errorMessage = "Tile_Base_ID::adc_id(pmt_id,adc) result is not OK: ID = "
		+ result.getString() ;
	    throw TileID_Exception(errorMessage , 1);

	    //      region_id.show();
	}

	expId << adc;

	if (!m_full_adc_range.match(expId)) { 
	    std::string errorMessage = "Tile_Base_ID::adc_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_region_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }

    return result;
}

Identifier
Tile_Base_ID::adc_id          ( int section, int side,
                                int module, int tower, int sample, 
                                int pmt, int adc )            const
{
    if(m_do_checks) {
	
	// Check that id is within allowed range

	// Fill expanded id
	ExpandedIdentifier expId(tile_exp());
	expId << encode_section (section) << side << module << tower << sample << pmt << adc;

	if (!m_full_adc_range.match(expId)) { 
	    std::string errorMessage = "Tile_Base_ID::adc_id() result is not OK: ID, range = "
		+ std::string(expId) + " , " + (std::string)m_full_region_range;
	    throw TileID_Exception(errorMessage , 1);
	}
    }

    Identifier result;

    int id[8];
    id[m_SYSTEM_INDEX]  = tile_field_value();
    id[m_SECTION_INDEX] = encode_section (section);
    id[m_SIDE_INDEX]    = side;
    id[m_MODULE_INDEX]  = module;
    id[m_TOWER_INDEX]   = tower;
    id[m_SAMPLE_INDEX]  = sample;
    id[m_PMT_INDEX]     = pmt;
    id[m_ADC_INDEX]     = adc;
        
    Identifier compactId(0);
    if (!dict()->pack32(id, 0, m_ADC_INDEX, m_tile_region_index, compactId)) {
	result = compactId;   
    }

    return result;
}

bool Tile_Base_ID::module_id  ( const Identifier& region_id,
                                int module, Identifier& module_id ) const
{
  ExpandedIdentifier id; 
  IdContext context = region_context();
  get_expanded_id(region_id, id, &context);
  id << module;
  if (!m_full_module_range.match(id)) { 
    return false;
  }

  Identifier compactId(0);
  if (!dict()->pack32(id,0, id.fields() - 1,compactId)) {
    module_id = compactId;   
  }
  return true;
}

bool Tile_Base_ID::tower_id   ( const Identifier& region_id,
                                int module, int tower, Identifier& tower_id ) const
{
  ExpandedIdentifier id; 
  IdContext context = region_context();
  get_expanded_id(region_id, id, &context);
  id << module << tower;
  if (!m_full_tower_range.match(id)) { 
    return false;
  }

  Identifier compactId(0);
  if (!dict()->pack32(id,0, id.fields() - 1,compactId)) {
    tower_id = compactId;   
  }
  return true;
}

bool Tile_Base_ID::cell_id    ( const Identifier& region_id,
                                int module, int tower, int sample,
                                Identifier& cell_id ) const
{
  ExpandedIdentifier id; 
  IdContext context = region_context();
  get_expanded_id(region_id, id, &context);
  id << module << tower << sample;
  if (!m_full_cell_range.match(id)) { 
    return false;
  }

  Identifier compactId(0);
  if (!dict()->pack32(id,0, id.fields() - 1,compactId)) {
    cell_id = compactId;   
  }
  return true;
}

bool Tile_Base_ID::pmt_id     ( const Identifier& cell_id,
                                int pmt, Identifier& pmt_id ) const
{
  ExpandedIdentifier id; 
  IdContext context = cell_context();
  get_expanded_id(cell_id, id, &context);
  id << pmt;
  if (!m_full_pmt_range.match(id)) { 
    return false;
  }

  Identifier compactId(0);
  if (!dict()->pack32(id,0, id.fields() - 1,compactId)) {
    pmt_id = compactId;   
  }
  return true;
}

bool Tile_Base_ID::adc_id     ( const Identifier& pmt_id,
                                int adc, Identifier& adc_id ) const
{
  ExpandedIdentifier id; 
  IdContext context = pmt_context();
  get_expanded_id(pmt_id, id, &context);
  id << adc;
  if (!m_full_adc_range.match(id)) { 
    return false;
  }

  Identifier compactId(0);
  if (!dict()->pack32(id,0, id.fields() - 1,compactId)) {
    adc_id = compactId;   
  }
  return true;
}

bool Tile_Base_ID::adc_id     ( const Identifier& cell_id,
                                int pmt, int adc, Identifier& adc_id ) const
{
  ExpandedIdentifier id; 
  IdContext context = cell_context();
  get_expanded_id(cell_id, id, &context);
  id << pmt << adc;
  if (!m_full_adc_range.match(id)) { 
    return false;
  }

  Identifier compactId(0);
  if (!dict()->pack32(id,0, id.fields() - 1,compactId)) {
    adc_id = compactId;   
  }
  return true;
}

Identifier  Tile_Base_ID::cell_id     (const IdentifierHash& hash_id) const
{
  if (hash_id < channels().hash_max()) {
    return channel_id (hash_id);
  } else {
    std::string errorMessage = "Tile_Base_ID::cell_id(hash_id) - hash_id out of range ";
    throw TileID_Exception(errorMessage , 1);
  }
}

IdentifierHash  Tile_Base_ID::cell_hash  (const Identifier& id) const
{
  IdentifierHash ret = 0;
  if (channels().get_hash (id, ret) == 0)
    return ret;

  std::string errorMessage = "Tile_Base_ID::cell_hash(cell_id) - cell_id out of range ";
  throw TileID_Exception(errorMessage , 1);
}

IdContext	
Tile_Base_ID::region_context  (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_SIDE_INDEX));
}

IdContext	
Tile_Base_ID::module_context  (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_MODULE_INDEX));
}

IdContext	
Tile_Base_ID::tower_context   (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_TOWER_INDEX));
}

IdContext	
Tile_Base_ID::cell_context    (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_SAMPLE_INDEX));
}

IdContext	
Tile_Base_ID::pmt_context     (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_PMT_INDEX));
}

IdContext	
Tile_Base_ID::adc_context     (void)  const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_ADC_INDEX));
}

int
Tile_Base_ID::get_id     (const IdentifierHash& hash_id,
                          Identifier& id,
                          const IdContext* context ) const
{
  int result = 1;
  id.clear();

  size_t begin = (context) ? context->begin_index(): 0;
  // cannot get hash if end is 0:
  size_t end   = (context) ? context->end_index()  : 0; 

  if (0 == begin) {

    if ( m_ADC_INDEX == end ) {
      if (hash_id < m_adcs.hash_max()) {
	id = m_adcs.id (hash_id);
	result = 0;
      } else {
	MsgStream log(m_msgSvc, "Tile_Base_ID" );
	log << MSG::ERROR << "get_id: adc hash_id is out of range " << hash_id
            << " > " << m_adcs.hash_max() << endmsg;
      }
    } else if ( m_PMT_INDEX == end ) {
      if (hash_id < m_pmts.hash_max()) {
	id = m_pmts.id (hash_id);
	result = 0;
      } else {
	MsgStream log(m_msgSvc, "Tile_Base_ID" );
	log << MSG::ERROR << "get_id: pmt hash_id is out of range " << hash_id
            << " >= " << m_pmts.hash_max() << endmsg;
      }
    } else if ( m_SAMPLE_INDEX == end ) {
      if (hash_id < channels().hash_max()) {
	id = channels().id (hash_id);
	result = 0;
      } else {
        MsgStream log(m_msgSvc, "Tile_Base_ID" );
	log << MSG::ERROR << "get_id: cell hash_id is out of range " << hash_id 
            << " >= " << channels().hash_max() << endmsg;
      }
    } else if ( m_TOWER_INDEX == end ) {
      if (hash_id < m_towers.hash_max()) {
	id = m_towers.id (hash_id);
	result = 0;
      } else {
	MsgStream log(m_msgSvc, "Tile_Base_ID" );
	log << MSG::ERROR << "get_id: tower hash_id is out of range " << hash_id
            << " >= " << m_towers.hash_max() << endmsg;
      }
    } else if ( m_MODULE_INDEX == end ) {
      if (hash_id < m_modules.hash_max()) {
	id = m_modules.id (hash_id);
	result = 0;
      } else {
	MsgStream log(m_msgSvc, "Tile_Base_ID" );
	log << MSG::ERROR << "get_id: module hash_id is out of range " << hash_id
            << " >= " << m_modules.hash_max() << endmsg;
      }
    } else if ( m_SIDE_INDEX == end ) {
      if (hash_id < regions().hash_max()) {
        id = regions().id (hash_id);
	result = 0;
      } else {
        MsgStream log(m_msgSvc, "Tile_Base_ID" );
	log << MSG::ERROR << "get_id: region hash_id is out of range " << hash_id
            << " >= " << regions().hash_max() << endmsg;
      }
    }
  }
  return(result);
}


int
Tile_Base_ID::get_hash  (const Identifier& id,
                         IdentifierHash& hash_id,
                         const IdContext* context ) const
{
  hash_id = 0;
  int result = 1;
    
  size_t begin = (context) ? context->begin_index(): 0;
  size_t end   = (context) ? context->end_index()  : 0; 

  if (0 == begin) { 
    if ( m_ADC_INDEX == end ) {
      result = m_adcs.get_hash (id, hash_id);
    } else if ( m_PMT_INDEX == end ) {
      result = m_pmts.get_hash (pmt_id(id), hash_id);
    } else if ( m_SAMPLE_INDEX == end ) {
      result = channels().get_hash (cell_id(id), hash_id);
    } else if ( m_TOWER_INDEX == end ) {
      result = m_towers.get_hash (tower_id(id), hash_id);
    } else if ( m_MODULE_INDEX == end ) {
      result = m_modules.get_hash (module_id(id), hash_id);
    } else if ( m_SIDE_INDEX == end ) {
      result = regions().get_hash (region_id(id), hash_id);
    }
  }

  return (result);
}

int
Tile_Base_ID::initialize_base_from_dictionary (const IdDictMgr& dict_mgr,
                                               const std::string& group_name)
{
  MsgStream log(m_msgSvc, "Tile_Base_ID" );
  log << MSG::DEBUG << "initialize_base_from_dictionary " << endmsg;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    if(m_msgSvc)log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    if(m_msgSvc)log << MSG::DEBUG << "(Re)initialize" << endmsg;
  }

  log << MSG::DEBUG << "calling base initialize_base_from_dictionary" << endmsg;

  // init base object
  if(CaloIDHelper::initialize_base_from_dictionary(dict_mgr,
                                                   "TileCalorimeter"))
    return (1);

  // Initialize the field indices
  if(initLevelsFromDict (group_name)) return (1);


  // Find value for the field Tile Calorimeter   
  const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
  int tileField   = -1;
  if (atlasDict->get_label_value("subdet", "TileCalorimeter", tileField)) {
    log << MSG::ERROR << "Could not get value for label 'TileCalorimeter' of field 'subdet' in dictionary " 
        << atlasDict->m_name
        << endmsg;
    return (1);
  }

  // Set up id for region and range prefix
  ExpandedIdentifier tile_id;
  tile_id.add(tileField);
  ExpandedIdentifier reg_id = tile_id;
  Range prefix;

  m_full_adc_range    = dict()->build_multirange(reg_id, group_name, prefix, "adc");
  m_full_pmt_range    = dict()->build_multirange(reg_id, group_name, prefix, "pmt");
  m_full_cell_range   = dict()->build_multirange(reg_id, group_name, prefix, "sampling");
  m_full_tower_range  = dict()->build_multirange(reg_id, group_name, prefix, "tower");
  m_full_module_range = dict()->build_multirange(reg_id, group_name, prefix, "module");
  m_full_region_range = dict()->build_multirange(reg_id, group_name, prefix, "side");

  // Find value for the field Tile Online
  int tilehwField   = -1;
  if (dict()->get_label_value("section", "Online", tilehwField)) {
    log << MSG::ERROR << "Could not get value for label 'Online' of field 'section' in dictionary " 
        << dict()->m_name
        << endmsg;
  } else {
    // remove online ID from all ranges
    reg_id = tile_id;
    reg_id.add(tilehwField);
    m_full_adc_range.remove_range(reg_id);
    m_full_pmt_range.remove_range(reg_id);
    m_full_cell_range.remove_range(reg_id);
    m_full_tower_range.remove_range(reg_id);
    m_full_module_range.remove_range(reg_id);
    m_full_region_range.remove_range(reg_id);
  }
    
  // Find value for the field Tile Testbeam
  int tiletbField = -1;
  if (dict()->get_label_value("section", "Testbeam", tiletbField)) {
    log << MSG::ERROR << "Could not get value for label 'Testbeam' of field 'section' in dictionary " 
        << dict()->m_name
        << endmsg;
  } else {
    // remove testbeam ID from all ranges
    reg_id = tile_id;
    reg_id.add(tiletbField);
    m_full_adc_range.remove_range(reg_id);
    m_full_pmt_range.remove_range(reg_id);
    m_full_cell_range.remove_range(reg_id);
    m_full_tower_range.remove_range(reg_id);
    m_full_module_range.remove_range(reg_id);
    m_full_region_range.remove_range(reg_id);
  }

  if (!m_quiet) {
    log << MSG::DEBUG << "initialize_from_dict : "                                << endmsg;
    log << MSG::DEBUG << " region range -> "  << (std::string)m_full_region_range << endmsg;
    log << MSG::DEBUG << " module range -> "  << (std::string)m_full_module_range << endmsg;
    log << MSG::DEBUG << " tower range ->  "  << (std::string)m_full_tower_range  << endmsg;
    log << MSG::DEBUG << " cell range ->   "  << (std::string)m_full_cell_range   << endmsg;
    log << MSG::DEBUG << " pmt range ->    "  << (std::string)m_full_pmt_range    << endmsg;
    log << MSG::DEBUG << " adc range ->    "  << (std::string)m_full_adc_range    << endmsg;
  }
    
  // Setup the hash tables
  if(init_hashes()) return (1);

  // Setup hash tables for finding neighbors
  if(m_do_neighbours)  {
    // temporary workaround to get the super_cell neighbor file directly
    // instead of from the db. In the long term the file name should
    // come from the db as well
    std::string neighbourFile;
    if ( m_supercell ) {
      neighbourFile = "TileSuperCellNeighbour.txt";
    }
    else {
      neighbourFile = dict_mgr.find_metadata("TILENEIGHBORS");
    }
    if (!neighbourFile.size()) throw std::runtime_error("TileID: Cannot find the Tile Neighbour file name");
    if(m_neighbour.initialize(this, neighbourFile)) return (1);
  }

  return 0;
}

Tile_Base_ID::size_type     Tile_Base_ID::region_hash_max (void) const
{
  return regions().hash_max();
}

Tile_Base_ID::size_type     Tile_Base_ID::module_hash_max (void) const
{
  return m_modules.hash_max();
}

Tile_Base_ID::size_type     Tile_Base_ID::tower_hash_max (void) const
{
  return m_towers.hash_max();
}

Tile_Base_ID::size_type     Tile_Base_ID::cell_hash_max (void) const
{
  return channels().hash_max();
}

Tile_Base_ID::size_type     Tile_Base_ID::pmt_hash_max (void) const
{
  return m_pmts.hash_max();
}

Tile_Base_ID::size_type     Tile_Base_ID::adc_hash_max (void) const
{
  return m_adcs.hash_max();
}

Tile_Base_ID::id_iterator Tile_Base_ID::reg_begin       (void) const
{
  return regions().begin();
}

Tile_Base_ID::id_iterator Tile_Base_ID::reg_end         (void) const
{
  return regions().end();
}

Tile_Base_ID::id_range Tile_Base_ID::reg_range() const
{
  return regions().range();
}

Tile_Base_ID::id_iterator Tile_Base_ID::module_begin    (void) const
{
  return m_modules.begin();
}

Tile_Base_ID::id_iterator Tile_Base_ID::module_end      (void) const
{
  return m_modules.end();
}

Tile_Base_ID::id_range Tile_Base_ID::module_range() const
{
  return m_modules.range();
}

Tile_Base_ID::id_iterator Tile_Base_ID::tower_begin     (void) const
{
  return m_towers.begin();
}

Tile_Base_ID::id_iterator Tile_Base_ID::tower_end       (void) const
{
  return m_towers.end();
}

Tile_Base_ID::id_range Tile_Base_ID::tower_range() const
{
  return m_towers.range();
}

Tile_Base_ID::id_iterator Tile_Base_ID::cell_begin      (void) const
{
  return channels().begin();
}

Tile_Base_ID::id_iterator Tile_Base_ID::cell_end        (void) const
{
  return channels().end();
}

Tile_Base_ID::id_range Tile_Base_ID::cell_range() const
{
  return channels().range();
}

Tile_Base_ID::id_iterator Tile_Base_ID::pmt_begin       (void) const
{
  return m_pmts.begin();
}

Tile_Base_ID::id_iterator Tile_Base_ID::pmt_end         (void) const
{
  return m_pmts.end();
}

Tile_Base_ID::id_range Tile_Base_ID::pmt_range() const
{
  return m_pmts.range();
}

Tile_Base_ID::id_iterator Tile_Base_ID::adc_begin       (void) const
{
  return m_adcs.begin();
}

Tile_Base_ID::id_iterator Tile_Base_ID::adc_end         (void) const
{
  return m_adcs.end();
}

Tile_Base_ID::id_range Tile_Base_ID::adc_range() const
{
  return m_adcs.range();
}

const std::vector<Identifier>& Tile_Base_ID::region_ids() const
{
  return regions().ids();
}

const std::vector<Identifier>& Tile_Base_ID::module_ids() const
{
  return m_modules.ids();
}

const std::vector<Identifier>& Tile_Base_ID::tower_ids() const
{
  return m_towers.ids();
}

const std::vector<Identifier>& Tile_Base_ID::cell_ids() const
{
  return channels().ids();
}

const std::vector<Identifier>& Tile_Base_ID::pmt_ids() const
{
  return m_pmts.ids();
}

const std::vector<Identifier>& Tile_Base_ID::adc_ids() const
{
  return m_adcs.ids();
}


Identifier Tile_Base_ID::region_id (const ExpandedIdentifier& exp_id) const
{
  return this->region_id (exp_id[m_SECTION_INDEX],
                          exp_id[m_SIDE_INDEX]);
}

Identifier Tile_Base_ID::module_id (const ExpandedIdentifier& exp_id) const
{
  return this->module_id (exp_id[m_SECTION_INDEX],
                          exp_id[m_SIDE_INDEX], 
                          exp_id[m_MODULE_INDEX]);
}

Identifier Tile_Base_ID::tower_id (const ExpandedIdentifier& exp_id) const
{
  return this->tower_id  (exp_id[m_SECTION_INDEX],
                          exp_id[m_SIDE_INDEX], 
                          exp_id[m_MODULE_INDEX], 
                          exp_id[m_TOWER_INDEX]);
}

Identifier Tile_Base_ID::cell_id (const ExpandedIdentifier& exp_id) const
{
  return this->cell_id   (exp_id[m_SECTION_INDEX],
                          exp_id[m_SIDE_INDEX], 
                          exp_id[m_MODULE_INDEX], 
                          exp_id[m_TOWER_INDEX],
                          exp_id[m_SAMPLE_INDEX]);
}

Identifier Tile_Base_ID::pmt_id (const ExpandedIdentifier& exp_id) const
{
  return this->pmt_id (exp_id[m_SECTION_INDEX],
                       exp_id[m_SIDE_INDEX], 
                       exp_id[m_MODULE_INDEX], 
                       exp_id[m_TOWER_INDEX],
                       exp_id[m_SAMPLE_INDEX],
                       exp_id[m_PMT_INDEX]);
}

Identifier Tile_Base_ID::adc_id (const ExpandedIdentifier& exp_id) const
{
  return this->adc_id (exp_id[m_SECTION_INDEX],
                       exp_id[m_SIDE_INDEX], 
                       exp_id[m_MODULE_INDEX], 
                       exp_id[m_TOWER_INDEX],
                       exp_id[m_SAMPLE_INDEX],
                       exp_id[m_PMT_INDEX],
                       exp_id[m_ADC_INDEX]);
}

int Tile_Base_ID::eta_min(const Identifier& id, const IdContext& context) const
{
  ExpandedIdentifier expId;
  if(!get_expanded_id(id, expId, &context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_tower_range.size(); ++i) {
      const Range& range = m_full_tower_range[i];
      if (range.match(expId)) {
	const Range::field& eta_field = range[m_TOWER_INDEX];
	if (eta_field.has_minimum()) {
	  int etamin = eta_field.get_minimum();
	  if (-999 == result) {
	    result = etamin;
	  }
	  else {
	    if (etamin < result) result = etamin;
	  }
	}
      }
    }
    return (result);
  }
  return (-999);
}

int Tile_Base_ID::eta_max(const Identifier& id, const IdContext& context) const
{
  ExpandedIdentifier expId;
  if(!get_expanded_id(id, expId, &context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_cell_range.size(); ++i) {
      const Range& range = m_full_cell_range[i];
      if (range.match(expId)) {
	const Range::field& eta_field = range[m_TOWER_INDEX];
	if (eta_field.has_maximum()) {
	  int etamax = eta_field.get_maximum();
	  if (result < etamax) result = etamax;
	}
      }
    }
    return (result);
  }
  return (-999);  // default
}

int Tile_Base_ID::eta_min(const Identifier& id) const
{
  IdContext context = tower_context();
  return eta_min (id, context);
}

int Tile_Base_ID::eta_max(const Identifier& id) const
{
  IdContext context = cell_context();
  return eta_max (id, context);
}

int Tile_Base_ID::region_eta_min(const Identifier& id) const
{
  IdContext context = region_context();
  return eta_min (id, context);
}

int Tile_Base_ID::region_eta_max(const Identifier& id) const
{
  IdContext context = region_context();
  return eta_max (id, context);
}

int Tile_Base_ID::phi_max(const Identifier& id) const
{
  ExpandedIdentifier expId;
  IdContext context = cell_context();
  if(!get_expanded_id(id, expId, &context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_cell_range.size(); ++i) {
      const Range& range = m_full_cell_range[i];
      if (range.match(expId)) {
	const Range::field& phi_field = range[m_MODULE_INDEX];
	if (phi_field.has_maximum()) {
	  int phimax = phi_field.get_maximum();
	  if (result < phimax) result = phimax;
	}
      }
    }
    return (result);
  }
  return (-999);  // default
}

int Tile_Base_ID::get_expanded_id  (const Identifier& id,
                                    ExpandedIdentifier& exp_id,
                                    const IdContext* context) const
{
  int result = 1;

  size_t begin = (context) ? context->begin_index() : 0;
  size_t end   = (context) ? context->end_index()   : m_ADC_INDEX;
  assert (end <= m_ADC_INDEX);

  if (0 == end) {
    result = 0;
  }
  else if ( 0 == begin) {
    ExpandedIdentifier empty;
    result = dict()->unpack(id, empty, end, exp_id);
  }
  else {
    result = dict()->unpack(id, context->prefix_id(), end, exp_id);
  }
  return result;
}

int Tile_Base_ID::initLevelsFromDict (const std::string& group_name) 
{
  MsgStream log(m_msgSvc, "Tile_Base_ID" );

  if(!dict()) {
    log << MSG::ERROR << "initLevelsFromDict - dictionary NOT initialized "
        << endmsg;
    return (1);
  }

  // Find out which identifier field corresponds to each level.

  m_SYSTEM_INDEX        = 999;
  m_SECTION_INDEX       = 999;
  m_SIDE_INDEX          = 999;
  m_MODULE_INDEX        = 999;
  m_TOWER_INDEX         = 999;
  m_SAMPLE_INDEX        = 999;
  m_PMT_INDEX           = 999;
  m_ADC_INDEX           = 999;

  IdDictGroup* group = dict()->find_group(group_name);
  if ( !group ){
    log << MSG::ERROR << "initLevelsFromDict - cannot find " << group_name
        << " group' field " << endmsg;
  }
  else {
	m_tile_region_index = group->regions()[0]->m_index;
  }

  // Fing a Tile region
  IdDictField* field = dict()->find_field("subdet") ;
  if (field) {
    m_SYSTEM_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR << "initLevelsFromDict - unable to find 'subdet' field "
        << endmsg ;
    return (1);
  }

  field = dict()->find_field("section") ;
  if (field) {
    m_SECTION_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'section' field "
        << endmsg ;
    return (1);
  }

  field = dict()->find_field("side") ;
  if (field) {
    m_SIDE_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'side' field "
        << endmsg ;
    return (1);
  }

  field = dict()->find_field("module") ;
  if (field) {
    m_MODULE_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'module' field "
        << endmsg ;
    return (1);
  }

  field = dict()->find_field("tower") ;
  if (field) {
    m_TOWER_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'tower' field "
        << endmsg ;
    return (1);
  }

  field = dict()->find_field("sampling") ;
  if (field) {
    m_SAMPLE_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'sampling' field "
        << endmsg ;
    return (1);
  }

  field = dict()->find_field("pmt") ;
  if (field) {
    m_PMT_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'pmt' field "
        << endmsg ;
    return (1);
  }

  field = dict()->find_field("adc") ;
  if (field) {
    m_ADC_INDEX = field->m_index ;
  }
  else {
    log << MSG::ERROR <<  "initLevelsFromDict - unable to find 'adc' field "
        << endmsg ;
    return (1);
  }

  const IdDictRegion& region = *dict()->m_regions[m_tile_region_index];

  m_system_impl  = region.m_implementation[m_SYSTEM_INDEX]; 
  m_section_impl = region.m_implementation[m_SECTION_INDEX];
  m_side_impl    = region.m_implementation[m_SIDE_INDEX]; 
  m_module_impl  = region.m_implementation[m_MODULE_INDEX]; 
  m_tower_impl   = region.m_implementation[m_TOWER_INDEX]; 
  m_sample_impl  = region.m_implementation[m_SAMPLE_INDEX]; 
  m_pmt_impl     = region.m_implementation[m_PMT_INDEX]; 
  m_adc_impl     = region.m_implementation[m_ADC_INDEX]; 

  if (!m_quiet) {
    log << MSG::DEBUG << "initLevelsFromDict decode index and bit fields for each level: "              << endmsg ;
    log << MSG::DEBUG << " system   [" << m_SYSTEM_INDEX   << "]  " << m_system_impl.show_to_string()   << endmsg ;
    log << MSG::DEBUG << " section  [" << m_SECTION_INDEX  << "]  " << m_section_impl.show_to_string()  << endmsg ;
    log << MSG::DEBUG << " side     [" << m_SIDE_INDEX     << "]  " << m_side_impl.show_to_string()     << endmsg ;
    log << MSG::DEBUG << " module   [" << m_MODULE_INDEX   << "]  " << m_module_impl.show_to_string()   << endmsg ;
    log << MSG::DEBUG << " tower    [" << m_TOWER_INDEX    << "]  " << m_tower_impl.show_to_string()    << endmsg ;
    log << MSG::DEBUG << " sampling [" << m_SAMPLE_INDEX   << "]  " << m_sample_impl.show_to_string()   << endmsg ;
    log << MSG::DEBUG << " pmt      [" << m_PMT_INDEX      << "]  " << m_pmt_impl.show_to_string()      << endmsg ;
    log << MSG::DEBUG << " adc      [" << m_ADC_INDEX      << "]  " << m_adc_impl.show_to_string()      << endmsg ;
  }
  
  return(0) ;
}

int Tile_Base_ID::init_hashes(void) 
{
  if (regions().init (*this, "regions", m_full_region_range,
                       &Tile_Base_ID::region_id,
                       m_SIDE_INDEX))
    return 1;

  if (m_modules.init (*this, "modules", m_full_module_range,
                       &Tile_Base_ID::module_id,
                       m_MODULE_INDEX))
    return 1;

  if (m_towers.init (*this, "towers", m_full_tower_range,
                     &Tile_Base_ID::tower_id,
                     m_TOWER_INDEX))
    return 1;

  if (channels().init (*this, "channels", m_full_cell_range,
                       &Tile_Base_ID::cell_id,
                       m_SAMPLE_INDEX))
    return 1;

  if (m_pmts.init (*this, "pmts", m_full_pmt_range,
                   &Tile_Base_ID::pmt_id,
                   m_PMT_INDEX))
    return 1;

  if (m_adcs.init (*this, "adcs", m_full_adc_range,
                   &Tile_Base_ID::adc_id,
                   m_ADC_INDEX))
    return 1;

  return (0);
}


int Tile_Base_ID::get_neighbours(const IdentifierHash& id_orig, const LArNeighbours::neighbourOption& option,
                           std::vector<IdentifierHash>& neighbourList) const
{
  int result = 1; 

  if(!m_do_neighbours) {

    MsgStream log(m_msgSvc, "TileID" );
    log << MSG::WARNING << "get_neighbours: neighbours not initialized !!! returning empty list" << endmsg;

    neighbourList.resize(0);
    return result;
  }

  neighbourList.clear();

  bool prevSmp = (option & prevInSamp);
  bool nextSmp = (option & nextInSamp);

  bool corners2DOnly  = ( (option & all2D) == corners2D );

  bool prevPhi = corners2DOnly || (option & prevInPhi);
  bool nextPhi = corners2DOnly || (option & nextInPhi);
  bool prevEta = corners2DOnly || (option & prevInEta);
  bool nextEta = corners2DOnly || (option & nextInEta);

  std::vector<IdentifierHash> inpList;
  std::vector<IdentifierHash>::iterator inpItr;
  std::vector<IdentifierHash>::iterator inpLast;

  inpList.push_back(id_orig);

  // all phi neighbours (just 2)
  if (prevPhi) m_neighbour.prev_phi(id_orig,neighbourList);
  if (nextPhi) m_neighbour.next_phi(id_orig,neighbourList);
  
  if ( (option & all2D) == faces2D) { 
    // do nothing

  } else if (corners2DOnly) {
    // neighbours of phi-neigbours are 2D corners
    inpList = neighbourList; // copy 2 phi neighbours to input list 
    neighbourList.clear();   // and clear neighbour list

  } else if ( (option & all2D) == all2D) {
    // copy all neighbours to input list
    std::copy(neighbourList.begin(), neighbourList.end(), std::back_inserter(inpList));

  }
  
  // all eta neighbours
  inpItr  = inpList.begin();
  inpLast = inpList.end();
  for ( ; inpItr != inpLast; ++inpItr) {
    IdentifierHash id = (*inpItr);
    if (prevEta) m_neighbour.prev_eta(id,neighbourList);
    if (nextEta) m_neighbour.next_eta(id,neighbourList);
  }

  // all sampling neighbours
  if( (option & all3DwithCorners) == all3DwithCorners ) {
    
    inpItr  = inpList.begin();
    inpLast = inpList.end();
    for ( ; inpItr != inpLast; ++inpItr) {
      IdentifierHash id = (*inpItr);
      m_neighbour.prev_samp_wide(id,neighbourList);
      m_neighbour.next_samp_wide(id,neighbourList);
    }

  } else {
    
    if (prevSmp) m_neighbour.prev_samp(id_orig,neighbourList);
    if (nextSmp) m_neighbour.next_samp(id_orig,neighbourList);
    
  }

  if (neighbourList.size() > 0) result = 0;

  return result;
}

