/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_QCDTRUTHJETFILTER_H
#define GENERATORFILTERS_QCDTRUTHJETFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>

class IAtRndmGenSvc;

class QCDTruthJetFilter : public GenFilter {
public:

  QCDTruthJetFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  double m_MinPt;  //!< Min pT for the truth jets
  double m_MaxPt;  //!< Max pT for the truth jets
  double m_MinEta; //!< Min eta for the truth jets
  double m_MaxEta; //!< Max eta for the truth jets
  double m_StartMinEta; //!< Default start value for min eta
  bool   m_SymEta; //!< Use symmetric cut for min eta? (Default false for p-Pb run filters)
  double m_MinPhi;  //!< Min phi for the lead truth jet
  double m_MaxPhi;  //!< Max phi for the lead truth jet
  std::string m_TruthJetContainerName;  //!< Name of the truth jet container

  ServiceHandle<IAtRndmGenSvc> m_rand;  //!< Random number generator

  long m_total;    //!< Total number of events tested
  long m_passed;   //!< Number of events passing all cuts
  long m_ptfailed; //!< Number of events failing the pT cuts

  double m_norm;   //!< Normalization for weights
  double m_high;   //!< High-side function level

  bool m_doShape;  //!< Attempt to flatten the pT distribution

public:

  /// @todo Move to an inline in the cc
  static double fitFnR(const double * x , const double *) {
    static const double p[7] = //{100000,-5,0.0004,0.0000001,1./3600.,23,0.8};
      { 1000000, -5.5, 0.0001, 0.00000012, 1./4150., 23, 0.4 };
    return p[0]*std::pow(x[0],p[1]+p[2]*x[0]+p[3]*pow(x[0],2))*pow(1-x[0]*p[4],p[5])*pow(x[0],p[6]);
  }

  /// @todo Move to an inline in the cc
  static double fitFn(const double x ) {
    static const double p[7] = //{100000,-5,0.0004,0.0000001,1./3600.,23,0.8};
      { 1000000, -5.5, 0.0001, 0.00000012, 1./4150., 23, 0.4 };
    return p[0]*std::pow(x,p[1]+p[2]*x+p[3]*pow(x,2))*pow(1-x*p[4],p[5])*pow(x,p[6]);
  }

};

#endif
