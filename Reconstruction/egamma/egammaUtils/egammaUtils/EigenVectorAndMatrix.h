/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAUTILS_HEPVECTORANDMATRIX_H
#define EGAMMAUTILS_HEPVECTORANDMATRIX_H

#include "EventPrimitives/EventPrimitives.h"

class EigenVectorAndMatrix {

public:
 EigenVectorAndMatrix(Amg::VectorX &v, Amg::MatrixX  &m) : vector(v), matrix(m) {
  }
  Amg::VectorX& getVector() {
    return vector;
  }
  Amg::MatrixX& getMatrix() {
    return matrix;
  }
  const Amg::VectorX& getVector() const {
    return vector;
  }
  const Amg::MatrixX& getMatrix() const {
    return matrix;
  }

private:
  Amg::VectorX vector;
  Amg::MatrixX matrix;
};

#endif
