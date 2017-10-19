/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardRegionGeoModelFactory.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoEllipticalTube.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Sin.hh"
#include "CLHEP/GenericFunctions/Cos.hh"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Units/SystemOfUnits.h"


GeoPhysVol* ForwardRegionGeoModelFactory::insertMagnetEnvelope(std::string name, double x, double y, double z, double rotationAngle, double diameter, double halfL, double dL, GeoPhysVol* fwrPhys)
{
    const GeoTube     *tube  = new GeoTube(0, diameter/2, halfL-dL);

    HepGeom::Transform3D shift = HepGeom::Translate3D(x,y,z);
    HepGeom::Transform3D rotate = HepGeom::RotateY3D(rotationAngle);
//    HepGeom::Transform3D rotateX180 = HepGeom::RotateX3D(180*CLHEP::deg);

    const GeoShapeShift& magTube0 = (*tube)<<rotate<<shift;
//    const GeoShapeUnion& magTube = magTube0.add((*tube)<<rotate<<shift<<rotateX180);

    const GeoLogVol   *tubeLog  = new  GeoLogVol(name, &magTube0, m_MapMaterials[std::string("std::Vacuum")]);

    GeoPhysVol        *tubePhys = new GeoPhysVol(tubeLog);

    GeoNameTag *tag = new GeoNameTag(name);
    fwrPhys->add(tag);

    fwrPhys->add(tubePhys);

    return tubePhys;
}

void ForwardRegionGeoModelFactory::insertCircularElement(std::string name, double x, double y, double z, double rotationAngle, double xAperture, double yAperture, double halfL, double dL, double tubeThickness, GeoPhysVol* fwrPhys)
{
    double r0 = std::max(xAperture,yAperture)*CLHEP::mm/2;

    const GeoTube     *ringTube  = new GeoTube(r0, r0+tubeThickness, halfL-dL);

    const GeoLogVol   *ringLog  = new  GeoLogVol(name+"Log", ringTube, m_MapMaterials[std::string("Steel")]);

    GeoPhysVol        *ringPhys = new GeoPhysVol(ringLog);

    //create rotation and traslation and add them to the tree of volumes of the world (move and rotate the volume)
    GeoTransform *move           = new GeoTransform(HepGeom::Translate3D(x,y,z));
    fwrPhys->add(move);
    GeoTransform *rotate         = new GeoTransform(HepGeom::RotateY3D(rotationAngle));
    fwrPhys->add(rotate);

    GeoNameTag *tag = new GeoNameTag(name);
    fwrPhys->add(tag);

    fwrPhys->add(ringPhys);

//    // The other side of the forward region may be obtained by rotation
//    GeoTransform *rotateX180  = new GeoTransform(HepGeom::RotateX3D(180*CLHEP::deg));

//    // the other side
//    fwrPhys->add(rotateX180);
//    fwrPhys->add(move);
//    fwrPhys->add(rotate);
//    tag = new GeoNameTag(name+"_L");
//    fwrPhys->add(tag);
//    fwrPhys->add(ringPhys);
}

void ForwardRegionGeoModelFactory::insertEllipticalElement(std::string name, double x, double y, double z, double rotationAngle, double xAperture, double yAperture, double halfL, double dL, double tubeThickness, GeoPhysVol* fwrPhys)
{
    const GeoShape * ringTube0;
    GeoShape * ringTube2;

    // GeoEllipticalTube causes VP1 to fall, so for visualization GeoBox is used
    if(!m_Config.vp1Compatibility) {
        ringTube0  = new GeoEllipticalTube(xAperture*CLHEP::mm/2+tubeThickness, yAperture*CLHEP::mm/2+tubeThickness, halfL-dL);
        ringTube2  = new GeoEllipticalTube(xAperture*CLHEP::mm/2, yAperture*CLHEP::mm/2, halfL-dL);
    }
    else {
        ringTube0  = new GeoBox(xAperture*CLHEP::mm/2+tubeThickness, yAperture*CLHEP::mm/2+tubeThickness, halfL-dL);
        ringTube2  = new GeoBox(xAperture*CLHEP::mm/2, yAperture*CLHEP::mm/2, halfL-dL);
    }
    GeoShapeSubtraction * ringTube = new GeoShapeSubtraction(ringTube0, ringTube2);

    const GeoLogVol   *ringLog  = new  GeoLogVol(name+"Log", ringTube, m_MapMaterials[std::string("Steel")]);

    GeoPhysVol        *ringPhys = new GeoPhysVol(ringLog);

    //create rotation and traslation and add them to the tree of volumes of the world (move and rotate the volume)
    GeoTransform *move           = new GeoTransform(HepGeom::Translate3D(x,y,z));
    fwrPhys->add(move);
    GeoTransform *rotate         = new GeoTransform(HepGeom::RotateY3D(rotationAngle));
    fwrPhys->add(rotate);

    GeoNameTag *tag = new GeoNameTag(name);
    fwrPhys->add(tag);

    fwrPhys->add(ringPhys);

//    // The other side of the forward region may be obtained by rotation
//    GeoTransform *rotateX180  = new GeoTransform(HepGeom::RotateX3D(180*CLHEP::deg));

//    // the other side
//    fwrPhys->add(rotateX180);
//    fwrPhys->add(move);
//    fwrPhys->add(rotate);
//    tag = new GeoNameTag(name+"_L");
//    fwrPhys->add(tag);
//    fwrPhys->add(ringPhys);
}

