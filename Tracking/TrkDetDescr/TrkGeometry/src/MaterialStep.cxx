/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialStep.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/MaterialStep.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iostream>

Trk::MaterialStep::MaterialStep()
    : m_steplength(0.),
      m_hitX(0.),
      m_hitY(0.),
      m_hitZ(0.),
      m_hitR(0.),
      m_material(0.,0.,0.,0.,0.)
    {}

Trk::MaterialStep::MaterialStep(
    float x, float y, float z,
    float stepL,
    float matX0,
    float matL0,
    float matA,
    float matZ,
    float matRho,
    float matdEdX)
    : m_steplength(stepL),
      m_hitX(x),
      m_hitY(y),
      m_hitZ(z),
      m_hitR(sqrt(x*x+y*y)),
      m_material(matX0, matL0, matA, matZ, matRho, matdEdX)
{}

Trk::MaterialStep::MaterialStep(
    float x, float y, float z,
    float stepL,
    float matX0,
    float matL0,
    float matA,
    float matZ,
    float matRho,
    const std::vector<unsigned char>& elements,
    const std::vector<unsigned char>& fractions,
    float matdEdX)
    : m_steplength(stepL),
      m_hitX(x),
      m_hitY(y),
      m_hitZ(z),
      m_hitR(sqrt(x*x+y*y)),
      m_material(matX0, matL0, matA, matZ, matRho, matdEdX, new Trk::MaterialComposition(elements, fractions))

    {}

Trk::MaterialStep::MaterialStep(const MaterialStep& mstep)
    
    

    = default;

Trk::MaterialStep::~MaterialStep() = default;

Trk::MaterialStep& Trk::MaterialStep::operator =( const Trk::MaterialStep& mstep ) 
{
    if (&mstep != this) {
        m_steplength  = mstep.m_steplength;
        m_hitX        = mstep.m_hitX;
        m_hitY        = mstep.m_hitY;
        m_hitZ        = mstep.m_hitZ;
        m_hitR        = mstep.m_hitR;
        m_material    = mstep.m_material;
    }
    return (*this);
}


MsgStream& Trk::MaterialStep::dump(MsgStream& sl) const
{
    sl << "----> MaterialStep - recorded at (x/y/z) | r : ("  << hitX() << "/ ";
    sl                                                        << hitY() << "/ ";
    sl                                                        << hitZ() << ") |  ";
    sl                                                        << hitR() << std::endl;
    sl << "      Material (t/x0/l0/A/Z/rho) : ("              << steplength() << "/ ";
    sl                                                        << x0() << "/ ";
    sl                                                        << l0() << "/ ";
    sl                                                        << A() << "/ ";
    sl                                                        << Z() << "/  ";
    sl                                                        << rho() << ")" << std::endl;
    return sl;
}

std::ostream& Trk::MaterialStep::dump(std::ostream& sl) const
{
    sl << "----> MaterialStep - recorded at (x/y/z) | r : ("  << hitX() << "/ ";
    sl                                                        << hitY() << "/ ";
    sl                                                        << hitZ() << ") |  ";
    sl                                                        << hitR() << std::endl;
    sl << "      Material (t/x0/l0/A/Z/rho) : ("              << steplength() << "/ ";
    sl                                                        << x0() << "/ ";
    sl                                                        << l0() << "/ ";
    sl                                                        << A() << "/ ";
    sl                                                        << Z() << "/  ";
    sl                                                        << rho() << ")" << std::endl;
    return sl;
}

MsgStream& Trk::operator << ( MsgStream& sl, const Trk::MaterialStep& mstep)
{
    return mstep.dump(sl);
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::MaterialStep& mstep)
{
    return mstep.dump(sl);
}

