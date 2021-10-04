/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_LOCVEC2D_H
#define DCMATH_LOCVEC2D_H

/** local 2D vector */

#include <cmath>
#include <ostream>

namespace TrkDriftCircleMath {
    /// Implementation of 2 dimensional vector class
    ///
    class LocVec2D {
    public:
        /// Constructor parsing the coordinates
        LocVec2D(double x, double y) : m_x{x}, m_y{y} {}
        /// Default copy & move constructors
        LocVec2D(const LocVec2D&) = default;
        LocVec2D(LocVec2D&&) = default;

        ~LocVec2D() = default;
        /// Returns the x coordinate of the vector
        double x() const { return m_x; }
        /// Returns the y coordinate of the vector
        double y() const { return m_y; }

        /// Setter function using another vector
        void set(const LocVec2D& vec) {
            m_x = vec.x();
            m_y = vec.y();
        }
        /// Set x & y simultaenously
        void set(double x, double y) {
            m_x = x;
            m_y = y;
        }
        /// Set only x
        void setX(double x) { m_x = x; }
        /// Set only y
        void setY(double y) { m_y = y; }

        /// Plus and minus operators
        LocVec2D operator+(const LocVec2D& lv) const { return LocVec2D(x() + lv.x(), y() + lv.y()); }

        LocVec2D operator-(const LocVec2D& lv) const { return LocVec2D(x() - lv.x(), y() - lv.y()); }

        /// Default assignment operators (Equivalent to the setter x)
        LocVec2D& operator=(const LocVec2D&) = default;
        LocVec2D& operator=(LocVec2D&&) = default;

        /// Incrementation operator
        LocVec2D& operator+=(const LocVec2D& lv) {
            m_x += lv.x();
            m_y += lv.y();
            return *this;
        }

        LocVec2D& operator-=(const LocVec2D& lv) {
            m_x -= lv.x();
            m_y -= lv.y();
            return *this;
        }

        double operator*(const LocVec2D& lv) const { return x() * lv.x() + y() * lv.y(); }

        LocVec2D operator*(double v) const {
            LocVec2D n(*this);
            return n *= v;
        }

        LocVec2D& operator*=(double v) {
            m_x *= v;
            m_y *= v;
            return *this;
        }

        LocVec2D& operator/=(double v) {
            m_x /= v;
            m_y /= v;
            return *this;
        }

        double cross(const LocVec2D& lv) const { return y() * lv.x() - x() * lv.y(); }

        double mag() const { return std::hypot(m_x, m_y); }

    private:
        double m_x{0};
        double m_y{0};
    };

    std::ostream& operator<<(std::ostream& os, const TrkDriftCircleMath::LocVec2D& lv);
}  // namespace TrkDriftCircleMath

#endif