void ForwardRegionGeoModelFactory::insertXRecticircularElement(std::string name, double x, double y, double z, double rotationAngle, double xAperture, double yAperture, double halfL, double dL, double tubeThickness, GeoPhysVol* fwrPhys)
{
    double beamScreenSeparation = 1.5*CLHEP::mm;
    double beamScreenCuThick = 0.05*CLHEP::mm;
    double beamScreenSteelThick = 1*CLHEP::mm;

    const GeoTube     *ringTube  = new GeoTube(yAperture*CLHEP::mm/2+beamScreenCuThick+beamScreenSteelThick+beamScreenSeparation, yAperture*CLHEP::mm/2+beamScreenCuThick+beamScreenSteelThick+beamScreenSeparation+tubeThickness, halfL-dL);
    const GeoTube     *circ  = new GeoTube(0, yAperture*CLHEP::mm/2, halfL-dL);
    const GeoBox      *rect  = new GeoBox(xAperture*CLHEP::mm/2, yAperture*CLHEP::mm/2, halfL-dL);
    GeoShapeIntersection *innerVac = new GeoShapeIntersection(rect,circ);

    const GeoTube     *circ2  = new GeoTube(0, yAperture*CLHEP::mm/2+beamScreenCuThick, halfL-dL);
    const GeoBox      *rect2  = new GeoBox(xAperture*CLHEP::mm/2+beamScreenCuThick, yAperture*CLHEP::mm/2+beamScreenCuThick, halfL-dL);
    GeoShapeIntersection *beamScreenCu0 = new GeoShapeIntersection(rect2,circ2);
    GeoShapeSubtraction *beamScreenCu = new GeoShapeSubtraction(beamScreenCu0, innerVac);

    const GeoTube     *circ3  = new GeoTube(0, yAperture*CLHEP::mm/2+beamScreenCuThick+beamScreenSteelThick, halfL-dL);
    const GeoBox      *rect3  = new GeoBox(xAperture*CLHEP::mm/2+beamScreenCuThick+beamScreenSteelThick, yAperture*CLHEP::mm/2+beamScreenCuThick+beamScreenSteelThick, halfL-dL);
    GeoShapeIntersection *beamScreenSteel01 = new GeoShapeIntersection(rect3,circ3);
    GeoShapeSubtraction *beamScreenSteel02 = new GeoShapeSubtraction(beamScreenSteel01, innerVac);
    GeoShapeSubtraction *beamScreenSteel = new GeoShapeSubtraction(beamScreenSteel02, beamScreenCu);

    const GeoLogVol   *ringLog  = new  GeoLogVol(name+"BeamPipe", ringTube, m_MapMaterials[std::string("Steel")]);
    const GeoLogVol   *ringLogCu = new  GeoLogVol(name+"BeamScreenCu", beamScreenCu, m_MapMaterials[std::string("Copper")]);
    const GeoLogVol   *ringLogSteel = new  GeoLogVol(name+"BeamScreenSteel", beamScreenSteel, m_MapMaterials[std::string("Steel")]);

    GeoPhysVol        *ringPhys = new GeoPhysVol(ringLog);
    GeoPhysVol        *ringPhysCu = new GeoPhysVol(ringLogCu);
    GeoPhysVol        *ringPhysSteel = new GeoPhysVol(ringLogSteel);

    //create rotation and traslation and add them to the tree of volumes of the world (move and rotate the volume)
    GeoTransform *move           = new GeoTransform(HepGeom::Translate3D(x,y,z));
    fwrPhys->add(move);
    GeoTransform *rotate         = new GeoTransform(HepGeom::RotateY3D(rotationAngle));
    fwrPhys->add(rotate);

    GeoNameTag *tag = new GeoNameTag(name+"BeamPipe");
    fwrPhys->add(tag);

    fwrPhys->add(ringPhys);
    //detectorManager->addTreeTop(ringPhys);

    // add beam scren
    fwrPhys->add(move);
    fwrPhys->add(rotate);
    tag = new GeoNameTag(name+"BeamScreenCu");
    fwrPhys->add(tag);
    fwrPhys->add(ringPhysCu);
    fwrPhys->add(move);
    fwrPhys->add(rotate);
    tag = new GeoNameTag(name+"BeamScreenSteel");
    fwrPhys->add(tag);
    fwrPhys->add(ringPhysSteel);


//    // The other side of the forward region may be obtain by rotation
//    GeoTransform *rotateX180  = new GeoTransform(HepGeom::RotateX3D(180*CLHEP::deg));

//    // the other side
//    fwrPhys->add(rotateX180);
//    fwrPhys->add(move);
//    fwrPhys->add(rotate);
//    tag = new GeoNameTag(name+"BeamPipe_L");
//    fwrPhys->add(tag);
//    fwrPhys->add(ringPhys);

//    fwrPhys->add(rotateX180);
//    fwrPhys->add(move);
//    fwrPhys->add(rotate);
//    tag = new GeoNameTag(name+"BeamScreenCu_L");
//    fwrPhys->add(tag);
//    fwrPhys->add(ringPhysCu);

//    fwrPhys->add(rotateX180);
//    fwrPhys->add(move);
//    fwrPhys->add(rotate);
//    tag = new GeoNameTag(name+"BeamScreenSteel_L");
//    fwrPhys->add(tag);
//    fwrPhys->add(ringPhysSteel);
}

