/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETIDENTIFIER_PIXELID_H
#define INDETIDENTIFIER_PIXELID_H
/**
 * @file PixelID.h
 *
 * @brief This is an Identifier helper class for the Pixel
 *  subdetector. This class is a factory for creating compact
 *  Identifier objects and IdentifierHash or hash ids. And it also
 *  allows decoding of these ids.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: PixelID.h,v 1.32.2.2 2009-03-30 00:48:48 jtseng Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"
#include "Identifier/Identifier32.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "Identifier/IdHelper.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <string>
#include <assert.h>
#include <algorithm>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IdDictDictionary;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 **  @class PixelID
 **  
 **  @brief This is an Identifier helper class for the Pixel
 **  subdetector. This class is a factory for creating compact
 **  Identifier objects and IdentifierHash or hash ids. And it also
 **  allows decoding of these ids.
 ** 
 **  Definition and the range of values for the levels of the
 **  identifier are:
 ** 
 ** @verbatim
 **    element           range    bits          meaning
 **    -------           -----    ----          -------
 ** 
 **    barrel_ec          0         2            barrel 
 **                    -4  / 4                     neg ec /  pos ec (-4 and +4 for DBM)
 **    layer_disk       0 to 2      2          for barrel
 **                     0 to 2      2          for ec
 **    phi_module       0 to <29    5   for barrel (depends upon layer)
 **                     0 to <72    7   for ec     (depends upon disk)
 **    eta_module       0 to 12     4   for barrel, along z
 **                     0 to 1      1   for ec, along r
 **    phi_index        0 to 327    9   for barrel, pixel coordinate along r/phi
 **                     0 to 192    8   for ec,      "
 **    eta_index        0 to 205    8   for barrel, pixel coordinate along z
 **                     0 to 164    8   for ec,       "       "        "   r
 ** @endverbatim
 ** 
 **/

