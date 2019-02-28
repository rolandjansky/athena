/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMiniFCAL_ID_H
#define LARMiniFCAL_ID_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "IdDict/IdDictDefs.h"

#include "CaloIdentifier/CaloIDHelper.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/LArNeighbours.h"

#include "boost/range/iterator_range.hpp"
#include <vector>
#include <algorithm>
#include <set>


/**
*
* @class LArMiniFCAL_ID
* @brief Helper class for LArMiniFCAL offline identifiers
*  
  * 
  * This class provides an interface to decode and generate offline identifiers
  * for the LAr MiniFCAL detector. <p>
  * 
  * Definition and range of values for the fields: <p>
  * <pre>
  *
  * pos_neg = +/- 2 (A/C side)<br><br>
  *
  * module = [0] : Only one module <br><br>
  *
  * depth  = [0,3] 
  *
  * phi = [0,phimax] : depends on layout <br><br>
  *
  * eta = [0,8] depends on layout ; <br>
  * </pre>
  *
  * @author started by RD Schaffer
  * @author maintained by F Ledroit
  * @author neighbour list/code provided by Sven Menke
  * @author adapted from Fcal for MiniFcal by M Fincke
  */  

class Range;
class LArMiniFCAL_region;

class LArMiniFCAL_ID : public CaloIDHelper
{
public:
        
  typedef Identifier::size_type  size_type ;

  LArMiniFCAL_ID(void);
        
  ~LArMiniFCAL_ID(void);

  /** check whether LArMiniFCAL ID helper has been initialized - 
      returns false if description is not available    */
  bool        is_initialized() const;

  /** module identifier for a channel from ExpandedIdentifier  */
  Identifier  module_id (const ExpandedIdentifier& exp_id) const ;

  /** cell identifier for a channel from ExpandedIdentifier */
  Identifier  channel_id (const ExpandedIdentifier& exp_id) const ;

  /** build a module identifier for a channel  */
  Identifier  module_id ( int pos_neg, int module ) const ;
  Identifier  module_id ( int pos_neg, int module, bool checks ) const ;

  /** build a cell identifier for a channel  */
  Identifier  channel_id( int pos_neg,  int module,  int depth,
                          int eta,      int phi ) const ;
  Identifier  channel_id( int pos_neg,  int module,  int depth,
                          int eta,      int phi, bool checks ) const ;

  /** allows to know in which region is a channel/cell
      -- valid for both kinds of channels */
  Identifier  module_id (const Identifier id ) const;
     
  /** allows to build a channel id starting from a module id (e.g. from descriptors) 
      -- valid for both kinds of channels */
  Identifier  channel_id( const Identifier moduleId, int depth,
                          int eta, int phi) const ;
  Identifier  channel_id( const Identifier moduleId, int depth,
                          int eta, int phi, bool checks) const ;
                             
  bool is_supercell (const Identifier channelId) const;
                                 
    /** create module id from hash id */
  Identifier module_id  (IdentifierHash module_hash_id) const;

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
  id_iterator mod_begin    () const;
  /** end iterator over set of module Identifiers */
  id_iterator mod_end      () const;
  /** range over set of module Identifiers */
  id_range    mod_range    () const;

  /** begin iterator over full set of Fcal Identifiers for channels */
  id_iterator minifcal_begin    () const;
  /** end iterator over full set of Fcal Identifiers for channels */
  id_iterator minifcal_end      () const;
  /** range over full set of Fcal Identifiers for channels */
  id_range    minifcal_range    () const;

  /** provide access to channel id vector, accessed via hash */
  const std::vector<Identifier>& channel_ids() const;

  /** provide access to module id vector, accessed via hash */
  const std::vector<Identifier>& module_ids() const;

                             
  /** pos_neg : +/- 2 (A/C side) */
  int         pos_neg         (const Identifier id)const; 
  /** module [0] */
  int         module          (const Identifier id)const;
  /** depth [0,3] */
  int         depth            (const Identifier id)const;
  /** eta [0,63] module 1 ; [0,31] module 2 ; [0,15] module 3 */
  int         eta             (const Identifier id)const;
  /** phi [0,15] */
  int         phi             (const Identifier id)const; 

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
  
  /** access to hashes for neighbours      return == 0 for neighbours found  <br>
      option = all2D, prevInSamp, nextInSamp, all3D <br>
               in 'nextInSamp', next means 'away from the centre of Atlas' <br>
               in 'prevInSamp', prev means 'towards   the centre of Atlas' <br> */
  int    get_neighbours(const IdentifierHash id, const LArNeighbours::neighbourOption& option, std::vector<IdentifierHash>& neighbourList) const;

