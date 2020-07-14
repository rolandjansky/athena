/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMomentTools/JetVoronoiDiagramHelpers.h"


using boost::geometry::area;
using boost::geometry::intersection;


namespace JetVoronoiDiagramHelpers{
    // operators *
    Point operator*(double a, const Point &b){
        coord x = (a * b.x);
        coord y = (a * b.y);
        return Point (x,y);
    }

    Point operator*(const Point &b,double a){
        return a * b;
    }

    double operator*(const Point &a, const Point &b){
        coord x = (a.x * b.x);
        coord y = (a.y * b.y);
        return x+y;
    }


    // operators +
    Point operator+(const Point &a, const Point &b){
        coord x = (a.x + b.x);
        coord y = (a.y + b.y);
        return Point (x,y);
    }

    Point operator+(double a, const Point &b){
        coord x = (a + b.x);
        coord y = (a + b.y);
        return Point (x,y);
    }

    Point operator+(const Point &b, double a){
        return a+b;
    }


    // operators -
    Point operator-(const Point &a, const Point &b){
        coord x = (a.x - b.x);
        coord y = (a.y - b.y);
        return Point (x,y);
    }

    Point operator-(const Point &b){
        return -1*b;
    }


    // logic operators
    bool operator== (const Point &a, const Point &b){
        bool isSame = true;
        isSame &= (a.x == b.x);
        isSame &= (a.y == b.y);
        return isSame;
    }

    bool operator!= (const Point &a, const Point &b){
        return ! (a==b);
    }


    // space functions
    Point Center(const Point &a, const Point &b) {
        return  0.5* (a+b);
    }

    Point Norm(const Point &a) {
        return  Point(a.y, -a.x);
    }



    Diagram::Diagram(const std::string & name)
        : asg::AsgTool(name)
        , m_x_min(-10.)
        , m_x_max( 10.)
        , m_y_min(-4. )
        , m_y_max( 4. )
        , m_scaleIntFloat( 1 )
        , m_area_borders( 0 )
        , m_N_points( 0 )
    {
    }


    StatusCode Diagram::initialize() {
        clearDiagram();
        // recalculate borders
        double w_x =  (m_x_max - m_x_min) ;
        double w_y =  (m_y_max - m_y_min) ;
        if (w_x<=0 || w_y<=0){
            ATH_MSG_WARNING( "Wrong range setting x: " << m_x_min << " .. " << m_x_max << " y " << m_y_min << " .. " << m_y_max );
            return StatusCode::FAILURE;
        }
        m_scaleIntFloat = ( (w_x >= w_y ) ? 1e7/w_x : 1e7/w_y );
        m_borders.Add( m_x_min, m_y_min );
        m_borders.Add( m_x_max, m_y_min );
        m_borders.Add( m_x_max, m_y_max );
        m_borders.Add( m_x_min, m_y_max );
        m_area_borders = w_x*w_y;
        ATH_MSG_INFO( "Initialized with range x: " << m_x_min << " .. " << m_x_max << " y " << m_y_min << " .. " << m_y_max );
        return StatusCode::SUCCESS;
    }


    float Diagram::getCellArea(const coord x, const coord y) const{
        Point p ( x , y );
        size_t i_p = findPointIndex(p);
        if ( i_p >= m_N_points ) {
            ATH_MSG_WARNING("Can not find point : " << p.x << " " << p.y);
            return 0;
        }
        return m_area_cells[i_p];
    }


    size_t Diagram::findPointIndex(const Point &a) const{
        size_t index=0;
        for (const Point& b: m_voro_vtx){
            if (a==b) return index;
            index++;
        }
        index++;
        return index;
    }


    void Diagram::clearDiagram(){
        m_voro_vtx   .clear();
        m_area_cells .clear();
        // m_voro_cells.clear();
        m_N_points = 0;
    }