void ForwardRegionGeoModelFactory::insertYRecticircularElement(std::string name, double x, double y, double z, double rotationAngle, double xAperture, double yAperture, double halfL, double dL, double tubeThickness, GeoPhysVol* fwrPhys)
{
    double beamScreenSeparation = 1.5*CLHEP::mm;
    double beamScreenCuThick = 0.05*CLHEP::mm;
    double beamScreenSteelThick = 1*CLHEP::mm;

    const GeoTube     *ringTube  = new GeoTube(xAperture*CLHEP::mm/2+beamScreenCuThick+beamScreenSteelThick+beamScreenSeparation, xAperture*CLHEP::mm/2+beamScreenCuThick+beamScreenSteelThick+beamScreenSeparation+tubeThickness, halfL-dL);
    const GeoTube     *circ  = new GeoTube(0, xAperture*CLHEP::mm/2, halfL-dL);
    const GeoBox      *rect  = new GeoBox(xAperture*CLHEP::mm/2, yAperture*CLHEP::mm/2, halfL-dL);
    GeoShapeIntersection *innerVac = new GeoShapeIntersection(rect,circ);

    const GeoTube     *circ2  = new GeoTube(0, xAperture*CLHEP::mm/2+beamScreenCuThick, halfL-dL);
    const GeoBox      *rect2  = new GeoBox(xAperture*CLHEP::mm/2+beamScreenCuThick, yAperture*CLHEP::mm/2+beamScreenCuThick, halfL-dL);
    GeoShapeIntersection *beamScreenCu0 = new GeoShapeIntersection(rect2,circ2);
    GeoShapeSubtraction *beamScreenCu = new GeoShapeSubtraction(beamScreenCu0, innerVac);

    const GeoTube     *circ3  = new GeoTube(0, xAperture*CLHEP::mm/2+beamScreenCuThick+beamScreenSteelThick, halfL-dL);
    const GeoBox      *rect3  = new GeoBox(xAperture*CLHEP::mm/2+beamScreenCuThick+beamScreenSteelThick, yAperture*CLHEP::mm/2+beamScreenCuThick+beamScreenSteelThick, halfL-dL);
    GeoShapeIntersection *beamScreenSteel01 = new GeoShapeIntersection(rect3,circ3);
    GeoShapeSubtraction *beamScreenSteel02 = new GeoShapeSubtraction(beamScreenSteel01, innerVac);
    GeoShapeSubtraction *beamScreenSteel = new GeoShapeSubtraction(beamScreenSteel02, beamScreenCu);

    const GeoLogVol   *ringLog  = new  GeoLogVol(name+"BeamPipe", ringTube, m_MapMaterials[std::string("Steel")]);
    const GeoLogVol   *ringLogCu = new  GeoLogVol(name+"BeamScreenCu", beamScreenCu, m_MapMaterials[std::string("Copper")]);
    const GeoLogVol   *ringLogSteel = new  GeoLogVol(name+"BeamScreenSteel", beamScreenSteel, m_MapMaterials[std::string("Steel")]);

    GeoPhysVol        *ringPhys = new GeoPhysVol(ringLog);
    GeoPhysVol        *ringPhysCu = new GeoPhysVol(ringLogCu);
    GeoPhysVol        *ringPhysSteel = new GeoPhysVol(ringLogSteel);

    //create rotation and traslation and add them to the tree of volumes of the world (move and rotate the volume)
    GeoTransform *move           = new GeoTransform(HepGeom::Translate3D(x,y,z));
    fwrPhys->add(move);
    GeoTransform *rotate         = new GeoTransform(HepGeom::RotateY3D(rotationAngle));
    fwrPhys->add(rotate);

    GeoNameTag *tag = new GeoNameTag(name+"BeamPipe");
    fwrPhys->add(tag);

    fwrPhys->add(ringPhys);

    // add beam scren
    fwrPhys->add(move);
    fwrPhys->add(rotate);
    tag = new GeoNameTag(name+"BeamScreenCu");
    fwrPhys->add(tag);
    fwrPhys->add(ringPhysCu);
    fwrPhys->add(move);
    fwrPhys->add(rotate);
    tag = new GeoNameTag(name+"BeamScreenSteel");
    fwrPhys->add(tag);
    fwrPhys->add(ringPhysSteel);

//    // The other side of the forward region may be obtain by rotation
//    GeoTransform *rotateX180  = new GeoTransform(HepGeom::RotateX3D(180*CLHEP::deg));

//    // the other side
//    fwrPhys->add(rotateX180);
//    fwrPhys->add(move);
//    fwrPhys->add(rotate);
//    tag = new GeoNameTag(name+"BeamPipe_L");
//    fwrPhys->add(tag);
//    fwrPhys->add(ringPhys);

//    fwrPhys->add(rotateX180);
//    fwrPhys->add(move);
//    fwrPhys->add(rotate);
//    tag = new GeoNameTag(name+"BeamScreenCu_L");
//    fwrPhys->add(tag);
//    fwrPhys->add(ringPhysCu);

//    fwrPhys->add(rotateX180);
//    fwrPhys->add(move);
//    fwrPhys->add(rotate);
//    tag = new GeoNameTag(name+"BeamScreenSteel_L");
//    fwrPhys->add(tag);
//    fwrPhys->add(ringPhysSteel);
}

