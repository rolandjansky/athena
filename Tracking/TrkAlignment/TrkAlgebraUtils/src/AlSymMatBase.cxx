/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PBdR (17Apr2007) base for the symmetric matrix (~virtual)
#include <GaudiKernel/StatusCode.h>

#include "TrkAlgebraUtils/AlSymMatBase.h"
#include "TrkAlgebraUtils/AlVec.h"
#include "TrkAlgebraUtils/AlMat.h"

#include <iomanip>
#include <fstream>
#include <math.h>
#include <float.h> //for DBL_EPSILON

namespace Trk {

//______________________________________________________________________________
AlSymMatBase::AlSymMatBase()
{
  _matrix_type = 0;
  _size = 0;
  _nele = 0;
}

//______________________________________________________________________________
AlSymMatBase::AlSymMatBase(long int N)
{
  _matrix_type = 0;
  _size = N;
  _nele = N*(N+1)/2;

}

//______________________________________________________________________________
AlSymMatBase::AlSymMatBase(const AlSymMatBase & m)
{
  _matrix_type = 0;
  _size      = m.size();
  _nele      = m._nele;
}

//______________________________________________________________________________
AlSymMatBase & AlSymMatBase::operator=(const AlSymMatBase & m)
{
  if (this==&m)
    return *this;
  _matrix_type=m._matrix_type;
  _nele=m._nele;

  return *this;
}

//______________________________________________________________________________
AlSymMatBase::~AlSymMatBase()
{
  // ptr_map.clear();
}


} // end namespace Trk
