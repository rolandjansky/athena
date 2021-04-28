/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef INDETIDENTIFIER_SCT_ID_H
#define INDETIDENTIFIER_SCT_ID_H
/**
 * @file SCT_ID.h
 *
 * @brief This is an Identifier helper class for the SCT
 *  subdetector. This class is a factory for creating compact
 *  Identifier objects and IdentifierHash or hash ids. And it also
 *  allows decoding of these ids.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 */


#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "Identifier/IdHelper.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <string>
#include <cassert>
#include <algorithm>
#include <array>
#include <functional>


class IdDictDictionary;


/**
**  @class SCT_ID
**
**  @brief This is an Identifier helper class for the SCT
**  subdetector. This class is a factory for creating compact
**  Identifier objects and IdentifierHash or hash ids. And it also
**  allows decoding of these ids.
**
**  Definition and the range of values for the levels of the
**  identifier are:
**
** @verbatim
**    element           range              meaning
**    -------           -----              -------
**
**    barrel_ec          0                 barrel
**                    -2  / 2         neg ec /  pos ec
**    layer_disk       0 to 3            for barrel
**                     0 to 8            for ec
**    phi_module       0 to <56   for barrel (depends upon layer)
**                     0 to <52   for ec     (depends upon wheel)
**    eta_module      -6 to -1    for neg barrel, along z
**                     1 to 6     for pos barrel, along z
**                     0 to 2     for ec, along r
**    side             0 to 1     inner/outer of pairs of Si crystals
**    strip            0 to 767   strip number
** @endverbatim
**
*/
class SCT_ID: public AtlasDetectorID
{
public:
  /// @name public typedefs
  //@{
  typedef Identifier::size_type size_type;
  typedef std::vector<Identifier>::const_iterator const_id_iterator;
  typedef MultiRange::const_identifier_factory const_expanded_id_iterator;
  //@}

  /// @name structors
  //@{
  SCT_ID(void);
  virtual ~SCT_ID(void) = default;
  //@}

  /// @name Creators for wafer ids and pixel ids
  //@{
  /// For a single crystal
  Identifier module_id(int barrel_ec,
                       int layer_disk,
                       int phi_module,
                       int eta_module) const;
                       
  Identifier module_id(int barrel_ec,
                       int layer_disk,
                       int phi_module,
                       int eta_module,
                       bool checks) const;

  /// For a module from a wafer id
  Identifier module_id(const Identifier& wafer_id) const;

  /// For a single side of module
  Identifier wafer_id(int barrel_ec,
                      int layer_disk,
                      int phi_module,
                      int eta_module,
                      int side) const;
                      
  Identifier wafer_id(int barrel_ec,
                      int layer_disk,
                      int phi_module,
                      int eta_module,
                      int side,
                      bool checks) const;

  /// For a single side of module from a strip id
  Identifier wafer_id(const Identifier& strip_id) const;
  
  /// For a side of module from ExpandedId
  Identifier wafer_id(const ExpandedIdentifier & expId) const;

  /// From hash - optimized
  Identifier wafer_id(const IdentifierHash& wafer_hash) const;

  /// For an individual strip
  Identifier strip_id(int barrel_ec,
                      int layer_disk,
                      int phi_module,
                      int eta_module,
                      int side,
                      int strip) const;
  Identifier strip_id(int barrel_ec,
                      int layer_disk,
                      int phi_module,
                      int eta_module,
                      int side,
                      int strip,
                      bool check) const;

  Identifier strip_id(int barrel_ec,
                      int layer_disk,
                      int phi_module,
                      int eta_module,
                      int side,
                      int row,
                      int strip) const;
  Identifier strip_id(int barrel_ec,
                      int layer_disk,
                      int phi_module,
                      int eta_module,
                      int side,
                      int row,
                      int strip,
                      bool checks) const;



  Identifier strip_id(const Identifier& wafer_id,
                      int strip) const;

