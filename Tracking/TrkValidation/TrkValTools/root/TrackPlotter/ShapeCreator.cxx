/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ShapeCreator.h"

#include "TLine.h"
#include "TArc.h"
#include "TBox.h"
#include "TArrow.h"
#include "TPolyLine.h"
#include "TText.h"
//#include "CLHEP/Vector/ThreeVector.h"
#include "TVector3.h"

#include <iostream>
//#include <math.h>
#include "TMath.h"

ShapeCreator::ShapeCreator() :
    m_projection(0){
}

//void ShapeCreator::applyProjection(const CLHEP::Hep3Vector& pos, double& x1, double& x2) {
void ShapeCreator::applyProjection(const TVector3& pos, double& x1, double& x2) {
    //void ShapeCreator::applyProjection(const Hep3Vector& pos, double& x1, double& x2){

    if(m_projection == 0) {
        x1 = pos.x();
        x2 = pos.y();
    } else if(m_projection == 1) {
        x1 = pos.x();
        x2 = pos.z();
    } else if(m_projection == 2) {
        x1 = pos.z();
        x2 = pos.y();
    } else if(m_projection == 3) {
        // Cylindrical Coordinates: Z Phi

        //const double phiFactor = 180. / TMath::Pi(); // use deg
        const double phiFactor = 1000.; // use mrad
        x1 = pos.z();
        x2 = pos.Phi() * phiFactor;

    } else if(m_projection == 4) {
        // Cylindrical Coordinates: Z vs. R*Phi
        x1 = pos.z();
        x2 = pos.Phi() * pos.Perp();
    }

}

void ShapeCreator::applyDirectionProjectionUnit(const TVector3& dir, double& x1, double& x2, const TVector3& pos) {
    //void ShapeCreator::applyProjection(const Hep3Vector& pos, double& x1, double& x2){

    if(m_projection == 0) {
        x1 = dir.x();
        x2 = dir.y();
    } else if(m_projection == 1) {
        x1 = dir.x();
        x2 = dir.z();
    } else if(m_projection == 2) {
        x1 = dir.z();
        x2 = dir.y();
    } else if(m_projection == 3) {
        // Cylindrical Coordinates: Z Phi
        //const double phiFactor = 180. / TMath::Pi(); // use deg
        const double phiFactor = 1000.; // use mrad
        x1 = dir.z();
        // directions:
        TVector3 pos2(pos+dir);
        x2 = (pos2.Phi() - pos.Phi()) * phiFactor;
//         if (radius != 0.){
//             //x2 = atan(sqrt(pow(dir.x(),2.)+pow(dir.y(),2.))/radius) * phiFactor;
//             x2 = atan(dir.Perp()/radius) * phiFactor;
//             //std::cout << "radius: " << radius << ", dir.Perp:" << dir.Perp() << std::endl;
//         } else {
//             x2=0.;
//         }

    } else if(m_projection == 4) {
        // Cylindrical Coordinates: Z vs. R*Phi
        x1 = dir.z();
        x2 = dir.Phi() * dir.Perp();
    }
 
    double length = sqrt(x1*x1 + x2*x2);
    if (length >0) {
        x1/=length;
        x2/=length;
    }

}

void ShapeCreator::setProjection(int p) {
//void ShapeCreator::setProjection(int p, double parameter) {
    if(p != 0 && p != 1 && p != 2 && p != 3  && p != 4)
        std::cout << "ERROR Wrong projection " << std::endl;
    m_projection = p;
    //m_parameter = parameter;
//    std::cout << "ShapeCreator:  using projection type " << m_projection << std::endl;
}


//TLine* ShapeCreator::createLine(const CLHEP::Hep3Vector& pos, const CLHEP::Hep3Vector& dir, double length) {
TLine* ShapeCreator::createLine(const TVector3& pos, const TVector3& dir, double length) {
    double x1,x2;
    double dx1,dx2;

    applyProjection(pos,x1,x2);
    applyDirectionProjectionUnit(dir,dx1,dx2, pos);
//    std::cout << "ShapeCreator:  creating line from (" << x1 << "," << x2 << ") to ("<< dx1 << "," << dx2 << ") " << std::endl;

    double xu1 = x1 + 0.5*dx1*length;
    double xu2 = x2 + 0.5*dx2*length;
    double xl1 = x1 - 0.5*dx1*length;
    double xl2 = x2 - 0.5*dx2*length;

   return new TLine(xu1,xu2,xl1,xl2);
//   return new TLine(x1, x2, x1+dx1*length, x2+dx2*length);
}