class PixelID : public AtlasDetectorID
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
    PixelID(void);
    ~PixelID(void);
    //@}

    /// @name Creators for wafer ids and pixel ids
    //@{
    /// For a single crystal
    Identifier  wafer_id ( int barrel_ec,  
                           int layer_disk, 
                           int phi_module, 
                           int eta_module ) const;
    Identifier  wafer_id ( int barrel_ec,  
                           int layer_disk, 
                           int phi_module, 
                           int eta_module,
                           bool checks) const;

    /// For a single crystal from a pixel id - DO NOT USE wafer id as
    /// input
    Identifier  wafer_id ( const Identifier& pixel_id ) const;

    /// From hash
    Identifier  wafer_id ( IdentifierHash wafer_hash ) const;

    /// For an individual pixel
    Identifier  pixel_id ( int barrel_ec,  
                           int layer_disk, 
                           int phi_module, 
                           int eta_module, 
                           int phi_index,  
                           int eta_index) const;
    Identifier  pixel_id ( int barrel_ec,  
                           int layer_disk, 
                           int phi_module, 
                           int eta_module, 
                           int phi_index,  
                           int eta_index,
                           bool checks) const;
    Identifier  pixel_id ( const Identifier& id, 
                           int phi_index,   
                           int eta_index) const;
    //@}

    /// @name Hash table maximum sizes
    //@{
    size_type   wafer_hash_max          (void) const;
    size_type   pixel_hash_max          (void) const;
    //@}

    /// @name Access to all ids
    //@{
    /// Iterators over full set of ids. Wafer iterator is sorted
    const_id_iterator   wafer_begin     (void) const;
    const_id_iterator   wafer_end       (void) const;
    /// For pixel ids, only expanded id iterators are available. Use
    /// following "pixel_id" method to obtain a compact identifier
    const_expanded_id_iterator  pixel_begin     (void) const;  
    const_expanded_id_iterator  pixel_end       (void) const;
    //@}
    

    /// @name Optimized accessors - ASSUMES id IS a pixel id, i.e. NOT SCT or other
    //@{
    /// wafer hash from id 
    IdentifierHash      wafer_hash      (Identifier wafer_id) const;

    /// Test for barrel - WARNING: id MUST be pixel id, otherwise answer is not accurate. Use SiliconID for generic test.
    bool        is_barrel       (const Identifier& id) const;  

    /// Test for dbm - WARNING: id MUST be pixel id, otherwise answer is not accurate. Use SiliconID for generic test.
    bool        is_dbm       (const Identifier& id) const; //DBM

    /// Test for b-layer - WARNING: id MUST be pixel id, otherwise answer is not accurate. Use SiliconID for generic test.
    bool        is_blayer       (const Identifier& id) const;  

    /// Values of different levels (failure returns 0)
    int         barrel_ec       (const Identifier& id) const;  
    int         layer_disk      (const Identifier& id) const; 
    int         phi_module      (const Identifier& id) const; 
    int         eta_module      (const Identifier& id) const; 
    int         phi_index       (const Identifier& id) const;
    int         eta_index       (const Identifier& id) const;

    /// Max/Min values for each field (error returns -999)
    int         layer_disk_max  (const Identifier& id) const;
    int         phi_module_max  (const Identifier& id) const;
    int         eta_module_max  (const Identifier& id) const;
    int         eta_module_min  (const Identifier& id) const;
    int         phi_index_max   (const Identifier& id) const;
    int         eta_index_max   (const Identifier& id) const;
    //@}

    /// @name module eta/phi navigation
    //@{
    /// Previous wafer hash in phi (return == 0 for neighbor found)
    int         get_prev_in_phi (const IdentifierHash& id, IdentifierHash& prev) const;
    /// Next wafer hash in phi (return == 0 for neighbor found)
    int         get_next_in_phi (const IdentifierHash& id, IdentifierHash& next) const;
    /// Previous wafer hash in eta (return == 0 for neighbor found)
    int         get_prev_in_eta (const IdentifierHash& id, IdentifierHash& prev) const;
    /// Next wafer hash in eta (return == 0 for neighbor found)
    int         get_next_in_eta (const IdentifierHash& id, IdentifierHash& next) const;
    
    /// To check for when phi wrap around may be needed
    bool        is_phi_module_max(const Identifier& id) const;
    /// For the barrel
    bool        is_eta_module_min(const Identifier& id) const;
    /// For the barrel
    bool        is_eta_module_max(const Identifier& id) const;
    //@}


    /// @name contexts to distinguish wafer id from pixel id
    //@{
    IdContext   wafer_context           (void) const;
    IdContext   pixel_context           (void) const;
    //@}

    /// @name methods from abstract interface - slower than opt version
    //@{
    /// Create compact id from hash id (return == 0 for OK)
    virtual int         get_id          (const IdentifierHash& hash_id,
                                         Identifier& id,
                                         const IdContext* context = 0) const;
    
    /// Create hash id from compact id (return == 0 for OK)
    virtual int         get_hash        (const Identifier& id, 
                                         IdentifierHash& hash_id,
                                         const IdContext* context = 0) const;
    //@}

    /// Create a compact id from a value (e.g., from a persistent object).
    /// This repacks fields in case it's a special pixel channel id.
    Identifier          pixel_id        (Identifier::value_type val) const;

    /// Test if this is a valid shortened pixel channel id.
    bool is_shortened_pixel_id(Identifier32::value_type val) const;
    bool is_shortened_pixel_id(const Identifier32& id) const;
    bool is_shortened_pixel_id(const Identifier& id) const;

    /// Create a compact pixel id from a (fixed format) legacy
    /// pixel channel id.  If compiled in 32-bit mode, this method
    /// does nothing.
    Identifier pixel_id_from_shortened(Identifier32::value_type val) const;

    /// Return the lowest bit position used in the channel id.
    int                 base_bit        (void) const;

    /// Calculate a channel offset between the two identifiers.
    Identifier::diff_type calc_offset(const Identifier& base,
                                      const Identifier& target) const;

    /// Create an identifier with a given base and channel offset
    Identifier pixel_id_offset(const Identifier& base,
                               Identifier::diff_type offset) const;

    /// @name interaction with id dictionary
    //@{
    /// Create pixel Identifier from expanded id, which is returned by the
    /// id_iterators
    Identifier          pixel_id        (const ExpandedIdentifier& pixel_id) const;
    Identifier          pixel_id        (const ExpandedIdentifier& pixel_id, bool checks) const;

    /// Create expanded id from compact id (return == 0 for OK)
    void                get_expanded_id (const Identifier& id,
                                         ExpandedIdentifier& exp_id,
                                         const IdContext* context = 0) const;

    /// Initialization from the identifier dictionary
    virtual int         initialize_from_dictionary(const IdDictMgr& dict_mgr);

    /// Tests of packing
    void        test_wafer_packing      (void) const;
    //@}

