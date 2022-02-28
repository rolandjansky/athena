/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ******************************************************************************
// ATLAS Muon Identifier Helpers Package
// -----------------------------------------
// ******************************************************************************

#ifndef MUONIDHELPERS_STGCIDHELPER_H
#define MUONIDHELPERS_STGCIDHELPER_H

// Includes
class MsgStream;
#include "MuonIdHelpers/MuonIdHelper.h"

// ******************************************************************************
// class sTgcIdHelper
// ******************************************************************************//
// Description
// -----------
// This factory class constructs sTGC identifiers and ranges and provides access
// to the levels.  ATLAS note ATL-MUON-2001-014 provides a complete description
// of the hierarchical identifier scheme.  sTgcIdHelper provides an interface to the
// following fields of the identifier.
//
// Field           Range               Notes
// ==============================================================================
// diehl: todo: update this section for sTGC
// StationName     unsigned integer    maps to T1F,T3E,etc.
// StationEta      [-4,-1]             backward endcap (-1 at lowest R)
//                 [1,4]               forward endcap (1 at lowest R)
// StationPhi      [1,8]               increases with phi
// Technology      [4]                 maps to sTGC
// GasGap          [1,2]               doublet: increases with |Z|
//                 [1,3]               triplet: increases with |Z|
// ChannelType     [0,2]               0 if pad, 1 if strip, 2 if wire
// Channel         [1,n]               increases with R         for channelType=1 (strip)
//                                     increases with phi       for channelType=2 (wire)
//                                     increases with phi and R for channelType=0 (pad)
//
// ==============================================================================
//
// Inheritance
// -----------
// Inherits from MuonIdHelpers/MuonIdHelper.
//
// Authors
// ------
// Edward Diehl <diehl@umich.edu> based on TgcIdHelper.h
// Nektarios Chr. Benekos <nectarios.benekos@cern.ch>
// Philipp Fleischmann <philipp.fleischmann@cern.ch>
//
// ******************************************************************************

class sTgcIdHelper : public MuonIdHelper {
public:
    // Constructor
    sTgcIdHelper();

    // Destructor
    virtual ~sTgcIdHelper() = default;

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

    Identifier channelID(int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channelType, int channel,
                         bool check = false, bool* isValid = 0) const;
    Identifier channelID(const std::string& stationNameStr, int stationEta, int stationPhi, int multilayer, int gasGap, int channelType,
                         int channel, bool check = false, bool* isValid = 0) const;
    Identifier channelID(const Identifier& id, int multilayer, int gasGap, int channelType, int channel, bool check = false,
                         bool* isValid = 0) const;

    Identifier padID(int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channelType, int padEta, int padPhi,
                     bool check = false, bool* isValid = 0) const;
    Identifier padID(const std::string& stationNameStr, int stationEta, int stationPhi, int multilayer, int gasGap, int channelType,
                     int padEta, int padPhi, bool check = false, bool* isValid = 0) const;
    Identifier padID(const Identifier& id, int multilayer, int gasGap, int channelType, int padEta, int padPhi, bool check = false,
                     bool* isValid = 0) const;

    Identifier parentID(const Identifier& id) const;

    Identifier multilayerID(const Identifier& channeldID) const;
    Identifier multilayerID(const Identifier& moduleID, int multilayer, bool check = false, bool* isValid = 0) const;

    // for an Identifier id, get the list of the daughter readout channel ids
    void idChannels(const Identifier& id, std::vector<Identifier>& vect) const;

    // Access to levels: missing field returns 0
    int gasGap(const Identifier& id) const;
    int multilayer(const Identifier& id) const;
    int channelType(const Identifier& id) const;
    bool measuresPhi(const Identifier& id) const;
    int channel(const Identifier& id) const;
    int numberOfMultilayers(const Identifier& id) const;
    int padEta(const Identifier& id) const;
    int padPhi(const Identifier& id) const;

    // Access to min and max of level ranges
    // to be remove when we moved to compact ids
    int stationEtaMin() const;
    int stationEtaMax() const;
    int stationPhiMin() const;
    int stationPhiMax() const;
    int multilayerMin() const;
    int multilayerMax() const;
    int gasGapMin() const;
    int gasGapMax() const;
    int channelTypeMin() const;
    int channelTypeMax() const;
    int channelMin() const;
    int channelMax() const;
    int padEtaMin() const;
    int padEtaMax() const;
    int padPhiMin() const;
    int padPhiMax() const;

    // Access to min and max of level ranges
    int stationEtaMin(const Identifier& id) const;
    int stationEtaMax(const Identifier& id) const;
    int stationPhiMin(const Identifier& id) const;
    int stationPhiMax(const Identifier& id) const;
    int multilayerMin(const Identifier& id) const;
    int multilayerMax(const Identifier& id) const;
    int gasGapMin(const Identifier& id) const;
    int gasGapMax(const Identifier& id) const;
    int channelTypeMin(const Identifier& id) const;
    int channelTypeMax(const Identifier& id) const;
    int channelMin(const Identifier& id) const;
    int channelMax(const Identifier& id) const;

    // Public validation of levels
    bool valid(const Identifier& id) const;
    bool validElement(const Identifier& id) const;

    // Type indices
    enum sTgcChannelTypes { Pad = 0, Strip = 1, Wire = 2 };

private:
    int init_id_to_hashes();
    unsigned int m_module_hashes[60][20][48]{};             // ED Probably need to change this
    unsigned int m_detectorElement_hashes[60][20][8][3]{};  // Nektar Probably need to change this

    // compact id indices
    size_type m_GASGAP_INDEX;
    size_type m_CHANNELTYPE_INDEX;

    IdDictFieldImplementation m_gap_impl;
    IdDictFieldImplementation m_typ_impl;
    IdDictFieldImplementation m_cha_impl;
    IdDictFieldImplementation m_mplet_impl;

    // Check level values
    bool validElement(const Identifier& id, int stationName, int stationEta, int stationPhi) const;
    bool validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channelType,
                      int channel) const;
    bool validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channelType,
                      int padEta, int padPhi) const;

    // Utility methods
    int stgcTechnology() const;
    bool LargeSector(int stationName) const;
    bool SmallSector(int stationName) const;

    // Level indices
    enum sTgcIndices { MultilayerIndex = 5, GasGapIndex = 6, ChannelTypeIndex = 7, ChannelIndex = 8 };

    // Level ranges
    enum sTgcRanges {
        StationEtaMin = -4,
        StationEtaMax = 4,
        StationPhiMin = 1,
        StationPhiMax = 8,
        MultilayerMin = 1,
        MultilayerMax = 2,
        GasGapMin = 1,
        GasGapMax = 4,
        ChannelTypeMin = 0,
        ChannelTypeMax = 2,
        ChannelMin = 1,
        ChannelMax = 315,
        PadPhiMin = 1,
        PadPhiMax = 8,
        PadEtaMin = 1,
        PadEtaMax = 18  // arbitrary values. these values should be taken from an XML
    };
};  // end class sTgcIdHelper
/*******************************************************************************/
// For backwards compatibility
typedef sTgcIdHelper sTGC_ID;

CLASS_DEF(sTgcIdHelper, 4174, 1)

#endif  // MUONIDHELPERS_STGCIDHELPER_H
