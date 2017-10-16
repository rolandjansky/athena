/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGAFPHYPO_AFPPROTONTRANSPORTPOLYNOMIAL_H
#define TRIGAFPHYPO_AFPPROTONTRANSPORTPOLYNOMIAL_H

class AFPProtonTransportPolynomial {
public:
  /// Default constructor of kinRecoPolynomial. Takes number of coefficients and coefficients itself up to 10 as arguments.
  AFPProtonTransportPolynomial(int n, double c0, double c1 = 0, double c2 = 0, double c3 = 0, double c4 = 0,
                               double c5 = 0, double c6 = 0, double c7 = 0, double c8 = 0, double c9 = 0);

  /// This function returns the number of coefficients. 
  int numCoefficients() const;
  /// This function sets the coefficients of the polynomial. 
  void setCoefficients(int n, double c0, double c1 = 0, double c2 = 0, double c3 = 0, double c4 = 0,
                       double c5 = 0, double c6 = 0, double c7 = 0, double c8 = 0, double c9 = 0);

  /** 
   * This function of returns the sum of the polynomial 
   * coeficients of a given argument x
   */
  double evaluate(double x) const;
private:
  int m_numCoefficients;
  double m_coefficients[10];
};

#endif
