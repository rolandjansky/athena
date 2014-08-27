/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: GeneratorFilters/HtoVVFilter.cxx
// Description:
//   
//  The purpose of this EF is to select events by the numbers
// of Higgs Bosons  
//
// AuthorList:
// 
//  James Ferrando
//


#ifndef GENERATORFILTERSMULTIHIGGSFILTER_H
#define GENERATORFILTERSMULTIHIGGSFILTER_H

#include "GeneratorModules/GenFilter.h"

class MultiHiggsFilter:public GenFilter {
public:
  MultiHiggsFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MultiHiggsFilter();
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:
  // Setable Properties:-
  
  // Local Member Data:-
  int m_NHiggs;
  int m_Status;
  bool m_UseStatus;
  bool m_Exclusive;

  // Private Methods:=
  int m_nMultiHiggs;
  int m_nGoodMultiHiggs;
};

#endif