  Identifier strip_id(const Identifier& wafer_id,
                      int row, int strip) const;


  //@}


  /// @name Hash table maximum sizes
  //@{
  size_type wafer_hash_max(void) const;
  size_type strip_hash_max(void) const;
  //@}

  /// @name Access to all ids
  //@{
  /// Iterators over full set of ids. Wafer iterator is sorted
  const_id_iterator wafer_begin(void) const;
  const_id_iterator wafer_end(void) const;
  /// For strip ids, only expanded id iterators are available. Use
  /// following "strip_id" method to obtain a compact identifier
  const_expanded_id_iterator strip_begin(void) const;
  const_expanded_id_iterator strip_end(void) const;
  //@}


  /// @name Optimized accessors  - ASSUMES id IS a sct id, i.e. NOT pixel or other
  //@{
  /// wafer hash from id - optimized
  IdentifierHash wafer_hash(const Identifier& wafer_id) const;

  /// Test for barrel - WARNING: id MUST be sct id, otherwise answer is not accurate. Use SiliconID for generic test.
  bool is_barrel(const Identifier& id) const;

  /// Values of different levels (failure returns 0)
  int barrel_ec(const Identifier& id) const;
  int layer_disk(const Identifier& id) const;
  int phi_module(const Identifier& id) const;
  int eta_module(const Identifier& id) const;
  int side(const Identifier& id) const;
  int row(const Identifier& id) const;
  int strip(const Identifier& id) const;

  /// Max/Min values for each field (-999 == failure)
  int layer_disk_max(const Identifier& id) const;
  int phi_module_max(const Identifier& id) const;
  int eta_module_max(const Identifier& id) const;
  int eta_module_min(const Identifier& id) const;
  int strip_max(const Identifier& id) const;
  //@}

  /// @name module eta/phi navigation
  //@{
  /// Previous wafer hash in phi (return == 0 for neighbor found)
  int get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const;
  /// Next wafer hash in phi (return == 0 for neighbor found)
  int get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const;
  /// Previous wafer hash in eta (return == 0 for neighbor found)
  int get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const;
  /// Next wafer hash in eta (return == 0 for neighbor found)
  int get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const;
  /// Wafer hash on other side
  int get_other_side(const IdentifierHash& id, IdentifierHash& other) const;
  
  ///The same, but without reference argument for returning value_type;
  ///failure is indicated by returning an invalid identifierHash
  IdentifierHash get_prev_in_phi(const IdentifierHash& id) const;
  /// Next wafer hash in phi (return == 0 for neighbor found)
  IdentifierHash get_next_in_phi(const IdentifierHash& id) const;
  /// Previous wafer hash in eta (return == 0 for neighbor found)
  IdentifierHash get_prev_in_eta(const IdentifierHash& id) const;
  /// Next wafer hash in eta (return == 0 for neighbor found)
  IdentifierHash get_next_in_eta(const IdentifierHash& id) const;
  /// Wafer hash on other side
  IdentifierHash get_other_side(const IdentifierHash& id) const;

  // To check for when phi wrap around may be needed, use
  bool is_phi_module_max(const Identifier& id) const;
  /// For the barrel
  bool is_eta_module_min(const Identifier& id) const;
  /// For the barrel
  bool is_eta_module_max(const Identifier& id) const;
  
  //all neighbours: opposite and then eta direction first
  std::array<IdentifierHash, 5>
  neighbours_by_eta(const IdentifierHash & idh) const;
  
  //all neighbours: opposite and then phi direction first
  std::array<IdentifierHash, 5>
  neighbours_by_phi(const IdentifierHash & idh) const;
  
  /// return functions to give neighbours in order: opposite, eta minus, eta plus,
  /// phi minus, phi plus : in case you dont want to create temporary array of IdentifierHashes
  /// (to be investigated in context of SP formation)
  std::array<std::function< IdentifierHash(const IdentifierHash &)>, 5 >
  neighbour_calls_by_eta() const;
  //@}

