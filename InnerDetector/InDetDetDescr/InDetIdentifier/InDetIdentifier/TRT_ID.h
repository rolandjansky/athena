/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETIDENTIFIER_TRT_ID_H
#define INDETIDENTIFIER_TRT_ID_H
/**
 * @file TRT_ID.h
 *
 * @brief This is an Identifier helper class for the TRT
 *  subdetector. This class is a factory for creating compact
 *  Identifier objects and IdentifierHash or hash ids. And it also
 *  allows decoding of these ids.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: TRT_ID.h,v 1.26 2008-12-23 07:29:17 dquarrie Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "Identifier/IdHelper.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <algorithm>
#include <string>
#include <map>
#include <assert.h>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IdDictDictionary;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 **  @class TRT_ID
 **  
 **  @brief This is an Identifier helper class for the TRT
 **  subdetector. This class is a factory for creating compact
 **  Identifier objects and IdentifierHash or hash ids. And it also
 **  allows decoding of these ids.
 **
 **  For the TRT there are four different identifiers that one may
 **  define, where each one defines only a certain number of "levels"
 **  for the identifier: 
 **    - barrel/endcap: levels indet/trt/b_ec
 **    - module or wheel: b_ec + /phi module/layer or wheel
 **    - straw layer: previous + straw layer within a module/wheel
 **    - straw: previous + straw number within a layer
 **    .
 **
 **  Definition and the range of values for the levels of the
 **  identifier are:
 **
 **  @verbatim
 **    element           range              meaning
 **    -------           -----              -------
 **
 **    barrel_ec        +/- 1            positive/negative barrel
 **                     +/- 2            positive/negative endcap
 **                     0 to 17          endcap wheel range
 **    phi module       0 to 31          for barrel modular geometry
 **                        0             for barrel "simple layer" geometry
 **                     0 to 31          for endcap
 **    layer or wheel   0 to 2           barrel module layer number
 **                     0 to 72          is straw layer for barrel "simple layer" geometry
 **    straw_layer      0 to 18 (19)     for barrel module 0 for modular (layer) geometry
 **                     0 to 23          for barrel module 1       "        "      "
 **                     0 to 29 (28)     for barrel module 2       "        "      "
 **                        0            for barrel "simple layer" geometry
 **                     0 to 15 or 7     for ec (middle modules have 8 layers, 
 **                                              224 total layers in ec)
 **    straw            0 to <28         for barrel, depends upon layer
 **                     0 to <981        for barrel "simple layer" geometry
 **                     0 to <23        for ec (fewer for last 4 modules/wheels due
 **                                            to smaller inner radius)
 **  @endverbatim
 **
 */

class TRT_ID : public AtlasDetectorID
{
public:
        
    /// @name public typedefs
    //@{
    typedef Identifier::size_type                       size_type; 
    typedef std::vector<Identifier>::const_iterator     const_id_iterator;
    typedef MultiRange::const_identifier_factory        const_expanded_id_iterator;
    //@}

    /// @name strutors
    //@{
    TRT_ID(void);
    ~TRT_ID(void);
    //@}

    /// @name Check whether TRT_ID is valid for the current layour
    //@{
    bool        is_valid() const;
    //@}

    /// @name Creators for the different ids
    //@{
    /// For +/-barrel or +/-endcap id
    Identifier  barrel_ec_id( int barrel_ec ) const;
    Identifier  barrel_ec_id( int barrel_ec, bool checks ) const;

    /// For an individual module phi sector
    Identifier  module_id ( int barrel_ec, 
                            int phi_module,
                            int layer_or_wheel ) const;
    Identifier  module_id ( int barrel_ec, 
                            int phi_module,
                            int layer_or_wheel,
                            bool checks) const;

    /// From hash - optimized, no checks
    Identifier  module_id ( IdentifierHash module_hash ) const;

