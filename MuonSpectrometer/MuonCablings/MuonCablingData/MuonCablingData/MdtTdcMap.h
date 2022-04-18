/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MDTAMTMAP_H
#define MUONMDT_CABLING_MDTAMTMAP_H

/**
 *
 *  @brief Atlas MDT Tdc Mapping class
 *
 *  This class implements the channel mapping of a single MDT TDC.
 *  The mapping is between the TDC channel and the tube multilayer, layer, number in
 *  the offline convention
 *
 *
 **/

#include <stdint.h>

#include <array>

#include "GaudiKernel/MsgStream.h"
#include "MuonCablingData/MdtCablingData.h"
class MdtMezzanineType;
class MdtIdHelper;

class MdtTdcMap {
public:
    static constexpr uint8_t CHANMAX = 24;
    static constexpr uint8_t NOTSET = 250;

    /** constructor */
    /** arguments are the mezzanine type, one channel (usually chan 0) and the */
    /** corresponding offline informations */
    MdtTdcMap(const MdtMezzanineType* mezType, const MdtCablingData& cabling_data, const MdtIdHelper* helper, MsgStream& ext_log);

    /** destructor */
    ~MdtTdcMap() = default;

    /** add a channel */
    bool setChannel(uint8_t channel, int layer, int tube, MsgStream& log);

    /** retrieve the full information */
    bool offlineId(MdtCablingData& cabling_data, MsgStream& log) const;
    /** retrieve the full information for online */
    bool onlineId(MdtCablingData& cabling_data, MsgStream& log) const;

    /** get the mezzanine type */
    uint8_t mezzanineType() const { return m_mezType; }

    /** return the tdc id */
    uint8_t moduleId() const { return m_statId.tdcId; }
    /** return the mrod associated to the tdc */
    uint8_t mrod() const { return m_statId.mrod; }
    /** return the csm of the associated tdc */
    uint8_t csm() const { return m_statId.csm; }

    /** get the layer number */
    int layer(uint8_t channel) const { return m_chanToLayer.at(channel).first; }

    /** get the tube number */
    int tube(uint8_t channel) const { return m_chanToLayer[channel].second; }

    /** get the offline identifier */
    const MdtCablingOffData& offId() const { return m_statId; }
    /** get the multilayer (independent of the channel) */
    int multiLayer() const { return m_statId.multilayer; }
    /* get the station Name */
    int stationName() const { return m_statId.stationIndex; }
    /* get the station Eta */
    int stationEta() const { return m_statId.eta; }
    /* get the station Phi */
    int stationPhi() const { return m_statId.phi; }

    /* get the zero channels */
    uint8_t tdcZero() const { return m_statId.channelId; }
    /* tube zero */
    int tubeZero() const { return m_statId.tube; }
    /* layer zero */
    int layerZero() const { return m_statId.layer; }

    /// Minimum tube number & maximum tube number found
    int minTube() const { return m_minTube; }
    int maxTube() const { return m_maxTube; }
    /// Minimum & maximum layer number
    int minLayer() const { return m_minLayer; }
    int maxLayer() const { return m_maxLayer; }

private:
    /** Private functions */

    /** initialize the channel-to-tube map */
    bool initMap(const MdtMezzanineType* mezType, uint8_t channel, int layerZero, int tubeZero, MsgStream& log);

    /** tube corresponding to each tdc channel */
    MdtCablingData m_statId{};
    /// std::pair
    std::array<std::pair<int, int>, CHANMAX> m_chanToLayer{};
    std::map<std::pair<int, int>, uint8_t> m_LayTubeToChan{};

    int m_minTube{NOTSET};
    int m_maxTube{0};
    int m_minLayer{NOTSET};
    int m_maxLayer{0};

    /** mezzanine type */
    uint8_t m_mezType{0};
    const MdtIdHelper* m_mdtIdHelper{nullptr};
};

/// Helper struct to search through the std::set if a conversion from
///    offline -> online is needed
struct MdtTdcOffSorter {
    MdtTdcOffSorter() = default;
    MdtTdcOffSorter(const MdtTdcMap* ptr) : m_ptr{ptr} {}
    const MdtTdcMap* operator->() const { return m_ptr; }
    bool operator!() const { return !m_ptr; }   
    operator bool() const { return m_ptr; }
private:
    const MdtTdcMap* m_ptr{nullptr};
};
/// Helper struct to search through the std::set if a conversion from
///      online -> offline is needed
struct MdtTdcOnlSorter {
    MdtTdcOnlSorter() = default;
    MdtTdcOnlSorter(const MdtTdcMap* ptr) : m_ptr{ptr} {}
    const MdtTdcMap* operator->() const { return m_ptr; }
    bool operator!() const { return !m_ptr; }
    operator bool() const { return m_ptr; }

private:
    const MdtTdcMap* m_ptr{nullptr};
};
/// Operators used for navigation later

/// The minimum and maximum tube of a tdc define its range of covered channels
/// The MdtHit belongs exactly to the corresponding readout channel if its
/// tube number is within that range
inline bool operator<(const MdtTdcOffSorter& a, const MdtCablingData& b) { return a->maxTube() < b.tube; }
inline bool operator<(const MdtCablingData& a, const MdtTdcOffSorter& b) { return a.tube < b->minTube(); }
/// Order the tdc identifiers according to their minimum tube
inline bool operator<(const MdtTdcOffSorter& a, const MdtTdcOffSorter& b) { return a->minTube() < b->minTube(); }

inline bool operator<(const MdtTdcOnlSorter& a, const MdtTdcOnlSorter& b) { return a->moduleId() < b->moduleId(); }
inline bool operator<(const MdtTdcOnlSorter& a, const MdtCablingData& b) { return a->moduleId() < b.tdcId; }
inline bool operator<(const MdtCablingData& a, const MdtTdcOnlSorter& b) { return a.tdcId < b->moduleId(); }




#endif  // MUONMDT_CABLING_MDTAMTMAP_H