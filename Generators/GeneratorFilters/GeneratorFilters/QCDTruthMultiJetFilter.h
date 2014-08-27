/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_QCDTRUTHMULTIJETFILTER_H
#define GENERATORFILTERS_QCDTRUTHMULTIJETFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>

class IAtRndmGenSvc;

class QCDTruthMultiJetFilter : public GenFilter {
public:

  QCDTruthMultiJetFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  int m_Njet;  //!< Number of truth jets required above m_NjetMinPt
  double m_NjetMinPt;  //!< Min pT for N jets truth jets required
  double m_MinLeadJetPt;  //!< Min pT for the leading truth jet
  double m_MaxLeadJetPt;  //!< Max pT for the leading truth jet
  double m_MaxEta; //!< Max eta for the truth jets

  std::string m_TruthJetContainerName;  //!< Name of the truth jet container

  ServiceHandle<IAtRndmGenSvc> m_rand;  //!< Random number generator

  long m_total;    //!< Total number of events tested
  long m_passed;   //!< Number of events passing all cuts
  long m_ptfailed; //!< Number of events failing the pT cuts
  long m_nJetsFailed; //!< Number of events failing the nJets cuts

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
