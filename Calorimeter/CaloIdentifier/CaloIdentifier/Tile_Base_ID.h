// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/Tile_Base_ID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Factor out code common between TileID and Tile_SuperCell_ID.
 */

#ifndef CALOIDENTIFIER_TILE_BASE_ID_H
#define CALOIDENTIFIER_TILE_BASE_ID_H

#include "AtlasDetDescr/AtlasDetectorID.h"
// #include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "CaloIdentifier/CaloIDHelper.h"
#include "CaloIdentifier/TileID_Exception.h"
#include "CaloIdentifier/TileNeighbour.h"

#include "AthenaKernel/CLASS_DEF.h"

#include "boost/range/iterator_range.hpp"
#include <string>
#include <vector>
#include <set>

class IdentifierHash;


/**  
 *
 * This class factors out code common between TileID and Tile_SuperCell_ID.
 */
class Tile_Base_ID : public CaloIDHelper {

  friend class TileNeighbour;

public:

    /** Constructor.  If SUPERCELL is true, this helper is for supercells. */
    Tile_Base_ID (const std::string& name, bool supercell);

    enum SECTION { ONLINE = 0, BARREL = 1, EXTBAR = 2, GAPDET = 3, AUXDET = 4 ,
                   SBARREL = 5, SEXTBAR = 6,
                   SECTION_MASK = 3,
                   SUPERCELL_MASK = 4
                 };
    enum SAMPLE { SAMP_A = 0, 
                  SAMP_B = 1, SAMP_BC = 1, SAMP_C = 1, // synonyms
                  SAMP_D = 2, SAMP_E = 3, SAMP_X = 4 };
    enum SIDE { NEGATIVE = -1, POSITIVE = 1, TWOSIDES = 0 };
    enum GAIN { LOWGAIN = 0,   HIGHGAIN = 1 };
    enum SIZE { NSIDES = 2, NSECTIONS = 3, NREGIONS = NSIDES * NSECTIONS };

    /** build identifier for whole TileCal */
    Identifier          tile_system             ()     const;

    /** build identifier for Tile Barrel */
    Identifier          tile_barrel             ()     const;

    /** build identifier for Tile Extended Barrel */
    Identifier          tile_extbarrel          ()     const;

    /** build identifier for Tile Gap detector (ITC + gap/crack scintillators) */
    Identifier          tile_gap                ()     const;

    /** build identifier for any Tilecal section */
    Identifier          tile_det                ( int section)                  const;
    Identifier          tile_det                ( int section, bool checks)     const;

    /** build single region, module, tower, cell, pmt, adc identifiers */
    Identifier          region_id               ( int index)                    const;
    Identifier          region_id               ( int section, int side)        const;
    Identifier          region_id               ( int section, int side, bool checks) const;
    Identifier          region_id               ( const Identifier& any_id)     const;

    Identifier          module_id               ( int section, int side,
                                                  int module)                   const;
    Identifier          module_id               ( int section, int side,
                                                  int module, bool checks)      const;
    Identifier          module_id               ( const Identifier& any_id)     const;
    Identifier          tower_id                ( int section,  int side,
                                                  int module,   int tower)      const;
    Identifier          tower_id                ( int section,  int side,
                                                  int module,   int tower,
                                                  bool checks)                  const;
    Identifier          tower_id               ( const Identifier& any_id)      const;
    Identifier          cell_id                 ( const Identifier & any_id )   const;
    Identifier          cell_id                 ( int section,  int side,
                                                  int module,   int tower,
                                                  int sample )                  const;
    Identifier          cell_id                 ( int section,  int side,
                                                  int module,   int tower,
                                                  int sample, bool checks )     const;
    Identifier          pmt_id                  ( const Identifier & any_id )   const;
    Identifier          pmt_id                  ( const Identifier & cell_id,
                                                  int pmt )                     const;
    Identifier          pmt_id                  ( const Identifier & cell_id,
                                                  int pmt, bool checks )        const;
    Identifier          pmt_id                  ( int section,  int side,
                                                  int module,   int tower,
                                                  int sample,   int pmt )       const;
    Identifier          pmt_id                  ( int section,  int side,
                                                  int module,   int tower,
                                                  int sample,   int pmt,
                                                  bool checks)                  const;
    Identifier          adc_id                  ( const Identifier & cell_id,
                                                  int pmt, int adc )            const;
    Identifier          adc_id                  ( const Identifier & cell_id,
                                                  int pmt, int adc,
                                                  bool checks )                 const;
    Identifier          adc_id                  ( const Identifier & pmt_id,
                                                  int adc )                     const;
    Identifier          adc_id                  ( const Identifier & pmt_id,
                                                  int adc, bool checks )        const;
    Identifier          adc_id                  ( int section,  int side,
                                                  int module,   int tower,
                                                  int sample,   int pmt,
                                                  int adc ) const;
    Identifier          adc_id                  ( int section,  int side,
                                                  int module,   int tower,
                                                  int sample,   int pmt,
                                                  int adc, bool checks ) const;

