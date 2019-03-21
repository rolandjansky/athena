/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOUTILS_H
#define GEOUTILS_H
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include "TrkSurfaces/PlaneSurface.h"
#include <vector>
 
namespace NSWL1{
    using Vertex=boost::geometry::model::d2::point_xy<double>;
    using Polygon=boost::geometry::model::polygon<Vertex>;
    using Vertices=std::vector<Vertex>;
	    
    //to convert from cartesian coordinates
     float eta(float,float,float);
     float phi(float,float);
     float phi(float,float,float);
    //a function that convers a polygon in eta/phi to x,y on a  a Z-plane.
     Polygon etaphi2xyTransform(const Polygon&,float);
     
    //given a polygon written on a plane Z=Z0, move it into another Z=Z1 plane keeping eta phi same
     Polygon Project(const Polygon& ,float ,float );
        
        
    //Print something on the screen in a well defined format
     Polygon buildPolygon(const Vertices&);//hm
     
    template <class T>
     void print(const T& t){std::cout<<boost::geometry::wkt(t)<<std::endl;}
    //given two  polygons, the number of intersections can be more than one in general
    //even though we do not expect it for sTGC pad gemotery let us implement it here just in case
        
    //Compute the area of a polygon
     float area(const Polygon&);
        
    //compute the geometric center of a polygon
     Vertex centroid(const Polygon&);
        
     std::vector<Polygon> allIntersections(const Polygon&, const Polygon&);
        
    //get the first of the intersections of two polygons. We expect to get only one of them for sTGC pad geometry
     Polygon firstIntersection(const Polygon&,const Polygon&);

    //given  two polygons, calculate the overlap with the largest area       
     Polygon largestIntersection(const Polygon&,const Polygon&);
    //given a set of polygons calculate the overlap if there is any at the end...
        
    //we take the first overlap(s) into account. For the pad geometry we, in principle, should have only one overlap at each selected pair of layers
     Polygon intersectionRegion(const std::vector<Polygon>&);

    //to access vertical& horizontal coordinates of a given vertex
    template <int XY>
     float coordinate(const Vertex& v){
        return boost::geometry::get<XY>(v);
    }
        
    //given a polygon, return the number of vertices... (keep in mind that a polygon is a closed loop in our sense so first and the last vertices are the same)
     int nVertices(const Polygon&);
        
     Vertex getVertex(const Polygon&,unsigned int);
        
    //Given a polygon get minimum and maximum bounds of a vertical or horizontal coordinate
    template <int XY>
     std::pair<float,float> coordinateRange(const Polygon& p){
      std::vector<float> values;
        for(auto vertex : boost::geometry::exterior_ring(p)){
            float currVal=boost::geometry::get<XY>(vertex); 
            values.push_back(currVal);
        }
        std::sort(values.begin(),values.end() );
        return std::make_pair(values[0],values.back());
   }
        
    
    Polygon globalToLocal(const Polygon& ,float z,const Trk::PlaneSurface & );//use Z-depth of the given polygon in 3D
    
    
    //Absorbing  Element : a closed loop made up of 4 points (0,0)
     Polygon NilPolygon();
        
        

 }
 
#endif