private:

    enum {NOT_VALID_HASH        = 64000,
          MAX_BIT               = Identifier::MAX_BIT,
          BITS32                = Identifier::ALL_BITS };

    typedef std::vector<Identifier>     id_vec;
    typedef id_vec::const_iterator      id_vec_it;
    typedef std::vector<unsigned short> hash_vec;
    typedef hash_vec::const_iterator    hash_vec_it;

    void wafer_id_checks ( int barrel_ec,  
                           int layer_disk, 
                           int phi_module, 
                           int eta_module ) const;
    void pixel_id_checks ( int barrel_ec,  
                           int layer_disk, 
                           int phi_module, 
                           int eta_module, 
                           int phi_index,  
                           int eta_index) const;

    int         initLevelsFromDict(void);

    int         init_hashes(void);

    int         init_neighbors(void);

    size_type                   m_pixel_region_index;
    size_type                   m_INDET_INDEX;
    size_type                   m_PIXEL_INDEX;
    size_type                   m_BARREL_EC_INDEX;
    size_type                   m_LAYER_DISK_INDEX;
    size_type                   m_PHI_MODULE_INDEX;
    size_type                   m_ETA_MODULE_INDEX;
    size_type                   m_PHI_INDEX_INDEX;
    size_type                   m_ETA_INDEX_INDEX;
    int                         m_ETA_MODULE_OFFSET;
    Identifier                  m_pixel_id;
    const IdDictDictionary*     m_dict;
    MultiRange                  m_full_wafer_range;
    MultiRange                  m_full_pixel_range;
    size_type                   m_wafer_hash_max;
    size_type                   m_pixel_hash_max;
    Range::field                m_barrel_field;
    id_vec                      m_wafer_vec;
    hash_vec                    m_prev_phi_wafer_vec;
    hash_vec                    m_next_phi_wafer_vec;
    hash_vec                    m_prev_eta_wafer_vec;
    hash_vec                    m_next_eta_wafer_vec;

    IdDictFieldImplementation   m_indet_impl;
    IdDictFieldImplementation   m_pixel_impl;
    IdDictFieldImplementation   m_bec_impl;
    IdDictFieldImplementation   m_lay_disk_impl;
    IdDictFieldImplementation   m_phi_mod_impl;
    IdDictFieldImplementation   m_eta_mod_impl;
    IdDictFieldImplementation   m_bec_shift_impl;
    IdDictFieldImplementation   m_lay_disk_shift_impl;
    IdDictFieldImplementation   m_phi_mod_shift_impl;
    IdDictFieldImplementation   m_eta_mod_shift_impl;
    IdDictFieldImplementation   m_phi_index_impl;
    IdDictFieldImplementation   m_eta_index_impl;
    IdDictFieldImplementation   m_bec_eta_mod_impl; // bec to phi_module

    Range::field                m_dbm_field; //DBM

};
    


/////////////////////////////////////////////////////////////////////////////
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
/////////////////////////////////////////////////////////////////////////////

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF(PixelID, 2516, 1)

//----------------------------------------------------------------------------
inline Identifier
PixelID::wafer_id ( int barrel_ec, 
                    int layer_disk, 
                    int phi_module, 
                    int eta_module,
                    bool checks) const
{
    
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_indet_impl.pack    (indet_field_value(), result);
    m_pixel_impl.pack    (pixel_field_value(), result);
    m_bec_impl.pack      (barrel_ec,           result);
    m_lay_disk_impl.pack (layer_disk,          result);
    m_phi_mod_impl.pack  (phi_module,          result);
    m_eta_mod_impl.pack  (eta_module,          result);

    // Do checks
    if(checks) {
        wafer_id_checks ( barrel_ec, layer_disk, phi_module, eta_module );
    }

    return result;
}

