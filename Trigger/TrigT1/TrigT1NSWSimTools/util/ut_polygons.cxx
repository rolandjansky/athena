#include "TrigT1NSWSimTools/GeoUtils.h"

int main(){
    
    //GeoUtils *geo=GeoUtils::init();
    
    Vertices vts1;
    //a trapezoid
    vts1.push_back(Vertex(0,0));
    vts1.push_back(Vertex(0,10));
    vts1.push_back(Vertex(10,10));
    vts1.push_back(Vertex(10,0));
    vts1.push_back(Vertex(0,0));
    
    //a rectangle
    std::vector<Vertex> vts2={
            Vertex(3,3),
            Vertex(3,12),
            Vertex(12,12),
            Vertex(12,3),
            Vertex(3,3)
    };
    
    std::vector<Vertex> vts3={
      Vertex(8,8),
      Vertex(8,15),
      Vertex(16,15),
      Vertex(16,8),
      Vertex(8,8)
    };
    
    
    Polygon pol1=GeoUtils::buildPolygon(vts1);
    Polygon pol2=GeoUtils::buildPolygon(vts2);
    Polygon pol3=GeoUtils::buildPolygon(vts3);
    GeoUtils::print(pol1);
    GeoUtils::print(pol2);
    GeoUtils::print(pol3);
    
    std::cout<<"polygon 1 has "<<GeoUtils::nVertices(pol1)<<" vertices"<<std::endl;
    Vertex v=GeoUtils::getVertex(pol1,3);
    std::cout<<"Whose fourth vertex coordinate of x is :"<<GeoUtils::coordinate<0>(v)<<std::endl;
    std::cout<<"Whose fourth vertex coordinate of y is :"<<GeoUtils::coordinate<1>(v)<<std::endl;
    
    
    std::vector<Polygon> overlaps=GeoUtils::allIntersections(pol1,pol3);
    //test the firstoverlap method
    Polygon overlap0_13=GeoUtils::firstIntersection(pol1,pol3);
    std::cout<<"There are "<<overlaps.size()<<" intersections in between polygon1-3"<<std::endl;
    GeoUtils::print(overlap0_13);
    
    std::vector<Polygon> polygons={pol1,pol2,pol3};
    
    Polygon overallintersection=GeoUtils::intersectionRegion(polygons);
    
    unsigned int nvtx=GeoUtils::nVertices(overallintersection);
    std::cout<<"The resulting overlap between (1)/(2)/(3) : "<<std::endl;
    GeoUtils::print(overallintersection);
    
    std::cout<<"X-Y Coordinates of the overlapping region polygons 1-2-3 are : "<<std::endl;
    for(unsigned int i=0;i<nvtx;i++){
        Vertex v=GeoUtils::getVertex(overallintersection,i);
        std::cout<<GeoUtils::coordinate<0>(v)<<" "<<GeoUtils::coordinate<1>(v)<<std::endl;
    }
    
    std::cout<<"Area of the overlap (1)/(2)/(3) : "<<GeoUtils::area(overallintersection)<<std::endl;
    
    return 0;
}
