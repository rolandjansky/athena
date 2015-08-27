/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   WeightMatrix_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef WEIGHT_MATRIX_P1_TRK_H
#define WEIGHT_MATRIX_P1_TRK_H

#include <vector>

namespace Trk
{
    class WeightMatrix_p1
    {
        public:
            WeightMatrix_p1() {}
        private:
            std :: vector<double> m_mat;
            int                   m_nrow;
    };
}

#endif // WEIGHT_MATRIX_P1_TRK_H
