/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetVoronoiDiagramHelpers.h

#ifndef JETMOMENTTOOLS_JETVORONOIDIAGRAMHELPERS_H
#define JETMOMENTTOOLS_JETVORONOIDIAGRAMHELPERS_H

/// Jakub Cuth \n
/// May 2015
///
/// Typedef and namespace definition and tools for JetVoronoiMomentsTool.
///

// athena
#include "AsgTools/AsgTool.h"

//stl
#include <string>
#include <vector>
//using std::vector;

// boost polygon
#include <boost/polygon/voronoi.hpp>
//using boost::polygon::voronoi_builder;
//using boost::polygon::voronoi_diagram;
typedef boost::polygon::voronoi_diagram<double>              VoronoiBoost;
typedef boost::polygon::voronoi_diagram<double>::vertex_type VoronoiVtxBoost;
typedef boost::polygon::voronoi_diagram<double>::cell_type   VoronoiCellBoost;
typedef boost::polygon::voronoi_diagram<double>::edge_type   VoronoiEdgeBoost;

// boost geometry
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
//using boost::geometry::area;
//using boost::geometry::intersection;
typedef boost::geometry::model::d2::point_xy<double> VoronoiPointBoost;
typedef boost::geometry::model::polygon<VoronoiPointBoost> VoronoiPolygonBoost;

namespace JetVoronoiDiagramHelpers {
    /// @typedef coord
    /// Type of coordination system. Default integer in boost.
    typedef double coord;


    /// @class Point
    /// Default object of 2D space. 
    struct Point{
        Point(coord the_x=0, coord the_y=0): x(the_x), y(the_y){};
        Point(const Point &a): x(a.x), y(a.y) {};
        // data members
        coord x;
        coord y;
    };

    // operators *
    Point operator*(double a, const Point &b);
    Point operator*(const Point &b,double a);
    double operator*(const Point &a, const Point &b);

    // operators +
    Point operator+(const Point &a, const Point &b);
    Point operator+(double a, const Point &b);
    Point operator+(const Point &b, double a);

    // operators -
    Point operator-(const Point &a, const Point &b);
    Point operator-(const Point &b);

    // logic operators
    bool operator== (const Point &a, const Point &b);
    bool operator!= (const Point &a, const Point &b);

    // space functions
    Point Center(const Point &a, const Point &b) ;
    Point Norm(const Point &a);


    /// @class Segment
    /// Straight line in 2D space. Consisted of two points.
    struct Segment{
        Segment(coord x1, coord y1, coord x2, coord y2) : p0(x1, y1), p1(x2, y2) {}
        Segment(Point a, Point b) : p0(a), p1(b) {}
        // data members
        Point p0;
        Point p1;
    };


    /// @class Polygon
    /// List of points in 2D space. Not necessary to be closed polygon.
    struct Polygon : public std::vector<Point> {
        void Add(coord x, coord y){
            push_back(Point(x,y));
        }
        void FillVoroPolygon(VoronoiPolygonBoost & out) const {
            if (empty()) return;
            // add all
            for ( const Point& p : *this ) out.outer().push_back(VoronoiPointBoost(p.x,p.y));
            // add first again to close shape if necessary
            if ( front() != back() ) out.outer().push_back(VoronoiPointBoost(front().x,front().y));
            // correct geometry
            boost::geometry::correct(out);
        }
    };


    /// @typedef PolygonList
    /// List of polygons.
    typedef std::vector<Polygon> PolygonList;


    /// @class SegmentList
    /// List of segments.
    struct SegmentList : public std::vector<Segment> {
        void Add(coord x1, coord y1, coord x2, coord y2) {
            push_back(Segment(x1,y1,x2,y2));
        }
    };


    /// @class Diagram
    /// Tool for calculating and storing diagram.
    struct Diagram : public asg::AsgTool  {
        Diagram ( const std::string & name);
        // functions
        StatusCode initialize();
        float getCellArea( const coord x, const  coord y) const;
        size_t findPointIndex(const Point &a) const;
        void clearDiagram();
        StatusCode createVoronoiDiagram();
        Point interpolateInfinityVertex(const int i_a, const int i_b);
        bool checkSameNumber(double in, double out, const std::string & description);
        StatusCode checkStatus(const VoronoiBoost &vd, size_t n_cells_processed);
        double intersectionAndArea(JetVoronoiDiagramHelpers::Polygon const & geo1, JetVoronoiDiagramHelpers::Polygon const & geo2, JetVoronoiDiagramHelpers::Polygon &out);

        // Data members
        double m_x_min;
        double m_x_max;
        double m_y_min;
        double m_y_max;
        double m_scaleIntFloat;
        Polygon m_borders;
        Polygon m_voro_vtx;
        std::vector<double> m_area_cells;
        double m_area_borders;
        // Polygon m_voro_cells; //!< needed for future implementation of "within"
        size_t m_N_points;
    };
} // JetVoronoiDiagramHelpers


// interfacing  Helpers to Boost
//using JetVoronoiDiagramHelpers::coord;
//using JetVoronoiDiagramHelpers::Point;
//using JetVoronoiDiagramHelpers::Segment;
//using JetVoronoiDiagramHelpers::SegmentList;
//using JetVoronoiDiagramHelpers::Polygon;
//using JetVoronoiDiagramHelpers::PolygonList;
//using JetVoronoiDiagramHelpers::Diagram;

namespace boost {
    namespace polygon {
        // Point concept
        template <>
            struct geometry_concept<JetVoronoiDiagramHelpers::Point> {
                typedef point_concept type;
            };
        template <>
            struct point_traits<JetVoronoiDiagramHelpers::Point> {
                typedef JetVoronoiDiagramHelpers::coord coordinate_type;
                static inline coordinate_type get(
                        const JetVoronoiDiagramHelpers::Point& point, orientation_2d orient) {
                    return (orient == HORIZONTAL) ? point.x : point.y;
                }
            };
        template <>
            struct point_mutable_traits<JetVoronoiDiagramHelpers::Point> {
                typedef JetVoronoiDiagramHelpers::coord coordinate_type;
                static inline void set(JetVoronoiDiagramHelpers::Point& point, orientation_2d orient, int value) {
                    if(orient == HORIZONTAL)
                        point.x = value;
                    else
                        point.y = value;
                }
                static inline JetVoronoiDiagramHelpers::Point construct(int x_value, int y_value) {
                    JetVoronoiDiagramHelpers::Point retval;
                    retval.x = x_value;
                    retval.y = y_value; 
                    return retval;
                }
            };

        // Segment concept
        template <>
            struct geometry_concept<JetVoronoiDiagramHelpers::Segment> {
                typedef segment_concept type;
            };
        template <>
            struct segment_traits<JetVoronoiDiagramHelpers::Segment> {
                typedef JetVoronoiDiagramHelpers::coord coordinate_type;
                typedef JetVoronoiDiagramHelpers::Point point_type;
                static inline point_type get(const JetVoronoiDiagramHelpers::Segment& segment, direction_1d dir) {
                    return dir.to_int() ? segment.p1 : segment.p0;
                }
            };
    }  // polygon
} // boost

#endif

