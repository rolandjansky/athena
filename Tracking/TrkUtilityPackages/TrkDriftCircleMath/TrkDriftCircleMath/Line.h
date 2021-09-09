/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_LINE_H
#define DCMATH_LINE_H

#include <cmath>
#include <iostream>
#include <ostream>

#include "TrkDriftCircleMath/LocVec2D.h"
#include "TrkDriftCircleMath/RotPhi.h"

namespace TrkDriftCircleMath {

    class Line {
    public:
        /**
        @brief Constructors  starting from the global parameters
        */
        Line(const LocVec2D& pos, const LocVec2D& dir) : Line{pos, std::atan2(dir.y(), dir.x())} {}

        Line(const LocVec2D& pos, double phi) : m_phi{phi}, m_pos{pos} { updateLocalParameters(); }

        Line(LocVec2D&& pos, const LocVec2D& dir) : Line{pos, std::atan2(dir.y(), dir.x())} {}

        Line(LocVec2D&& pos, double phi) : m_phi{phi}, m_pos{pos} { updateLocalParameters(); }

        Line(double x0, double y0, double phi) : m_phi{phi}, m_loc_pos{x0, y0} { updateGlobalParameters(); }

        ~Line() = default;

        /** set methods to reduce copying of data */
        void set(const LocVec2D& pos, const LocVec2D& dir) { set(pos, std::atan2(dir.y(), dir.x())); }

        void set(const LocVec2D& pos, double phi) {
            m_phi = phi;
            m_pos = pos;
            updateLocalParameters();
        }

        void set(double x0, double y0, double phi) {
            m_phi = phi;
            m_loc_pos.set(x0, y0);
            updateGlobalParameters();
        }

        void updateLocalParameters() {
            RotPhi rot(m_phi);
            m_dir.set(rot.cosphi(), rot.sinphi());
            m_loc_pos = (rot * m_pos);
        }

        void updateGlobalParameters() {
            RotPhi rot{-m_phi};
            m_pos = rot * m_loc_pos;
            /// Use symmetry cos(x) = cos(-x) & sin(x) = -sin(-x)
            m_dir.set(rot.cosphi(), -rot.sinphi());
        }

        double phi() const { return m_phi; }
        double x0() const { return m_loc_pos.x(); }
        double y0() const { return m_loc_pos.y(); }
        const LocVec2D& position() const { return m_pos; }
        const LocVec2D& direction() const { return m_dir; }

    private:
        double m_phi{0};
        LocVec2D m_pos{0., 0.};
        LocVec2D m_dir{0., 0.};
        LocVec2D m_loc_pos{0., 0.};
    };

    std::ostream& operator<<(std::ostream& os, const TrkDriftCircleMath::Line& l);

}  // namespace TrkDriftCircleMath

#endif
