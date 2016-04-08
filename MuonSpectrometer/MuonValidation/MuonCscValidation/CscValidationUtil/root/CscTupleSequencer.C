/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscTupleSequencer.C

#include "CscTupleSequencer.h"

int CscTupleSequencer::sequence(const TTree* ptree) const {
  m_ents.clear();
  if ( ptree == 0 ) {
    cout << "CscTupleSequencer: null tree" << endl;
    return 1;
  }
  int nent = ptree->GetEntries();
  int iinit = m_peval->init(ptree);
  if ( iinit != 0 ) {
    cout << "CscTupleSequencer: Error initializing evaluatior: " << iinit << endl;
    return 2;
  }
  for ( int ient=0; ient<nent; ++ient ) {
    m_vals.push_back((*m_peval)(ient));
    m_ents.push_back(ient);
  }
  TMath::Sort(nent, &m_vals[0], &m_ents[0], false);
  return 0;
}

template<class R>
int EventNumberEvaluator<R>::init(TTree* ptree) const {
  if ( ptree == 0 ) {
    cout << "EventNumberEvaluator:init: null tree " << endl;
    return -1;
  }
  if ( m_ptree != ptree ) {
    m_ptree = ptree;
    m_preader = m_getfun();
    if ( m_preader == 0 ) {
      cout << "EventNumberEvaluator:init: null reader " << endl;
      return -2;
    }
  }
  return 0;
}

template<class R>
double EventNumberEvaluator<R>::operator()(int ient) const {
  if ( m_ptree == 0 ) {
    cout << "EventNumberEvaluator: null tree " << endl;
    return -1.0;
  }
  if ( m_preader == 0 ) {
    cout << "EventNumberEvaluator: null reader " << endl;
    return -2.0;
  }
  int entry_size = m_preader->GetEntry(ient);
  if ( entry_size <= 0 ) {
    cout << "EventNumberEvaluator: GetEntry returned " << entry_size << endl;
    return -3.0;
  }
  int ievt = m_preader->evt;
  double xevt = ievt;
  return xevt;
}
  
