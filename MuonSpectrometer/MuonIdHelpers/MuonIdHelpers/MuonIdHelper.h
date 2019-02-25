/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ******************************************************************************
// ATLAS Muon Identifier Helpers Package
// -----------------------------------------
// ******************************************************************************

//<doc><file> $Id: MuonIdHelper.h,v 1.31 2007-11-25 18:19:00 ketevi Exp $
//<version>   $Name: not supported by cvs2svn $

#ifndef DETECTORDESCRIPTION_MUONIDHELPER_H
#define DETECTORDESCRIPTION_MUONIDHELPER_H

// Includes

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "IdDict/IdDictDefs.h"
#include "Identifier/Range.h"
#include "Identifier/IdHelper.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <cassert>
#include <string>
#include <vector>
#include <set>
#include <stdexcept>

class IdDictDictionary;
class IMessageSvc;

// ******************************************************************************
// class MuonIdHelper
// ******************************************************************************
//
// Description
// -----------
// This is a base class for the Muon Identifier and Range factory classes.
// ATLAS note ATL-MUON-2001-014 provides a complete description of the
// hierarchical identifier scheme.  MuonIdHelper provides an interface to the
// common station "prefix" of the identifier, as described below.
//
// Field           Value               Notes
// ==============================================================================
// StationName     [0..n]              maps to "BIL", "EMS", "T1F", etc.
//                                     there can be holes in the list of indices
// StationEta      integer             increases with eta (0 at eta=0)
// StationPhi      non-zero integer    increases with phi
// Technology      [0..5]              maps to "MDT", "CSC", "RPC", "TGC", "STGC", "MM"
// ==============================================================================
//
// Inheritance
// -----------
// Inherits from DetectorDescription/AtlasDetectorID.
// It is inherited by the following technology-specific concrete classes.
//
//   MdtIdHelper factory of MDT-specific Identifiers and Ranges
//   CscIdHelper factory of CSC-specific Identifiers and Ranges
//   RpcIdHelper factory of RPC-specific Identifiers and Ranges
//   TgcIdHelper factory of TGC-specific Identifiers and Ranges

// for nSW
//   sTgcIdHelper factory of sTGC-specific Identifiers and Ranges
//   MmIdHelper factory of MicroMegas-specific Identifiers and Ranges

//
// Methods for External Use
// ------------------------
// int stationNameIndex(const std::string name)   (returns stationName index)
// int technologyIndex(const std::string name)    (returns technology index)
// std::string stationNameString(const int index) (returns stationName string)
// std::string technologyString(const int index)  (returns technology string)
//
// Author
// ------
// Steven Goldfarb <Steven.Goldfarb@cern.ch>
//
// Compact Id implementation by
// Ketevi A. Assamagan <ketevi@bnl.gov>
// CERN, February 20th, 2003
//

// for nSW
// Nektarios Chr. Benekos <nectarios.benekos@cern.ch>
// CERN, Feb. 2013
// ******************************************************************************

class MuonIdHelper : public AtlasDetectorID
{
 public:

  // Constructor

  MuonIdHelper();

  // Destructor

  virtual ~MuonIdHelper();

  // Build identifier

  Identifier muon() const;

  // Test for technology type

  bool is_muon(const Identifier& id) const;
  bool is_mdt (const Identifier& id) const;
  bool is_csc (const Identifier& id) const;
  bool is_rpc (const Identifier& id) const;
  bool is_tgc (const Identifier& id) const;

  // for nSW
  bool is_stgc (const Identifier& id) const;
  bool is_mm (const Identifier& id) const;

  // Extract station parts

  int stationName(const Identifier& id) const;
  int stationEta (const Identifier& id) const;
  int stationPhi (const Identifier& id) const;
  int technology (const Identifier& id) const;

  // Methods used by Moore

  int stationRegion(const Identifier& id) const;

  bool isBarrel    (const Identifier& id) const;
  bool isEndcap    (const Identifier& id) const;
  bool isForward   (const Identifier& id) const;
  bool isSmall     (const Identifier& id) const;
  bool isBarrel    (const int& stationNameIndex) const;
  bool isEndcap    (const int& stationNameIndex) const;
  bool isForward   (const int& stationNameIndex) const;
  bool isSmall     (const int& stationNameIndex) const;

