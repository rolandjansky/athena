/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1NSWSimTools/GeoUtils.h"

int main(){
    
    using namespace NSWL1;
    
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
    
    
    Polygon pol1=buildPolygon(vts1);
    Polygon pol2=buildPolygon(vts2);
    Polygon pol3=buildPolygon(vts3);
    print(pol1);
    print(pol2);
    print(pol3);
    
    std::cout<<"polygon 1 has "<<nVertices(pol1)<<" vertices"<<std::endl;
    Vertex v=getVertex(pol1,3);
    std::cout<<"Whose fourth vertex coordinate of x is :"<<coordinate<0>(v)<<std::endl;
    std::cout<<"Whose fourth vertex coordinate of y is :"<<coordinate<1>(v)<<std::endl;
    
    
    std::vector<Polygon> overlaps=allIntersections(pol1,pol3);
    //test the firstoverlap method
    Polygon overlap0_13=firstIntersection(pol1,pol3);
    std::cout<<"There are "<<overlaps.size()<<" intersections in between polygon1-3"<<std::endl;
    print(overlap0_13);
    
    std::vector<Polygon> polygons={pol1,pol2,pol3};
    
    Polygon overallintersection=intersectionRegion(polygons);
    
    unsigned int nvtx=nVertices(overallintersection);
    std::cout<<"The resulting overlap between (1)/(2)/(3) : "<<std::endl;
    print(overallintersection);
    
    std::cout<<"X-Y Coordinates of the overlapping region polygons 1-2-3 are : "<<std::endl;
    for(unsigned int i=0;i<nvtx;i++){
        Vertex v=getVertex(overallintersection,i);
        std::cout<<coordinate<0>(v)<<" "<<coordinate<1>(v)<<std::endl;
    }
    
    std::cout<<"Area of the overlap (1)/(2)/(3) : "<<area(overallintersection)<<std::endl;
    
    return 0;
}
