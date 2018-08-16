#ifndef GEOUTILS_H
#define GEOUTILS_H
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <vector>


//You don't have to instantiate the class. Most methods are static and are wrappers on top of boost ones
 
/*
 author :   Serhat Istin
 */ 
 
 
typedef boost::geometry::model::d2::point_xy<double> Vertex;//A two dimensional vertex model
typedef boost::geometry::model::polygon<Vertex> Polygon;//A polygon made out of "Vertices"...
typedef std::vector<Vertex> Vertices;

 class GeoUtils{
    public:
        static GeoUtils* init();
        ~GeoUtils();
        
        //to convert from cartesian coordinates
        static float eta(float,float,float);
        static float phi(float,float);
        static float phi(float,float,float);
        //write a function that convers a polygon in eta/phi to x,y on a  a Z-plane.
        static Polygon etaphi2xyTransform(const Polygon&,float);

        
        //given a polygon written on a plane Z=Z0, move it into another Z=Z1 plane keeping eta phi same
        static Polygon Project(const Polygon& ,float ,float );
        
        
        //Print something on the screen in a well defined format
        static Polygon buildPolygon(const Vertices);
        template <class T>
        static void print(const T& t){std::cout<<boost::geometry::wkt(t)<<std::endl;}
        //given two  polygons, the number of intersections can be more than one in general
        //even though we do not expect it for sTGC pad gemotery let us implement it here just in case
        
        //Compute the area of a polygon
        static float area(const Polygon&);
        
        //compute the geometric center of a polygon
        static Vertex centroid(const Polygon&);
        
        static std::vector<Polygon> allIntersections(const Polygon&, const Polygon&);
        
        //get the first of the intersections of two polygons. We expect to get only one of them for sTGC pad geometry
        static Polygon firstIntersection(const Polygon&,const Polygon&);

        //given a two polygon calculate the overlap with largest area       
        static Polygon largestIntersection(const Polygon&,const Polygon&);
        //given a set of polygons calculate the overlap if there is any at the end...
        //we take the first overlap(s) into account. For the pad geometry we, in principle, should have only one overlap at each selected pair of layers
        static Polygon intersectionRegion(const std::vector<Polygon>&);

        //to access vertical& horizontal coordinates of a given vertex
        template <int XY>
        static float coordinate(const Vertex& v){
            return boost::geometry::get<XY>(v);
        }
        
        //given a polygon, return the number of vertices... (keep in mind that a polygon is a closed loop in our sense so first and the last vertices are the same)
        static int nVertices(const Polygon&);
        
        //write a function : given a polygon, return the nth vertex...
        static Vertex getVertex(const Polygon&,unsigned int);
        
        //Given a polygon get minimum and maximum bounds of a vertical or horizontal coordinate
        template <int XY>
        static std::pair<float,float> coordinateRange(const Polygon& p){
          std::vector<float> values;
            for(auto vertex : boost::geometry::exterior_ring(p)){
            float currVal=boost::geometry::get<XY>(vertex); 
            values.push_back(currVal);
          }
          std::sort(values.begin(),values.end() );
         return std::make_pair(values[0],values.back());
       }
        
        
        //Absorbing  Element : a closed loop made up of 4 points (0,0)
        static Polygon NilPolygon();
        
        
    private :
        GeoUtils();
        static GeoUtils* m_instance;
 };
 
#endif