    /// For an individual straw layer
    Identifier  layer_id ( int barrel_ec, 
                           int phi_module,
                           int layer_or_wheel, 
                           int straw_layer ) const;
    Identifier  layer_id ( int barrel_ec, 
                           int phi_module,
                           int layer_or_wheel, 
                           int straw_layer,
                           bool checks) const;

    /// From hash - optimized, no checks
    Identifier  layer_id ( IdentifierHash straw_layer_hash ) const;

    /// For an individual straw layer
    Identifier  layer_id ( const Identifier& module_id, 
                           int straw_layer ) const;
    Identifier  layer_id ( const Identifier& module_id, 
                           int straw_layer,
                           bool checks) const;

    /// For an individual straw layer from straw id
    Identifier  layer_id ( const Identifier& straw_id ) const;

    /// Three ways of getting id for a single straw: 
    Identifier  straw_id ( int barrel_ec, 
                           int phi_module,
                           int layer_or_wheel, 
                           int straw_layer, 
                           int straw ) const;
    Identifier  straw_id ( int barrel_ec, 
                           int phi_module,
                           int layer_or_wheel, 
                           int straw_layer, 
                           int straw,
                           bool checks) const;

    Identifier  straw_id (const Identifier& module_id, 
                          int straw_layer, 
                          int straw ) const;
    Identifier  straw_id (const Identifier& module_id, 
                          int straw_layer, 
                          int straw,
                          bool checks) const;

    Identifier  straw_id (const Identifier& layer_id, 
                          int straw ) const;
    Identifier  straw_id (const Identifier& layer_id, 
                          int straw,
                          bool checks) const;

    /// Access to straw id from hash - must have done
    /// init_straw_hash_vector before using this
    Identifier  straw_id ( IdentifierHash straw_hash ) const;
    //@}

    /// @name Special method to allow use of straw_id from straw Hash
    //@{
    /// The init will use up ~2 MB of memory, so one should do init,
    /// use straw_id and then reset
    void        init_straw_hash_vector(void); 
    void        reset_straw_hash_vector(void); 
    //@}

    /// @name Hash table maximum sizes
    //@{
    size_type   module_hash_max         (void) const;
    size_type   straw_layer_hash_max    (void) const;
    size_type   straw_hash_max          (void) const;
    //@}


    /// @name Access to all ids
    //@{
    /// Iterators over full set of module ids. Module ids are sorted
    const_id_iterator   module_begin                    (void) const;
    const_id_iterator   module_end                      (void) const;
    /// Iterators over full set of straw_layer ids. Straw_layer ids are sorted
    const_id_iterator   straw_layer_begin               (void) const;
    const_id_iterator   straw_layer_end                 (void) const;
    /// For straw ids, only expanded id iterators are available. Use
    /// "straw_id" method to obtain a compact identifier
    const_expanded_id_iterator  straw_begin             (void) const;  
    const_expanded_id_iterator  straw_end               (void) const;
    //@}

    /// @name Optimized accessors
    //@{
    /// module hash from id - optimized
    IdentifierHash      module_hash      (Identifier module_id) const;

    /// straw_layer hash from id - optimized
    IdentifierHash      straw_layer_hash (Identifier straw_layer_id) const;

    /// straw hash from id - optimized
    IdentifierHash      straw_hash       (Identifier straw_id) const;

    /// Test for barrel
    bool        is_barrel       (const Identifier& id) const;  

    /// Values of different levels (failure returns 0)
    int         barrel_ec       (const Identifier& id) const;  
    int         phi_module      (const Identifier& id) const; 
    int         layer_or_wheel  (const Identifier& id) const; 
    int         straw_layer     (const Identifier& id) const; 
    int         straw           (const Identifier& id) const;
    
    /// Max values for each field (-999 == failure)
    int         straw_layer_max (const Identifier& id) const;
    int         straw_max       (const Identifier& id) const;
    //@}


    /// @name contexts to distinguish between different ids
    //@{
    /// barrel_ec id
    IdContext   barrel_context          (void) const;
    /// module id
    IdContext   module_context          (void) const;
    /// straw_layer id
    IdContext   straw_layer_context     (void) const;
    /// straw id
    IdContext   straw_context           (void) const;
    //@}


