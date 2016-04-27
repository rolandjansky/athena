/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SHAPECREATOR_H
#define SHAPECREATOR_H

class TLine;
class TArc;
class TBox;
class TArrow;
class TPolyLine;
class TText;
class TEllipse;

//namespace CLHEP {
//class Hep3Vector;
//}

class TVector3;

class ShapeCreator {
public:

    ShapeCreator();
    void   setProjection(int p);
//    void   setProjection(int p, double parameter = 1.);
    TLine* createLine(const TVector3& pos, const TVector3& dir, double length);
    TLine* createLine(const TVector3& pos1, const TVector3& pos2);
    TLine* createOrthogonalLine(const TVector3& pos, const TVector3& dir, double length);
    TArrow* createArrow(const TVector3& pos, const TVector3& dir, double length);
    TPolyLine* createTriangle(const TVector3& pos, const TVector3& dir, double length);

    TArc*  createArc(const TVector3& pos, double r);
    TBox*  createBox(const TVector3& upperleft, const TVector3& lowerright);
    TText* createText(const TVector3& pos, const char* text);
    TEllipse* createEllipse(const TVector3& pos, double r1, double r2);
private:
    void   applyProjection(const TVector3& pos, double& x1, double& x2);
    void   applyDirectionProjectionUnit(const TVector3& dir, double& x1, double& x2, const TVector3& pos);
    int m_projection;
    //double m_parameter;
};




#endif
