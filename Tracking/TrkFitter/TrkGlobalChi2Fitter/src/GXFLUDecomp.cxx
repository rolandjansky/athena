/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGlobalChi2Fitter/GXFLUDecomp.h"

namespace Trk {
  void GXFLUDecomp::SetMatrix(Amg::SymMatrixX & matrix) {
    m_matrix = matrix;
    m_luSet = false;
  } 
  
  Amg::VectorX GXFLUDecomp::Solve(Amg::VectorX & vector, bool &ok) {
    ok = true;
    
    if (!m_luSet) {
      m_lu = Eigen::LLT < Eigen::MatrixXd > (m_matrix);
      m_luSet = true;
    }
    
    if (m_lu.info() != Eigen::Success) {
      ok = false;
      return Eigen::VectorXd::Zero(vector.size());
    }
    
    return m_lu.solve(vector);
  }

  Amg::SymMatrixX GXFLUDecomp::Invert(bool &ok) {
    ok = true;
    
    if (!m_luSet) {
      m_lu = Eigen::LLT < Eigen::MatrixXd > (m_matrix);
      m_luSet = true;
    }

    int ncols = m_matrix.cols();
    
    if (m_lu.info() != Eigen::Success) {
      ok = false;
      return Eigen::MatrixXd::Identity(ncols, ncols);
    }

    return m_lu.solve(Eigen::MatrixXd::Identity(ncols, ncols));
  }
}                               // end namespace Trk