    /// @name methods from abstract interface - slower than opt version
    //@{
    /// Create compact id from hash id (return == 0 for OK)
    virtual int get_id          (const IdentifierHash& hash_id,
                                 Identifier& id,
                                 const IdContext* context = 0) const;
    
    /// Create hash id from compact id (return == 0 for OK)
    virtual int get_hash        (const Identifier& id, 
                                 IdentifierHash& hash_id,
                                 const IdContext* context = 0) const;
    //@}

    /// @name interaction with id dictionary
    //@{

    /// Create straw Identifier from expanded id, which is returned by the
    /// id_iterators
    Identifier  straw_id        (const ExpandedIdentifier& straw_id) const;

    
/*     /// Specifying range from strings */
/*     Range       range ( const std::string& barrel_ec,   */
/*                      const std::string& phi_module,  */
/*                      const std::string& layer_or_wheel,       */
/*                         const std::string& straw_layer,  */
/*                      const std::string& straw ) const; */

    /// Set the IdDictionary
    virtual int initialize_from_dictionary(const IdDictMgr& dict_mgr);

    /// Special method for timing tests
    int         get_straw_layer_hash_calc (const ExpandedIdentifier& id, 
                                           IdentifierHash& hash_id) const;
    

    //// Create expanded id from compact id (return == 0 for OK)
    void        get_expanded_id (const Identifier& id,
                                 ExpandedIdentifier& exp_id,
                                 const IdContext* context = 0) const;

    /// Tests of packing
    void        test_trt_ids    (void) ;
    //@}
    

    enum        MODULE_RANGE { NUMBER_OF_EC_MODULES = 18};

private:

    typedef std::vector<Identifier>     hash_vec;
    typedef hash_vec::const_iterator    hash_vec_it;

    enum        VALUES { BARREL_VALUE           = 1 };

    void        barrel_ec_id_checks( int barrel_ec ) const;
    void        module_id_checks ( int barrel_ec, 
                                   int phi_module,
                                   int layer_or_wheel ) const;
    void        straw_id_checks ( int barrel_ec, 
                                  int phi_module,
                                  int layer_or_wheel, 
                                  int straw_layer, 
                                  int straw ) const;
    void        layer_id_checks ( int barrel_ec, 
                                  int phi_module,
                                  int layer_or_wheel, 
                                  int straw_layer ) const;

    int         initLevelsFromDict(void);

    void        init_hashes(void);
    // Private access only for hash from binary search - must have
    // done init_straw_hashes for this to work
    IdentifierHash      straw_hash_bin   (Identifier straw_id) const;

    // print out the invalid message
    void        invalidMessage() const;

    bool                        m_is_valid;
    size_type                   m_trt_region_index;
    size_type                   m_INDET_INDEX;
    size_type                   m_TRT_INDEX;
    size_type                   m_BARREL_EC_INDEX;
    size_type                   m_PHI_MODULE_INDEX;
    size_type                   m_LAYER_OR_WHEEL_INDEX;
    size_type                   m_STRAW_LAYER_INDEX;
    size_type                   m_STRAW_INDEX;
    const IdDictDictionary*     m_dict;
    MultiRange                  m_full_straw_range;
    MultiRange                  m_full_straw_layer_range;
    MultiRange                  m_full_module_range;
    size_type                   m_module_hash_max;
    size_type                   m_straw_layer_hash_max;
    size_type                   m_straw_hash_max;
    Range::field                m_barrel_field;
    hash_vec                    m_module_vec;
    hash_vec                    m_straw_layer_vec;
    hash_vec                    m_straw_vec;

    class HashCalc
    {
    public:
        HashCalc() :
                m_hash(0),
                m_deltaPhi(0),
                m_layerMin(0),
                m_strLayerMin(0),
                m_nStrawLayers(0),
                m_nstraws(0)
            {}
        IdentifierHash m_hash;
        size_type      m_deltaPhi;
        size_type      m_layerMin;
        size_type      m_strLayerMin;
        size_type      m_nStrawLayers;
        size_type      m_nstraws;
    };