    /** fast conversion from hash to ID for cells */
    Identifier          cell_id                 (const IdentifierHash& hash_id) const;
    /** fast conversion from ID to hash for cells */
    IdentifierHash      cell_hash               (const Identifier& cell_id) const;

    /** Test of an Identifier to see if it belongs to a particular part of the calorimeter */
//    bool	is_tile                         (const Identifier& id) const;
    bool	is_tile_barrel                  (const Identifier& id) const;
    bool	is_tile_extbarrel               (const Identifier& id) const;
    bool	is_tile_gap                     (const Identifier& id) const;
    bool	is_tile_gapscin                 (const Identifier& id) const;
    bool	is_tile_aux                     (const Identifier& id) const;

    bool        is_negative                     (const Identifier& id) const;
    bool        is_positive                     (const Identifier& id) const;
    bool        is_twosides                     (const Identifier& id) const;

    bool        is_low_gain                     (const Identifier& id) const;
    bool        is_high_gain                    (const Identifier& id) const;

    bool        is_supercell                    (const Identifier& id) const;
  
    int                 region                  ( int section, int side )       const;
    int                 region                  ( const Identifier & id )       const;
    int                 system                  ( const Identifier & id )       const;
    int                 section                 ( const Identifier & id )       const;
    int                 side                    ( const Identifier & id )       const;
    int                 module                  ( const Identifier & id )       const;
    int                 tower                   ( const Identifier & id )       const;
    int                 sample                  ( const Identifier & id )       const;
    int                 pmt                     ( const Identifier & id )       const;
    int                 adc                     ( const Identifier & id )       const;

    // fast way to get Tile sampling (be careful - doesn't work for MBTS)
    int                 sampling                ( const Identifier & id )       const { return m_sample_impl.unpack(id); } 

    std::string         to_string               (const Identifier & id, int level=0)      const;
    std::string         to_string               (const IdentifierHash & hash_id, int level=0)      const;


  /** access to IdContext's which define which levels of fields are contained in the id */
  /** id for regions */
  IdContext    region_context   (void) const;

  /** id for modules */
  IdContext    module_context   (void) const;

  /** id for towers */
  IdContext    tower_context    (void) const;

  /** id for cells */
  IdContext    cell_context     (void) const;

  /** id for PMTs */
  IdContext    pmt_context      (void) const;

  /** id for ADCs */
  IdContext    adc_context      (void) const;


    /** create compact id from hash id (return == 0 for OK)*/
  virtual int  get_id    (const IdentifierHash& hash_id, Identifier& id, const IdContext* context = 0 ) const;

    /** create hash id from compact id (return == 0 for OK)*/
  virtual int  get_hash  (const Identifier& id, IdentifierHash& hash_id, const IdContext* context = 0 ) const;

  /** initialization from the identifier dictionary*/
  int  initialize_base_from_dictionary (const IdDictMgr& dict_mgr,
                                        const std::string& group_name);

  /**  hash tables max size */
  size_type     region_hash_max (void) const;
  size_type     module_hash_max (void) const;
  size_type     tower_hash_max  (void) const;
  size_type     cell_hash_max   (void) const;
  size_type     pmt_hash_max    (void) const;
  size_type     adc_hash_max    (void) const;

  /** Type for iterators over identifiers. */
  typedef std::vector<Identifier>::const_iterator id_iterator;
  /** Type for range over identifiers. */
  typedef boost::iterator_range<id_iterator> id_range;

  /** iterator over set of region Identifiers */
  id_iterator reg_begin    () const;
  id_iterator reg_end      () const;
  id_range    reg_range    () const;

  /** iterator over set of region Identifiers (synonyms for naming consistency) */
  id_iterator region_begin    () const { return reg_begin(); }
  id_iterator region_end      () const { return reg_end(); }
  id_range    region_range    () const { return  reg_range(); }

  /** iterator over set of module Identifiers */
  id_iterator module_begin () const;
  id_iterator module_end   () const;
  id_range    module_range () const;

  /** iterator over set of tower Identifiers */
  id_iterator tower_begin  () const;
  id_iterator tower_end    () const;
  id_range    tower_range  () const;

  /** iterator over set of cell Identifiers */
  id_iterator cell_begin   () const;
  id_iterator cell_end     () const;
  id_range    cell_range   () const;

  /** iterator over set of pmt Identifiers */
  id_iterator pmt_begin    () const;
  id_iterator pmt_end      () const;
  id_range    pmt_range    () const;

  /** iterator over set of adc Identifiers */
  id_iterator adc_begin    () const;
  id_iterator adc_end      () const;
  id_range    adc_range    () const;

  /** provide access to id vectors, accessed via hash */
  const std::vector<Identifier>& region_ids() const;
  const std::vector<Identifier>& module_ids() const;
  const std::vector<Identifier>& tower_ids() const;
  const std::vector<Identifier>& cell_ids() const;
  const std::vector<Identifier>& pmt_ids() const;
  const std::vector<Identifier>& adc_ids() const;
  

