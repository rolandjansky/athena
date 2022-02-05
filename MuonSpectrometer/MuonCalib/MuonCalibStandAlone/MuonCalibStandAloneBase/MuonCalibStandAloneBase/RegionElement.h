/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RegionElement_H
#define RegionElement_H

#include "MuonCalibStandAloneBase/RegionSelectorBase.h"

// c - c++
#include "set"
#include "string"
#include "vector"

namespace MuonCalib {

    class MuonFixedId;

    class RegionElement : public RegionSelectorBase {
    public:
        /**constructor*/
        RegionElement() = default;
        RegionElement(const std::string &region) : RegionSelectorBase(), m_region(region) {}
        ~RegionElement() = default;
        /** Initialize functions */
        bool Initialize(const std::string &region);
        /** return true if in region */
        bool Result(const MuonFixedId &region) const;
        /** print region */
        void Print(std::ostream &os) const;

    private:
        std::string m_region;
        //! regions
        std::set<int> m_stations;
        std::vector<int> m_eta_start;
        std::vector<int> m_eta_end;
        std::vector<int> m_phi_start;
        std::vector<int> m_phi_end;
        int m_ml{-1};
        //! process a astation name string
        bool process_station_name(std::string &name);
        //! process a numerical id
        bool process_int_range(std::string &range, std::vector<int> &target_start, std::vector<int> &target_end);
    };

}  // namespace MuonCalib
#endif