    typedef std::pair<unsigned int, unsigned int> intPair;
    typedef std::map<IdentifierHash, HashCalc> hashCalcMap;
    std::vector<HashCalc>       m_hash_calcs;
    std::vector<intPair>        m_hash_calcs_barrel_indexes;
    std::vector<intPair>        m_hash_calcs_endcap_indexes;

    IdDictFieldImplementation   m_indet_impl;
    IdDictFieldImplementation   m_trt_impl;
    IdDictFieldImplementation   m_bec_impl;
    IdDictFieldImplementation   m_phi_mod_impl;
    IdDictFieldImplementation   m_lay_wheel_impl;
    IdDictFieldImplementation   m_str_lay_impl;
    IdDictFieldImplementation   m_straw_impl;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>

/////////////////////////////////////////////////////////////////////////////
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
/////////////////////////////////////////////////////////////////////////////

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF(TRT_ID, 2518, 1)


//----------------------------------------------------------------------------
inline bool
TRT_ID::is_valid() const 
{
    return (m_is_valid);
}


//----------------------------------------------------------------------------
inline Identifier
TRT_ID::barrel_ec_id( int barrel_ec, bool checks ) const
{
    
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_indet_impl.pack    (indet_field_value(), result);
    m_trt_impl.pack      (trt_field_value(),   result);
    m_bec_impl.pack      (barrel_ec,           result);

    if(checks) {
        barrel_ec_id_checks ( barrel_ec );
    }

    return result;
}

inline Identifier
TRT_ID::barrel_ec_id( int barrel_ec ) const
{
  return barrel_ec_id (barrel_ec, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier
TRT_ID::module_id ( int barrel_ec, 
                    int phi_module,
                    int layer_or_wheel,
                    bool checks) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_indet_impl.pack    (indet_field_value(), result);
    m_trt_impl.pack      (trt_field_value(),   result);
    m_bec_impl.pack      (barrel_ec,           result);
    m_phi_mod_impl.pack  (phi_module,          result);
    m_lay_wheel_impl.pack(layer_or_wheel,      result);

    if(checks) {
        module_id_checks ( barrel_ec, phi_module, layer_or_wheel );
    }

    return result;
}

inline Identifier
TRT_ID::module_id ( int barrel_ec, 
                    int phi_module,
                    int layer_or_wheel ) const
{
  return module_id (barrel_ec, phi_module, layer_or_wheel, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier  
TRT_ID::module_id ( IdentifierHash module_hash ) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_module_vec[module_hash]);
}

//----------------------------------------------------------------------------
inline Identifier  TRT_ID::layer_id ( IdentifierHash layer_hash ) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_straw_layer_vec[layer_hash]);
}


//----------------------------------------------------------------------------
inline Identifier
TRT_ID::layer_id ( int barrel_ec, 
                   int phi_module,
                   int layer_or_wheel, 
                   int straw_layer,
                   bool checks) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_indet_impl.pack    (indet_field_value(), result);
    m_trt_impl.pack      (trt_field_value(),   result);
    m_bec_impl.pack      (barrel_ec,           result);
    m_phi_mod_impl.pack  (phi_module,          result);
    m_lay_wheel_impl.pack(layer_or_wheel,      result);
    m_str_lay_impl.pack  (straw_layer,         result);

    if(checks) {
        layer_id_checks ( barrel_ec, phi_module, layer_or_wheel, straw_layer );
    }

    return result;
}

inline Identifier
TRT_ID::layer_id ( int barrel_ec, 
                   int phi_module,
                   int layer_or_wheel, 
                   int straw_layer ) const
{
  return layer_id (barrel_ec, phi_module, layer_or_wheel, straw_layer,
                   do_checks());
}

