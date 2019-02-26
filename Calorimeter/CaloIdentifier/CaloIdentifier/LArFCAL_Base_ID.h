/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOIDENTIFIER_LARFCAL_BASE_ID_H
#define CALOIDENTIFIER_LARFCAL_BASE_ID_H

#include "CaloIdentifier/CaloIDHelper.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "CaloIdentifier/LArNeighbours.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "boost/range/iterator_range.hpp"

class IdDictRegion;

class LArFCAL_Base_ID : public CaloIDHelper
{
public:
        
  typedef Identifier::size_type  size_type ;

  LArFCAL_Base_ID(const std::string& name, bool supercell);

  /** module identifier for a channel from ExpandedIdentifier */
  Identifier  module_id	(const ExpandedIdentifier& exp_id) const ;

  /** cell identifier for a channel from ExpandedIdentifier */
  Identifier  channel_id (const ExpandedIdentifier& exp_id) const ;

  /** build a module identifier for a channel  */
  Identifier  module_id	( int pos_neg, int module ) const ;
  Identifier  module_id	( int pos_neg, int module, bool checks ) const ;

  /** build a cell identifier for a channel  */
  Identifier  channel_id( int pos_neg,  int module,
			  int eta,      int phi ) const ;
  Identifier  channel_id( int pos_neg,  int module,
			  int eta,      int phi, bool checks ) const ;

  /** allows to know in which region is a channel/cell
      -- valid for both kinds of channels */
  Identifier  module_id	(const Identifier id ) const;
     
  /** allows to build a channel id starting from a module id (e.g. from descriptors) 
      -- valid for both kinds of channels */
  Identifier  channel_id( const Identifier moduleId,
			  int eta, int phi) const ;
  Identifier  channel_id( const Identifier moduleId,
			  int eta, int phi, bool checks) const ;
			     
    /** create module id from hash id*/
  Identifier module_id	(IdentifierHash module_hash_id) const;

    /** create hash id from module id*/
  IdentifierHash module_hash  (Identifier module_id) const;

  /** create hash id from channel id  -- method NOT optimised, please use channel_hash() above */
  IdentifierHash channel_hash_binary_search (Identifier channelId) const;

  /**  region hash table max size */
  size_type     module_hash_max (void) const;

  /** Type for iterators over identifiers. */
  typedef std::vector<Identifier>::const_iterator id_iterator;
  /** Type for range over identifiers. */
  typedef boost::iterator_range<id_iterator> id_range;

  /** begin iterator over set of module Identifiers */
  id_iterator mod_begin    (void) const;
  /** end iterator over set of module Identifiers */
  id_iterator mod_end      (void) const;
  /** Range over full set of FCAL modules. */
  id_range mod_range () const;

  /** begin iterator over full set of Fcal Identifiers for channels */
  id_iterator fcal_begin    (void) const;
  /** end iterator over full set of Fcal Identifiers for channels */
  id_iterator fcal_end      (void) const;
  /** Range over full set of FCAL Identifiers. */
  id_range fcal_range () const;

  /** provide acces to channel id vector, accessed via hash */
  const std::vector<Identifier>& channel_ids() const;

			     
  /** pos_neg : +/- 2 (A/C side) */
  int         pos_neg         (const Identifier id)const; 
  /** module [1,3] */
  int         module          (const Identifier id)const;
  /** eta [0,63] module 1 ; [0,31] module 2 ; [0,15] module 3 */
  int         eta             (const Identifier id)const;
  /** phi [0,15] */
  int         phi             (const Identifier id)const; 

  /** Test if the identifier represents a supercell. */
  bool        is_supercell    (const Identifier id)const;

  /** min value of eta index (-999 == failure) 
      @warning input = REGION ID !! */
  int eta_min(const Identifier regId) const;
  /** max value of eta index (-999 == failure) 
      @warning input = REGION ID !! */
  int eta_max(const Identifier regId) const;
  /** min value of phi index (-999 == failure) 
      @warning input = REGION ID !! */
  int phi_min(const Identifier regId) const;
  /** max value of phi index (-999 == failure) 
      @warning input = REGION ID !! */
  int phi_max(const Identifier regId) const;

  /** context for modules --  method kept for backward compatibility. NOT optimised <br>
      access to IdContext's which define which levels of fields are contained in the id */
  IdContext    module_context   (void) const;



