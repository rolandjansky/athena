/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKIPATFITTERUTILS_FITMATRIX_H
#define TRKIPATFITTERUTILS_FITMATRIX_H

#define mxmeas 1000
#define mxparam 256

namespace Trk {

struct fitMatrix
{
  double derivative[mxmeas][mxparam];
};

} // end of namespace

#endif // TRKIPATFITTERUTILS_FITMATRIX_H