//----------------------------------------------------------------------------
inline Identifier  
TRT_ID::layer_id ( const Identifier& module_id, 
                   int straw_layer,
                   bool checks) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    // Build identifier
    Identifier result(module_id); 

    // Reset field and then pack it
    m_str_lay_impl.reset (result);
    m_str_lay_impl.pack  (straw_layer, result);

    if(checks) {
        layer_id_checks ( barrel_ec(module_id), 
                          phi_module(module_id), 
                          layer_or_wheel(module_id), 
                          straw_layer );
    }
    return result;
}

inline Identifier  
TRT_ID::layer_id ( const Identifier& module_id, 
                   int straw_layer ) const
{
  return layer_id (module_id, straw_layer, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier
TRT_ID::layer_id ( const Identifier& straw_id ) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    Identifier result(straw_id);
    //  Reset the straw field
    m_straw_impl.reset(result);
    return (result);
}

//----------------------------------------------------------------------------
inline Identifier
TRT_ID::straw_id ( int barrel_ec, 
                   int phi_module,
                   int layer_or_wheel, 
                   int straw_layer, 
                   int straw,
                   bool checks) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_indet_impl.pack    (indet_field_value(), result);
    m_trt_impl.pack      (trt_field_value(),   result);
    m_bec_impl.pack      (barrel_ec,           result);
    m_phi_mod_impl.pack  (phi_module,          result);
    m_lay_wheel_impl.pack(layer_or_wheel,      result);
    m_str_lay_impl.pack  (straw_layer,         result);
    m_straw_impl.pack    (straw,               result);

    if(checks) {
        straw_id_checks ( barrel_ec, phi_module, layer_or_wheel, straw_layer, straw );
    }

    return result;
}

inline Identifier
TRT_ID::straw_id ( int barrel_ec, 
                   int phi_module,
                   int layer_or_wheel, 
                   int straw_layer, 
                   int straw ) const
{
  return straw_id (barrel_ec, phi_module, layer_or_wheel, straw_layer, straw,
                   do_checks());
}

//----------------------------------------------------------------------------
inline Identifier               
TRT_ID::straw_id        (const ExpandedIdentifier& id) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    Identifier result;
    if (m_STRAW_INDEX < id.fields()) {
        result = straw_id(id[m_BARREL_EC_INDEX],
                          id[m_PHI_MODULE_INDEX],
                          id[m_LAYER_OR_WHEEL_INDEX],
                          id[m_STRAW_LAYER_INDEX],
                          id[m_STRAW_INDEX]);
    }
    return (result);
}

//----------------------------------------------------------------------------
inline Identifier
TRT_ID::straw_id(const Identifier& module_id, 
                 int straw_layer, 
                 int straw,
                 bool checks) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    // Build identifier
    Identifier result(module_id); 

    // Reset fields and then pack them
    m_str_lay_impl.reset (result);
    m_straw_impl.reset   (result);
    m_str_lay_impl.pack  (straw_layer, result);
    m_straw_impl.pack    (straw,       result);

    if(checks) {
        straw_id_checks ( barrel_ec(module_id), 
                          phi_module(module_id), 
                          layer_or_wheel(module_id), 
                          straw_layer, 
                          straw );
    }
    return result;
}