void ForwardRegionGeoModelFactory::insertTrousersElement(std::string name, double x, double y, double z, double rotationAngle, GeoPhysVol* fwrPhys)
{
    // "Trousers" -- transition from 1 to 2 beampipes
    // rewritten from Knut Dundas Moraa's AGDD file and modified

    // CONSTANTS
    double TAN_A, TAN_B, TAN_C, TAN_Dsmall, TAN_Dbig, TAN_thick1, TAN_xseparation;
    TAN_A = 700*CLHEP::mm;
    TAN_B = 500*CLHEP::mm;
    TAN_C = 3700*CLHEP::mm;
    TAN_Dsmall = 52*CLHEP::mm;
    TAN_Dbig = 212*CLHEP::mm;
    TAN_thick1 = 4.5*CLHEP::mm;
    TAN_xseparation = 80*CLHEP::mm;

    // Derived constants
    double TAN_Rsmall, TAN_Rbig, TAN_coneZh, TAN_coneR, TAN_coneXh;//, TAN_halflength;
    TAN_Rsmall = 0.5*TAN_Dsmall;
    TAN_Rbig = 0.5*TAN_Dbig;
    TAN_coneZh = TAN_B*cos(5*M_PI/180);
    TAN_coneR = 2*TAN_B*sin(5*M_PI/180)+TAN_Rsmall;
    TAN_coneXh = TAN_Rbig-TAN_B*sin(5*M_PI/180)-TAN_Rsmall;
    //TAN_halflength = 0.5*(TAN_A+TAN_B+TAN_C);

    // volume construction

    // inner part
    GeoPcon *TANi_cone0 = new GeoPcon(0,360*CLHEP::deg);
    TANi_cone0->addPlane(2*TAN_coneZh, TAN_Rsmall, 2*TAN_Rbig);
    TANi_cone0->addPlane(TAN_coneZh, TAN_Rsmall, 2*TAN_Rbig);
    TANi_cone0->addPlane(-TAN_coneZh, TAN_coneR, 2*TAN_Rbig);
    HepGeom::Transform3D TAN_moveCone = HepGeom::Translate3D(TAN_coneXh,0,0.5*(TAN_A-TAN_B));
    HepGeom::Transform3D TAN_rotateCone = HepGeom::RotateY3D(5*CLHEP::deg);
    const GeoShapeShift& TANi_cone = (*TANi_cone0)<<TAN_rotateCone<<TAN_moveCone;

    const GeoBox *TAN_box0 = new GeoBox(2*TAN_Rbig,2*TAN_Rbig,TAN_A+TAN_B);
    HepGeom::Transform3D TAN_moveBox = HepGeom::Translate3D(-2*TAN_Rbig,0,0);
    const GeoShapeShift& TAN_box = (*TAN_box0)<<TAN_moveBox;

    const GeoTube *TANi_bigtube  = new GeoTube(0, TAN_Rbig, 0.5*(TAN_A+TAN_B));

    GeoShapeSubtraction *TANi_hcyl = new GeoShapeSubtraction(TANi_bigtube, &TAN_box);

    GeoShapeSubtraction *TANi_h = new GeoShapeSubtraction(TANi_hcyl, &TANi_cone);
    HepGeom::Transform3D TAN_moveH = HepGeom::Translate3D(0,0,-0.5*TAN_C);
    HepGeom::Transform3D TAN_rotateH = HepGeom::RotateZ3D(180*CLHEP::deg);

    HepGeom::Transform3D TAN_moveTube1 = HepGeom::Translate3D(TAN_xseparation,0,0.5*(TAN_A+TAN_B));
    HepGeom::Transform3D TAN_moveTube2 = HepGeom::Translate3D(-TAN_xseparation,0,0.5*(TAN_A+TAN_B));

    // outer part
    GeoPcon *TANo_cone0 = new GeoPcon(0,360*CLHEP::deg);
    TANo_cone0->addPlane(2*TAN_coneZh, TAN_Rsmall+TAN_thick1, 2*TAN_Rbig);
    TANo_cone0->addPlane(TAN_coneZh, TAN_Rsmall+TAN_thick1, 2*TAN_Rbig);
    TANo_cone0->addPlane(-TAN_coneZh, TAN_coneR+TAN_thick1, 2*TAN_Rbig);
    const GeoShapeShift& TANo_cone = (*TANo_cone0)<<TAN_rotateCone<<TAN_moveCone;

    const GeoTube *TANo_bigtube  = new GeoTube(0, TAN_Rbig+TAN_thick1, 0.5*(TAN_A+TAN_B));

    GeoShapeSubtraction *TANo_hcyl = new GeoShapeSubtraction(TANo_bigtube, &TAN_box);

    GeoShapeSubtraction *TANo_h = new GeoShapeSubtraction(TANo_hcyl, &TANo_cone);

    const GeoTube *TAN_antiblock0  = new GeoTube(0, TAN_Rsmall, 0.5*(TAN_A+2*TAN_B)); // antiblock tube -- just in case..
    HepGeom::Transform3D TAN_moveAntiblock = HepGeom::Translate3D(TAN_xseparation,0,0);
    const GeoShapeShift& TAN_antiblock = (*TAN_antiblock0)<<TAN_moveAntiblock;

    GeoShapeSubtraction *TAN_shape0 = new GeoShapeSubtraction(TANo_h, TANi_h);
    GeoShapeSubtraction *TAN_shape = new GeoShapeSubtraction(TAN_shape0, &TAN_antiblock);
    const GeoShapeShift& TAN_shape1 = (*TAN_shape)<<TAN_moveH;
    const GeoShapeShift& TAN_shape2 = (*TAN_shape)<<TAN_moveH<<TAN_rotateH;

    const GeoTube *TANo_ftube = new GeoTube(TAN_Rsmall,TAN_Rsmall+TAN_thick1,0.5*TAN_C-0.1*CLHEP::mm);
    const GeoShapeShift& TANo_ftube1 = (*TANo_ftube)<<TAN_moveTube1;
    const GeoShapeShift& TANo_ftube2 = (*TANo_ftube)<<TAN_moveTube2;

    GeoShapeUnion *TANo0 = new GeoShapeUnion(&TAN_shape1, &TAN_shape2);
    GeoShapeUnion *TANo1 = new GeoShapeUnion(TANo0, &TANo_ftube1);
    GeoShapeUnion *TANo = new GeoShapeUnion(TANo1, &TANo_ftube2); // complete outer part

    // PLACEMENT
    const GeoLogVol   *ringLog  = new  GeoLogVol(name+"Log", TANo, m_MapMaterials[std::string("Steel")]);

    GeoPhysVol        *ringPhys = new GeoPhysVol(ringLog);

    //create rotation and traslation and add them to the tree of volumes of the world (move and rotate the volume)
    GeoTransform *move           = new GeoTransform(HepGeom::Translate3D(x,y,z));
    fwrPhys->add(move);
    GeoTransform *rotate         = new GeoTransform(HepGeom::RotateY3D(rotationAngle));
    fwrPhys->add(rotate);

    GeoNameTag *tag = new GeoNameTag(name);
    fwrPhys->add(tag);

    fwrPhys->add(ringPhys);

//    // The other side of the forward region may be obtained by rotation
//    GeoTransform *rotateX180  = new GeoTransform(HepGeom::RotateX3D(180*CLHEP::deg));

//    // the other side
//    fwrPhys->add(rotateX180);
//    fwrPhys->add(move);
//    fwrPhys->add(rotate);
//    tag = new GeoNameTag(name+"_L");
//    fwrPhys->add(tag);
//    fwrPhys->add(ringPhys);
}