  /** context for modules --  method kept for backward compatibility. NOT optimised <br>
      access to IdContext's which define which levels of fields are contained in the id */
  IdContext    module_context   (void) const;

  /** initialization from the identifier dictionary*/
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);

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

  std::vector<std::set<IdentifierHash> >  m_neighbors_2d_vec;
  std::vector<std::set<IdentifierHash> >  m_neighbors_3d_next_vec;
  std::vector<std::set<IdentifierHash> >  m_neighbors_3d_prev_vec;

  // Check methods

  void  module_id_checks      ( int pos_neg, int module ) const;
  void  channel_id_checks     ( int pos_neg,  int module,
                                int depth,  int eta,      int phi ) const;
  void  channel_id_checks     ( const Identifier moduleId,
                                int depth, int eta, int phi) const;
                             
  /** create expanded Identifier from Identifier (return == 0 for OK) */
  virtual int  get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;

  int         initLevelsFromDict(void) ;

  int         init_hashes(void) ;

  int         init_neighbors_from_file(std::string filename, std::vector<std::set<IdentifierHash> > &vec);

  int         init_neighbors_2d(std::string filename)
    { 
      return init_neighbors_from_file(filename, m_neighbors_2d_vec);
    }

  int         init_neighbors_3d_next(std::string filename)
    { 
      return init_neighbors_from_file(filename, m_neighbors_3d_next_vec);
    }

  int         init_neighbors_3d_prev(std::string filename)
    { 
      return init_neighbors_from_file(filename, m_neighbors_3d_prev_vec);
    }

  int         init_neighbors(const IdDictMgr& dict_mgr) ;

  // flag to protect for when helper hasn't been able to be
  // initialized - no description in xml dictionary
  bool                          m_is_initialized;  
  size_type                     m_fcal_region_index;
  size_type                     m_minifcal_region_index;
  size_type                     m_LAR_INDEX;
  size_type                     m_MiniFCAL_INDEX;
  size_type                     m_POSNEG_INDEX;
  size_type                     m_MODULE_INDEX;
  size_type                     m_DEPTH_INDEX;
  size_type                     m_ETA_INDEX;
  size_type                     m_PHI_INDEX;

  MultiRange                    m_full_channel_range;
  MultiRange                    m_full_module_range;
  MultiRange                    m_full_depth_range;

  bool                          m_two_sym_sides;

  std::vector<short int>        m_vecOfCellInfo;
  std::vector<short int>        m_vecOfPhiMin;


  IdDictFieldImplementation     m_lar_impl;
  IdDictFieldImplementation     m_fcal_impl;
  IdDictFieldImplementation     m_pn_impl;
  IdDictFieldImplementation     m_module_impl;
  IdDictFieldImplementation     m_depth_impl;
  IdDictFieldImplementation     m_eta_impl;
  IdDictFieldImplementation     m_phi_impl;

  IdDictFieldImplementation     m_pn_mod_impl;

};

//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( LArMiniFCAL_ID , 79264204  , 1 )


//--------------------
// Inline definitions
//--------------------

//----------------------------------------------------------------------------
inline bool
LArMiniFCAL_ID::is_initialized() const
{
    return m_is_initialized;
}

//----------------------------------------------------------------------------
inline Identifier 
LArMiniFCAL_ID::module_id (const ExpandedIdentifier& exp_id) const 
{
  return this->module_id   ( exp_id[m_POSNEG_INDEX],
                             exp_id[m_MODULE_INDEX] );
}

//----------------------------------------------------------------------------
inline Identifier 
LArMiniFCAL_ID::channel_id (const ExpandedIdentifier& exp_id) const 
{
  return this->channel_id (exp_id[m_POSNEG_INDEX],
                           exp_id[m_MODULE_INDEX], 
                           exp_id[m_DEPTH_INDEX],
                           exp_id[m_ETA_INDEX],
                           exp_id[m_PHI_INDEX]);
}

//----------------------------------------------------------------------------
inline Identifier 
LArMiniFCAL_ID::module_id (int pos_neg, int module, bool checks) const 
{

    Identifier result(0);

    // Do checks 
    if(checks) {
        module_id_checks ( pos_neg, module );
    }

    // Pack fields independently
    m_lar_impl.pack      (lar_field_value(),      result);
    m_fcal_impl.pack     (lar_fcal_field_value(), result);
    m_pn_impl.pack       (pos_neg,                result);
    m_module_impl.pack   (module,                 result);

    return result;
}