  /// @name contexts to distinguish wafer id from pixel id
  //@{
  IdContext wafer_context(void) const;
  IdContext strip_context(void) const;
  //@}

  /// @name methods from abstract interface - slower than opt version
  //@{
  /// Create compact id from hash id (return == 0 for OK)
  virtual int get_id(const IdentifierHash& hash_id,
                     Identifier& id,
                     const IdContext* context = 0) const;

  /// Create hash id from compact id (return == 0 for OK)
  virtual int get_hash(const Identifier& id,
                       IdentifierHash& hash_id,
                       const IdContext* context = 0) const;
  //@}

  /// Return the lowest bit position used in the channel id
  int base_bit(void) const;

  /// Calculate a channel offset between the two identifiers.
  Identifier::diff_type calc_offset(const Identifier& base,
                                    const Identifier& target) const;

  /// Create an identifier with a given base and channel offset
  Identifier strip_id_offset(const Identifier& base,
                             Identifier::diff_type offset) const;

  /// @name interaction with id dictionary
  //@{
  /// Create strip Identifier from expanded id, which is returned by the
  /// id_iterators
  Identifier strip_id(const ExpandedIdentifier& strip_id) const;

  /// Create expanded id from compact id (return == 0 for OK)
  void get_expanded_id(const Identifier& id,
                       ExpandedIdentifier& exp_id,
                       const IdContext* context = 0) const;

  /// Initialization from the identifier dictionary
  virtual int initialize_from_dictionary(const IdDictMgr& dict_mgr);

  //@}
private:
  typedef std::vector<Identifier>     id_vec;
  typedef id_vec::const_iterator id_vec_it;
  typedef std::vector<IdentifierHash> hash_vec;
  typedef hash_vec::const_iterator hash_vec_it;
  enum ExpandedIdIndices {INDET, SCT, BARREL_EC, LAYER_DISK, PHI, ETA, SIDE, STRIP,ROW, NUM_INDICES};
  std::array<std::function< IdentifierHash(const IdentifierHash & )>, 5> m_neighboursByEta;
  
  //this is a bit clumsy, but it reproduces the original messaging behaviour with/without Gaudi
  //it *SHOULD NOT* be used for messaging in event loop code, as it is expensive!
  void 
  localMessage(const std::string & msgTxt, const std::string &func, const MSG::Level & lvl) const;
  
  void wafer_id_checks(int barrel_ec,
                       int layer_disk,
                       int phi_module,
                       int eta_module,
                       int side) const;

  void strip_id_checks(int barrel_ec,
                       int layer_disk,
                       int phi_module,
                       int eta_module,
                       int side,
                       int strip) const;

  void strip_id_checks(int barrel_ec,
                       int layer_disk,
                       int phi_module,
                       int eta_module,
                       int side,
                       int row,
                       int strip) const;
                       
  int getMaxField(const Identifier & id, const ExpandedIdIndices &fieldIndx) const;
                       
  void strip_id_checks(const ExpandedIdentifier & expId) const;


  int initLevelsFromDict(void);

  int init_hashes(void);

  int init_neighbors(void);

  IdentifierHash
  nextInSequence(const IdentifierHash& id, const hash_vec& vectorOfHashes) const;

  // Temporary method for adapting an identifier for the MultiRange
  // check - MR is missing the InnerDetector level
  Identifier idForCheck(const Identifier& id) const;

  size_type m_sct_region_index{0};
 
  std::array<size_t, NUM_INDICES> m_indices{0,1,2,3,4,5,6,7,999};
  const IdDictDictionary* m_dict;
  MultiRange m_full_wafer_range;
  MultiRange m_full_strip_range;
  size_type m_wafer_hash_max;
  size_type m_strip_hash_max;
  Range::field m_barrel_field;
  id_vec m_wafer_vec;
  hash_vec m_prev_phi_wafer_vec;
  hash_vec m_next_phi_wafer_vec;
  hash_vec m_prev_eta_wafer_vec;
  hash_vec m_next_eta_wafer_vec;
  bool m_hasRows;