  /** max values (-999 == failure) */
  int eta_min(const Identifier& id) const;
  int eta_max(const Identifier& id) const;
  int region_eta_min(const Identifier& id) const;
  int region_eta_max(const Identifier& id) const;
  int phi_max(const Identifier& id) const;
  
  enum {NOT_VALID_HASH = 64000};

  /** Construct identifiers with checking. */
  bool        module_id (const Identifier& region_id,
                         int module,
                         Identifier& module_id ) const;
  bool        tower_id  (const Identifier& region_id,
                         int module, int tower,
                         Identifier& tower_id ) const;
  bool        cell_id   (const Identifier& region_id,
                         int module, int tower, int sample,
                         Identifier& cell_id ) const;
  bool        pmt_id    (const Identifier& cell_id,
                         int pmt,
                         Identifier& pmt_id ) const;
  bool        adc_id    (const Identifier& pmt_id,
                         int adc,
                         Identifier& adc_id ) const;
  bool        adc_id    (const Identifier& cell_id,
                         int pmt, int adc,
                         Identifier& adc_id ) const;

  /// Construct identifiers from ExpandedIdentifier's.
  Identifier region_id (const ExpandedIdentifier& exp_id) const;
  Identifier module_id (const ExpandedIdentifier& exp_id) const;
  Identifier tower_id (const ExpandedIdentifier& exp_id) const;
  Identifier cell_id (const ExpandedIdentifier& exp_id) const;
  Identifier pmt_id (const ExpandedIdentifier& exp_id) const;
  Identifier adc_id (const ExpandedIdentifier& exp_id) const;


  /** access to hashes for neighbours      return == 0 for neighbours found 
      option = prevInPhi, nextInPhi, prevInEta, nextInEta, ..., ..., all2D, prevInSamp, nextInSamp, ..., all3D

               in 'nextInEta' next always means higher eta (i.e. smaller abs(eta) for negative eta)
               in 'prevInEta' prev always means smaller eta
               in 'nextInPhi' next always means higher phi (i.e. smaller abs(phi) for negative phi)
               in 'prevInPhi' prev always means smaller phi
               in 'nextInSamp', next means 'away from the centre of Atlas'
               in 'prevInSamp', prev means 'towards   the centre of Atlas'
	       all2D means PrevInEta and nextInEta for all cells obtained in prevInPhi and nextInPhi
	       all3D means prevInSamp and nextInSamp for all cells obtained in all2D 
      NOTA  neighbour is also returned across Barrel-ITC-Gap.Sin-Ext.Barrel boundaries */
  int    get_neighbours(const IdentifierHash& id,
                        const LArNeighbours::neighbourOption& option,
                        std::vector<IdentifierHash>& neighbourList) const;


private:

  int decode_section (int section) const
  {
    return section >= SBARREL ? (section & SECTION_MASK) : section;
  }

  int encode_section (int section) const
  {
    return m_supercell ? (section | SUPERCELL_MASK) : section;
  }

  /** create expanded Identifier from Identifier (return == 0 for OK) */
  virtual int  get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;

  /** unpack one field from Identifier */
  int                   get_field               ( const Identifier & id,
                                                  size_type index )             const;

  int         initLevelsFromDict (const std::string& group_name) ;

  int         init_hashes(void) ;

  int eta_min(const Identifier& id, const IdContext& context) const;
  int eta_max(const Identifier& id, const IdContext& context) const;

  unsigned                      m_supercell;
  
  size_type                     m_tile_region_index;
  size_type                     m_SYSTEM_INDEX;
  size_type                     m_SECTION_INDEX;
  size_type                     m_SIDE_INDEX;
  size_type                     m_MODULE_INDEX;
  size_type                     m_TOWER_INDEX;
  size_type                     m_SAMPLE_INDEX;
  size_type                     m_PMT_INDEX;
  size_type                     m_ADC_INDEX;

  IdDictFieldImplementation     m_system_impl;
  IdDictFieldImplementation     m_section_impl;
  IdDictFieldImplementation     m_side_impl;
  IdDictFieldImplementation     m_module_impl;
  IdDictFieldImplementation     m_tower_impl;
  IdDictFieldImplementation     m_sample_impl;
  IdDictFieldImplementation     m_pmt_impl;
  IdDictFieldImplementation     m_adc_impl;
  IdDictFieldImplementation     m_slar_impl;

  MultiRange                    m_full_region_range;
  MultiRange                    m_full_module_range;
  MultiRange                    m_full_tower_range;
  MultiRange                    m_full_cell_range;
  MultiRange                    m_full_pmt_range;
  MultiRange                    m_full_adc_range;

  CaloIDHelper::HashGroup       m_modules;
  CaloIDHelper::HashGroup       m_towers;
  CaloIDHelper::HashGroup       m_pmts;
  CaloIDHelper::HashGroup       m_adcs;

  TileNeighbour                 m_neighbour;
};


#endif // not CALOIDENTIFIER_TILE_BASE_ID_H