inline Identifier 
LArMiniFCAL_ID::module_id (int pos_neg, int module) const 
{
  return module_id (pos_neg, module, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier 
LArMiniFCAL_ID::channel_id   ( int pos_neg, int module, int depth,
                               int eta,     int phi,
                               bool checks ) const 
{  

    // Do checks 
    if(checks) {
        channel_id_checks( pos_neg, module, depth, eta, phi );
    }

    Identifier result(0);
    // Pack fields independently
    m_lar_impl.pack      (lar_field_value(),      result);
    m_fcal_impl.pack     (lar_fcal_field_value(), result);
    m_pn_impl.pack       (pos_neg,                result);
    m_module_impl.pack   (module,                 result);
    m_depth_impl.pack    (depth,                  result);
    m_eta_impl.pack      (eta,                    result);
    m_phi_impl.pack      (phi,                    result);

    return result;
}

inline Identifier 
LArMiniFCAL_ID::channel_id   ( int pos_neg, int module, int depth,
                               int eta,     int phi ) const
{
  return channel_id (pos_neg, module, depth, eta, phi, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier 
LArMiniFCAL_ID::module_id   ( const Identifier channelId ) const 
{

  Identifier result(channelId);
  //  reset depth/eta/phi
  m_depth_impl.reset(result);
  m_eta_impl.reset(result);
  m_phi_impl.reset(result);
  return (result);
}

//----------------------------------------------------------------------------
inline Identifier 
LArMiniFCAL_ID::channel_id   ( const Identifier module_id, int depth,
                               int eta,       int phi,
                               bool checks) const 
{
    // Do checks 
    if(checks) {
      channel_id_checks( module_id, depth, eta, phi );
    }

    Identifier result(module_id);

    // Reset the fields and then set the values
    m_depth_impl.reset   (result);
    m_eta_impl.reset     (result);
    m_phi_impl.reset     (result);
    m_depth_impl.pack    (depth, result);
    m_eta_impl.pack      (eta, result);
    m_phi_impl.pack      (phi, result);

    return result;
}

inline Identifier 
LArMiniFCAL_ID::channel_id   ( const Identifier module_id, int depth,
                               int eta,       int phi) const
{
  return channel_id (module_id, depth, eta, phi, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier LArMiniFCAL_ID::module_id    (IdentifierHash module_hash_id) const
{
  return region_id(module_hash_id);
}

//----------------------------------------------------------------------------
inline IdentifierHash LArMiniFCAL_ID::module_hash (Identifier module_id) const
{
  return region_hash (module_id);
}

//----------------------------------------------------------------------------
inline IdentifierHash LArMiniFCAL_ID::channel_hash_binary_search  (Identifier channelId) const
{
  return channel_hash (channelId);
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::size_type     LArMiniFCAL_ID::module_hash_max (void) const
{
  return region_hash_max();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::id_iterator LArMiniFCAL_ID::mod_begin    (void) const
{
  return regions().begin();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::id_iterator LArMiniFCAL_ID::mod_end      (void) const
{
  return regions().end();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::id_range LArMiniFCAL_ID::mod_range() const
{
  return regions().range();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::id_iterator LArMiniFCAL_ID::minifcal_begin    (void) const
{
  return channels().begin();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::id_iterator LArMiniFCAL_ID::minifcal_end      (void) const
{
  return channels().end();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::id_range LArMiniFCAL_ID::minifcal_range() const
{
  return channels().range();
}

//----------------------------------------------------------------------------
inline const std::vector<Identifier>& LArMiniFCAL_ID::channel_ids() const
{
  return channels().ids();
}

//----------------------------------------------------------------------------
inline const std::vector<Identifier>& LArMiniFCAL_ID::module_ids() const
{
  return regions().ids();
}

//----------------------------------------------------------------------------
inline int LArMiniFCAL_ID::pos_neg         (const Identifier id)const
{
  return (m_pn_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int LArMiniFCAL_ID::module              (const Identifier id)const
{
  return (m_module_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int LArMiniFCAL_ID::depth             (const Identifier id)const
{
  int result = m_depth_impl.unpack(id);
  return(result);
}

//----------------------------------------------------------------------------
inline int LArMiniFCAL_ID::eta               (const Identifier id)const
{
  int result = m_eta_impl.unpack(id);
  return(result);
}

//----------------------------------------------------------------------------
inline int LArMiniFCAL_ID::phi(const Identifier id)const
{
  int result = m_phi_impl.unpack(id);
  return(result);
}

//----------------------------------------------------------------------------
inline int LArMiniFCAL_ID::phi_min(const Identifier modId) const
{
  IdentifierHash modHash = module_hash(modId);
  unsigned int index = modHash;
  return (m_vecOfPhiMin[index]);
}

//----------------------------------------------------------------------------
inline bool LArMiniFCAL_ID::is_supercell(const Identifier /*channelId*/) const
{
  return false;
}

#endif // LARMiniFCAL_ID_H