  IdDictFieldImplementation m_indet_impl;
  IdDictFieldImplementation m_sct_impl;
  IdDictFieldImplementation m_bec_impl;
  IdDictFieldImplementation m_lay_disk_impl;
  IdDictFieldImplementation m_phi_mod_impl;
  IdDictFieldImplementation m_eta_mod_impl;
  IdDictFieldImplementation m_side_impl;
  IdDictFieldImplementation m_row_impl;
  IdDictFieldImplementation m_strip_impl;
};


//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF(SCT_ID, 2517, 1)

//----------------------------------------------------------------------------
inline Identifier
SCT_ID::module_id(int barrel_ec,
                  int layer_disk,
                  int phi_module,
                  int eta_module,
                  bool checks) const {
  // Build identifier
  Identifier result((Identifier::value_type) 0);

  // Pack fields independently
  m_indet_impl.pack(indet_field_value(), result);
  m_sct_impl.pack(sct_field_value(), result);
  m_bec_impl.pack(barrel_ec, result);
  m_lay_disk_impl.pack(layer_disk, result);
  m_phi_mod_impl.pack(phi_module, result);
  m_eta_mod_impl.pack(eta_module, result);

  // Do checks
  if (checks) {
    wafer_id_checks(barrel_ec, layer_disk, phi_module, eta_module, 0);
  }

  return result;
}