inline Identifier
PixelID::wafer_id ( int barrel_ec, 
                    int layer_disk, 
                    int phi_module, 
                    int eta_module ) const
{
  return wafer_id (barrel_ec, layer_disk, phi_module, eta_module, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier  
PixelID::wafer_id ( const Identifier& pixel_id ) const
{
    Identifier result(pixel_id);
    m_phi_index_impl.reset      (result);
    m_eta_index_impl.reset      (result);
    return (result);
}

//----------------------------------------------------------------------------
inline Identifier  PixelID::wafer_id ( IdentifierHash wafer_hash ) const
{
    return (m_wafer_vec[wafer_hash]);
}

//----------------------------------------------------------------------------
inline IdentifierHash      PixelID::wafer_hash      (Identifier wafer_id) const 
{
    id_vec_it it = std::lower_bound(m_wafer_vec.begin(), 
                                    m_wafer_vec.end(), 
                                    wafer_id);
    // Require that wafer_id matches the one in vector
    if (it != m_wafer_vec.end() && wafer_id == (*it)) {
        return (it - m_wafer_vec.begin());
    }
    IdentifierHash result;
    return (result); // return hash in invalid state
}

//----------------------------------------------------------------------------
inline Identifier
PixelID::pixel_id ( int barrel_ec,  
                    int layer_disk, 
                    int phi_module, 
                    int eta_module,      
                    int phi_index,
                    int eta_index,
                    bool checks) const
{

    // Build identifier
    Identifier result((Identifier::value_type)0);
    m_indet_impl.pack          (indet_field_value(), result);
    m_pixel_impl.pack          (pixel_field_value(), result);
    m_bec_impl.pack            (barrel_ec,   result);
    m_lay_disk_impl.pack       (layer_disk,  result);
    m_phi_mod_impl.pack        (phi_module,  result);
    m_eta_mod_impl.pack        (eta_module,  result);
    m_phi_index_impl.pack      (phi_index,   result);
    m_eta_index_impl.pack      (eta_index,   result);

    if(checks) {

        pixel_id_checks ( barrel_ec, 
                          layer_disk, 
                          phi_module, 
                          eta_module, 
                          phi_index,    
                          eta_index);
    }
    
    return result;
}

inline Identifier
PixelID::pixel_id ( int barrel_ec,  
                    int layer_disk, 
                    int phi_module, 
                    int eta_module,      
                    int phi_index,
                    int eta_index) const
{
  return pixel_id (barrel_ec, layer_disk, phi_module, eta_module,
                   phi_index, eta_index, do_checks());
}

/// Create pixel Identifier from expanded id, which is returned by the
/// id_iterators
//----------------------------------------------------------------------------
inline Identifier               
PixelID::pixel_id       (const ExpandedIdentifier& id,
                         bool checks) const
{
    Identifier result;
    result = pixel_id(id[m_BARREL_EC_INDEX],
                      id[m_LAYER_DISK_INDEX],
                      id[m_PHI_MODULE_INDEX],
                      id[m_ETA_MODULE_INDEX],
                      id[m_PHI_INDEX_INDEX],
                      id[m_ETA_INDEX_INDEX]);

    if(checks) {
        pixel_id_checks (id[m_BARREL_EC_INDEX],
                         id[m_LAYER_DISK_INDEX],
                         id[m_PHI_MODULE_INDEX],
                         id[m_ETA_MODULE_INDEX],
                         id[m_PHI_INDEX_INDEX],
                         id[m_ETA_INDEX_INDEX]);
    }

    return (result);
}

inline Identifier               
PixelID::pixel_id       (const ExpandedIdentifier& id) const
{
  return pixel_id (id, do_checks());
}



//----------------------------------------------------------------------------
inline Identifier  
PixelID::pixel_id ( const Identifier& wafer_id, 
                    int phi_index,
                    int eta_index) const
{
    Identifier result(wafer_id);
    m_phi_index_impl.reset     (result);
    m_eta_index_impl.reset     (result);
    m_phi_index_impl.pack      (phi_index,   result);
    m_eta_index_impl.pack      (eta_index, result);
    return (result);
}

//----------------------------------------------------------------------------
inline Identifier
PixelID::pixel_id ( Identifier::value_type val ) const
{
    // a pixel channel id has bit 31 set, and low 8 bits > 0.
    // This could also have been shifted up by 32 bits.

    // first test to see if this is a 32-bit value promoted to 64-bit
    // (low word != 0, high word == 0).
    if ((val << 32) && !(val >> 32)) {
      // test to see if this is a genuine 64-bit pixel channel id;
      // these would not have the low 8-bits nonzero.
      // (This will break if pixel channel id's fill more than 56 bits!)
      Identifier32::value_type valshort = static_cast<Identifier32::value_type>(val);
      if (is_shortened_pixel_id(valshort)) {
         return pixel_id_from_shortened(valshort);
      }

    } else if (!(val << 32) && (val >> 32)) {
      // low word 0, high word != 0
      Identifier32::value_type valshort = static_cast<Identifier32::value_type>(val >> 32);
      if (is_shortened_pixel_id(valshort)) {
         return pixel_id_from_shortened(valshort);
      }
 
    }
    return Identifier(val); // genuine 64-bit pixel channel id

}

//----------------------------------------------------------------------------
inline bool
PixelID::is_shortened_pixel_id(Identifier32::value_type val) const
{
  return (val >> 31) && (val & 0xff);
}

//----------------------------------------------------------------------------
inline bool
PixelID::is_shortened_pixel_id(const Identifier32& id) const
{
  return is_shortened_pixel_id(id.get_compact());
}

//----------------------------------------------------------------------------
inline bool
PixelID::is_shortened_pixel_id(const Identifier& id) const
{
  Identifier::value_type val = id.get_compact();
  Identifier32::value_type valshort;
  // first test if value is contained in one half or the other
  if ((val << 32) && !(val >> 32)) {
    // low word contains value, high word empty
    valshort = static_cast<Identifier32::value_type>(val);
  } else if (!(val << 32) && (val >> 32)) {
    // high word contains value, low word empty
    valshort = static_cast<Identifier32::value_type>(val >> 32);
  } else {
    return false;
  }
  return is_shortened_pixel_id(valshort);
}

//----------------------------------------------------------------------------
inline Identifier
PixelID::pixel_id_from_shortened(Identifier32::value_type val) const
{
  int bec_val[4] = { -2, 0, 2, -999 }; // CHECK decodings
  int bec = bec_val[(val >> 29) & 0x03];
  if (bec < -2) return Identifier();

  return pixel_id( bec,
                   /* lay_disk */  ((val >> 27) & 0x03),
                   /* phi_mod */   ((val >> 21) & 0x3f),
                   /* eta_mod */   (((val >> 17) & 0x0f) + m_ETA_MODULE_OFFSET),
                   /* phi_index */ ((val >>  8) & 0x1ff),
                   /* eta_index */ ((val      ) & 0xff) - 1 );
}

//----------------------------------------------------------------------------
inline Identifier::diff_type
PixelID::calc_offset(const Identifier& base, const Identifier& target) const
{
  Identifier::diff_type tval = static_cast<Identifier::diff_type>(target.get_compact() >> base_bit());
  Identifier::diff_type bval = static_cast<Identifier::diff_type>(base.get_compact() >> base_bit());
  return (tval - bval);
}

//----------------------------------------------------------------------------
inline Identifier
PixelID::pixel_id_offset(const Identifier& base,
                         Identifier::diff_type offset) const
{
  Identifier::value_type bval = base.get_compact() >> base_bit();
  return Identifier((bval + offset) << base_bit());
}

//----------------------------------------------------------------------------
inline int
PixelID::base_bit ( void ) const
{
  int base = static_cast<int>(m_eta_index_impl.shift()); // lowest field base
  return (base > 32) ? 32 : base;
  // max base is 32 so we can still read old strip id's and differences
  // from non-SLHC releases.
}

//----------------------------------------------------------------------------
inline IdContext        
PixelID::wafer_context          (void) const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_ETA_MODULE_INDEX));
}

