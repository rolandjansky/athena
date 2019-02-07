/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  BrandeisMomentumResolution.h 
 *
 *  Created on: March 4, 2013
 *  Author(s): G.Sciolla, A.Venturini, S.Zambito
 *
 *      stefano.zambito@cern.ch
 *      alessio.venturini@cern.ch
 *      gabriella.sciolla@cern.ch
 */
#ifndef BRANDEISMOMENTUMRESOLUTION_H_
#define BRANDEISMOMENTUMRESOLUTION_H_

namespace Brandeis
{
  // constants
  static const double etaID =         2.15;
  static const double p2ID  =     0.957071;
  static const double p3ID  =  -0.00404691;
  static const double p4ID  =  1.85087e-05;
  static const double p5ID  = -1.93901e-08;
  static const double p6ID  =    -0.272664;
  static const double p7ID  =  -0.00123656;
  static const double p8ID  = -1.39701e-06;
  static const double p9ID  =  1.78057e-09;
  static const double p10ID =    0.0410159;
  static const double p11ID =  0.000641245;
  static const double p12ID = -1.73726e-07;
  static const double p13ID =   1.1309e-10;
  static const double p14ID =  -0.00100343;
  static const double p15ID =  -2.8498e-05;
  static const double p16ID = -1.16844e-07;
  static const double p17ID =  1.06385e-10;
  static const double p18ID =      6.01054;
  static const double p19ID =    -0.127711;
  static const double p20ID =  0.000451186;
  static const double p21ID = -4.55246e-07;
  static const double p22ID =      27.1306;
  static const double p23ID =     0.194475;
  static const double p24ID = -0.000715187;
  static const double p25ID =  5.41585e-07;
  static const double p26ID =    0.0166012;
  static const double p27ID =    0.0362636;
  static const double p28ID =     0.283329;

  // methods
  double GetMomentumResolutionID( double mu_pt, double mu_eta, bool isMC );
}

#endif /* BRANDEISMOMENTUMRESOLUTION_H_ */
