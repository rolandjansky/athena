/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGAFPHYPO_AFPPROTONTRANSPORTPARAM_H
#define TRIGAFPHYPO_AFPPROTONTRANSPORTPARAM_H

#include "TrigAFPHypo/AFPProtonTransportPolynomial.h"

class AFPProtonTransportParam {
public:
  /*! Default constructor of kinRecoVarParam class. Takes nominal energy and array of polynomials as arguments.*/
  AFPProtonTransportParam (double E0_, AFPProtonTransportPolynomial* p_[8]);
  /*! Default destructor of kinRecoVarParam class. */
  ~AFPProtonTransportParam();

  /*! Evaluates value of parameterization given a certain polynomial */
  double evaluate(double x0, double y0, double z0, double sx, double sy, double E) const;
private:
  ///Nominal energy
  double m_energy;
  ///Object of the AFPProtonTransportPolynomial class used for the position calculations
  AFPProtonTransportPolynomial* m_polynomial[8];
};

#endif