//----------------------------------------------------------------------------
inline IdContext        
PixelID::pixel_context  (void) const
{
  // For pixel only, the prefix is the first two levels
  ExpandedIdentifier id;
  id << indet_field_value() << pixel_field_value();
  return (IdContext(id, m_BARREL_EC_INDEX, m_ETA_INDEX_INDEX));
}

//----------------------------------------------------------------------------
inline bool     
PixelID::is_barrel      (const Identifier& id) const
{
    // Normal unshifted id
    return (m_barrel_field.match(m_bec_impl.unpack(id)));
}

//----------------------------------------------------------------------------
inline bool
PixelID::is_dbm (const Identifier& id) const
{
    // Normal unshifted id
    return (m_dbm_field.match(m_bec_impl.unpack(id)));
}

//----------------------------------------------------------------------------
inline int         
PixelID::barrel_ec       (const Identifier& id) const
{
    // Normal unshifted id
    return (m_bec_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int         
PixelID::layer_disk      (const Identifier& id) const
{
    // Normal unshifted id
    return (m_lay_disk_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline bool        
PixelID::is_blayer       (const Identifier& id) const 
{
    // Check if it is pixel id
    if(is_barrel(id)) {
        return (0 == layer_disk(id));
    }
    else {
        return (false);
    }
}

//----------------------------------------------------------------------------
inline int         
PixelID::phi_module      (const Identifier& id) const
{
    // Normal unshifted id
    return (m_phi_mod_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int         
PixelID::eta_module            (const Identifier& id) const
{
    // Normal unshifted id
    return (m_eta_mod_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int         
PixelID::phi_index            (const Identifier& id) const
{
    return (m_phi_index_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int         
PixelID::eta_index              (const Identifier& id) const
{
    return (m_eta_index_impl.unpack(id));
}

#endif // INDETIDENTIFIER_PIXELID_H
