/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONIDHELPERS_RPCIDHELPER_H
#define MUONIDHELPERS_RPCIDHELPER_H

#include "MuonIdHelpers/MuonIdHelper.h"

class MsgStream;
// ******************************************************************************
// class RpcIdHelper
// ******************************************************************************
//
// Description
// -----------
// This factory class constructs RPC identifiers and ranges and provides access
// to the levels.  ATLAS note ATL-MUON-2002-??? (https://cds.cern.ch/record/681542?ln=de)
// provides a complete description of the hierarchical identifier scheme.  
// RpcIdHelper provides an interface to the following fields of the identifier.
//
// Field           Range               Notes
// ==============================================================================
// StationName     unsigned integer    maps to "BIL","BMS",etc.
// StationEta      [-8,8]              increases with Z
// StationPhi      [1,8]               increases with phi
// Technology      [2]                 maps to "RPC"
// DoubletR        [1,2]               increases with R
// DoubletZ        [1,3]               increases with |Z|
// DoubletPhi      [1,2]               increases with phi
// GasGap          [1,2]               increases with R
// MeasuresPhi     [0,1]               0 if measures Z, 1 if measures phi
// Strip           [1,n]               increases with Z for MeasuresPhi=0
//                                     increases with phi for MeasuresPhi=1
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
// BNL, March 11th, 2003
//
// ******************************************************************************

class RpcIdHelper : public MuonIdHelper {
public:
    RpcIdHelper();
    virtual ~RpcIdHelper() = default;

    ///////////// compact identifier stuff begins //////////////////////////////////////

    /// Initialization from the identifier dictionary
    virtual int initialize_from_dictionary(const IdDictMgr& dict_mgr) override;
    virtual int get_module_hash(const Identifier& id, IdentifierHash& hash_id) const override;
    virtual int get_detectorElement_hash(const Identifier& id, IdentifierHash& hash_id) const override;

    ///////////// compact identifier stuff ends   //////////////////////////////////////

    // Identifier builders

    Identifier elementID(int stationName, int stationEta, int stationPhi, int doubletR) const;
    Identifier elementID(int stationName, int stationEta, int stationPhi, int doubletR, bool& isValid) const;
    

    Identifier elementID(const std::string& stationNameStr, int stationEta, int stationPhi, int doubletR) const;
    Identifier elementID(const std::string& stationNameStr, int stationEta, int stationPhi, int doubletR, bool& isValid) const;
    

    Identifier elementID(const Identifier& elementID, int doubletR) const;
    Identifier elementID(const Identifier& elementID, int doubletR, bool& isValid) const;
    
    Identifier elementID(const Identifier& channelID) const;

    Identifier channelID(int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap,
                         int measuresPhi, int strip) const;
    Identifier channelID(int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap,
                         int measuresPhi, int strip, bool& isValid) const;
                         
    
    Identifier channelID(const std::string& stationNameStr, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi,
                         int gasGap, int measuresPhi, int strip) const;
    Identifier channelID(const std::string& stationNameStr, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi,
                         int gasGap, int measuresPhi, int strip, bool& isValid) const;
   
    Identifier channelID(const Identifier& id, int doubletZ, int doubletPhi, int gasGap, int measuresPhi, int strip) const;
     Identifier channelID(const Identifier& id, int doubletZ, int doubletPhi, int gasGap, int measuresPhi, int strip, bool& isValid) const;

    Identifier parentID(const Identifier& id) const;
    Identifier doubletZID(const Identifier& id) const;

    Identifier padID(const Identifier& elementID, int doubletZ, int doubletPhi) const;
    Identifier padID(const Identifier& elementID, int doubletZ, int doubletPhi, bool& isValid) const;
    
    Identifier padID(int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi) const;
    Identifier padID(int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, bool& isValid) const;

    Identifier panelID(const Identifier& padID, int gasGap, int measuresPhi) const;
    Identifier panelID(const Identifier& padID, int gasGap, int measuresPhi, bool& isValid) const;


    Identifier panelID(const Identifier& channelID) const;
    Identifier panelID(int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap,
                       int measuresPhi) const;
  Identifier panelID(int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap,
                       int measuresPhi, bool& isValid) const;


    Identifier gapID(const Identifier& padID, int gasGap) const;
    Identifier gapID(const Identifier& padID, int gasGap, bool& isValid) const;
    
    Identifier gapID(const Identifier& panelID) const;
    Identifier gapID(int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap) const;
    Identifier gapID(int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi, int gasGap,
                     bool& isValid) const;