void ForwardRegionGeoModelFactory::insertTCLElement(std::string name, double x, double y, double z, GeoPhysVol* fwrPhys, double TCLJawDistO, double TCLJawDistI, bool tungstenInsteadOfCopper)
{
    // Constants
    double TCL_BOX_halflength, TCL_BOX_halfwidth, TCL_BOX_halfheight, TCL_BOX_sideThickness, TCL_BOX_topBottomThickness, TCL_BOX_endThickness, TCL_TUBE_halflength, TCL_TUBE_halfapperture, TCL_TUBE_thickness;
    TCL_BOX_sideThickness = 6*CLHEP::mm;
    TCL_BOX_topBottomThickness = 18*CLHEP::mm;
    TCL_BOX_endThickness = 18*CLHEP::mm;

    TCL_BOX_halflength = 621*CLHEP::mm;
    TCL_BOX_halfwidth = 132*CLHEP::mm;
    TCL_BOX_halfheight = 60+TCL_BOX_topBottomThickness;

    TCL_TUBE_halflength = 59.5*CLHEP::mm;
    TCL_TUBE_halfapperture = 53*CLHEP::mm;
    TCL_TUBE_thickness = 2*CLHEP::mm;

    double TCL_CuBlock_halflength, TCL_CuBlock_halfwidth, TCL_CuBlock_halfheight, TCL_CuBlockCylCut_zDepth, TCL_CuBlockCylCut_angle, TCL_CuBlockCylCut_cylR, TCL_CuBlockCylCut_cylHalflength, TCL_CuBlockCylCut_xDepth, TCL_CuBlockCylCut_xShift;
    TCL_CuBlock_halflength = 597*CLHEP::mm;
    TCL_CuBlock_halfwidth = 14.5*CLHEP::mm;
    TCL_CuBlock_halfheight = 40*CLHEP::mm;

    TCL_CuBlockCylCut_zDepth = 90*CLHEP::mm;
    TCL_CuBlockCylCut_angle = 12*CLHEP::deg;
    TCL_CuBlockCylCut_cylR = 40*CLHEP::mm;

    TCL_CuBlockCylCut_cylHalflength = TCL_CuBlockCylCut_zDepth/cos(TCL_CuBlockCylCut_angle);
    TCL_CuBlockCylCut_xDepth = TCL_CuBlockCylCut_zDepth*tan(TCL_CuBlockCylCut_angle);
    TCL_CuBlockCylCut_xShift = -TCL_CuBlock_halfwidth-TCL_CuBlockCylCut_cylR/cos(TCL_CuBlockCylCut_angle)+TCL_CuBlockCylCut_xDepth;

    double TCL_CuBeam_halflength, TCL_CuBeam_halfwidth, TCL_CuBeam_halfheight, TCL_Cooling_width;
    TCL_CuBeam_halflength = 530*CLHEP::mm;
    TCL_CuBeam_halfwidth = 15*CLHEP::mm;
    TCL_CuBeam_halfheight = 40*CLHEP::mm;

    TCL_Cooling_width = 9*CLHEP::mm;



    // rotate by 180 deg around X and Y
    HepGeom::Transform3D rotateX180 = HepGeom::RotateX3D(180*CLHEP::deg);
    HepGeom::Transform3D rotateY180 = HepGeom::RotateY3D(180*CLHEP::deg);

    // inner vacuum volume solid
    const GeoBox * boxIn = new GeoBox(TCL_BOX_halfwidth-TCL_BOX_sideThickness, TCL_BOX_halfheight-TCL_BOX_topBottomThickness, TCL_BOX_halflength-TCL_BOX_endThickness);
    const GeoTube * tubeIn = new GeoTube(0, TCL_TUBE_halfapperture, TCL_TUBE_halflength+0.5*TCL_BOX_endThickness);
    HepGeom::Transform3D moveTubeIn = HepGeom::Translate3D(0, 0, TCL_BOX_halflength+TCL_TUBE_halflength-0.5*TCL_BOX_endThickness);
    const GeoShapeShift& tubeIn1 = (*tubeIn)<<moveTubeIn;
    const GeoShapeShift& tubeIn2 = (*tubeIn)<<moveTubeIn<<rotateY180;
    const GeoShapeUnion * innerVac0 = new GeoShapeUnion(boxIn,&tubeIn1);
    GeoShapeUnion * innerVac = new GeoShapeUnion(innerVac0,&tubeIn2);

    // outer steel case solid
    const GeoBox * boxFull = new GeoBox(TCL_BOX_halfwidth, TCL_BOX_halfheight, TCL_BOX_halflength);
    const GeoTube * tubeOut = new GeoTube(TCL_TUBE_halfapperture, TCL_TUBE_halfapperture+TCL_TUBE_thickness, TCL_TUBE_halflength);
    HepGeom::Transform3D moveTubeOut = HepGeom::Translate3D(0, 0, TCL_BOX_halflength+TCL_TUBE_halflength);
    const GeoShapeShift& tubeOut1 = (*tubeOut)<<moveTubeOut;
    const GeoShapeShift& tubeOut2 = (*tubeOut)<<moveTubeOut<<rotateY180;
    const GeoShapeUnion * outerSteelFull0 = new GeoShapeUnion(boxFull,&tubeOut1);
    const GeoShapeUnion * outerSteelFull = new GeoShapeUnion(outerSteelFull0,&tubeOut2);
    GeoShapeSubtraction * outerSteel = new GeoShapeSubtraction(outerSteelFull,innerVac);

    // Copper block solid
    const GeoBox * cuBoxFull = new GeoBox(TCL_CuBlock_halfwidth, TCL_CuBlock_halfheight, TCL_CuBlock_halflength);
    const GeoTube * cylCut0 = new GeoTube(0, TCL_CuBlockCylCut_cylR, TCL_CuBlockCylCut_cylHalflength);
    HepGeom::Transform3D rotateCylCut = HepGeom::RotateY3D(TCL_CuBlockCylCut_angle);
    HepGeom::Transform3D moveCylCut = HepGeom::Translate3D(TCL_CuBlockCylCut_xShift, 0, TCL_CuBlock_halflength);
    const GeoShapeShift& cylCut1 = (*cylCut0)<<rotateCylCut<<moveCylCut;
    const GeoShapeShift& cylCut2 = (*cylCut0)<<rotateCylCut<<moveCylCut<<rotateX180;
    const GeoShapeSubtraction * cuBox0 = new GeoShapeSubtraction(cuBoxFull, &cylCut1);
    const GeoShapeSubtraction * cuBox1 = new GeoShapeSubtraction(cuBox0, &cylCut2);
    HepGeom::Transform3D moveCuBoxI = HepGeom::Translate3D(TCLJawDistI+TCL_CuBlock_halfwidth, 0, 0);
    const GeoShapeShift& cuBoxI = (*cuBox1)<<moveCuBoxI;
    HepGeom::Transform3D moveCuBoxO = HepGeom::Translate3D(+TCLJawDistO+TCL_CuBlock_halfwidth, 0, 0);
    const GeoShapeShift& cuBoxO = (*cuBox1)<<moveCuBoxO<<rotateY180;

    // Copper beam solid
    const GeoBox * cuBeamFull = new GeoBox(TCL_CuBeam_halfwidth, TCL_CuBeam_halfheight, TCL_CuBeam_halflength);
    HepGeom::Transform3D moveCuBeamI = HepGeom::Translate3D(TCLJawDistI+2*TCL_CuBlock_halfwidth+TCL_Cooling_width+TCL_CuBeam_halfwidth, 0, 0);
    const GeoShapeShift& cuBeamI = (*cuBeamFull)<<moveCuBeamI;
    HepGeom::Transform3D moveCuBeamO = HepGeom::Translate3D(+TCLJawDistO+2*TCL_CuBlock_halfwidth+TCL_Cooling_width+TCL_CuBeam_halfwidth, 0, 0);
    const GeoShapeShift& cuBeamO = (*cuBeamFull)<<moveCuBeamO<<rotateY180;

    // Watter cooling in first aproximation (water box)
    const GeoBox * waterBox = new GeoBox(0.5*TCL_Cooling_width, TCL_CuBlock_halfheight, TCL_CuBlock_halflength);
    HepGeom::Transform3D moveWaterBoxI = HepGeom::Translate3D(TCLJawDistI+2*TCL_CuBlock_halfwidth+0.5*TCL_Cooling_width, 0, 0);
    const GeoShapeShift& waterBoxI = (*waterBox)<<moveWaterBoxI;
    HepGeom::Transform3D moveWaterBoxO = HepGeom::Translate3D(+TCLJawDistO+2*TCL_CuBlock_halfwidth+0.5*TCL_Cooling_width, 0, 0);
    const GeoShapeShift& waterBoxO = (*waterBox)<<moveWaterBoxO<<rotateY180;


    // Logical and physical volumes
    const GeoLogVol   *ringLog  = new  GeoLogVol(name+"Log", outerSteel, m_MapMaterials[std::string("Steel")]);
    const GeoLogVol   *ringLog2  = new  GeoLogVol(name+"Fill", innerVac, m_MapMaterials[std::string("std::Vacuum")]);

    GeoPhysVol        *ringPhys = new GeoPhysVol(ringLog);
    GeoPhysVol        *ringPhys2 = new GeoPhysVol(ringLog2);

    const GeoLogVol   *cuBoxLogI  = new  GeoLogVol(name+"CuBoxI", &cuBoxI, m_MapMaterials[std::string(tungstenInsteadOfCopper ? "Tungsten" : "Copper")]);
    GeoPhysVol        *cuBoxPhysI = new GeoPhysVol(cuBoxLogI);
    const GeoLogVol   *cuBoxLogO  = new  GeoLogVol(name+"CuBoxO", &cuBoxO, m_MapMaterials[std::string(tungstenInsteadOfCopper ? "Tungsten" : "Copper")]);
    GeoPhysVol        *cuBoxPhysO = new GeoPhysVol(cuBoxLogO);

    const GeoLogVol   *cuBeamLogI  = new  GeoLogVol(name+"CuBeamI", &cuBeamI, m_MapMaterials[std::string("GlidCopAL15")]);
    GeoPhysVol        *cuBeamPhysI = new GeoPhysVol(cuBeamLogI);
    const GeoLogVol   *cuBeamLogO  = new  GeoLogVol(name+"CuBeamO", &cuBeamO, m_MapMaterials[std::string("GlidCopAL15")]);
    GeoPhysVol        *cuBeamPhysO = new GeoPhysVol(cuBeamLogO);

    const GeoLogVol   *waterBoxLogI  = new  GeoLogVol(name+"waterBoxI", &waterBoxI, m_MapMaterials[std::string("water")]);
    GeoPhysVol        *waterBoxPhysI = new GeoPhysVol(waterBoxLogI);
    const GeoLogVol   *waterBoxLogO  = new  GeoLogVol(name+"waterBoxO", &waterBoxO, m_MapMaterials[std::string("water")]);
    GeoPhysVol        *waterBoxPhysO = new GeoPhysVol(waterBoxLogO);

    //create rotation and traslation and add them to the tree of volumes of the world (move and rotate the volume)
    GeoTransform *move           = new GeoTransform(HepGeom::Translate3D(x,y,z));
    fwrPhys->add(move);

    GeoNameTag *tag = new GeoNameTag(name);
    fwrPhys->add(tag);

    fwrPhys->add(ringPhys);

    // add filling (inside of the tube)
    fwrPhys->add(move);
    tag = new GeoNameTag(name+"Fill");
    fwrPhys->add(tag);
    fwrPhys->add(ringPhys2);

    // add copper absorbers
    ringPhys2->add(cuBoxPhysI);
    ringPhys2->add(cuBoxPhysO);

    ringPhys2->add(cuBeamPhysI);
    ringPhys2->add(cuBeamPhysO);

    ringPhys2->add(waterBoxPhysI);
    ringPhys2->add(waterBoxPhysO);
}
