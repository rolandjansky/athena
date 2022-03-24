/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETECTORDESCRIPTION_MUONIDHELPER_H
#define DETECTORDESCRIPTION_MUONIDHELPER_H

#include <cassert>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "AthenaKernel/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDict/IdDictDefs.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "Identifier/IdHelper.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"

class IdDictDictionary;

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

class MuonIdHelper : public AtlasDetectorID {
public:
    // Constructor

    MuonIdHelper(const std::string& logName);

    // Destructor

    virtual ~MuonIdHelper() = default;

    // Build identifier

    Identifier muon() const;

    // Test for technology type

    bool is_muon(const Identifier& id) const;
    bool is_mdt(const Identifier& id) const;
    bool is_csc(const Identifier& id) const;
    bool is_rpc(const Identifier& id) const;
    bool is_tgc(const Identifier& id) const;

    // for nSW
    bool is_stgc(const Identifier& id) const;
    bool is_mm(const Identifier& id) const;

    // Extract station parts

    int stationName(const Identifier& id) const;
    int stationEta(const Identifier& id) const;
    int stationPhi(const Identifier& id) const;
    int technology(const Identifier& id) const;

    // Methods used by Moore

    int stationRegion(const Identifier& id) const;

    bool isBarrel(const Identifier& id) const;
    bool isEndcap(const Identifier& id) const;
    bool isForward(const Identifier& id) const;
    bool isSmall(const Identifier& id) const;
    bool isBarrel(const int& stationNameIndex) const;
    bool isEndcap(const int& stationNameIndex) const;
    bool isForward(const int& stationNameIndex) const;
    bool isSmall(const int& stationNameIndex) const;

    // Access to name and technology maps

    int stationNameIndex(const std::string& name) const;
    int technologyIndex(const std::string& name) const;
    const std::string& stationNameString(const int& index) const;
    const std::string& technologyString(const int& index) const;
    int nStationNames() const;

    // Print out contents of an id

    void print(const Identifier& id) const;

    // Check whether helper is fully initialized
    bool isInitialized() const;

    ///////////// compact identifier stuff begins /////////////////////////////////

public:
    typedef Identifier::size_type size_type;
    typedef MultiRange::const_identifier_factory const_expanded_id_iterator;
    typedef std::vector<Identifier>::const_iterator const_id_iterator;

    /// Initialization from the identifier dictionary
    virtual int initialize_from_dictionary(const IdDictMgr& dict_mgr) override;

    ///
    /// access to IdContext's which define which levels or fields are
    /// contained in the Muon id
    ///
    /// id for technology
    IdContext technology_context() const;
    /// id for module
    IdContext module_context() const;
    /// id for detector element
    IdContext detectorElement_context() const;
    /// id for channel
    IdContext channel_context() const;

    /// get the hashes
    virtual int gasGap(const Identifier& id) const = 0;
    virtual int channel(const Identifier& id) const = 0;
    virtual bool measuresPhi(const Identifier& id) const = 0;
    virtual int get_module_hash(const Identifier& id, IdentifierHash& hash_id) const;
    virtual int get_detectorElement_hash(const Identifier& id, IdentifierHash& hash_id) const;
    virtual int get_channel_hash(const Identifier& id, IdentifierHash& hash_id) const;

    /// Create compact id from hash id (return == 0 for OK)
    virtual int get_id(const IdentifierHash& hash_id, Identifier& id, const IdContext* context = 0) const override;

    /// Create hash id from compact id (return == 0 for OK)
    virtual int get_hash(const Identifier& id, IdentifierHash& hash_id, const IdContext* context = 0) const override;

