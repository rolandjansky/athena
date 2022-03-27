/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ******************************************************************************
// ATLAS Muon Identifier Helpers Package
// -----------------------------------------
// ******************************************************************************

//<doc><file> $Id: MdtIdHelper.h,v 1.34 2009-01-20 22:44:13 kblack Exp $
//<version>   $Name: not supported by cvs2svn $

#ifndef MUONIDHELPERS_MDTIDHELPER_H
#define MUONIDHELPERS_MDTIDHELPER_H

// Includes

#include "MuonIdHelpers/MuonIdHelper.h"

// ******************************************************************************
// class MdtIdHelper
// ******************************************************************************
//
// Description
// -----------
// This factory class constructs MDT identifiers and ranges and provides access
// to the levels.  ATLAS note ATL-MUON-2002-??? provides a complete description
// of the hierarchical identifier scheme.  MdtIdHelper provides an interface to the
// following fields of the identifier.
//
// Field           Range               Notes
// ==============================================================================
// StationName     unsigned integer    maps to "BIL", "EMS" ,etc.
// StationEta      [-6,-1]             backward endcap (-1 at lowest R)
//                 [-8,8]              barrel (increases with Z)
//                 [-6,-1]+[1,6]       forward endcap (1 at lowest R)
// StationPhi      [1,8]               increases with phi
// Technology      [0]                 maps to "MDT"
// Multilayer      [1,2]               barrel: increases with R
//                                     endcap: increases with |Z|
// TubeLayer       [1,4]               barrel: increases with R
//                                     endcap: increases with |Z|
// Tube            [1,n]               barrel: increases with |Z|
//                                     endcap: increases with R
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
// Compact Id implementation by
// Ketevi A. Assamagan <ketevi@bnl.gov>
// BNL, March 10th, 2003
//
// ******************************************************************************

class MdtIdHelper : public MuonIdHelper {
public:
    
    ///  The maxNTubesPerLayer represents the absolute maximum of tubes which
    ///  are built into a single multilayer in any MDT chamber plus some safety margin
    ///  Thus far the BIS78 chambers comprise the largest number of tubes in a single layer
    ///    - cf. https://gitlab.cern.ch/atlas-mpp-mdt/BIS78Info/-/blob/master/sMDT_ATLAS_param_v1.4.pdf
    ///  Define the number once here to be used at any place throughout the athena code base 
    static constexpr int maxNTubesPerLayer = 120;
    
    // Constructor
    MdtIdHelper();

    // Destructor

    virtual ~MdtIdHelper() = default;

    ////////////// compact identifier stuff begins //////////////////////////////////////

    /// Initialization from the identifier dictionary
    virtual int initialize_from_dictionary(const IdDictMgr& dict_mgr) override;
    virtual int get_module_hash(const Identifier& id, IdentifierHash& hash_id) const override;
    virtual int get_detectorElement_hash(const Identifier& id, IdentifierHash& hash_id) const override;

    ///////////// compact identifier stuff ends   //////////////////////////////////////

    // Atlas Identifier builders

    Identifier elementID(int stationName, int stationEta, int stationPhi) const;
    Identifier elementID(int stationName, int stationEta, int stationPhi, bool& isValid) const;
    
    Identifier elementID(const std::string& stationNameStr, int stationEta, int stationPhi) const;
    Identifier elementID(const std::string& stationNameStr, int stationEta, int stationPhi, bool& isValid) const;
    
    Identifier elementID(const Identifier& channelID) const;

    Identifier channelID(int stationName, int stationEta, int stationPhi, int multilayer, int tubeLayer, int tube) const;
    Identifier channelID(int stationName, int stationEta, int stationPhi, int multilayer, int tubeLayer, int tube, bool& isValid) const;

    Identifier channelID(const std::string& stationNameStr, int stationEta, int stationPhi, int multilayer, int tubeLayer, int tube) const;
    Identifier channelID(const std::string& stationNameStr, int stationEta, int stationPhi, int multilayer, int tubeLayer, int tube, bool& isValid) const;

