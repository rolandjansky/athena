/*                                                                                                    
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration                             
*/

#include "TrigAFPHypo/AFPProtonTransportParam.h"

AFPProtonTransportParam::AFPProtonTransportParam(double beam_energy, AFPProtonTransportPolynomial* poly_array[8]) {
  m_energy = beam_energy;
  for (int i = 0; i < 8; i++) m_polynomial[i] = poly_array[i];
}

AFPProtonTransportParam::~AFPProtonTransportParam () {
  for (int i = 0; i < 8; i++)
    delete m_polynomial[i];
}

double AFPProtonTransportParam::evaluate(double x0, double y0, double z0, double sx0, double sy0, double E) const {
  const double xi = (m_energy - E) / m_energy;

  double v0 = m_polynomial[0]->evaluate(xi);
  double v1 = x0 * m_polynomial[1]->evaluate(xi);
  double v2 = y0 * m_polynomial[2]->evaluate(xi);
  double v3 = z0 * m_polynomial[3]->evaluate(xi);
  double v4 = sx0 * m_polynomial[4]->evaluate(xi);
  double v5 = sy0 * m_polynomial[5]->evaluate(xi);
  double v6 = z0 * sx0 * m_polynomial[6]->evaluate(xi);
  double v7 = z0 * sy0 * m_polynomial[7]->evaluate(xi);

  double value = v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7;

  return value;
}