  // Access to name and technology maps

  int stationNameIndex(const std::string& name) const;
  int technologyIndex(const std::string& name) const;
  const std::string& stationNameString(const int& index) const;
  const std::string& technologyString(const int& index) const;
  int nStationNames() const;

  // Print out contents of an id

  void print(const Identifier& id) const;

  ///////////// compact identifier stuff begins /////////////////////////////////

 public:

  typedef Identifier::size_type                    size_type;
  typedef MultiRange::const_identifier_factory     const_expanded_id_iterator;
  typedef std::vector<Identifier>::const_iterator  const_id_iterator;

  /// Initialization from the identifier dictionary
  virtual int initialize_from_dictionary(const IdDictMgr& dict_mgr);

  ///
  /// access to IdContext's which define which levels or fields are
  /// contained in the Muon id
  ///
  /// id for technology
  IdContext  technology_context          (void) const;
  /// id for module
  IdContext  module_context              (void) const;
  /// id for detector element
  IdContext  detectorElement_context     (void) const;
  /// id for channel
  IdContext  channel_context             (void) const;

  /// get the hashes 
  virtual int gasGap(const Identifier& id) const =0 ;
  virtual int channel(const Identifier& id) const =0;
  virtual bool measuresPhi(const Identifier& id) const =0 ;
  virtual int get_module_hash          (const Identifier& id, IdentifierHash& hash_id ) const;
  virtual int get_detectorElement_hash (const Identifier& id, IdentifierHash& hash_id ) const;
  virtual int get_channel_hash         (const Identifier& id, IdentifierHash& hash_id ) const;

  /// Create compact id from hash id (return == 0 for OK)
  virtual int get_id   (const IdentifierHash& hash_id,Identifier& id, const IdContext* context = 0) const;

  /// Create hash id from compact id (return == 0 for OK)
  virtual int get_hash (const Identifier& id, IdentifierHash& hash_id, const IdContext* context = 0) const;

  /// Create expanded id from compact id (return == 0 for OK)
  int get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;

  /// Create Identifier from expanded id, which is returned by the
  /// id_iterators
  int get_id (const ExpandedIdentifier& old_id, Identifier& new_id) const;
  /// multirange
  MultiRange multiRange (void) const;

  /// the maximum hash value
  size_type  module_hash_max          (void) const;
  size_type  detectorElement_hash_max (void) const { return m_detectorElement_hash_max; } ;
  size_type  channel_hash_max         (void) const;

  /// the id's
  std::vector<Identifier> idVector    (void) const;

  ///
  /// Iterators over full set of ids. Module iterator is sorted
  ///
  const_id_iterator   module_begin                     (void) const;
  const_id_iterator   module_end                       (void) const;

  ///
  /// Iterators over full set of ids. detector element iterator is sorted
  ///
  const_id_iterator   detectorElement_begin            (void) const;
  const_id_iterator   detectorElement_end              (void) const;

  ///
  /// Iterators over full set of ids. Channel iterator is sorted
  ///
  const_id_iterator   channel_begin                    (void) const;
  const_id_iterator   channel_end                      (void) const;

  /// Access to hashes for neighbors in phi and eta
  ///  (return == 0 for neighbor found)
  int   get_prev_in_phi (const IdentifierHash& id, IdentifierHash& prev) const;
  int   get_next_in_phi (const IdentifierHash& id, IdentifierHash& next) const;
  int   get_prev_in_eta (const IdentifierHash& id, IdentifierHash& prev) const;
  int   get_next_in_eta (const IdentifierHash& id, IdentifierHash& next) const;

  /// Tests of packing
  void   test_module_packing    (void) const;
  void   test_id                (const Identifier& id, const IdContext& context) const;

  int stationNameIndexMax() const;
  int technologyNameIndexMax() const;

  static const std::string BAD_NAME;

 protected:

  enum {NOT_VALID_HASH            = 64000};

  typedef std::vector<Identifier>     id_vec;
  typedef id_vec::const_iterator      id_vec_it;
  typedef std::vector<unsigned short> hash_vec;
  typedef hash_vec::const_iterator    hash_vec_it;

