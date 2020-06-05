/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <stdexcept>
#include <iostream>

#include "CLHEP/Units/PhysicalConstants.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "G4GeometryTolerance.hh"

#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "LArWheelSliceSolid.h"
#include "G4ShiftedCone.h"

#ifdef DEBUG_LARWHEELSLICESOLID
G4int LArWheelSliceSolid::Verbose = 0;
#endif

// these are internal technical constants, should not go in DB
const unsigned int LArWheelSliceSolid::s_IterationsLimit = 50; // That's enough even for 10e-15 IterationPrecision
const G4double LArWheelSliceSolid::s_Tolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / 2;
const G4double LArWheelSliceSolid::s_AngularTolerance = G4GeometryTolerance::GetInstance()->GetAngularTolerance() / 2;
const G4double LArWheelSliceSolid::s_IterationPrecision = 0.001*CLHEP::mm;
const G4double LArWheelSliceSolid::s_IterationPrecision2 = s_IterationPrecision * s_IterationPrecision;

LArWheelSliceSolid::LArWheelSliceSolid(
    const G4String& name,
    pos_t pos, type_t type, size_t slice,
    G4int zside,
    const LArWheelCalculator *calc
) : G4VSolid(name), m_Pos(pos), m_Type(type),
    m_Calculator(calc),
    m_msg("LArWSS")
{
    createSolid(name, zside, slice);
}

LArWheelSliceSolid::LArWheelSliceSolid(const G4String& name)
  : G4VSolid(name), m_Calculator(0), m_msg("LArWSS")
{
    if(name.find("::Inner") != G4String::npos) m_Pos = Inner;
    else if(name.find("::Outer") != G4String::npos) m_Pos = Outer;
    else G4Exception(
        "LArWheelSliceSolid", "NoPos", FatalException,
        (std::string("Constructor: can't get Inner/Outer from ") + name).c_str()
    );

    if(name.find("::Absorber") != G4String::npos) m_Type = Absorber;
    else if(name.find("::Electrode") != G4String::npos) m_Type = Electrode;
    else if(name.find("::Glue") != G4String::npos) m_Type = Glue;
    else if(name.find("::Lead") != G4String::npos) m_Type = Lead;
    else G4Exception(
        "LArWheelSliceSolid", "NoType", FatalException,
        (std::string("Constructor: can't get Type from ") + name).c_str()
    );

    size_t s = name.find("Slice");
    size_t slice = 0;
    if(G4String::npos != s){
        slice = (size_t) atoi(name.substr(s + 5, 2).c_str());
    } else G4Exception(
        "LArWheelSliceSolid", "NoSlice", FatalException,
        (std::string("Constructor: can't get Slice from ") + name).c_str()
    );

    G4int zside = 0;
    if(name.find("::Pos::") != G4String::npos) zside = 1;
    else if(name.find("::Neg::") != G4String::npos) zside = -1;
    else G4Exception(
        "LArWheelSliceSolid", "NoSide", FatalException,
        (std::string("Constructor: can't get zSide from ") + name).c_str()
    );
    createSolid(name, zside, slice);
}

void LArWheelSliceSolid::createSolid(const G4String& name, G4int zside, size_t slice)
{
    m_f_area = m_f_vol = m_f_area_on_pc = m_f_length = m_f_side_area = 0;

    LArG4::LArWheelCalculator_t calc_type = LArG4::LArWheelCalculator_t(0);
    switch(m_Pos){
    case Inner:
        switch(m_Type){
        case Absorber: calc_type = LArG4::InnerAbsorberWheel; break;
        case Electrode: calc_type = LArG4::InnerElectrodWheel; break;
        case Glue: calc_type = LArG4::InnerGlueWheel; break;
        case Lead: calc_type = LArG4::InnerLeadWheel; break;
        }
        break;
    case Outer:
        switch(m_Type){
        case Absorber: calc_type = LArG4::OuterAbsorberWheel; break;
        case Electrode: calc_type = LArG4::OuterElectrodWheel; break;
        case Glue: calc_type = LArG4::OuterGlueWheel; break;
        case Lead: calc_type = LArG4::OuterLeadWheel; break;
        }
        break;
    }
    if(m_Calculator == 0) m_Calculator = new LArWheelCalculator(calc_type, zside);
    else if(m_Calculator->type() != calc_type){
        G4Exception(
            "LArWheelSliceSolid", "WrongCalculatorType", FatalException,
            "Constructor: external LArWheelCalculator of wrong type provided"
        );
    }

    const G4String bs_name = name + "-Bounding";

#ifdef DEBUG_LARWHEELSLICESOLID
    const char *venv = getenv("LARWHEELSLICESOLID_VERBOSE");
    if(venv) Verbose = atoi(venv);
    static bool first = true;
    if(first){
        std::cout << "The LArWheelSliceSolid build "
                  << __DATE__ << " " << __TIME__  << std::endl
                  << "LArWheelSliceSolid verbosity level is "
                  << Verbose << std::endl;
        first = false;
    }
#endif

    m_FanHalfThickness = GetCalculator()->GetFanHalfThickness();
    m_FHTplusT = m_FanHalfThickness + s_Tolerance;
    m_FHTminusT = m_FanHalfThickness - s_Tolerance;

    switch(m_Pos){
    case Inner: inner_solid_init(bs_name, slice); break;
    case Outer: outer_solid_init(bs_name, slice); break;
    }
    ATH_MSG_DEBUG(m_BoundingShape->GetName() + " is the m_BoundingShape");

    init_tests();
    test(); // activated by env. variable
    clean_tests();

    ATH_MSG_DEBUG("slice " << m_Pos << " "  << m_Type
        << " " << slice << " initialized" << endmsg);
#ifdef DEBUG_LARWHEELSLICESOLID
    std::cout << "LArWSS(" << m_Pos << ", " << m_Type
              << "): slice " << slice << ", Zmin = " << m_Zmin
              << ", Zmax = " << m_Zmax << std::endl
              << GetName() << std::endl;
#endif
}

