/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialStep.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/MaterialStep.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <cmath>
#include <iostream>

Trk::MaterialStep::MaterialStep(float x, float y, float z, float stepL,
                                float matX0, float matL0, float matA,
                                float matZ, float matRho, float matdEdX)
    : m_steplength(stepL),
      m_hitX(x),
      m_hitY(y),
      m_hitZ(z),
      m_hitR(std::sqrt(x * x + y * y)),
      m_material(matX0, matL0, matA, matZ, matRho, matdEdX) {}

Trk::MaterialStep::MaterialStep(float x, float y, float z, float stepL,
                                float matX0, float matL0, float matA,
                                float matZ, float matRho,
                                const std::vector<unsigned char>& elements,
                                const std::vector<unsigned char>& fractions,
                                float matdEdX)
    : m_steplength(stepL),
      m_hitX(x),
      m_hitY(y),
      m_hitZ(z),
      m_hitR(std::sqrt(x * x + y * y)),
      m_material(matX0, matL0, matA, matZ, matRho, matdEdX,
                 new Trk::MaterialComposition(elements, fractions))

{}

MsgStream& Trk::MaterialStep::dump(MsgStream& sl) const {
  sl << "----> MaterialStep - recorded at (x/y/z) | r : (" << hitX() << "/ ";
  sl << hitY() << "/ ";
  sl << hitZ() << ") |  ";
  sl << hitR() << std::endl;
  sl << "      Material (t/x0/l0/A/Z/rho) : (" << steplength() << "/ ";
  sl << x0() << "/ ";
  sl << l0() << "/ ";
  sl << A() << "/ ";
  sl << Z() << "/  ";
  sl << rho() << ")" << std::endl;
  return sl;
}

std::ostream& Trk::MaterialStep::dump(std::ostream& sl) const {
  sl << "----> MaterialStep - recorded at (x/y/z) | r : (" << hitX() << "/ ";
  sl << hitY() << "/ ";
  sl << hitZ() << ") |  ";
  sl << hitR() << std::endl;
  sl << "      Material (t/x0/l0/A/Z/rho) : (" << steplength() << "/ ";
  sl << x0() << "/ ";
  sl << l0() << "/ ";
  sl << A() << "/ ";
  sl << Z() << "/  ";
  sl << rho() << ")" << std::endl;
  return sl;
}

MsgStream& Trk::operator<<(MsgStream& sl, const Trk::MaterialStep& mstep) {
  return mstep.dump(sl);
}

std::ostream& Trk::operator<<(std::ostream& sl,
                              const Trk::MaterialStep& mstep) {
  return mstep.dump(sl);
}

