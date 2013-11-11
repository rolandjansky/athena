/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        VectorUtils.h
// package:     Reconstruction/tauEventTPCnv
// authors:     Lukasz Janyst
// date:        2008-04-22
// description: Helpers for converting vectors
//-----------------------------------------------------------------------------


#include <vector>
#include <algorithm>

template <typename cl1, typename cl2>
void AssignVector( std :: vector<cl1> &target, const std :: vector<cl2> &source )
{
  target.resize( source.size() );
  std :: copy( source.begin(), source.end(), target.begin() );
}