inline Identifier
TRT_ID::straw_id(const Identifier& module_id, 
                 int straw_layer, 
                 int straw ) const
{
  return straw_id (module_id, straw_layer, straw, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier
TRT_ID::straw_id(const Identifier& layer_id, 
                 int straw,
                 bool checks) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    // Build identifier
    Identifier result(layer_id); 

    // Reset field and then pack it
    m_straw_impl.reset   (result);
    m_straw_impl.pack    (straw, result);

    if(checks) {
        straw_id_checks ( barrel_ec(layer_id), 
                          phi_module(layer_id), 
                          layer_or_wheel(layer_id), 
                          straw_layer(layer_id), 
                          straw );
    }
    return result;
}

inline Identifier
TRT_ID::straw_id(const Identifier& layer_id, 
                 int straw ) const
{
  return straw_id (layer_id, straw, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier
TRT_ID::straw_id ( IdentifierHash straw_hash ) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();


    // Access id from vector ONLY if the vector has been initialized 
    //  (done via       init_straw_hash_vector(); 
    //   and reset by   reset_straw_hash_vector(); 
    if (straw_hash < m_straw_vec.size()) {
        return (m_straw_vec[straw_hash]);
    }
    // Else return invalid id
    Identifier result;
    return (result); // return hash in invalid state
    
    // The following is a non-debugged version of a calculation from hash to id

//     // Find which HashCalc to use:
//     hashCalcMap::const_iterator it = m_hash_calc_map.upper_bound(straw_hash);
//     if (it == m_hash_calc_map.begin()) {
//      Identifier result;
//      return (result);
//     }
//     --it; // move back to HashCalc below or equal to straw_hash

//     int barrel_ec = (*it).second.m_barrel_ec;
        
//     // If we have gone more than one cycle in phi, must refind the correct HashCalc
//     if (straw_hash >= (*it).second.m_deltaPhi) {
//      unsigned int startHash;
//      if (barrel_ec == -2) {
//          startHash = m_beStartHash[0];
//      }
//      else if (barrel_ec == -1) {
//          startHash = m_beStartHash[1];
//      }
//      else if (barrel_ec ==  1) {
//          startHash = m_beStartHash[2];
//      }
//      else if (barrel_ec ==  2) {
//          startHash = m_beStartHash[3];
//      }
//      else {
//          Identifier result;
//          return (result);
//      }
//      unsigned int h  = (straw_hash - startHash) % (*it).second.m_deltaPhi + startHash;
//      std::cout << "straw_id: h, straw_hash, (*it).second.m_deltaPhi, startHash " 
//                << h << " " << straw_hash << " " << (*it).second.m_deltaPhi << " " 
//                << startHash << std::endl;

//      it = m_hash_calc_map.upper_bound(h);
//      if (it == m_hash_calc_map.begin()) {
//          Identifier result;
//          return (result);
//      }
//      --it; // move back to HashCalc below or equal to straw_hash
//     }
    
//     const HashCalc& hc = (*it).second;
    
//     unsigned int hash  = (straw_hash - hc.m_hash) % hc.m_deltaPhi;
//     int phi_module     = 0;
//     if (hc.m_deltaPhi > 0) phi_module = straw_hash/hc.m_deltaPhi;
//     int layer_or_wheel = 0;
//     if (hc.m_nStrawLayers > 0) layer_or_wheel = (hash % hc.m_nLayers)/hc.m_nStrawLayers 
//      + hc.m_layerMin;
//     int straw_layer    = 0;
//     if (hc.m_nstraws > 0) straw_layer = (hash % hc.m_nStrawLayers)/hc.m_nstraws
//      + hc.m_strLayerMin;
//     int straw          = hash % hc.m_nstraws;


//     std::cout << "straw_id: " << (*it).first << " " << straw_hash << " "  << hash << " "  
//            << hc.m_deltaPhi << " "  
//            << hc.m_nLayers << " "  
//            << hc.m_nStrawLayers << " "  
//            << hc.m_nstraws << " "  
//            << hash % hc.m_deltaPhi << " "  
//            << hash % hc.m_nStrawLayers << " "  
//            << hash % hc.m_nstraws << " "  
//            << hc.m_layerMin << " "
//            << hc.m_strLayerMin << " "
//            << std::endl;
//     std::cout << "straw_id: " << barrel_ec << " "  
//            << phi_module << " "  
//            << layer_or_wheel << " "  
//            << straw_layer << " "  
//            << straw << std::endl;
    
//     return (straw_id ( barrel_ec, 
//                     phi_module,
//                     layer_or_wheel, 
//                     straw_layer, 
//                     straw ));
    
}


//----------------------------------------------------------------------------
inline IdContext        
TRT_ID::barrel_context          (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    ExpandedIdentifier id;
    return (IdContext(id, 0, m_BARREL_EC_INDEX));
}

//----------------------------------------------------------------------------
inline IdContext        
TRT_ID::module_context          (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    ExpandedIdentifier id;
    return (IdContext(id, 0, m_LAYER_OR_WHEEL_INDEX));
}

//----------------------------------------------------------------------------
inline IdContext        
TRT_ID::straw_layer_context     (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    ExpandedIdentifier id;
    return (IdContext(id, 0, m_STRAW_LAYER_INDEX));
}

//----------------------------------------------------------------------------
inline IdContext        
TRT_ID::straw_context           (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    ExpandedIdentifier id;
    return (IdContext(id, 0, m_STRAW_INDEX));
}

//----------------------------------------------------------------------------
inline IdentifierHash      TRT_ID::module_hash      (Identifier module_id) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    hash_vec_it it = std::lower_bound(m_module_vec.begin(), 
                                      m_module_vec.end(), 
                                      module_id);
    // Require that module_id matches the one in vector
    if (it != m_module_vec.end() && module_id == (*it)) {
        return (it - m_module_vec.begin());
    }
    IdentifierHash result;
    return (result); // return hash in invalid state
}

//----------------------------------------------------------------------------
inline 
IdentifierHash
TRT_ID::straw_layer_hash (Identifier straw_layer_id) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    hash_vec_it it = std::lower_bound(m_straw_layer_vec.begin(), 
                                      m_straw_layer_vec.end(), 
                                      straw_layer_id);
    // Require that straw_layer_id matches the one in vector
    if (it != m_straw_layer_vec.end() && straw_layer_id == (*it)) {
        return (it - m_straw_layer_vec.begin());
    }
    IdentifierHash result;
    return (result); // return hash in invalid state
}