  int         initLevelsFromDict(void);
  int         init_hashes(void);
  virtual int init_detectorElement_hashes(void);
  int         init_channel_hashes(void);
  int         init_neighbors(void);

  // Create expanded id from compact id (return == 0 for OK)
  int     get_expanded_id_calc (const Identifier& compact_id, ExpandedIdentifier& id, const IdContext* context) const;

  // Create compact id from expanded or hash id (return == 0 for OK)
  int     get_compact_id       (const ExpandedIdentifier& id, Identifier& compact_id, const IdContext* context) const;
  // Create hash from compact
  virtual int  get_hash_calc   (const Identifier& compact_id, IdentifierHash& hash_id, const IdContext* context) const;

  size_type                  m_station_region_index;
  size_type                  m_MUON_INDEX;
  size_t                     m_GROUP_INDEX;
  size_type                  m_NAME_INDEX;
  size_type                  m_ETA_INDEX;
  size_type                  m_PHI_INDEX;
  size_type                  m_TECHNOLOGY_INDEX;
  size_type                  m_MODULE_INDEX;
  size_type                  m_DETECTORELEMENT_INDEX;
  size_type                  m_CHANNEL_INDEX;
  const IdDictDictionary*    m_dict;

  MultiRange    m_muon_range;
  MultiRange    m_full_module_range;
  size_type     m_module_hash_max;
  id_vec        m_module_vec;

  MultiRange    m_muon_channel_range;
  MultiRange    m_full_channel_range;
  size_type     m_channel_hash_max;
  id_vec        m_channel_vec;

  MultiRange    m_muon_detectorElement_range;
  MultiRange    m_full_detectorElement_range;
  size_type     m_detectorElement_hash_max;
  id_vec        m_detectorElement_vec;

  hash_vec      m_prev_phi_module_vec;
  hash_vec      m_next_phi_module_vec;
  hash_vec      m_prev_eta_module_vec;
  hash_vec      m_next_eta_module_vec;

  IdDictFieldImplementation   m_muon_impl;
  IdDictFieldImplementation   m_sta_impl;
  IdDictFieldImplementation   m_eta_impl;
  IdDictFieldImplementation   m_phi_impl;
  IdDictFieldImplementation   m_tec_impl;

  ///////////// compact identifier stuff ends//////////////////////////////////////

 protected:

  // Gaudi message service
  IMessageSvc* m_msgSvc;

  // Check values down to station level

  bool validStation(int stationName, int technology) const;

  // Append station ID fields

  void addStationID(Identifier& id, int stationName, int stationEta, int stationPhi, int technology) const ;

  void addStationID(ExpandedIdentifier& id, int stationName, int stationEta, int stationPhi, int technology) const;

 private:

  //Copy construcor - do not use!
  inline MuonIdHelper(const MuonIdHelper & other) : AtlasDetectorID(other) {}
  inline MuonIdHelper & operator=(const MuonIdHelper &right);

  // vectors for stationNames and technologies

  std::vector< std::string > m_stationNameVector;
  std::vector< std::string > m_technologyNameVector;

  // little helpers to speed things up
  std::set< int > m_isBarrel;
  std::set< int > m_isSmall;
  std::set< int > m_isForward;

  // Constants

  enum MuonIndices
  {
    StationNameIndex = 1,
    StationEtaIndex  = 2,
    StationPhiIndex  = 3,
    TechnologyIndex  = 4
  };

 protected:
  mutable MsgStream *m_Log;
  inline virtual void create_mlog() const;

};

// For backwards compatibility

typedef MuonIdHelper MuonID;

// Access to the ID