inline Identifier
SCT_ID::module_id(int barrel_ec,
                  int layer_disk,
                  int phi_module,
                  int eta_module) const {
  return module_id(barrel_ec, layer_disk, phi_module, eta_module, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier
SCT_ID::module_id(const Identifier& wafer_id) const {
  Identifier result(wafer_id);

  //  Reset the side and strip fields
  m_side_impl.reset(result);
  m_strip_impl.reset(result);
  if(m_hasRows) m_row_impl.reset(result);
  return(result);
}

//----------------------------------------------------------------------------
inline Identifier
SCT_ID::wafer_id(int barrel_ec,
                 int layer_disk,
                 int phi_module,
                 int eta_module,
                 int side,
                 bool checks) const {
  // Build identifier
  Identifier result((Identifier::value_type) 0);

  // Pack fields independently
  m_indet_impl.pack(indet_field_value(), result);
  m_sct_impl.pack(sct_field_value(), result);
  m_bec_impl.pack(barrel_ec, result);
  m_lay_disk_impl.pack(layer_disk, result);
  m_phi_mod_impl.pack(phi_module, result);
  m_eta_mod_impl.pack(eta_module, result);
  m_side_impl.pack(side, result);

  // Do checks
  if (checks) {
    wafer_id_checks(barrel_ec, layer_disk, phi_module, eta_module, side);
  }

  return result;
}

inline Identifier
SCT_ID::wafer_id(int barrel_ec,
                 int layer_disk,
                 int phi_module,
                 int eta_module,
                 int side) const {
  return wafer_id(barrel_ec, layer_disk, phi_module, eta_module, side,
                  do_checks());
}

//----------------------------------------------------------------------------
inline Identifier
SCT_ID::wafer_id(const Identifier& strip_id) const {
  Identifier result(strip_id);

  // reset the strip field
  m_strip_impl.reset(result);
  if(m_hasRows) m_row_impl.reset(result);
  return(result);
}

//----------------------------------------------------------------------------
inline Identifier
SCT_ID::wafer_id(const IdentifierHash& wafer_hash) const {
  return(m_wafer_vec[wafer_hash]);
}

//----------------------------------------------------------------------------
inline IdentifierHash
SCT_ID::wafer_hash(const Identifier& wafer_id) const {
  id_vec_it it = std::lower_bound(m_wafer_vec.begin(),
                                  m_wafer_vec.end(),
                                  wafer_id);

  // Require that wafer_id matches the one in vector
  if (it != m_wafer_vec.end() && wafer_id == (*it)) {
    return(it - m_wafer_vec.begin());
  }
  IdentifierHash result;
  return(result); // return hash in invalid state
}

//----------------------------------------------------------------------------
inline Identifier
SCT_ID::strip_id(int barrel_ec,
                 int layer_disk,
                 int phi_module,
                 int eta_module,
                 int side,
                 int strip,
                 bool checks) const {
  // Build identifier
  Identifier result((Identifier::value_type) 0);

  // Pack fields independently
  m_indet_impl.pack(indet_field_value(), result);
  m_sct_impl.pack(sct_field_value(), result);
  m_bec_impl.pack(barrel_ec, result);
  m_lay_disk_impl.pack(layer_disk, result);
  m_phi_mod_impl.pack(phi_module, result);
  m_eta_mod_impl.pack(eta_module, result);
  m_side_impl.pack(side, result);
  m_strip_impl.pack(strip, result);

  // Do checks
  if (checks) {
    strip_id_checks(barrel_ec, layer_disk, phi_module, eta_module, side, strip);
  }
  return result;
}

inline Identifier
SCT_ID::strip_id(int barrel_ec,
                 int layer_disk,
                 int phi_module,
                 int eta_module,
                 int side,
                 int strip) const {
  return strip_id(barrel_ec, layer_disk, phi_module, eta_module, side, strip,
                  do_checks());
}

//----------------------------------------------------------------------------

inline Identifier
SCT_ID::strip_id(int barrel_ec,
                 int layer_disk,
                 int phi_module,
                 int eta_module,
                 int side,
                 int row,
                 int strip,
                 bool checks) const {
  // Build identifier
  Identifier result((Identifier::value_type) 0);


  // Pack fields independently
  m_indet_impl.pack(indet_field_value(), result);
  m_sct_impl.pack(sct_field_value(), result);
  m_bec_impl.pack(barrel_ec, result);
  m_lay_disk_impl.pack(layer_disk, result);
  m_phi_mod_impl.pack(phi_module, result);
  m_eta_mod_impl.pack(eta_module, result);
  m_side_impl.pack(side, result);
  if (m_hasRows) {
    m_row_impl.pack(row, result);
  }
  m_strip_impl.pack(strip, result);


  // Do checks
  if (checks) {
    if (m_hasRows) strip_id_checks(barrel_ec, layer_disk, phi_module, eta_module, side, row, strip);
    else strip_id_checks(barrel_ec, layer_disk, phi_module, eta_module, side, strip);
  }

  return result;
}

inline Identifier
SCT_ID::strip_id(int barrel_ec,
                 int layer_disk,
                 int phi_module,
                 int eta_module,
                 int side,
                 int row,
                 int strip) const {
  return strip_id(barrel_ec, layer_disk, phi_module, eta_module,
                  side, row, strip, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier
SCT_ID::strip_id(const ExpandedIdentifier& id) const {
  // Build identifier
  Identifier result((Identifier::value_type) 0);

  // Pack fields independently
  m_indet_impl.pack(indet_field_value(), result);
  m_sct_impl.pack(sct_field_value(), result);
  m_bec_impl.pack(id[m_indices[BARREL_EC]], result);
  m_lay_disk_impl.pack(id[m_indices[LAYER_DISK]], result);
  m_phi_mod_impl.pack(id[m_indices[PHI]], result);
  m_eta_mod_impl.pack(id[m_indices[ETA]], result);
  m_side_impl.pack(id[m_indices[SIDE]], result);
  if (m_hasRows) {
    m_row_impl.pack(id[m_indices[ROW]], result);
  }
  m_strip_impl.pack(id[m_indices[STRIP]], result);

  // Do checks
  if (m_do_checks) {
    strip_id_checks(id);
  }
  return result;
}

//----------------------------------------------------------------------------
inline Identifier
SCT_ID::strip_id(const Identifier& wafer_id, int strip) const {
  // Build identifier
  Identifier result(wafer_id);

  // Reset strip and then add in value
  m_strip_impl.reset(result);
  m_strip_impl.pack(strip, result);

  if (m_do_checks) {
    strip_id_checks(barrel_ec(result),
                    layer_disk(result),
                    phi_module(result),
                    eta_module(result),
                    side(result),
                    strip);
  }
  return result;
}

//----------------------------------------------------------------------------
inline Identifier
SCT_ID::strip_id(const Identifier& wafer_id, int row, int strip) const {
  // Build identifier
  Identifier result(wafer_id);

  // Reset strip and then add in value
  if (m_hasRows) {
    m_row_impl.reset(result);
    m_row_impl.pack(row, result);
  }
  m_strip_impl.reset(result);
  m_strip_impl.pack(strip, result);

  if (m_do_checks) {
    if (m_hasRows) {
      strip_id_checks(barrel_ec(result),
                      layer_disk(result),
                      phi_module(result),
                      eta_module(result),
                      side(result),
                      row,
                      strip);
    } else {
      strip_id_checks(barrel_ec(result),
                      layer_disk(result),
                      phi_module(result),
                      eta_module(result),
                      side(result),
                      strip);
    }
  }
  return result;
}

//----------------------------------------------------------------------------
inline Identifier::diff_type
SCT_ID::calc_offset(const Identifier& base, const Identifier& target) const {
  Identifier::diff_type tval = static_cast<Identifier::diff_type>(target.get_compact() >> base_bit());
  Identifier::diff_type bval = static_cast<Identifier::diff_type>(base.get_compact() >> base_bit());
  return(tval - bval);
}

//----------------------------------------------------------------------------
inline Identifier
SCT_ID::strip_id_offset(const Identifier& base,
                        Identifier::diff_type offset) const {
  Identifier::value_type bval = base.get_compact() >> base_bit();
  return Identifier((bval + offset) << base_bit());
}

//----------------------------------------------------------------------------
inline int
SCT_ID::base_bit(void) const {
  int base = static_cast<int>(m_strip_impl.shift()); // lowest field base

  return (base > 32) ? 32 : base;
  // max base is 32 so we can still read old strip id's and differences
  // from non-SLHC releases.
}

//----------------------------------------------------------------------------
inline IdContext
SCT_ID::wafer_context(void) const {
  ExpandedIdentifier id;

  return(IdContext(id, 0, m_indices[SIDE]));
}

//----------------------------------------------------------------------------
inline IdContext
SCT_ID::strip_context(void) const {
  ExpandedIdentifier id;

  return(IdContext(id, 0, m_indices[STRIP]));
}

//----------------------------------------------------------------------------
inline bool
SCT_ID::is_barrel(const Identifier& id) const {
  // match bec field
  return(m_barrel_field.match(m_bec_impl.unpack(id)));
}

//----------------------------------------------------------------------------
inline int
SCT_ID::barrel_ec(const Identifier& id) const {
  return(m_bec_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int
SCT_ID::layer_disk(const Identifier& id) const {
  return(m_lay_disk_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int
SCT_ID::phi_module(const Identifier& id) const {
  return(m_phi_mod_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int
SCT_ID::eta_module(const Identifier& id) const {
  return(m_eta_mod_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int
SCT_ID::side(const Identifier& id) const {
  return(m_side_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int
SCT_ID::row(const Identifier& id) const {
  return(m_row_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int
SCT_ID::strip(const Identifier& id) const {
  return(m_strip_impl.unpack(id));
}

inline IdentifierHash
SCT_ID::nextInSequence(const IdentifierHash& id, const hash_vec& vectorOfHashes) const {
  unsigned short index = id;
  if (index < vectorOfHashes.size()) {
    return vectorOfHashes[index];
  }
  return IdentifierHash{};
}

#endif // INDETIDENTIFIER_SCT_ID_H