//----------------------------------------------------------------------------
inline
IdentifierHash  
TRT_ID::straw_hash       (Identifier straw_id) const
//TRT_ID::straw_hash       (Identifier straw_id, bool debug) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    // Get index into hashCalc vector
    unsigned int hcIndex = 0;
    int be = barrel_ec(straw_id);
    int lw = layer_or_wheel(straw_id);
    int sl = straw_layer(straw_id);

    if (is_barrel(straw_id)) {
        unsigned int index = lw*32 + sl;
        if (index < m_hash_calcs_barrel_indexes.size()) {
            hcIndex =  (be < 0) ? m_hash_calcs_barrel_indexes[index].first :
                m_hash_calcs_barrel_indexes[index].second;
        }
        else {
            std::cout << "straw_hash: index > barrel_index size - " << index 
                      << " " <<  m_hash_calcs_barrel_indexes.size() << std::endl;
        }
    }
    else {
        unsigned int index = lw;
        if (index < m_hash_calcs_endcap_indexes.size()) {
            hcIndex =  (be < 0) ? m_hash_calcs_endcap_indexes[index].first :
                m_hash_calcs_endcap_indexes[index].second;
        }
        else {
            std::cout << "straw_hash: index > endcap_index size - " << index 
                      << " " <<  m_hash_calcs_endcap_indexes.size() << std::endl;
        }
    }

//     if (debug) {
//      std::cout << "lw, sl, index " << lw << " " << sl << " " << hcIndex << std::endl;
//     }
    
    
    IdentifierHash result;
    if (hcIndex <  m_hash_calcs.size()) {
        const HashCalc& hc = m_hash_calcs[hcIndex];
        result = hc.m_hash + 
            phi_module(straw_id)*hc.m_deltaPhi +
            (lw - hc.m_layerMin)*hc.m_nStrawLayers +
            (sl - hc.m_strLayerMin)*hc.m_nstraws +
            straw(straw_id);

//      if (debug) {
//          std::cout << " hash, phi_module(straw_id), hc.m_deltaPhi, hc.m_layerMin, hc.m_nStrawLayers, hc.m_strLayerMin, hc.m_nstraws " 
//                    <<  hc.m_hash << " " 
//                    <<  phi_module(straw_id) << " " << hc.m_deltaPhi << " " 
//                    <<  hc.m_layerMin  << " " <<  hc.m_nStrawLayers
//                    << " " <<  hc.m_strLayerMin  << " " <<  hc.m_nstraws << std::endl;
//      }
    
        return (result);
    }
    else {
        std::cout << "straw_hash: hc index > hcCalc  size - " << hcIndex 
                  << " " <<  m_hash_calcs.size() << std::endl;
    }
    return (result);
}


