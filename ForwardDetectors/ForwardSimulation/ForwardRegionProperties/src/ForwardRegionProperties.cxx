/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardRegionProperties.h"

ForwardRegionProperties::ForwardRegionProperties( const std::string& type, const std::string& name, const IInterface* parent )
: base_class( type, name, parent )
{
    m_Config.clear();
    declareProperty("twissFileB1",m_Config.twissFileB1="");
    declareProperty("twissFileB2",m_Config.twissFileB2="");
    declareProperty("momentum",m_Config.momentum = 0);

    declareProperty("pointQ1Start", m_Config.pointQ1Start);
    declareProperty("pointQ2aStart", m_Config.pointQ2aStart);
    declareProperty("pointQ2bStart", m_Config.pointQ2bStart);
    declareProperty("pointQ3Start", m_Config.pointQ3Start);
    declareProperty("pointQ4Start", m_Config.pointQ4Start);
    declareProperty("pointQ5Start", m_Config.pointQ5Start);
    declareProperty("pointQ6Start", m_Config.pointQ6Start);
    declareProperty("pointQ7aStart", m_Config.pointQ7aStart);
    declareProperty("pointQ7bStart", m_Config.pointQ7bStart);
    declareProperty("pointD1aStart", m_Config.pointD1aStart);
    declareProperty("pointD1bStart", m_Config.pointD1bStart);
    declareProperty("pointD1cStart", m_Config.pointD1cStart);
    declareProperty("pointD1dStart", m_Config.pointD1dStart);
    declareProperty("pointD1eStart", m_Config.pointD1eStart);
    declareProperty("pointD1fStart", m_Config.pointD1fStart);
    declareProperty("pointD2Start", m_Config.pointD2Start);

    declareProperty("pointQ1End", m_Config.pointQ1End);
    declareProperty("pointQ2aEnd", m_Config.pointQ2aEnd);
    declareProperty("pointQ2bEnd", m_Config.pointQ2bEnd);
    declareProperty("pointQ3End", m_Config.pointQ3End);
    declareProperty("pointQ4End", m_Config.pointQ4End);
    declareProperty("pointQ5End", m_Config.pointQ5End);
    declareProperty("pointQ6End", m_Config.pointQ6End);
    declareProperty("pointQ7aEnd", m_Config.pointQ7aEnd);
    declareProperty("pointQ7bEnd", m_Config.pointQ7bEnd);
    declareProperty("pointD1aEnd", m_Config.pointD1aEnd);
    declareProperty("pointD1bEnd", m_Config.pointD1bEnd);
    declareProperty("pointD1cEnd", m_Config.pointD1cEnd);
    declareProperty("pointD1dEnd", m_Config.pointD1dEnd);
    declareProperty("pointD1eEnd", m_Config.pointD1eEnd);
    declareProperty("pointD1fEnd", m_Config.pointD1fEnd);
    declareProperty("pointD2End", m_Config.pointD2End);

    declareProperty("fQ1RotZ", m_Config.fQ1RotZ);
    declareProperty("fQ2aRotZ", m_Config.fQ2aRotZ);
    declareProperty("fQ2bRotZ", m_Config.fQ2bRotZ);
    declareProperty("fQ3RotZ", m_Config.fQ3RotZ);
    declareProperty("fQ4RotZ", m_Config.fQ4RotZ);
    declareProperty("fQ5RotZ", m_Config.fQ5RotZ);
    declareProperty("fQ6RotZ", m_Config.fQ6RotZ);
    declareProperty("fQ7aRotZ", m_Config.fQ7aRotZ);
    declareProperty("fQ7bRotZ", m_Config.fQ7bRotZ);
    declareProperty("fD1aRotZ", m_Config.fD1aRotZ);
    declareProperty("fD1bRotZ", m_Config.fD1bRotZ);
    declareProperty("fD1cRotZ", m_Config.fD1cRotZ);
    declareProperty("fD1dRotZ", m_Config.fD1dRotZ);
    declareProperty("fD1eRotZ", m_Config.fD1eRotZ);
    declareProperty("fD1fRotZ", m_Config.fD1fRotZ);
    declareProperty("fD2RotZ", m_Config.fD2RotZ);

    declareProperty("bUseFLUKAMapsForInnerTriplet", m_Config.bUseFLUKAMapsForInnerTriplet=false);


    ATH_MSG_INFO("ForwardRegionProperties created");
}

