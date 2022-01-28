/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONIDHELPERS_TGCIDHELPER_H
#define MUONIDHELPERS_TGCIDHELPER_H

#include "MuonIdHelpers/MuonIdHelper.h"

// ******************************************************************************
// class TgcIdHelper
// ******************************************************************************
//
// Description
// -----------
// This factory class constructs TGC identifiers and ranges and provides access
// to the levels.  ATLAS note ATL-MUON-2001-014 provides a complete description
// of the hierarchical identifier scheme.  TgcIdHelper provides an interface to the
// following fields of the identifier.
//
// Field           Range               Notes
// ==============================================================================
// StationName     unsigned integer    maps to T1F,T3E,etc.
// StationEta      [-5,-1]             backward endcap (-1 at lowest R)
//                 [1,5]               forward endcap (1 at lowest R)
// StationPhi      [1,48]              endcap: increases with phi
//                 [1,24]              forward: increases with phi
// Technology      [3]                 maps to TGC
// GasGap          [1,2]               doublet: increases with |Z|
//                 [1,3]               triplet: increases with |Z|
// IsStrip         [0,1]               0 if wire, 1 if strip: corresponds to measuresPhi
// Channel         [1,n]               increases with R for IsStrip=0 (wire gang)
//                                     increases with phi for IsStrip=1 (strip)
// ==============================================================================
//
// Inheritance
// -----------
// Inherits from MuonIdHelpers/MuonIdHelper.
//
// Author
// ------
// Steven Goldfarb <Steven.Goldfarb@cern.ch>
//
// Compact ID implementation by
// Ketevi A. Assamagan <ketevi@bnl.gov>
// BNL, March 6th, 2003
//
// ******************************************************************************

class TgcIdHelper : public MuonIdHelper {
public:
    // Constructor

    TgcIdHelper();

    // Destructor

    virtual ~TgcIdHelper() = default;

    ///////////// compact identifier stuff begins //////////////////////////////////////

    /// Initialization from the identifier dictionary
    virtual int initialize_from_dictionary(const IdDictMgr& dict_mgr);
    virtual int get_module_hash(const Identifier& id, IdentifierHash& hash_id) const;
    virtual int get_detectorElement_hash(const Identifier& id, IdentifierHash& hash_id) const;

    ///////////// compact identifier stuff ends   //////////////////////////////////////

    // Identifier builders

    Identifier elementID(int stationName, int stationEta, int stationPhi, bool check = false, bool* isValid = 0) const;
    Identifier elementID(std::string_view stationNameStr, int stationEta, int stationPhi, bool check = false, bool* isValid = 0) const;
    Identifier elementID(const Identifier& channelID) const;
    Identifier channelID(int stationName, int stationEta, int stationPhi, int gasGap, int isStrip, int channel, bool check = false,
                         bool* isValid = 0) const;
    Identifier channelID(std::string_view stationNameStr, int stationEta, int stationPhi, int gasGap, int isStrip, int channel,
                         bool check = false, bool* isValid = 0) const;
    Identifier channelID(const Identifier& id, int gasGap, int isStrip, int channel, bool check = false, bool* isValid = 0) const;

    Identifier parentID(const Identifier& id) const;

    // for an Identifier id, get the list of the daughter readout channel ids
    void idChannels(const Identifier& id, std::vector<Identifier>& vect) const;

    // Access to levels: missing field returns 0

    int gasGap(const Identifier& id) const;

    /** isStrip corresponds to measuresPhi */
    int isStrip(const Identifier& id) const;
    bool measuresPhi(const Identifier& id) const;

    int channel(const Identifier& id) const;

    // Access to min and max of level ranges
    // to be remove when we moved to compact ids

    int stationEtaMin() const;
    int stationEtaMax() const;
    int stationPhiMin(bool endcap) const;
    int stationPhiMax(bool endcap) const;
    int gasGapMin() const;
    int gasGapMax(bool triplet) const;
    int isStripMin() const;
    int isStripMax() const;
    int channelMin() const;
    int channelMax() const;

    // Access to min and max of level ranges

    int stationEtaMin(const Identifier& id) const;
    int stationEtaMax(const Identifier& id) const;
    int stationPhiMin(const Identifier& id) const;
    int stationPhiMax(const Identifier& id) const;
    int gasGapMin(const Identifier& id) const;
    int gasGapMax(const Identifier& id) const;
    int isStripMin(const Identifier& id) const;
    int isStripMax(const Identifier& id) const;
    int channelMin(const Identifier& id) const;
    int channelMax(const Identifier& id) const;

    // Utility methods

    int chamberType(std::string stationName, int stationEta) const;
    int chamberType(int stationName, int stationEta) const;

    // Public validation of levels

    bool valid(const Identifier& id) const;
    bool validElement(const Identifier& id) const;

private:
    int init_id_to_hashes();
    unsigned int m_module_hashes[60][20][48]{};

    // compact id indices
    size_type m_GASGAP_INDEX;
    size_type m_ISSTRIP_INDEX;

    IdDictFieldImplementation m_gap_impl;
    IdDictFieldImplementation m_ist_impl;
    IdDictFieldImplementation m_cha_impl;

    // Check level values

    bool validElement(const Identifier& id, int stationName, int stationEta, int stationPhi) const;
    bool validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int gasGap, int isStrip, int channel) const;

    // Utility methods

    int tgcTechnology() const;
    bool endcapChamber(int stationName) const;
    bool tripletChamber(int stationName) const;

    // Level indices

    enum TgcIndices { GasGapIndex = 5, IsStripIndex = 6, ChannelIndex = 7 };

    // Level ranges

    enum TgcRanges {
        StationEtaMin = -8,
        StationEtaMax = 8,
        StationPhiEndcapMin = 1,
        StationPhiEndcapMax = 48,
        StationPhiForwardMin = 1,
        StationPhiForwardMax = 24,
        GasGapMin = 1,
        GasGapDoubletMax = 2,
        GasGapTripletMax = 3,
        IsStripMin = 0,
        IsStripMax = 1,
        ChannelMin = 1,
        ChannelMax = 135
    };
};

// For backwards compatibility

typedef TgcIdHelper TGC_ID;

CLASS_DEF(TgcIdHelper, 4173, 1)

// Construct ID from components

#endif  // MUONIDHELPERS_TGCIDHELPER_H
