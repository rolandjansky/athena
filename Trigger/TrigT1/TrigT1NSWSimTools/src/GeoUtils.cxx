#include "TrigT1NSWSimTools/GeoUtils.h"
#include <TMath.h>
#include <TVector3.h>
#include <algorithm>

float GeoUtils::eta(float x,float y,float z){
    TVector3 V(x,y,z);
    return V.Eta();
}


float GeoUtils::phi(float x,float y){
    TVector2 V(x,y); 
   float angle= V.Phi();
   return TVector2::Phi_mpi_pi(angle);
}

float GeoUtils::phi(float x, float y, float z){
    TVector3 V(x,y,z);
    return V.Phi();
    
}

Polygon GeoUtils::etaphi2xyTransform(const Polygon& p, float Z){
    //loop over vertices :
    Vertices vts;
    for(auto v : boost::geometry::exterior_ring(p)){
     float eta=GeoUtils::coordinate<0>(v);
     float phi=GeoUtils::coordinate<1>(v);
     float z=Z;
     float theta=2*TMath::ATan(TMath::Exp(-1*eta));
     float x=z*TMath::Tan(theta)*TMath::Cos(phi);
     float y=z*TMath::Tan(theta)*TMath::Sin(phi);
     vts.push_back(Vertex(x,y));
    }
    return buildPolygon(vts);
}





Polygon GeoUtils::Project(const Polygon& p,float Zinit,float Zfin){
    //loop over vertices
    Vertices vt;
    for(auto v : boost::geometry::exterior_ring(p)){
        float x=coordinate<0>(v);
        float y=coordinate<1>(v);
        float z=Zinit;        
        float et=eta(x,y,z);
        float p=phi(x,y,z);
        float theta=2*TMath::ATan(TMath::Exp(-1*et));
        float r=Zfin*TMath::Tan(theta);
        float xproj=r*TMath::Cos(p);
        float yproj=r*TMath::Sin(p);
        vt.push_back(Vertex(xproj,yproj));
    }
    return buildPolygon(vt);
    
    
}

int GeoUtils::nVertices(const Polygon& p){
    return boost::geometry::num_points(p);
}

Vertex GeoUtils::getVertex(const Polygon& p,unsigned int i){
    
    return boost::geometry::exterior_ring(p)[i];
}

GeoUtils* GeoUtils::m_instance=NULL;

GeoUtils* GeoUtils::init(){
    if(m_instance==NULL){
        m_instance=new GeoUtils();
    }
    return m_instance;
}

GeoUtils::GeoUtils(){
}

GeoUtils::~GeoUtils(){
    delete m_instance;
}

Polygon GeoUtils::buildPolygon(const Vertices pts){
    Polygon shape;
    Vertices vts=pts;
    //to complete the loop...
    vts.push_back(vts[0]);
    
    boost::geometry::assign_points(shape, pts);
    boost::geometry::correct(shape);
    return shape;
}

float GeoUtils::area(const Polygon& p){
    return boost::geometry::area(p);
}

Vertex GeoUtils::centroid(const Polygon& p){
    Vertex v;
    boost::geometry::centroid(p, v);
    return v;
}

 std::vector<Polygon> GeoUtils::allIntersections(const Polygon& p1, const Polygon& p2){
    std::vector<Polygon> out;
    boost::geometry::intersection(p1, p2, out);
    return out;
    
}


Polygon GeoUtils::firstIntersection(const Polygon& p1,const Polygon& p2){
    std::vector<Polygon> pols=GeoUtils::allIntersections(p1,p2);
     //we have to return something
    if(pols.size()==0) return NilPolygon();
    return GeoUtils::allIntersections(p1,p2).at(0);
}

Polygon GeoUtils::largestIntersection(const Polygon& p1,const Polygon& p2){
    std::vector<Polygon> pols=GeoUtils::allIntersections(p1,p2);
     if(pols.size()==0) return NilPolygon();
     
     std::sort(pols.begin(),pols.end(),[](const Polygon& a,const Polygon& b){return GeoUtils::area(a)>GeoUtils::area(b);});
     return pols[0];
    
}



Polygon GeoUtils::intersectionRegion(const std::vector<Polygon>& polygons){
    Polygon ovl=polygons[0];
    for(unsigned int i=1;i<polygons.size();i++){
        Polygon curr=polygons[i];
        Polygon overlap=GeoUtils::largestIntersection(curr,ovl);
        ovl=overlap;
    }
    return ovl;
}





Polygon GeoUtils::NilPolygon(){
    Vertices nil={
        Vertex(0,0),
        Vertex(0,0),
        Vertex(0,0),
        Vertex(0,0),
        Vertex(0,0) 
    };
    return GeoUtils::buildPolygon(nil);
}