ForwardRegionProperties::~ForwardRegionProperties(){}

PFWDMg_CONFIGURATION ForwardRegionProperties::getConf() {
    return &m_Config;
}

void ForwardRegionProperties::getMagTransforms(std::string name, int beam, HepGeom::Point3D<double> & pointMagStart, HepGeom::Point3D<double> & pointMagEnd, double & rotAxis, bool twiss)
{
    if(name.find("MagQ1") != std::string::npos || (twiss && name.find("MQXA.1") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ1Start[(beam-1)*3],m_Config.pointQ1Start[(beam-1)*3+1],m_Config.pointQ1Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ1End[(beam-1)*3],m_Config.pointQ1End[(beam-1)*3+1],m_Config.pointQ1End[(beam-1)*3+2]);
        rotAxis = m_Config.fQ1RotZ[beam-1];
    }
    else if(name.find("MagQ2a") != std::string::npos || (twiss && name.find("MQXB.A2") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ2aStart[(beam-1)*3],m_Config.pointQ2aStart[(beam-1)*3+1],m_Config.pointQ2aStart[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ2aEnd[(beam-1)*3],m_Config.pointQ2aEnd[(beam-1)*3+1],m_Config.pointQ2aEnd[(beam-1)*3+2]);
        rotAxis = m_Config.fQ2aRotZ[beam-1];
    }
    else if(name.find("MagQ2b") != std::string::npos || (twiss && name.find("MQXB.B2") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ2bStart[(beam-1)*3],m_Config.pointQ2bStart[(beam-1)*3+1],m_Config.pointQ2bStart[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ2bEnd[(beam-1)*3],m_Config.pointQ2bEnd[(beam-1)*3+1],m_Config.pointQ2bEnd[(beam-1)*3+2]);
        rotAxis = m_Config.fQ2bRotZ[beam-1];
    }
    else if(name.find("MagQ3") != std::string::npos || (twiss && name.find("MQXA.3") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ3Start[(beam-1)*3],m_Config.pointQ3Start[(beam-1)*3+1],m_Config.pointQ3Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ3End[(beam-1)*3],m_Config.pointQ3End[(beam-1)*3+1],m_Config.pointQ3End[(beam-1)*3+2]);
        rotAxis = m_Config.fQ3RotZ[beam-1];
    }
    else if(name.find("MagD1a") != std::string::npos || (twiss && name.find("MBXW.A4") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1aStart[(beam-1)*3],m_Config.pointD1aStart[(beam-1)*3+1],m_Config.pointD1aStart[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1aEnd[(beam-1)*3],m_Config.pointD1aEnd[(beam-1)*3+1],m_Config.pointD1aEnd[(beam-1)*3+2]);
        rotAxis = m_Config.fD1aRotZ[beam-1];
    }
    else if(name.find("MagD1b") != std::string::npos || (twiss && name.find("MBXW.B4") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1bStart[(beam-1)*3],m_Config.pointD1bStart[(beam-1)*3+1],m_Config.pointD1bStart[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1bEnd[(beam-1)*3],m_Config.pointD1bEnd[(beam-1)*3+1],m_Config.pointD1bEnd[(beam-1)*3+2]);
        rotAxis = m_Config.fD1bRotZ[beam-1];
    }
    else if(name.find("MagD1c") != std::string::npos || (twiss && name.find("MBXW.C4") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1cStart[(beam-1)*3],m_Config.pointD1cStart[(beam-1)*3+1],m_Config.pointD1cStart[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1cEnd[(beam-1)*3],m_Config.pointD1cEnd[(beam-1)*3+1],m_Config.pointD1cEnd[(beam-1)*3+2]);
        rotAxis = m_Config.fD1cRotZ[beam-1];
    }
    else if(name.find("MagD1d") != std::string::npos || (twiss && name.find("MBXW.D4") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1dStart[(beam-1)*3],m_Config.pointD1dStart[(beam-1)*3+1],m_Config.pointD1dStart[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1dEnd[(beam-1)*3],m_Config.pointD1dEnd[(beam-1)*3+1],m_Config.pointD1dEnd[(beam-1)*3+2]);
        rotAxis = m_Config.fD1dRotZ[beam-1];
    }
    else if(name.find("MagD1e") != std::string::npos || (twiss && name.find("MBXW.E4") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1eStart[(beam-1)*3],m_Config.pointD1eStart[(beam-1)*3+1],m_Config.pointD1eStart[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1eEnd[(beam-1)*3],m_Config.pointD1eEnd[(beam-1)*3+1],m_Config.pointD1eEnd[(beam-1)*3+2]);
        rotAxis = m_Config.fD1eRotZ[beam-1];
    }
    else if(name.find("MagD1f") != std::string::npos || (twiss && name.find("MBXW.F4") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointD1fStart[(beam-1)*3],m_Config.pointD1fStart[(beam-1)*3+1],m_Config.pointD1fStart[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD1fEnd[(beam-1)*3],m_Config.pointD1fEnd[(beam-1)*3+1],m_Config.pointD1fEnd[(beam-1)*3+2]);
        rotAxis = m_Config.fD1fRotZ[beam-1];
    }
    else if(name.find("MagD2") != std::string::npos || (twiss && name.find("MBRC.4") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointD2Start[(beam-1)*3],m_Config.pointD2Start[(beam-1)*3+1],m_Config.pointD2Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointD2End[(beam-1)*3],m_Config.pointD2End[(beam-1)*3+1],m_Config.pointD2End[(beam-1)*3+2]);
        rotAxis = m_Config.fD2RotZ[beam-1];
    }
    else if(name.find("MagQ4") != std::string::npos || (twiss && name.find("MQY.4") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ4Start[(beam-1)*3],m_Config.pointQ4Start[(beam-1)*3+1],m_Config.pointQ4Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ4End[(beam-1)*3],m_Config.pointQ4End[(beam-1)*3+1],m_Config.pointQ4End[(beam-1)*3+2]);
        rotAxis = m_Config.fQ4RotZ[beam-1];
    }
    else if(name.find("MagQ5") != std::string::npos || (twiss && name.find("MQML.5") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ5Start[(beam-1)*3],m_Config.pointQ5Start[(beam-1)*3+1],m_Config.pointQ5Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ5End[(beam-1)*3],m_Config.pointQ5End[(beam-1)*3+1],m_Config.pointQ5End[(beam-1)*3+2]);
        rotAxis = m_Config.fQ5RotZ[beam-1];
    }
    else if(name.find("MagQ6") != std::string::npos || (twiss && name.find("MQML.6") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ6Start[(beam-1)*3],m_Config.pointQ6Start[(beam-1)*3+1],m_Config.pointQ6Start[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ6End[(beam-1)*3],m_Config.pointQ6End[(beam-1)*3+1],m_Config.pointQ6End[(beam-1)*3+2]);
        rotAxis = m_Config.fQ6RotZ[beam-1];
    }
    else if(name.find("MagQ7a") != std::string::npos || (twiss && name.find("MQM.A7") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ7aStart[(beam-1)*3],m_Config.pointQ7aStart[(beam-1)*3+1],m_Config.pointQ7aStart[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ7aEnd[(beam-1)*3],m_Config.pointQ7aEnd[(beam-1)*3+1],m_Config.pointQ7aEnd[(beam-1)*3+2]);
        rotAxis = m_Config.fQ7aRotZ[beam-1];
    }
    else if(name.find("MagQ7b") != std::string::npos || (twiss && name.find("MQM.B7") != std::string::npos))
    {
        pointMagStart = HepGeom::Point3D<double>(m_Config.pointQ7bStart[(beam-1)*3],m_Config.pointQ7bStart[(beam-1)*3+1],m_Config.pointQ7bStart[(beam-1)*3+2]);
        pointMagEnd = HepGeom::Point3D<double>(m_Config.pointQ7bEnd[(beam-1)*3],m_Config.pointQ7bEnd[(beam-1)*3+1],m_Config.pointQ7bEnd[(beam-1)*3+2]);
        rotAxis = m_Config.fQ7bRotZ[beam-1];
    }
    else
    {
        pointMagStart = HepGeom::Point3D<double>(0,0,0);
        pointMagEnd = HepGeom::Point3D<double>(0,0,0);
        rotAxis = 0;
    }
}

void ForwardRegionProperties::getMagTransforms(std::string name, int beam, HepGeom::Point3D<double> & pointMagStart, HepGeom::Point3D<double> & pointMagEnd, bool twiss)
{
    double dummy;
    getMagTransforms(name, beam, pointMagStart, pointMagEnd, dummy, twiss);
}