/*******************************************************************************/
inline Identifier MuonIdHelper::muon() const
{
  return AtlasDetectorID::muon();
}
/*******************************************************************************/
// Print method
inline void MuonIdHelper::print(const Identifier& id) const
{
  AtlasDetectorID::print(id);
}
/*******************************************************************************/
inline IdContext MuonIdHelper::technology_context (void) const {
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_TECHNOLOGY_INDEX));

}
/*******************************************************************************/
inline IdContext MuonIdHelper::module_context (void) const {
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_MODULE_INDEX));

}
/*******************************************************************************/
inline IdContext MuonIdHelper::detectorElement_context (void) const {
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_DETECTORELEMENT_INDEX));

}
/*******************************************************************************/
inline IdContext MuonIdHelper::channel_context (void) const {
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_CHANNEL_INDEX));

}
/*******************************************************************************/
inline MultiRange MuonIdHelper::multiRange(void) const {
  return m_full_module_range;
}
/*******************************************************************************/
inline MuonIdHelper::size_type MuonIdHelper::module_hash_max(void) const {
  return  m_module_hash_max;
}
/*******************************************************************************/
inline MuonIdHelper::size_type MuonIdHelper::channel_hash_max(void) const {
  return  m_channel_hash_max;
}
/*******************************************************************************/
inline std::vector<Identifier> MuonIdHelper::idVector(void) const {
  return m_module_vec;
}
/*******************************************************************************/
inline MuonIdHelper::const_id_iterator    MuonIdHelper::module_begin        (void) const
{
  return (m_module_vec.begin());
}
/*******************************************************************************/
inline MuonIdHelper::const_id_iterator    MuonIdHelper::module_end        (void) const
{
  return (m_module_vec.end());
}
/*******************************************************************************/
inline MuonIdHelper::const_id_iterator    MuonIdHelper::detectorElement_begin        (void) const
{
  return (m_detectorElement_vec.begin());
}
/*******************************************************************************/
inline MuonIdHelper::const_id_iterator    MuonIdHelper::detectorElement_end        (void) const
{
  return (m_detectorElement_vec.end());
}

