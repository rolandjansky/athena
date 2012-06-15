/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Matrix/DiagMatrix.h"
#include "CLHEP/Matrix/GenMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Pile.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

// not implemented functions in CLHEP 1.8.1.0
inline void CLHEP::HepSymMatrix::invertHaywood4(int& ) {}
inline void CLHEP::HepMatrix::invertHaywood4(int& ) {}
inline void CLHEP::HepMatrix::invertHaywood5(int& ) {}
inline void CLHEP::HepMatrix::invertHaywood6(int& ) {}
inline CLHEP::HepMatrix& CLHEP::HepMatrix::operator=(const CLHEP::HepRotation &) {return *(new CLHEP::HepMatrix()); }

// must instatiate some templates

namespace AtlasSealCLHEP {
//    CLHEP::HepGenMatrix::Alloc<double,25>                        m_alloc;
//    CLHEP::HepGenMatrix::Alloc<double,25> m_all;
//    CLHEP::GenMatrix::Alloc<double,25>                        m_alloc;
//    CLHEP::HepMatrix  m_matrix;
//    std::vector<double, CLHEP::HepGenMatrix::Alloc<double,25> >  m_allocVec;
}