    Identifier channelID(const Identifier& id, int multilayer, int tubeLayer, int tube) const;
    Identifier channelID(const Identifier& id, int multilayer, int tubeLayer, int tube, bool& isValid) const;

    Identifier parentID(const Identifier& id) const;

    Identifier multilayerID(const Identifier& channeldID) const;

    Identifier multilayerID(const Identifier& moduleID, int multilayer) const;
    Identifier multilayerID(const Identifier& moduleID, int multilayer, bool& isValid) const;

    // for an Identifier id, get the list of the daughter readout channel ids
    void idChannels(const Identifier& id, std::vector<Identifier>& vect) const;

    // Access to levels: missing field returns 0
    int channel(const Identifier& id) const override;

    int multilayer(const Identifier& id) const;
    int tubeLayer(const Identifier& id) const;
    int tube(const Identifier& id) const;

    int numberOfMultilayers(const Identifier& id) const;

    // Access to min and max of level ranges

    int stationEtaMin(bool barrel) const;
    int stationEtaMax(bool barrel) const;
    int stationPhiMin() const;
    int stationPhiMax() const;
    int multilayerMin() const;
    int multilayerMax() const;
    int tubeLayerMin() const;
    int tubeLayerMax() const;
    int tubeMin() const;
    int tubeMax() const;

    // Access to min and max of level ranges

    int stationEtaMin(const Identifier& id) const;
    int stationEtaMax(const Identifier& id) const;
    int stationPhiMin(const Identifier& id) const;
    int stationPhiMax(const Identifier& id) const;
    int multilayerMin(const Identifier& id) const;
    int multilayerMax(const Identifier& id) const;
    int tubeLayerMin(const Identifier& id) const;
    int tubeLayerMax(const Identifier& id) const;
    int tubeMin(const Identifier& id) const;
    int tubeMax(const Identifier& id) const;

    // Public validation of levels

    bool valid(const Identifier& id) const;
    bool validElement(const Identifier& id) const;

    /// the gas-gap function for the MDT's returns the tube layer
    int gasGap(const Identifier& id) const override;
    /// always false for MDTs
    bool measuresPhi(const Identifier& id) const override;
    /// is this a BMG chamber
    bool isBMG(const Identifier& id) const;
    /// is this a BME chamber
    bool isBME(const Identifier& id) const;

private:
    bool isStNameInTech(const std::string& stationName) const override;
    int init_id_to_hashes();
    unsigned int m_module_hashes[60][20][8]{};
    unsigned int m_detectorElement_hashes[60][20][8][3]{};

    // compact id indices
    size_type m_TUBELAYER_INDEX{0};
    IdDictFieldImplementation m_mla_impl;
    IdDictFieldImplementation m_lay_impl;
    IdDictFieldImplementation m_tub_impl;

    // Private validation of levels

    bool validElement(const Identifier& id, int stationName, int stationEta, int stationPhi) const;
    bool validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int multilayer, int tubeLayer, int tube) const;

    // Utility method

    int mdtTechnology() const;
   
    // Level indices

    enum MdtIndices { MultilayerIndex = 5, TubeLayerIndex = 6, TubeIndex = 7 };

    // Level ranges

    enum MdtRanges {
        StationEtaBarrelMin = -8,
        StationEtaBarrelMax = 8,
        StationEtaEndcapMin = -6,
        StationEtaEndcapMax = 6,
        StationPhiMin = 1,
        StationPhiMax = 8,
        MultilayerMin = 1,
        MultilayerMax = 2,
        TubeLayerMin = 1,
        TubeLayerMax = 4,
        TubeMin = 1,
        TubeMax = 78
    };
    unsigned int m_tubesMax{UINT_MAX};  // maximum number of tubes in any chamber
};

// For backwards compatibility

typedef MdtIdHelper MDT_ID;

CLASS_DEF(MdtIdHelper, 4170, 1)

// Construct Atlas Identifiers from components

#endif  // MUONIDHELPERS_MDTIDHELPER_H