  /** access to hashes for neighbours      return == 0 for neighbours found  <br>
      option = all2D, prevInSamp, nextInSamp, all3D <br>
               in 'nextInSamp', next means 'away from the centre of Atlas' <br>
               in 'prevInSamp', prev means 'towards   the centre of Atlas' <br>  */
  int  get_neighbours(const IdentifierHash id,
                      const LArNeighbours::neighbourOption& option,
                      std::vector<IdentifierHash>& neighbourList) const;



  /** initialization from the identifier dictionary*/
  virtual int  initialize_base_from_dictionary (const IdDictMgr& dict_mgr,
						const std::string& group_name);


  using CaloIDHelper::channel_id;
  using CaloIDHelper::channel_hash;
  // Inherited from CaloIDHelper:
  // 
  //Identifier channel_id (IdentifierHash hashId) const;
  //Identifier region_id (IdentifierHash hashId) const;
  //IdentifierHash channel_hash (Identifier channelId) const;
  //IdentifierHash region_hash (Identifier regionId) const;
  //size_type channel_hash_max() const;
  //size_type region_hash_max() const;
  //IdContext channel_context() const;
  //IdContext region_context() const;
  //const std::vector<const IdDictRegion*>& dictRegions() const;
  //float etaGranularity(const IdentifierHash regHash) const;
  //float phiGranularity(const IdentifierHash regHash) const;
  //float eta0(const IdentifierHash regHash) const;
  //float phi0(const IdentifierHash regHash) const;
  //virtual int get_id (const IdentifierHash& hash_id,
  //                    Identifier& id,
  //                    const IdContext* context = 0 ) const;
  //virtual int  get_hash  (const Identifier& id,
  //                        IdentifierHash& hash_id,
  //                        const IdContext* context = 0 ) const;


private:    

  enum {NOT_VALID_HASH = 64000};

  int phi_min_init(const Identifier regId) const;

  // Check methods

  void  module_id_checks      ( int pos_neg, int module ) const;
  void  channel_id_checks     ( int pos_neg,  int module,
				int eta,      int phi ) const;
  void  channel_id_checks     ( const Identifier moduleId,
				int eta, int phi) const;
			     
  /** create expanded Identifier from Identifier (return == 0 for OK) */
  virtual int  get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;

  int         initLevelsFromDict(const std::string& group_name) ;
  int         init_hashes(void) ;

  int         init_neighbours_from_file(std::string filename, std::vector<std::set<IdentifierHash> > &vec);

  int         init_neighbours_2d(std::string filename)
    { 
      return init_neighbours_from_file(filename, m_neighbours_2d_vec);
    }

  int         init_neighbours_3d_next(std::string filename)
    { 
      return init_neighbours_from_file(filename, m_neighbours_3d_next_vec);
    }

  int         init_neighbours_3d_prev(std::string filename)
    { 
      return init_neighbours_from_file(filename, m_neighbours_3d_prev_vec);
    }

  int         init_neighbours(const IdDictMgr& dict_mgr) ;


private:
  unsigned                      m_slar;

  bool                          m_two_sym_sides;

  size_type                     m_fcal_region_index;
  size_type                     m_LAR_INDEX;
  size_type                     m_SLAR_INDEX;
  size_type                     m_FCAL_INDEX;
  size_type                     m_POSNEG_INDEX;
  size_type                     m_MODULE_INDEX;
  size_type                     m_ETA_INDEX;
  size_type                     m_PHI_INDEX;

  MultiRange                    m_full_channel_range;
  MultiRange                    m_full_module_range;

  std::vector<short int>        m_vecOfPhiMin;

  IdDictFieldImplementation     m_lar_impl;
  IdDictFieldImplementation  	m_fcal_impl;
  IdDictFieldImplementation  	m_pn_impl;
  IdDictFieldImplementation  	m_module_impl;
  IdDictFieldImplementation  	m_eta_impl;
  IdDictFieldImplementation  	m_phi_impl;
  IdDictFieldImplementation  	m_pn_mod_impl;

  IdDictFieldImplementation  	m_slar_impl;

  LArFCAL_Base_ID (const LArFCAL_Base_ID&);
  LArFCAL_Base_ID& operator= (const LArFCAL_Base_ID&);

  std::vector<std::set<IdentifierHash> >  m_neighbours_2d_vec;
  std::vector<std::set<IdentifierHash> >  m_neighbours_3d_next_vec;
  std::vector<std::set<IdentifierHash> >  m_neighbours_3d_prev_vec;
};

#include "CaloIdentifier/LArFCAL_Base_ID.icc"


#endif // not CALOIDENTIFIER_LARFCAL_BASE_ID_H
