/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File: GeneratorFilters/GapJetFilter.h
// Description: Filter suppresses events in the low 
//              Pt jets and low forward gaps
// AuthorList: Marek Tasevsky
//
// --------------------------------------------------

#ifndef GENERATOR_FILTERS_GAP_JET_FILTER_H
#define GENERATOR_FILTERS_GAP_JET_FILTER_H

#include "GeneratorModules/GenFilter.h"
#include <vector>
#include <string>

//Random number generator required
class TRandom3;

class GapJetFilter : public GenFilter
{
public:
  GapJetFilter(const std::string & name, ISvcLocator * pSvcLocator);
  virtual ~GapJetFilter();
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:
  // Setable Properties:-

  // Local Member Data:-
  std::string m_jetContainer;

  //Jet kinematic cuts
  double m_minPt1;//GeV
  double m_minPt2;
  double m_maxPt1;
  double m_maxPt2;

  // Particle kinematic cuts
  double m_PtCut; //MeV
  double m_EtaCut;


  //Weights to retrieve the original gap spectrum
  std::vector<double> m_weights;

  //Parameters of double exponential fit for the gap distribution
  double m_c0, m_c1, m_c2, m_c3, m_c4, m_c5, m_c6, m_c7, m_gapf, xsgapf;

  //Random number generator
  TRandom3* myRandGen;

  StoreGateSvc* m_storeGate;

};

#endif // GENERATOR_FILTERS_GAP_JET_FILTER_H