    // for an Identifier id, get the list of the daughter detector channel ids
    void idChannels(const Identifier& id, std::vector<Identifier>& vect) const;

    // Access to levels: missing field returns 0

    int channel(const Identifier& id) const override;

    int doubletR(const Identifier& id) const;
    int doubletZ(const Identifier& id) const;
    int doubletPhi(const Identifier& id) const;
    
    int gasGap(const Identifier& id) const override;
    bool measuresPhi(const Identifier& id) const override;
    int strip(const Identifier& id) const;

    // Access to min and max of level ranges

    int stationEtaMin() const;
    int stationEtaMax() const;
    int stationPhiMin() const;
    int stationPhiMax() const;
    int doubletRMin() const;
    int doubletRMax() const;
    int doubletZMin() const;
    int doubletZMax() const;
    int doubletPhiMin() const;
    int doubletPhiMax() const;
    int gasGapMin() const;
    int gasGapMax() const;
    int measuresPhiMin() const;
    int measuresPhiMax() const;
    int stripMin() const;
    int stripMax() const;

    // Access to min and max of level ranges

    int stationEtaMin(const Identifier& id) const;
    int stationEtaMax(const Identifier& id) const;
    int stationPhiMin(const Identifier& id) const;
    int stationPhiMax(const Identifier& id) const;
    int doubletRMin(const Identifier& id) const;
    int doubletRMax(const Identifier& id) const;
    int doubletZMin(const Identifier& id) const;
    int doubletZMax(const Identifier& id) const;
    int doubletPhiMin(const Identifier& id) const;
    int doubletPhiMax(const Identifier& id) const;
    int gasGapMin(const Identifier& id) const;
    int gasGapMax(const Identifier& id) const;
    int measuresPhiMin(const Identifier& id) const;
    int measuresPhiMax(const Identifier& id) const;
    int stripMin(const Identifier& id) const;
    int stripMax(const Identifier& id) const;

    // Public validation of levels

    bool valid(const Identifier& id) const;
    bool validElement(const Identifier& id) const;
    bool validPad(const Identifier& id) const;

private:
    bool isStNameInTech(const std::string& stationName) const override;
 
    virtual int init_detectorElement_hashes() override;
    int init_id_to_hashes();
    int zIndex(const Identifier& id) const;
    int zIndex(const std::string& name, int eta, int dR, int dZ, int dP) const;
    unsigned int m_module_hashes[60][20][8][2]{};
    unsigned int m_detectorElement_hashes[60][20][8][2][4]{};

    // compact id indices
    size_type m_DOUBLETR_INDEX{0};
    size_type m_DOUBLETZ_INDEX{0};
    size_type m_DOUBLETPHI_INDEX{0};
    size_type m_GASGAP_INDEX{0};
    size_type m_MEASURESPHI_INDEX{0};

    IdDictFieldImplementation m_dbr_impl;
    IdDictFieldImplementation m_dbz_impl;
    IdDictFieldImplementation m_dbp_impl;
    IdDictFieldImplementation m_gap_impl;
    IdDictFieldImplementation m_mea_impl;
    IdDictFieldImplementation m_str_impl;

    // Private validation of levels

    bool validElement(const Identifier& id, int stationName, int stationEta, int stationPhi, int doubletR, bool noPrint = false) const;
    bool validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi,
                      int gasGap, int measuresPhi, int strip, bool noPrint = false) const;
    bool validPad(const Identifier& id, int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi) const;

    // Utility methods

    int rpcTechnology() const;

    // Level indices

    enum RpcIndices { DoubletRIndex = 5, DoubletZIndex = 6, DoubletPhiIndex = 7, GasGapIndex = 8, MeasuresPhiIndex = 9, StripIndex = 10 };

    // Level ranges

    enum RpcRanges {
        StationEtaMin = -8,
        StationEtaMax = 8,
        StationPhiMin = 1,
        StationPhiMax = 8,
        DoubletRMin = 1,
        DoubletRMax = 2,
        DoubletZMin = 1,
        DoubletZMax = 3,
        DoubletPhiMin = 1,
        DoubletPhiMax = 2,
        GasGapMin = 1,
        GasGapMax = 4,
        MeasuresPhiMin = 0,
        MeasuresPhiMax = 1,
        StripMin = 1,
        StripMax = 99
    };
    unsigned int m_gasGapMax{UINT_MAX};  // maximum number of gas gaps
};

// For backwards compatibility

typedef RpcIdHelper RPC_ID;

CLASS_DEF(RpcIdHelper, 4172, 1)

// Construct ID from components

#endif  // MUONIDHELPERS_RPCIDHELPER_H
