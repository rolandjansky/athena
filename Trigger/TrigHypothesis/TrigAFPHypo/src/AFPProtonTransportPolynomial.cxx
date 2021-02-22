/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "AFPProtonTransportPolynomial.h"

AFPProtonTransportPolynomial::AFPProtonTransportPolynomial(int n, double c0, double c1, double c2, double c3, double c4,
                                                           double c5, double c6, double c7, double c8, double c9) {
  setCoefficients(n, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9);
}

int AFPProtonTransportPolynomial::numCoefficients() const {
  return m_numCoefficients;
}

void AFPProtonTransportPolynomial::setCoefficients
  (int n, double c0, double c1, double c2, double c3, double c4,
  double c5, double c6, double c7, double c8, double c9) {
  m_numCoefficients = n;
  m_coefficients[0] = c0;
  m_coefficients[1] = c1;
  m_coefficients[2] = c2;
  m_coefficients[3] = c3;
  m_coefficients[4] = c4;
  m_coefficients[5] = c5;
  m_coefficients[6] = c6;
  m_coefficients[7] = c7;
  m_coefficients[8] = c8;
  m_coefficients[9] = c9;
}

double AFPProtonTransportPolynomial::evaluate(double x) const {
  double xn[10];
  double sum = 0;

  xn[0] = 1;
  for (int iCoef = 1; iCoef < 10; iCoef++) {
    xn[iCoef] = xn[iCoef - 1] * x;
  }

  for (int iCoef = 0; iCoef < 10; iCoef++) {
    sum += m_coefficients[iCoef] * xn[iCoef];
  }
  return sum;
}