inline void LArWheelSliceSolid::check_slice(size_t size, size_t slice) const
{
    if(size <= slice + 1){
        G4Exception(
            "LArWheelSliceSolid", "WrongSlice", FatalException,
            std::string("Constructor: Slice number too big: " + GetName()).c_str()
        );
    }
}

void LArWheelSliceSolid::inner_solid_init(const G4String &bs_name, size_t slice)
{
    G4double zPlane[2], rInner[2], rOuter[2];
    zPlane[0] = 0.;
    zPlane[1] = GetCalculator()->GetWheelThickness();
    GetCalculator()->GetWheelInnerRadius(rInner);
    GetCalculator()->GetWheelOuterRadius(rOuter);

    std::vector<G4double> zsect;
    fill_zsect(zsect);
    check_slice(zsect.size(), slice);

    m_Zmin = zsect[slice]; m_Zmax = zsect[slice + 1];
    m_Rmin = rInner[0];      m_Rmax = rOuter[1];
    const G4double ainn = (rInner[1] - rInner[0]) / (zPlane[1] - zPlane[0]);
    const G4double aout = (rOuter[1] - rOuter[0]) / (zPlane[1] - zPlane[0]);
    const G4double R1inn = ainn * (m_Zmin - zPlane[0]) + rInner[0];
    const G4double R1out = aout * (m_Zmin - zPlane[0]) + rOuter[0];
    const G4double R2inn = ainn * (m_Zmax - zPlane[0]) + rInner[0];
    const G4double R2out = aout * (m_Zmax - zPlane[0]) + rOuter[0];

    m_BoundingShape = new G4ShiftedCone(
        bs_name + "Cone", m_Zmin, m_Zmax, R1inn, R1out, R2inn, R2out
    );

    const G4double FanPhiAmplitude = 0.065; // internal technical constant, should not go in DB
    const G4double phi_min = CLHEP::halfpi
                           - FanPhiAmplitude
                           - GetCalculator()->GetFanStepOnPhi() * 2;
    m_Xmax = rOuter[1]*cos(phi_min);
    m_Xmin = -m_Xmax;

    m_Ymin = m_Rmin * 0.9;
}

void LArWheelSliceSolid::outer_solid_init(const G4String &bs_name, size_t slice)
{
    G4double zPlane[3], rInner[3], rOuter[3];
    zPlane[0] = 0.;
    zPlane[1] = GetCalculator()->GetWheelInnerRadius(rInner);
    zPlane[2] = GetCalculator()->GetWheelThickness();
    GetCalculator()->GetWheelOuterRadius(rOuter);

    std::vector<G4double> zsect;
    fill_zsect(zsect, zPlane[1]);
    check_slice(zsect.size(), slice);

    m_Zmin = zsect[slice]; m_Zmax = zsect[slice + 1];
    m_Rmin = rInner[0];      m_Rmax = rOuter[1];
    const size_t i = m_Zmax > zPlane[1]? 1: 0;
    const G4double dz = zPlane[i + 1] - zPlane[i];
    const G4double ainn = (rInner[i + 1] - rInner[i]) / dz;
    const G4double aout = (rOuter[i + 1] - rOuter[i]) / dz;
    const G4double R1inn = ainn * (m_Zmin - zPlane[i]) + rInner[i];
    const G4double R1out = aout * (m_Zmin - zPlane[i]) + rOuter[i];
    const G4double R2inn = ainn * (m_Zmax - zPlane[i]) + rInner[i];
    const G4double R2out = aout * (m_Zmax - zPlane[i]) + rOuter[i];

    m_BoundingShape = new G4ShiftedCone(
        bs_name + "Cone", m_Zmin, m_Zmax, R1inn, R1out, R2inn, R2out
    );

    const G4double FanPhiAmplitude = 0.02; // internal technical constant, should not go in DB
    const G4double phi_min = CLHEP::halfpi
                           - FanPhiAmplitude
                           - GetCalculator()->GetFanStepOnPhi() * 2;
    m_Xmax = rOuter[1]*cos(phi_min);
    m_Xmin = -m_Xmax;

    m_Ymin = m_Rmin * 0.9;
}

void LArWheelSliceSolid::fill_zsect(std::vector<G4double> &zsect, G4double zMid) const
{
    const G4double half_wave_length = GetCalculator()->GetHalfWaveLength();
    const G4double sss = GetCalculator()->GetStraightStartSection();
    const G4int num_fs = GetCalculator()->GetNumberOfHalfWaves() + 1;
    const G4double wheel_thickness = GetCalculator()->GetWheelThickness();

    zsect.push_back(0.);
    zsect.push_back(sss + half_wave_length * 0.25);
    for(G4int i = 2; i < num_fs; i ++){
        const G4double zi = half_wave_length * (i - 1) + sss;
        if(m_Pos == Outer && zsect.back() < zMid && zi > zMid){
            zsect.push_back(zMid);
        }
        zsect.push_back(zi);
    }
    zsect.push_back(wheel_thickness - sss - half_wave_length * 0.25);
    zsect.push_back(wheel_thickness);
}
