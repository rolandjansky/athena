/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MDTCABLINGDATA_H
#define MUONMDT_CABLING_MDTCABLINGDATA_H

#include <cmath>
#include <iostream>
/*
 * @brief: Helper struct containing all information to convert from the offline identifiers to the online identifiers
 */

/// Split the offline part of the cabling apart to use it later for sorting
struct MdtCablingOffData {
    MdtCablingOffData() = default;
    MdtCablingOffData(const MdtCablingOffData& other) : MdtCablingOffData() { m_cache.hash = other.m_cache.hash; }
    MdtCablingOffData& operator=(const MdtCablingOffData& other) {
        if (&other != this) m_cache.hash = other.m_cache.hash;
        return *this;
    }
    int8_t& stationIndex{m_cache.cache[0]};  /// Station of the chamber (i.e, BIL,BIS,etc.)
    int8_t& eta{m_cache.cache[1]};           /// Eta of the MDT station
    int8_t& phi{m_cache.cache[2]};           /// Phi sector of the MDT station
    int8_t& multilayer{m_cache.cache[3]};    /// Multilayer inside the MDT station

    /// Equality operator
    bool operator==(const MdtCablingOffData& other) const { return m_cache.hash == other.m_cache.hash; }
    bool operator!=(const MdtCablingOffData& other) const { return m_cache.hash != other.m_cache.hash; }
    bool operator<(const MdtCablingOffData& other) const { return m_cache.hash < other.m_cache.hash; }

private:
    union {
        int hash{0};
        int8_t cache[4];
    } m_cache{};
};
struct MdtCablingOnData {
    MdtCablingOnData() = default;
    uint8_t& mrod{m_cache.cache[0]};           /// MROD number
    uint8_t& csm{m_cache.cache[1]};            ///  CSM number
    uint8_t& subdetectorId{m_cache.cache[2]};  /// Subdetector number

    MdtCablingOnData(const MdtCablingOnData& other) : MdtCablingOnData() { m_cache.hash = other.m_cache.hash; }
    MdtCablingOnData& operator=(const MdtCablingOnData& other) {
        if (&other != this) m_cache.hash = other.m_cache.hash;
        return *this;
    }

    bool operator==(const MdtCablingOnData& other) const { return m_cache.hash == other.m_cache.hash; }
    bool operator!=(const MdtCablingOnData& other) const { return m_cache.hash != other.m_cache.hash; }
    bool operator<(const MdtCablingOnData& other) const { return m_cache.hash < other.m_cache.hash; }

private:
    union {
        int hash{0};
        uint8_t cache[4];
    } m_cache{};
};
struct MdtCablingData : public MdtCablingOffData, MdtCablingOnData {
    MdtCablingData() = default;

    int layer{-99};  /// Layer inside the multilayer
    int tube{-99};   /// Tube number in the layer

    uint8_t mezzanine_type{0};  /// Mezzanine type
    /// the 0xff used to convert the full station)
    uint8_t tdcId{0xFF};  /// Identifier of the corresponding tdc

    uint8_t channelId{0xff};  /// Identifier of the corresponding channel on the tdc

    /// In the Muon spectrometer layouts, before Run 2, no chamber had more than 100 tubes per multilayer. Hence, the
    /// the tube layer and multilayer information is encoded into a 4 digit long number
    ///           MLTT
    /// where M represents the multilayer number, L the layer number and TT is the tube numbering per each multilayer
    static constexpr int legacy_tube_block_size = 100;
    static constexpr int legacy_layer_block_size = 10;

    /// Helper constants to extract tube,layer, multilayer information
    int tubes_per_layer{legacy_tube_block_size};
    int layer_block{legacy_layer_block_size};

    /// Equality operator
    bool operator==(const MdtCablingData& other) const {
        return this->MdtCablingOffData::operator==(other) && this->MdtCablingOnData::operator==(other) && tdcId == other.tdcId &&
               layer == other.layer && tube == other.tube && channelId == other.channelId;
    }
    bool operator!=(const MdtCablingData& other) const { return !((*this) == other); }
    bool operator<(const MdtCablingData& other) const {
        if (this->MdtCablingOffData::operator!=(other)) return this->MdtCablingOffData::operator<(other);
        if (this->MdtCablingOnData::operator!=(other)) return this->MdtCablingOnData::operator<(other);
        if (tdcId != other.tdcId) return tdcId < other.tdcId;
        return channelId < other.channelId;
    }
};

std::ostream& operator<<(std::ostream& ostr, const MdtCablingData& obj);

std::ostream& operator<<(std::ostream& ostr, const MdtCablingOffData& obj);
std::ostream& operator<<(std::ostream& ostr, const MdtCablingOnData& obj);

#endif