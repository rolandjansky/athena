/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ******************************************************************************
// ATLAS Muon Identifier Helpers Package
// -----------------------------------------
// ******************************************************************************

#ifndef MUONIDHELPERS_MMIDHELPER_H
#define MUONIDHELPERS_MMIDHELPER_H

// Includes
class MsgStream;
#include "MuonIdHelpers/MuonIdHelper.h"

// ******************************************************************************
// class MmIdHelper
// ******************************************************************************//
// Description
// -----------
// This factory class constructs MicroMegas (MM) identifiers and ranges and provides access
// to the levels.  ATLAS note ATL-MUON-2001-014 provides a complete description
// of the hierarchical identifier scheme.
// MmIdHelper provides an interface to the following fields of the identifier.
//
// Field           Range               Notes
// ==============================================================================
// nectar: todo: update this section for MM
// StationName     unsigned integer    maps to T1F,T3E,etc.
// StationEta      [-5,-1]             backward endcap (-1 at lowest R)
//                 [1,5]               forward endcap (1 at lowest R)
// StationPhi      [1,48]              endcap: increases with phi
//                 [1,24]              forward: increases with phi
// Technology      [5]                 maps to MM --> <label name="MM" value="5" /> in IdDictMuonSpectrometer_R.01.xml
// Multilayer       [1,2]               barrel: increases with R
//                                     endcap: increases with |Z|
// GasGap          [1,2]               doublet: increases with |Z|
//                 [1,3]               triplet: increases with |Z|
// Channel         [1,n]               increases with R for IsStrip=0 (wire gang)
//                                     increases with phi for IsStrip=1 (strip)
// ==============================================================================
//
// Inheritance
// -----------
// Inherits from MuonIdHelpers/MuonIdHelpers
//
// Author
// ------
// Nektarios Chr. Benekos <nectarios.benekos@cern.ch>
// Jochen Meyer <Jochen.Meyer@cern.ch>
// ******************************************************************************

class MmIdHelper : public MuonIdHelper {
public:
    // Constructor
    MmIdHelper();

    // Destructor
    virtual ~MmIdHelper() = default;

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

    Identifier channelID(int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channel, bool check = false,
                         bool* isValid = 0) const;
    Identifier channelID(const std::string& stationNameStr, int stationEta, int stationPhi, int multilayer, int gasGap, int channel,
                         bool check = false, bool* isValid = 0) const;
    Identifier channelID(const Identifier& id, int multilayer, int gasGap, int channel, bool check = false, bool* isValid = 0) const;

    Identifier parentID(const Identifier& id) const;

    Identifier multilayerID(const Identifier& channeldID) const;
    Identifier multilayerID(const Identifier& moduleID, int multilayer, bool check = false, bool* isValid = 0) const;

    // for an Identifier id, get the list of the daughter readout channel ids
    void idChannels(const Identifier& id, std::vector<Identifier>& vect) const;

    // Access to levels: missing field returns 0
    int gasGap(const Identifier& id) const;
    int multilayer(const Identifier& id) const;
    int channel(const Identifier& id) const;
    bool isStereo(const Identifier& id) const;
    bool measuresPhi(const Identifier& id) const;  // Returns false

    int numberOfMultilayers(const Identifier& id) const;

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
    int channelMin() const;
    int channelMax() const;

    // Access to min and max of level ranges
    int stationEtaMin(const Identifier& id) const;
    int stationEtaMax(const Identifier& id) const;
    int stationPhiMin(const Identifier& id) const;
    int stationPhiMax(const Identifier& id) const;
    int multilayerMin(const Identifier& id) const;
    int multilayerMax(const Identifier& id) const;
    int gasGapMin(const Identifier& id) const;
    int gasGapMax(const Identifier& id) const;
    int channelMin(const Identifier& id) const;
    int channelMax(const Identifier& id) const;

    // Utility methods
    int sectorType(std::string stationName, int stationEta) const;
    int sectorType(int stationName, int stationEta) const;

    // Public validation of levels
    bool valid(const Identifier& id) const;
    bool validElement(const Identifier& id) const;

private:
    int init_id_to_hashes();
    unsigned int m_module_hashes[60][20][48]{};             // Nektar Probably need to change this
    unsigned int m_detectorElement_hashes[60][20][8][3]{};  // Nektar Probably need to change this

    // compact id indices
    size_type m_GASGAP_INDEX;

    IdDictFieldImplementation m_mplet_impl;
    IdDictFieldImplementation m_gap_impl;
    IdDictFieldImplementation m_cha_impl;

    // Check level values
    bool validElement(const Identifier& id, int stationName, int stationEta, int stationPhi) const;
    bool validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channel) const;

    // Utility methods
    int mmTechnology() const;
    bool LargeSector(int stationName) const;
    bool SmallSector(int stationName) const;

    // Level indices
    enum MmIndices { MultilayerIndex = 5, GasGapIndex = 6, ChannelIndex = 7 };

    // Level ranges
    enum MmRanges {
        StationEtaMin = 0,
        StationEtaMax = 3,
        StationPhiMin = 1,  // ED: change for MM
        StationPhiMax = 8,  // ED: change for MM
        MultilayerMin = 1,
        MultilayerMax = 2,
        GasGapMin = 1,
        GasGapMax = 4,
        ChannelMin = 1,
        ChannelMax = 200
    };

};  // end class MmIdHelper
/*******************************************************************************/
// For backwards compatibility
typedef MmIdHelper MM_ID;

CLASS_DEF(MmIdHelper, 4175, 1)

#endif  // MUONIDHELPERS_MMIDHELPER_H
