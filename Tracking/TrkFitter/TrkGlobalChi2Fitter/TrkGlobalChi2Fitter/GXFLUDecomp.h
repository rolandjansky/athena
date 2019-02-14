/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GXFLUDECOMP_H
#define GXFLUDECOMP_H

#include "EventPrimitives/EventPrimitives.h"

/**
   * @brief A simple class to hold the LU decomposition of a matrix/

   @author amorley@cern.ch
  */

namespace Trk {
  class GXFLUDecomp {
  public:
    GXFLUDecomp() {} 
    void SetMatrix(Amg::SymMatrixX &);
    Amg::VectorX Solve(Amg::VectorX &, bool &ok);
    Amg::SymMatrixX Invert(bool &ok);
  private:
    Amg::SymMatrixX m_matrix;
    bool m_luSet = false;
    Eigen::LLT < Eigen::MatrixXd > m_lu;
  };
}

#endif