TLine* ShapeCreator::createLine(const TVector3& pos1, const TVector3& pos2) {
    double x1,y1;
    double x2,y2;

    applyProjection(pos1,x1,y1);
    applyProjection(pos2,x2,y2);

   return new TLine(x1,y1,x2,y2);
}

TLine* ShapeCreator::createOrthogonalLine(const TVector3& pos, const TVector3& dir, double length){
    double x1,x2;
    double dx1,dx2;

    applyProjection(pos,x1,x2);
    applyDirectionProjectionUnit(dir,dx1,dx2,pos);

    double xu1 = x1 + 0.5*dx2*length;
    double xu2 = x2 - 0.5*dx1*length;
    double xl1 = x1 - 0.5*dx2*length;
    double xl2 = x2 + 0.5*dx1*length;

   return new TLine(xu1,xu2,xl1,xl2);
//   return new TLine(x1, x2, x1-dx2*length, x2+dx1*length);
}

TArrow* ShapeCreator::createArrow(const TVector3& pos, const TVector3& dir, double length) {
    double x1,x2;
    double dx1,dx2;

    applyProjection(pos,x1,x2);
    applyDirectionProjectionUnit(dir,dx1,dx2, pos);
//    std::cout << "ShapeCreator:  creating line from (" << x1 << "," << x2 << ") to ("<< dx1 << "," << dx2 << ") " << std::endl;

//   return new TLine(xu1,xu2,xl1,xl2);
   return new TArrow(x1, x2, x1+dx1*length, x2+dx2*length);
}

TPolyLine* ShapeCreator::createTriangle(const TVector3& pos, const TVector3& dir, double length){
    double x1,x2;
    double dx1,dx2;

    applyProjection(pos,x1,x2);
    applyDirectionProjectionUnit(dir,dx1,dx2,pos);
    Double_t x[3] = { x1+(dx2/2.)*length ,x1+dx1*length ,x1-(dx2/2.)*length};
    Double_t y[3] = { x2-(dx1/2.)*length ,x2+dx2*length, x2+(dx1/2.)*length};
//    Double_t x[3] = { x1-(dx1-dx2/2.)*length ,x1 ,x1-(dx1+dx2/2.)*length};
//    Double_t y[3] = { x2-(dx2+dx1/2.)*length ,x2, x2-(dx2-dx1/2.)*length};
    return new TPolyLine(3,x,y);
}

//TArc*  ShapeCreator::createArc(const CLHEP::Hep3Vector& pos, double r) {
TArc*  ShapeCreator::createArc(const TVector3& pos, double r) {
    double x1,x2;
    applyProjection(pos,x1,x2);
//    std::cout << "ShapeCreator:  creating circle at (" << x1 << "," << x2 << ") with radius "<< r  << std::endl;

    return new TArc(x1,x2,r);
}

//TBox*  ShapeCreator::createBox(const CLHEP::Hep3Vector& upperleft, const CLHEP::Hep3Vector& lowerright) {
TBox*  ShapeCreator::createBox(const TVector3& upperleft, const TVector3& lowerright) {
    double x1,x2;
    double y1,y2;

    applyProjection(upperleft,x1,x2);
    applyProjection(lowerright,y1,y2);


    return new TBox(x1,x2,y1,y2);
}

TText* ShapeCreator::createText(const TVector3& pos, const char* text) {
    double x1,x2;
    applyProjection(pos,x1,x2);
    return new TText(x1, x2, text);
}

TEllipse*  ShapeCreator::createEllipse(const TVector3& pos, double r1, double r2) {
    double x1,x2;
    applyProjection(pos,x1,x2);
//    std::cout << "ShapeCreator:  creating circle at (" << x1 << "," << x2 << ") with radius "<< r  << std::endl;

    return new TEllipse(x1, x2, r1, r2);
}