//----------------------------------------------------------------------------
inline     
IdentifierHash    
TRT_ID::straw_hash_bin   (Identifier straw_id) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    hash_vec_it it = std::lower_bound(m_straw_vec.begin(), 
                                      m_straw_vec.end(), 
                                      straw_id);

//     std::cout << "straw_hash_bin - " << show_to_string(straw_id) << " "
//            << (it - m_straw_vec.begin())
//            << std::endl;
    

    if (it != m_straw_vec.end()) {
        return (it - m_straw_vec.begin());
    }
    IdentifierHash result;
    return (result); // return hash in invalid state
}


//----------------------------------------------------------------------------
inline     
void
TRT_ID::invalidMessage() const
{
    MsgStream log(m_msgSvc, "TRT_ID");
    if(m_msgSvc) {
        log << MSG::ERROR << " TRT_ID is NOT valid for this layout. " << endmsg;
        log << MSG::ERROR << " Please use 'trt_id->is_valid()' if a layout test is needed. " << endmsg;
    }
    else {
        std::cout << " ERROR TRT_ID is NOT valid for this layout. " << std::endl;
        std::cout << " ERROR Please use 'trt_id->is_valid()' if a layout test is needed. " << std::endl;
    }
}



//----------------------------------------------------------------------------
inline bool     
TRT_ID::is_barrel       (const Identifier& id) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_barrel_field.match(m_bec_impl.unpack(id)));
}

//----------------------------------------------------------------------------
inline int 
TRT_ID::barrel_ec       (const Identifier& id)  const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_bec_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int 
TRT_ID::phi_module      (const Identifier& id) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_phi_mod_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int 
TRT_ID::layer_or_wheel  (const Identifier& id) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_lay_wheel_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int 
TRT_ID::straw_layer     (const Identifier& id) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_str_lay_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int 
TRT_ID::straw           (const Identifier& id)  const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_straw_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline TRT_ID::size_type        
TRT_ID::module_hash_max (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return m_module_hash_max;
}

//----------------------------------------------------------------------------
inline TRT_ID::size_type        
TRT_ID::straw_layer_hash_max (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return m_straw_layer_hash_max;
}

//----------------------------------------------------------------------------
inline TRT_ID::size_type        
TRT_ID::straw_hash_max (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return m_straw_hash_max;
}

//----------------------------------------------------------------------------
inline TRT_ID::const_id_iterator        TRT_ID::module_begin            (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_module_vec.begin());
}

//----------------------------------------------------------------------------
inline TRT_ID::const_id_iterator        TRT_ID::module_end              (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_module_vec.end());
}

//----------------------------------------------------------------------------
inline TRT_ID::const_id_iterator        TRT_ID::straw_layer_begin       (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_straw_layer_vec.begin());
}

//----------------------------------------------------------------------------
inline TRT_ID::const_id_iterator        TRT_ID::straw_layer_end         (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_straw_layer_vec.end());
}

//----------------------------------------------------------------------------
inline TRT_ID::const_expanded_id_iterator       TRT_ID::straw_begin     (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_full_straw_range.factory_begin());
}

//----------------------------------------------------------------------------
inline TRT_ID::const_expanded_id_iterator       TRT_ID::straw_end       (void) const
{
    // Check if TRT_ID is valid for this layout
    if (!m_is_valid) invalidMessage();

    return (m_full_straw_range.factory_end());
}

#endif // INDETIDENTIFIER_TRT_ID_H
