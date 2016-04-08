/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKIPATFITTERUTILS_FITMATRIX_H
# define TRKIPATFITTERUTILS_FITMATRIX_H

#define     mxmeas      1000
#define     mxparam     256

namespace Trk
{
    
    struct fitMatrix
    {
	double	derivative[mxmeas][mxparam];
    } fitMatrix;
    
}	// end of namespace

#endif	// TRKIPATFITTERUTILS_FITMATRIX_H
