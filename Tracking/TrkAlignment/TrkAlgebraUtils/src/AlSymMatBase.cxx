/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PBdR (17Apr2007) base for the symmetric matrix (~virtual)
#include <GaudiKernel/StatusCode.h>

#include "TrkAlgebraUtils/AlSymMatBase.h"
#include "TrkAlgebraUtils/AlVec.h"
#include "TrkAlgebraUtils/AlMat.h"

#include <cfloat> //for DBL_EPSILON
#include <cmath>
#include <fstream>
#include <iomanip>

namespace Trk {

//______________________________________________________________________________
AlSymMatBase::AlSymMatBase()
{
  m_matrix_type = 0;
  m_size = 0;
  m_nele = 0;
}

//______________________________________________________________________________
AlSymMatBase::AlSymMatBase(long int N)
{
  m_matrix_type = 0;
  m_size = N;
  m_nele = N*(N+1)/2;

}

//______________________________________________________________________________
AlSymMatBase::AlSymMatBase(const AlSymMatBase & m)
{
  m_matrix_type = 0;
  m_size      = m.size();
  m_nele      = m.m_nele;
}

//______________________________________________________________________________
AlSymMatBase & AlSymMatBase::operator=(const AlSymMatBase & m)
{
  if (this==&m)
    return *this;
  m_matrix_type=m.m_matrix_type;
  m_nele=m.m_nele;

  return *this;
}

//______________________________________________________________________________
AlSymMatBase::~AlSymMatBase()
{
  // m_ptr_map.clear();
}


} // end namespace Trk
