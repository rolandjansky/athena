/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAUTILS_HEPVECTORANDMATRIX_H
#define EGAMMAUTILS_HEPVECTORANDMATRIX_H

#include "EventPrimitives/EventPrimitives.h"

class EigenVectorAndMatrix {

public:
 EigenVectorAndMatrix(Amg::VectorX &v, Amg::MatrixX  &m) : m_vector(v), m_matrix(m) {
  }
  Amg::VectorX& getVector() {
    return m_vector;
  }
  Amg::MatrixX& getMatrix() {
    return m_matrix;
  }
  const Amg::VectorX& getVector() const {
    return m_vector;
  }
  const Amg::MatrixX& getMatrix() const {
    return m_matrix;
  }

private:
  Amg::VectorX m_vector;
  Amg::MatrixX m_matrix;
};

#endif