    StatusCode Diagram::createVoronoiDiagram(){
        m_N_points = m_voro_vtx.size();
        if (m_N_points==0) {
            ATH_MSG_WARNING( "Trying to create diagram withou any input");
            return StatusCode::FAILURE;
        }
        Polygon mypts;
        for (Point p : m_voro_vtx) mypts.Add(p.x*m_scaleIntFloat,p.y*m_scaleIntFloat);
        VoronoiBoost vd;
        // boost make voronoi
        construct_voronoi(mypts.begin(), mypts.end(), &vd);
        int cell_i = 0;
        for (VoronoiCellBoost cell : vd.cells() ){ // for all cells in diagram
            Polygon cell_area_tmp; // temprary polygon of cell
            const VoronoiEdgeBoost * edge = cell.incident_edge(); // first edge
            int csi = cell.source_index(); // index of cell
            do { // for all edges which the cell has
                const VoronoiVtxBoost * vtx0 = edge->vertex0();
                const VoronoiVtxBoost * vtx1 = edge->vertex1();
                int csi_ngbr = edge->twin()->cell()->source_index() ; // index of neighbour cell
                if (edge->is_primary()){ // is nonintersecting edge (or something like that)
                    // first vertex in infinity -- interpolate
                    if ( vtx0==0 ) cell_area_tmp.push_back( interpolateInfinityVertex( csi, csi_ngbr ) );
                    // adding only second vertex if exists
                    if ( vtx1!=0 ) cell_area_tmp.Add(vtx1->x()/m_scaleIntFloat, vtx1->y()/m_scaleIntFloat); // correct for scale
                    // second vertex in infinity -- interpolate (notice the change of index order)
                    else cell_area_tmp.push_back( interpolateInfinityVertex( csi_ngbr, csi ) );
                }
                edge=edge->next();
            } while (edge != cell.incident_edge() );
            // intersection and are calculation
            Polygon cell_area; 
            double areasize = intersectionAndArea(cell_area_tmp,m_borders,cell_area);
            // m_voro_cells.push_back(cell_area); // save cell polygon
            m_area_cells.push_back(areasize);
            cell_i++;
        }
        return checkStatus(vd,cell_i);
    }


    Point Diagram::interpolateInfinityVertex(const int i_a, const int i_b){
        double sign = 300; // to limit infinity 
        Point A = m_voro_vtx[i_a];
        Point B = m_voro_vtx[i_b];
        return Center(A,B) + Norm(B-A) * sign ;
    }


    bool Diagram::checkSameNumber(double in, double out, const std::string & description){
        if ( fabs(in-out)/in > 1e4) {
            ATH_MSG_WARNING("Difference in " << description.c_str() << ": " << in << " (should be " << out << ")");
            return false;
        };
        return true;
    }


    StatusCode Diagram::checkStatus(const VoronoiBoost &vd, size_t n_cells_processed){
        bool isAllOK = true;
        double sum = 0; for (double voroarea : m_area_cells) sum+=voroarea;
        isAllOK &= checkSameNumber( vd.cells().size()  , m_voro_vtx.size(), "number of boost cells" );
        isAllOK &= checkSameNumber( n_cells_processed  , m_voro_vtx.size(), "number of processed cells" );
        isAllOK &= checkSameNumber( m_area_cells.size(), m_voro_vtx.size(), "number of calculated areas" );
        isAllOK &= checkSameNumber( sum                , m_area_borders   , "area sum" );
        if(!isAllOK){
            m_N_points = 0;
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }


    double Diagram::intersectionAndArea(Polygon const & geo1, Polygon const & geo2, Polygon &out){
        VoronoiPolygonBoost pol1, pol2;
        std::deque<VoronoiPolygonBoost> pol3; // list of poly's  because intersection could be more than one polygon (not in our case)
        geo1.FillVoroPolygon(pol1);
        geo2.FillVoroPolygon(pol2);
        // boost make intersection
        intersection(pol1,pol2,pol3);
        if ( pol3.size() != 0 ){ // it has something our shape is on first place
            for ( const VoronoiPointBoost vp: pol3.at(0).outer() ) out.Add(vp.x(),vp.y());
        }
        else return 0;
        // boost calculate area
        return area(pol3.at(0));
    }

}
