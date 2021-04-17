/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRACKSTEERINGSTRATEGY_H
#define MUON_MUONTRACKSTEERINGSTRATEGY_H

#include <bitset>
#include <iostream>
#include <string>
#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "MuonStationIndex/MuonStationIndex.h"

namespace Muon {
    class MuonTrackSteeringStrategy {
    public:
        enum Option {
            CutSeedsOnTracks = 0x0,
            CombineSegInStation = 0x1,
            DynamicSeeding = 0x2,
            PreferOutsideIn = 0x3,
            AllowOneSharedHit = 0x4,
            DoRefinement = 0x5,
            DoAmbiSolving = 0x6,
            BarrelEndcapFilter = 0x7,
            Last = 0x8
        };

        MuonTrackSteeringStrategy(const std::string name, std::bitset<Last> bits,
                                  std::vector<std::vector<MuonStationIndex::ChIndex> >& path, std::vector<unsigned int>& seedOrder) :
            m_name(name), m_bits(bits), m_path(path), m_seeds(seedOrder) {}

        /** @brief Constructor that assumes the seed order could come as part of the options */
        MuonTrackSteeringStrategy(const std::string name, std::vector<std::string> options,
                                  std::vector<std::vector<MuonStationIndex::ChIndex> >& path) :
            m_name(name), m_path(path) {
            for (unsigned int i = 0; i < Last; ++i) m_bits.set(i, 0);
            for (unsigned int i = 0; i < options.size(); ++i) setOption(options[i], true);
        }

        MuonTrackSteeringStrategy(const std::string name, std::vector<std::string> options,
                                  std::vector<std::vector<MuonStationIndex::ChIndex> >& path, std::vector<unsigned int>& seedOrder) :
            m_name(name), m_path(path), m_seeds(seedOrder) {
            for (unsigned int i = 0; i < Last; ++i) m_bits.set(i, 0);
            for (unsigned int i = 0; i < options.size(); ++i) setOption(options[i], true);
        }

        void setOption(const Option, bool value);
        void setOption(const std::string, bool value);
        bool option(Option) const;
        std::bitset<Last> allOptions() const { return m_bits; }

        // Set methods
        void setCh(const std::vector<MuonStationIndex::ChIndex> val, const unsigned int layer);
        void setCh(const std::vector<std::vector<MuonStationIndex::ChIndex> > val) { m_path = val; }

        // Get methods
        const std::vector<std::vector<MuonStationIndex::ChIndex> >& getAll() const { return m_path; }
        std::vector<MuonStationIndex::ChIndex> getCh(const unsigned int) const;

        // for the name
        const std::string getName() const { return m_name; }
        void setName(const std::string name) { m_name = name; }

        const std::vector<unsigned int>& seeds() const { return m_seeds; }
        void setSeeds(const std::vector<unsigned int> val) { m_seeds = val; }

    private:
        std::string m_name;
        std::bitset<Last> m_bits;  //!< List of all of the options available
        std::vector<std::vector<MuonStationIndex::ChIndex> > m_path;
        std::vector<unsigned int> m_seeds;
    };
}  // namespace Muon

inline void Muon::MuonTrackSteeringStrategy::setOption(const std::string opt, bool value) {
    if (opt == "CutSeedsOnTracks")
        setOption(CutSeedsOnTracks, value);
    else if (opt == "CombineSegInStation")
        setOption(CombineSegInStation, value);
    else if (opt == "DynamicSeeding")
        setOption(DynamicSeeding, value);
    else if (opt == "PreferOutsideIn")
        setOption(PreferOutsideIn, value);
    else if (opt == "AllowOneSharedHit")
        setOption(AllowOneSharedHit, value);
    else if (opt == "DoRefinement")
        setOption(DoRefinement, value);
    else if (opt == "DoAmbiSolving")
        setOption(DoAmbiSolving, value);
    else if (opt == "BarrelEndcapFilter")
        setOption(BarrelEndcapFilter, value);
    else {
        // Attempt to interpret the string as a seed sequence
        std::vector<unsigned int> seeds;
        bool success = true;
        for (unsigned int i = 0; i < opt.size() && success; ++i) {
            int holder = int(opt[i]) - 48;
            if (holder < 0 || holder > 9)
                success = false;
            else
                seeds.push_back(holder);
        }
        if (success) setSeeds(seeds);
    }
}

inline void Muon::MuonTrackSteeringStrategy::setOption(const Option opt, bool value) {
    if (opt == Last) return;
    m_bits[opt] = value;
}

inline void Muon::MuonTrackSteeringStrategy::setCh(const std::vector<MuonStationIndex::ChIndex> val, const unsigned int layer) {
    if (layer < m_path.size())
        m_path[layer] = val;
    else {  // assume the user knows what he or she is doing
        m_path.resize(layer + 1);
        m_path[layer] = val;
    }
}

inline std::vector<Muon::MuonStationIndex::ChIndex> Muon::MuonTrackSteeringStrategy::getCh(const unsigned int layer = 0) const {
    if (layer < m_path.size()) return m_path[layer];
    std::vector<Muon::MuonStationIndex::ChIndex> val(1);
    return val;
}

inline bool Muon::MuonTrackSteeringStrategy::option(Muon::MuonTrackSteeringStrategy::Option op) const {
    return m_bits[static_cast<unsigned int>(op)];
}

inline std::ostream& operator<<(std::ostream& sl, const Muon::MuonTrackSteeringStrategy& mtss) {
    sl << "MuonTrackSteeringStrategy " << mtss.getName() << " with options: " << mtss.allOptions() << " with seeds";
    if (mtss.seeds().size()) {
        for (unsigned int i = 0; i < mtss.seeds().size(); ++i) { sl << " " << mtss.seeds()[i]; }
    } else
        sl << " <none>";
    sl << " and chambers are: " << std::endl;
    for (unsigned int i = 0; i < mtss.getAll().size(); ++i) {
        sl << "\t Step " << i << " : ( ";
        for (unsigned int j = 0; j < mtss.getCh(i).size() - 1; ++j) sl << mtss.getCh(i)[j] << " , ";
        if (mtss.getCh(i).size()) sl << mtss.getCh(i)[mtss.getCh(i).size() - 1];
        sl << " ) " << std::endl;
    }
    return sl;
}

inline MsgStream& operator<<(MsgStream& sl, const Muon::MuonTrackSteeringStrategy& mtss) {
    sl << "MuonTrackSteeringStrategy " << mtss.getName() << " with options: " << mtss.allOptions() << " with seeds";
    if (mtss.seeds().size()) {
        for (unsigned int i = 0; i < mtss.seeds().size(); ++i) { sl << " " << mtss.seeds()[i]; }
    } else
        sl << " <none>";
    sl << " and chambers are: " << std::endl;
    for (unsigned int i = 0; i < mtss.getAll().size(); ++i) {
        sl << "\t Step " << i << " : ( ";
        for (unsigned int j = 0; j < mtss.getCh(i).size() - 1; ++j) sl << mtss.getCh(i)[j] << " , ";
        if (mtss.getCh(i).size()) sl << mtss.getCh(i)[mtss.getCh(i).size() - 1];
        sl << " ) " << std::endl;
    }
    return sl;
}

#endif
