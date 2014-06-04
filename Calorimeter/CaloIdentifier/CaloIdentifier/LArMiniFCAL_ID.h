/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMiniFCAL_ID_H
#define LARMiniFCAL_ID_H

#include "CLIDSvc/CLASS_DEF.h"
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
  *             Connected channels :
  *             ------------------
  * pos_neg = +/- 2 (A/C side)<br><br>
  *
  * module = [0] : Only one module <br><br>
  *
  * depth  = [0,3] 
  *
  * phi = [0,phimax] : depends on layout <br><br>
  *
  * eta = [0,8] depends on layout ; <br>
  *
  *
  *             Disconnected channels :
  *             ------------------
  * pos_neg = +/- 2 (A/C side)<br><br>
  *
  * no disconnected channels yet <br><br>
  *
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

  /** module identifier for a connected channel from ExpandedIdentifier  */
  Identifier  module_id	(const ExpandedIdentifier& exp_id) const ;

  /** cell identifier for a connected channel from ExpandedIdentifier */
  Identifier  channel_id (const ExpandedIdentifier& exp_id) const ;

  /** build a module identifier for a connected channel  */
  Identifier  module_id	( int pos_neg, int module ) const ;

  /** build a cell identifier for a connected channel  */
  Identifier  channel_id( int pos_neg,  int module,  int depth,
			  int eta,      int phi ) const ;

  /** module identifier for a disconnected channel from ExpandedIdentifier  */
  Identifier  disc_module_id	(const ExpandedIdentifier& exp_id) const ;

  /** cell identifier for a disconnected channel from ExpandedIdentifier */
  Identifier  disc_channel_id (const ExpandedIdentifier& exp_id) const ;

  /** build a module identifier for disconnected channels  */
  Identifier  disc_module_id	( int pos_neg, int module ) const ;

  /** build a cell identifier for disconnected channels  */
  Identifier  disc_channel_id( int pos_neg,  int module, int depth,
			       int eta,      int phi ) const ;

  /** allows to know in which region is a channel/cell
      -- valid for both kinds of channels (connected or not) */
  Identifier  module_id	(const Identifier id ) const;
     
  /** allows to build a channel id starting from a module id (e.g. from descriptors) 
      -- valid for both kinds of channels (connected or not) */
  Identifier  channel_id( const Identifier moduleId, int depth,
			  int eta, int phi) const ;
			     
  /** to disentangle between connected and disconnected channels 
      this method is CPU cheap */
  bool is_connected (const IdentifierHash hashId) const;
  /** to disentangle between connected and disconnected channels 
      this method is CPU cheap */
  bool is_connected (const Identifier channelId) const;
  /** to disentangle between connected and disconnected channels 
      this method is CPU expensive */
  bool is_connected ( int pos_neg, int module, int depth, int eta, int phi )  const throw(LArID_Exception);
  /** to disentangle between connected and disconnected channels 
      this method is CPU expensive */
  bool is_disconnected ( int pos_neg, int module, int depth, int eta, int phi )  const throw(LArID_Exception);

  bool is_supercell (const Identifier channelId) const;
				 
    /** create module id from hash id (connected channels only)*/
  Identifier module_id	(IdentifierHash module_hash_id) const;

    /** create hash id from module id*/
  IdentifierHash module_hash  (Identifier module_id) const;

  /** create hash id from connected channel id  -- method NOT optimised, please use channel_hash() above */
  IdentifierHash channel_hash_binary_search (Identifier channelId) const;

  /** create channel id from hash id  (all channels, connected + diconnected, together) */
  Identifier disc_channel_id	(IdentifierHash disc_channel_hash_id) const;	

    /** create hash id from disconnected channel id*/
  IdentifierHash disc_channel_hash (Identifier disc_channelId) const;

  /**  region hash table max size */
  size_type     module_hash_max (void) const;
  /** cell hash table max size (all channels: connected + disconnected) */
  size_type     disc_channel_hash_max (void) const;
  /** disconnected cell/channel hash table min index */
  size_type     disc_channel_hash_min (void) const;

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

  /** begin iterator over full set of Fcal Identifiers for connected channels */
  id_iterator minifcal_begin    () const;
  /** end iterator over full set of Fcal Identifiers for connected channels */
  id_iterator minifcal_end      () const;
  /** range over full set of Fcal Identifiers for connected channels */
  id_range    minifcal_range    () const;

  /** begin iterator over full set of Fcal Identifiers for DISconnected channels */
  id_iterator disc_minifcal_begin    () const;
  /** end iterator over full set of Fcal Identifiers for DISconnected channels */
  id_iterator disc_minifcal_end      () const;
  /** range over full set of Fcal Identifiers for DISconnected channels */
  id_range    disc_minifcal_range    () const;

  /** begin iterator over full set of Fcal Identifiers for DISconnected modules */
  id_iterator disc_mod_begin    () const;
  /** end iterator over full set of Fcal Identifiers for DISconnected modules */
  id_iterator disc_mod_end      () const;
  /** range over full set of Fcal Identifiers for DISconnected modules */
  id_range    disc_mod_range    () const;

  /** provide access to channel id vector, accessed via hash */
  const std::vector<Identifier>& channel_ids() const;

  /** provide access to module id vector, accessed via hash */
  const std::vector<Identifier>& module_ids() const;

			     
  /** pos_neg : +/- 2 (A/C side) */
  int         pos_neg         (const Identifier id)const; 
  /** module [0] */
  int         module          (const Identifier id)const;
  /** depth [0,3], -999 if disconnected channel */
  int         depth            (const Identifier id)const;
  /** eta [0,63] module 1 ; [0,31] module 2 ; [0,15] module 3, -999 if disconnected channel */
  int         eta             (const Identifier id)const;
  /** phi [0,15], -999 if disconnected channel */
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
               in 'prevInSamp', prev means 'towards   the centre of Atlas' <br> 
  Nota Bene = neighbours do not include diconnected channels */
  int    get_neighbours(const IdentifierHash id, const LArNeighbours::neighbourOption& option, std::vector<IdentifierHash>& neighbourList) const;

  /** eta index -- only for checks, since dummy for disconnected channels */
  int         disc_eta             (const Identifier id)const; 
  /** phi index -- only for checks, since dummy for disconnected channels */
  int         disc_phi             (const Identifier id)const; 

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

  void  module_id_checks      ( int pos_neg, int module ) const throw(LArID_Exception);
  void  channel_id_checks     ( int pos_neg,  int module,
				int depth,  int eta,      int phi ) const throw(LArID_Exception);
  void  channel_id_checks     ( const Identifier moduleId,
				int depth, int eta, int phi) const throw(LArID_Exception);
  void  disc_module_id_checks      ( int pos_neg, int module ) const throw(LArID_Exception);
  void  disc_channel_id_checks     ( int pos_neg,  int module,
				     int depth,   int eta,      int phi ) const throw(LArID_Exception);
  void  disc_channel_id_checks     ( const Identifier moduleId,
				     int depth,  int eta, int phi) const throw(LArID_Exception);
			     
  /** create expanded Identifier from Identifier (return == 0 for OK) */
  virtual int  get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;
  int  get_disc_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;


  int         initLevelsFromDict(void) ;

  int         init_hashes(void) ;
  int         init_disc_hashes(void) ;

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
  MultiRange                    m_full_disc_channel_range;
  MultiRange                    m_full_disc_module_range;

  CaloIDHelper::HashGroup       m_disc_channels;
  CaloIDHelper::HashGroup       m_disc_modules;

  bool                          m_two_sym_sides;

  std::vector<short int>        m_vecOfCellInfo;
  std::vector<short int>        m_vecOfPhiMin;


  IdDictFieldImplementation     m_lar_impl;
  IdDictFieldImplementation  	m_fcal_impl;
  IdDictFieldImplementation  	m_pn_impl;
  IdDictFieldImplementation  	m_module_impl;
  IdDictFieldImplementation  	m_depth_impl;
  IdDictFieldImplementation  	m_eta_impl;
  IdDictFieldImplementation  	m_phi_impl;

  IdDictFieldImplementation  	m_pn_mod_impl;

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
LArMiniFCAL_ID::module_id (int pos_neg, int module) const 
{

    Identifier result(0);

    // Do checks 
    if(m_do_checks) {
	module_id_checks ( pos_neg, module );
    }

    // Pack fields independently
    m_lar_impl.pack      (lar_field_value(),      result);
    m_fcal_impl.pack     (lar_fcal_field_value(), result);
    m_pn_impl.pack       (pos_neg,                result);
    m_module_impl.pack   (module,                 result);

    return result;
}

