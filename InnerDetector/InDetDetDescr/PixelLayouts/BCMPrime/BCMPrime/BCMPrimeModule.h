/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCMPRIMEMODULE_H
#define BCMPRIMEMODULE_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include <iostream>

#include "InDetGeoModelUtils/InDetMaterialManager.h"


class BCMPrimeModule
{
 public:
    GeoPhysVol* Build(int iLayer, const PixelGeoBuilderBasics *basics);

    double getTransX() {return m_transX;}
    double getTransY() {return m_transY;}

    double getRotX() {return m_rotX;}
    double getRotY() {return m_rotY;}
    double getRotZ() {return m_rotZ;}

    double getTilt() {return m_tilt;}
    double getRingRot() {return m_ringRot;}
    double getRingOffset() {return m_ringOffset;}

 private:
    mutable InDetMaterialManager *m_matMgr;
    mutable Athena::MsgStreamMember m_msg;
    
    double m_transX{};
    double m_transY{};

    double m_rotX{};
    double m_rotY{};
    double m_rotZ{};

    double m_tilt{};

    double m_ringRot{};
    double m_ringOffset{};

};

#endif