    /// Create expanded id from compact id (return == 0 for OK)
    int get_expanded_id(const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;

    /// Create Identifier from expanded id, which is returned by the
    /// id_iterators
    int get_id(const ExpandedIdentifier& old_id, Identifier& new_id) const;
    /// multirange
    MultiRange multiRange() const;

    /// the maximum hash value
    size_type module_hash_max() const;
    size_type detectorElement_hash_max() const { return m_detectorElement_hash_max; };
    size_type channel_hash_max() const;

    /// the id's
    std::vector<Identifier> idVector() const;

    ///
    /// Iterators over full set of ids. Module iterator is sorted
    ///
    const_id_iterator module_begin() const;
    const_id_iterator module_end() const;

    ///
    /// Iterators over full set of ids. detector element iterator is sorted
    ///
    const_id_iterator detectorElement_begin() const;
    const_id_iterator detectorElement_end() const;

    ///
    /// Iterators over full set of ids. Channel iterator is sorted
    ///
    const_id_iterator channel_begin() const;
    const_id_iterator channel_end() const;

    /// Access to hashes for neighbors in phi and eta
    ///  (return == 0 for neighbor found)
    int get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const;
    int get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const;
    int get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const;
    int get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const;

    /// Tests of packing
    void test_module_packing() const;
    void test_id(const Identifier& id, const IdContext& context) const;

    int stationNameIndexMax() const;
    int technologyNameIndexMax() const;

    static const std::string BAD_NAME;

protected:
    static constexpr int NOT_VALID_HASH = 64000;

    typedef std::vector<Identifier> id_vec;
    typedef id_vec::const_iterator id_vec_it;
    typedef std::vector<unsigned short> hash_vec;
    typedef hash_vec::const_iterator hash_vec_it;

    int initLevelsFromDict();
    int init_hashes();
    virtual int init_detectorElement_hashes();
    int init_channel_hashes();
    int init_neighbors();

    // Create expanded id from compact id (return == 0 for OK)
    int get_expanded_id_calc(const Identifier& compact_id, ExpandedIdentifier& id, const IdContext* context) const;

    // Create compact id from expanded or hash id (return == 0 for OK)
    int get_compact_id(const ExpandedIdentifier& id, Identifier& compact_id, const IdContext* context) const;
    // Create hash from compact
    virtual int get_hash_calc(const Identifier& compact_id, IdentifierHash& hash_id, const IdContext* context) const;

    /// The valid element checks converted the identifier to a stationName string
    /// in order to assess whether the stationName is good or not. However, the valid
    /// stations can be cached during initialization stage. For eacb stationName found in the
    /// dict, this method is called and the corresponding index is added to the stationToTech set
    virtual bool isStNameInTech(const std::string& stationName) const = 0;
    
    std::set<int> m_stationInTech{};
    
    
    size_type m_station_region_index{0};
    size_type m_MUON_INDEX{0};
    size_t m_GROUP_INDEX{6500};
    size_type m_NAME_INDEX{1};
    size_type m_ETA_INDEX{2};
    size_type m_PHI_INDEX{3};
    size_type m_TECHNOLOGY_INDEX{4};
    size_type m_MODULE_INDEX{5};
    size_type m_DETECTORELEMENT_INDEX{5};
    size_type m_CHANNEL_INDEX{10};
    const IdDictDictionary* m_dict{nullptr};

    MultiRange m_muon_range;
    MultiRange m_full_module_range;
    size_type m_module_hash_max{0};
    id_vec m_module_vec;

    MultiRange m_muon_channel_range;
    MultiRange m_full_channel_range;
    size_type m_channel_hash_max{0};
    id_vec m_channel_vec;

    MultiRange m_muon_detectorElement_range;
    MultiRange m_full_detectorElement_range;
    size_type m_detectorElement_hash_max{0};
    id_vec m_detectorElement_vec;

    hash_vec m_prev_phi_module_vec;
    hash_vec m_next_phi_module_vec;
    hash_vec m_prev_eta_module_vec;
    hash_vec m_next_eta_module_vec;

    IdDictFieldImplementation m_muon_impl;
    IdDictFieldImplementation m_sta_impl;
    IdDictFieldImplementation m_eta_impl;
    IdDictFieldImplementation m_phi_impl;
    IdDictFieldImplementation m_tec_impl;

    ///////////// compact identifier stuff ends//////////////////////////////////////

protected:
    // Check values down to station level

    bool validStation(int stationName, int technology) const;
    bool validStation(int stationName) const;
    bool validTechnology(int technology) const;
    

    // Append station ID fields

    void addStationID(Identifier& id, int stationName, int stationEta, int stationPhi, int technology) const;

    void addStationID(ExpandedIdentifier& id, int stationName, int stationEta, int stationPhi, int technology) const;

private:
    // Copy constructor - do not use!
    MuonIdHelper(const MuonIdHelper& other) = delete;
    MuonIdHelper& operator=(const MuonIdHelper& right);

    // vectors for stationNames and technologies

    /// Mapping  string -> int
    std::map<std::string, int> m_stationNameToIdxMap;
    /// Mapping int -> string
    std::map<int, std::string> m_stationIdxToNameMap;
    
    int m_stationIndexMax{-INT_MAX};
    /// Mapping string -> int
    std::map<std::string, int> m_technologyNameToIdxMap;
    /// Mapping int -> string
    std::map<int, std::string> m_technologyIdxToNameMap;
    int m_technologyIndexMax{-INT_MAX};

    // little helpers to speed things up
    std::set<int> m_isBarrel;
    std::set<int> m_isSmall;
    std::set<int> m_isForward;

    // Constants

    enum MuonIndices { StationNameIndex = 1, StationEtaIndex = 2, StationPhiIndex = 3, TechnologyIndex = 4 };

protected:
    std::string m_logName{};
    bool m_init{false};
};

// For backwards compatibility

typedef MuonIdHelper MuonID;

// Access to the ID

#endif  // DETECTORDESCRIPTION_MUONIDHELPER_H
