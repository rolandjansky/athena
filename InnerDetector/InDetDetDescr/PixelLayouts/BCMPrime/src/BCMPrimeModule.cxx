/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BCMPrime/BCMPrimeModule.h"
#include "BCMPrime/BCMPrimeXMLHelper.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"

GeoPhysVol* BCMPrimeModule::Build(int iModule, const PixelGeoBuilderBasics *basics)
{
    // module parameters

    BCMPrimeXMLHelper BCMPrimeDBHelper(iModule, basics);

    m_transX = BCMPrimeDBHelper.getTransX();
    m_transY = BCMPrimeDBHelper.getTransY();

    m_rotX = BCMPrimeDBHelper.getRotX();
    m_rotY = BCMPrimeDBHelper.getRotY();
    m_rotZ = BCMPrimeDBHelper.getRotZ();

    m_tilt = BCMPrimeDBHelper.getModuleTilt();
    m_ringRot = BCMPrimeDBHelper.getRingRot();
    m_ringOffset = BCMPrimeDBHelper.getRingOffset();

    double modLength = BCMPrimeDBHelper.getModuleLength();
    double modWidth = BCMPrimeDBHelper.getModuleWidth();
    double modTilt = BCMPrimeDBHelper.getModuleTilt();

    double mainWallCuThick = BCMPrimeDBHelper.getCuThickness();
    double mainWallG10Thick = BCMPrimeDBHelper.getG10Thickness();
    double mainWallThick = mainWallCuThick + mainWallG10Thick;

    double diamondThick = BCMPrimeDBHelper.getDiamThickness();
    double diamondSize = BCMPrimeDBHelper.getDiamSize();
    double safety = 0.001;

    m_msg = basics->msgStream();

    if ( tan(modTilt*CLHEP::deg) * BCMPrimeDBHelper.getDiamOffsetY() < diamondThick ) {
        m_msg << MSG::ERROR << "Incompatible values of BCMPrime parameters \"ModuleTilt\" and \"DiamOffsetY\": diamond too thick. Increase diamond offset or module tilt." << endreq;
    }

    m_matMgr = basics->matMgr();

    const GeoMaterial *ether = m_matMgr->getMaterial("special::Ether");
    const GeoMaterial *g10 = m_matMgr->getMaterial("std::G10");
    const GeoMaterial *diamond = m_matMgr->getMaterial("pix::Diamond");

    // ---------------------------------------------------------------------
    // Intermediate Ether envelope volume (G4Assemly when GeoModel is built)
    // ---------------------------------------------------------------------

    GeoSimplePolygonBrep *complexEnvShape = new GeoSimplePolygonBrep(modWidth / 2 + safety);
    std::vector<double> xShape, yShape;

    double length = modLength + mainWallThick / tan(modTilt*CLHEP::deg) + mainWallThick * tan(modTilt*CLHEP::deg) + safety;
    xShape.push_back(0.);
    yShape.push_back(0.);
    xShape.push_back(0.);
    yShape.push_back(length);
    xShape.push_back(-length * cos(modTilt*CLHEP::deg) * sin(modTilt*CLHEP::deg));
    yShape.push_back(length * pow(cos(modTilt*CLHEP::deg), 2));

    for (unsigned int iPt = 0; iPt < xShape.size(); iPt++)
        complexEnvShape->addVertex(xShape[iPt], yShape[iPt]);

    double bcmModLogPocket = mainWallThick / tan(modTilt*CLHEP::deg) + safety / 2;

    const GeoLogVol *env_bcmModLog = new GeoLogVol("EnvBcmModLog", complexEnvShape, ether);
    GeoPhysVol *env_bcmModPhys = new GeoPhysVol(env_bcmModLog);

    //diamond //bcmDiamondLog

    const GeoBox* diamondBoxA = new GeoBox(diamondThick / 2, diamondSize / 2, diamondSize /2);
    const GeoLogVol* diamondLogA = new GeoLogVol("bcmDiamondLog", diamondBoxA, diamond);
    GeoPhysVol *diamondVolA = new GeoPhysVol(diamondLogA);

    CLHEP::HepRotation rmC;

    double diamond_x = - mainWallThick - diamondThick / 2 ;
    CLHEP::Hep3Vector diamondPosA(diamond_x, bcmModLogPocket + BCMPrimeDBHelper.getDiamOffsetY() + diamondSize / 2, (BCMPrimeDBHelper.getDiamSeparZ() + diamondSize) / 2);
    GeoTransform* xform = new GeoTransform(HepGeom::Transform3D(rmC, diamondPosA));
    GeoNameTag* tag = new GeoNameTag("Diamond");
    env_bcmModPhys->add(tag);
    env_bcmModPhys->add(new GeoIdentifierTag(2)); // Unique diamond geo identifier
                                                  // 2 preferred to 0, 
                                                  // 0 is increased to 1, 
                                                  // when GeoModel is built
    env_bcmModPhys->add(xform);
    env_bcmModPhys->add(diamondVolA);
    
    const GeoBox* diamondBoxB = new GeoBox(diamondThick / 2, diamondSize / 2, diamondSize /2);
    const GeoLogVol* diamondLogB = new GeoLogVol("bcmDiamondLog", diamondBoxB, diamond);
    GeoPhysVol *diamondVolB = new GeoPhysVol(diamondLogB);

    CLHEP::Hep3Vector diamondPosB(diamond_x, bcmModLogPocket + BCMPrimeDBHelper.getDiamOffsetY() + diamondSize / 2, -(BCMPrimeDBHelper.getDiamSeparZ() + diamondSize) / 2);
    xform = new GeoTransform(HepGeom::Transform3D(rmC, diamondPosB));
    tag = new GeoNameTag("Diamond");
    env_bcmModPhys->add(tag);
    env_bcmModPhys->add(new GeoIdentifierTag(3)); // Unique diamond geo identifier
    env_bcmModPhys->add(xform);
    env_bcmModPhys->add(diamondVolB);

    //Main BCM' wall

    const GeoBox* bcmWallBox = new GeoBox(mainWallThick / 2, modLength / 2, modWidth / 2);
    const GeoLogVol* bcmWallLog = new GeoLogVol("bcmWallLog", bcmWallBox, g10);
    GeoPhysVol* bcmWallPhys = new GeoPhysVol(bcmWallLog);

    CLHEP::Hep3Vector bcmWallPos( - mainWallThick / 2, bcmModLogPocket + modLength / 2, 0.);
    CLHEP::HepRotation rmA;
    xform = new GeoTransform(HepGeom::Transform3D(rmA, bcmWallPos));
    tag = new GeoNameTag("bcmWall");
    env_bcmModPhys->add(tag);
    env_bcmModPhys->add(xform);
    env_bcmModPhys->add(bcmWallPhys);

    return env_bcmModPhys;
}