inline MuonIdHelper::const_id_iterator    MuonIdHelper::channel_begin        (void) const
{
  return (m_channel_vec.begin());
}
/*******************************************************************************/
inline MuonIdHelper::const_id_iterator    MuonIdHelper::channel_end        (void) const
{
  return (m_channel_vec.end());
}
/*******************************************************************************/
// Check common station fields
inline bool MuonIdHelper::validStation(int stationName, int technology) const
{
  if ((stationName <  0) || (stationName > stationNameIndexMax())) return false;
  if ((technology  <  0) || (technology  > technologyNameIndexMax())) return false;
  return true;
}
/*******************************************************************************/
inline void  MuonIdHelper::addStationID(ExpandedIdentifier& id, int stationName, int stationEta, int stationPhi, int technology) const {

  id << stationName << stationEta << stationPhi << technology;
}
/*******************************************************************************/
// Check if ID for muon system
inline bool MuonIdHelper::is_muon(const Identifier& id) const
{
  return AtlasDetectorID::is_muon(id);
}
/*******************************************************************************/
// Check if ID for MDT
inline bool MuonIdHelper::is_mdt(const Identifier& id) const
{
  return AtlasDetectorID::is_mdt(id);
}
/*******************************************************************************/
// Check if ID for CSC
inline bool MuonIdHelper::is_csc(const Identifier& id) const
{
  return AtlasDetectorID::is_csc(id);
}
/*******************************************************************************/
// Check if ID for RPC
inline bool MuonIdHelper::is_rpc(const Identifier& id) const
{
  return AtlasDetectorID::is_rpc(id);
}
/*******************************************************************************/
// Check if ID for TGC
inline bool MuonIdHelper::is_tgc(const Identifier& id) const
{
  return AtlasDetectorID::is_tgc(id);
}
/*******************************************************************************/
// Check if ID for sTGC
inline bool MuonIdHelper::is_stgc(const Identifier& id) const
{
  return AtlasDetectorID::is_stgc(id);
}
/*******************************************************************************/
// Check if ID for MicroMegas
inline bool MuonIdHelper::is_mm(const Identifier& id) const
{
  return AtlasDetectorID::is_mm(id);
}
/*******************************************************************************/
// Access to components of the ID
inline int MuonIdHelper::stationName(const Identifier& id) const
{
  int result = m_sta_impl.unpack(id);
  return result;
}
/*******************************************************************************/
inline int MuonIdHelper::stationEta(const Identifier& id) const
{
  int result = m_eta_impl.unpack(id);
  return result;
}
/*******************************************************************************/
inline int MuonIdHelper::stationPhi(const Identifier& id) const
{
  int result = m_phi_impl.unpack(id);
  return result;
}
/*******************************************************************************/
inline int MuonIdHelper::technology(const Identifier& id) const
{
  int result = m_tec_impl.unpack(id);
  return result;
}
/*******************************************************************************/
inline int MuonIdHelper::stationNameIndexMax() const
{
  return (m_stationNameVector.size() - 1);
}
/*******************************************************************************/
inline int MuonIdHelper::technologyNameIndexMax() const
{
  return (m_technologyNameVector.size() - 1);
}
/*******************************************************************************/
// Methods used by Moore
inline bool MuonIdHelper::isBarrel(const Identifier& id) const
{
  return isBarrel(stationName(id));
}
/*******************************************************************************/
inline bool MuonIdHelper::isEndcap(const Identifier& id) const
{
  return isEndcap(stationName(id));
}
/*******************************************************************************/
inline bool MuonIdHelper::isForward(const Identifier& id) const
{
  return isForward(stationName(id));
}
/*******************************************************************************/
inline bool MuonIdHelper::isSmall(const Identifier& id) const
{
  return isSmall(stationName(id));
}
/*******************************************************************************/
inline bool MuonIdHelper::isBarrel(const int& stationNameIndex) const
{
  return (m_isBarrel.count(stationNameIndex) == 1);
}
/*******************************************************************************/
inline bool MuonIdHelper::isEndcap(const int& stationNameIndex) const
{
  return (m_isBarrel.count(stationNameIndex) == 0);
}
/*******************************************************************************/
inline bool MuonIdHelper::isForward(const int& stationNameIndex) const
{
  return (m_isForward.count(stationNameIndex) == 1);
}
/*******************************************************************************/
inline bool MuonIdHelper::isSmall(const int& stationNameIndex) const
{
  return (m_isSmall.count(stationNameIndex) == 1);
}
/*******************************************************************************/
// Access to name and technology maps
inline int MuonIdHelper::stationNameIndex(const std::string& name) const
{
// (*m_Log) << MSG::WARNING << "Bad habit: please avoid using string" << endmsg;
  int index = -1;
  std::vector< std::string >::const_iterator first = m_stationNameVector.begin();
  std::vector< std::string >::const_iterator last   = m_stationNameVector.end();
  int counter = 0;
  while (first != last) {
    if (name.compare(*first) == 0) {
      index = counter;
      break;
    }
    ++first;
    ++counter;
  }
  return index;
}
/*******************************************************************************/
inline int MuonIdHelper::technologyIndex(const std::string& name) const
{
// (*m_Log) << MSG::WARNING << "Bad habit: please avoid using string" << endmsg;
  int index = -1;
  std::vector< std::string >::const_iterator first = m_technologyNameVector.begin();
  std::vector< std::string >::const_iterator last  = m_technologyNameVector.end();
  int counter = 0;
  while (first != last) {
    if (name.compare(*first) == 0) {
      index = counter;
      break;
    }
    ++first;
    ++counter;
  }
  return index;
}
/*******************************************************************************/
inline const std::string& MuonIdHelper::stationNameString(const int& index) const
{
  assert ( index >= 0 && index <= stationNameIndexMax() );
  static std::string name;
  if ( index >= 0 && index <= stationNameIndexMax() ) {
    name = m_stationNameVector[index];
    if (name.empty()) {
      name = BAD_NAME;
    }
  } else {
    name = BAD_NAME;
  }
  return name;
}
/*******************************************************************************/
inline const std::string& MuonIdHelper::technologyString(const int& index) const
{
  assert ( index >= 0 && index <= technologyNameIndexMax() );
  static std::string name;
  if (index >= 0 && index <= technologyNameIndexMax()) {
    name = m_technologyNameVector[index];
    if (name.empty()) {
      name = BAD_NAME;
    }
  } else {
    name = BAD_NAME;
  }
  return name;
}
/*******************************************************************************/
inline int MuonIdHelper::nStationNames() const
{
  return (int)m_isSmall.size();
}
/*******************************************************************************/
#endif // DETECTORDESCRIPTION_MUONIDHELPER_H