//----------------------------------------------------------------------------
inline Identifier 
LArMiniFCAL_ID::channel_id   ( int pos_neg, int module, int depth,
			   int eta,     int phi ) const 
{  

    // Do checks 
    if(m_do_checks) {
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

//----------------------------------------------------------------------------
inline Identifier 
LArMiniFCAL_ID::disc_module_id (const ExpandedIdentifier& exp_id) const 
{
  return this->disc_module_id   ( exp_id[m_POSNEG_INDEX],
                                  exp_id[m_MODULE_INDEX] );
}

//----------------------------------------------------------------------------
inline Identifier 
LArMiniFCAL_ID::disc_channel_id (const ExpandedIdentifier& exp_id) const 
{
  return this->disc_channel_id (exp_id[m_POSNEG_INDEX],
                                exp_id[m_MODULE_INDEX], 
                                exp_id[m_DEPTH_INDEX],
                                exp_id[m_ETA_INDEX],
                                exp_id[m_PHI_INDEX]);
}

//----------------------------------------------------------------------------
inline Identifier 
LArMiniFCAL_ID::disc_module_id (int pos_neg, int module) const 
{

    // Do checks 
    if(m_do_checks) {
	disc_module_id_checks ( pos_neg, module );
    }

    Identifier result(0);

    // Pack fields independently
    m_lar_impl.pack      (lar_field_value(),      result);
    m_fcal_impl.pack     (-3                    , result);
    m_pn_impl.pack       (pos_neg,                result);
    m_module_impl.pack   (module,                 result);

    return result;
}

//----------------------------------------------------------------------------
inline Identifier 
LArMiniFCAL_ID::disc_channel_id   ( int pos_neg, int module,  int depth,
				int eta,     int phi ) const 
{  

    // Do checks 
    if(m_do_checks) {
	disc_channel_id_checks( pos_neg, module, depth, eta, phi );
    }

    Identifier result(0);
    // Pack fields independently
    m_lar_impl.pack      (lar_field_value(),      result);
    m_fcal_impl.pack     (-3                    , result);
    m_pn_impl.pack       (pos_neg,                result);
    m_module_impl.pack   (module,                 result);
    m_depth_impl.pack    (depth,                  result);
    m_eta_impl.pack      (eta,                    result);
    m_phi_impl.pack      (phi,                    result);

    return result;
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
			   int eta,       int phi ) const 
{
    // Do checks 
    if(m_do_checks) {
      if(is_connected(module_id)) {
	channel_id_checks( module_id, depth, eta, phi );
      } else {
	disc_channel_id_checks( module_id, depth, eta, phi );
      }
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

//----------------------------------------------------------------------------
inline bool LArMiniFCAL_ID::is_connected(const IdentifierHash hashId) const
{
  return ( hashId < channels().hash_max() );
}

//----------------------------------------------------------------------------
inline bool LArMiniFCAL_ID::is_connected(const Identifier channelId) const
{
    return ( m_fcal_impl.unpack(channelId) == 3 );
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
inline Identifier LArMiniFCAL_ID::disc_channel_id    (IdentifierHash disc_channel_hash_id) const
{
  if(disc_channel_hash_id < channels().hash_max()) {
    return channel_id (disc_channel_hash_id);
  }
  else {
    return m_disc_channels.id (disc_channel_hash_id - channels().hash_max());
  }
}

//----------------------------------------------------------------------------
inline IdentifierHash LArMiniFCAL_ID::disc_channel_hash (Identifier disc_channelId) const
{
  if (is_connected(disc_channelId)) {
    return channel_hash (disc_channelId);
  }
  else {
    IdentifierHash hash = m_disc_channels.hash (disc_channelId);
    if (hash.is_valid())
      hash += channels().hash_max();
    return hash;
  }
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::size_type     LArMiniFCAL_ID::module_hash_max (void) const
{
  return region_hash_max();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::size_type     LArMiniFCAL_ID::disc_channel_hash_max (void) const
{
  return channels().hash_max() + m_disc_channels.hash_max();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::size_type     LArMiniFCAL_ID::disc_channel_hash_min (void) const
{
  return channels().hash_max() ;
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
inline LArMiniFCAL_ID::id_iterator LArMiniFCAL_ID::disc_minifcal_begin    (void) const
{
  return m_disc_channels.begin();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::id_iterator LArMiniFCAL_ID::disc_minifcal_end      (void) const
{
  return m_disc_channels.end();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::id_range LArMiniFCAL_ID::disc_minifcal_range() const
{
  return m_disc_channels.range();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::id_iterator LArMiniFCAL_ID::disc_mod_begin    (void) const
{
  return m_disc_modules.begin();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::id_iterator LArMiniFCAL_ID::disc_mod_end      (void) const
{
  return m_disc_modules.end();
}

//----------------------------------------------------------------------------
inline LArMiniFCAL_ID::id_range LArMiniFCAL_ID::disc_mod_range() const
{
  return m_disc_modules.range();
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
  int result = -999 ;
  if(is_connected(id)) result = m_depth_impl.unpack(id);
  return(result);
}

//----------------------------------------------------------------------------
inline int LArMiniFCAL_ID::eta               (const Identifier id)const
{
  int result = -999 ;
  if(is_connected(id)) result = m_eta_impl.unpack(id);
  return(result);
}

//----------------------------------------------------------------------------
inline int LArMiniFCAL_ID::phi(const Identifier id)const
{
  int result = -999 ;
  if(is_connected(id)) result = m_phi_impl.unpack(id);
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
