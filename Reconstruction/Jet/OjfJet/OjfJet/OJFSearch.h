/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OJFSearch.h,v 1.1.1.1 2007-05-10 09:02:21 seuster Exp $


//  +---------------------------------------------------------+
//  |                                                         |
//  |  OPTIMAL JET FINDER v1.0                                |
//  |                                                         |
//  |  (C) 2005 S. Chumakov, E. Jankowski and F. V. Tkachov   |
//  |                                                         |
//  |  Permission is granted for anyone to use this software  |       
//  |  for any purpose on any computer. The authors are not   |
//  |  responsible for the consequences of such use.          |        
//  |  This software is meant to be freely distributed.       |
//  |                                                         |
//  +---------------------------------------------------------+


// Implementation of a simple jet search based on the "core" 
// of Optimal Jet Finder

// References:
// [1] D. Yu. Grigoriev, E. Jankowski, F. V. Tkachov,
//     Comput. Phys. Commun. 155, 42 (2003) [hep-ph/0301226].
// [2] S. Chumakov, E. Jankowski, F. V. Tkachov,
//     the accompanying Comput. Phys. Commun. paper [paper.pdf].

// See the comments in the source code OJFSearch.cpp for more information.


#ifndef OJFSEARCH_H
#define OJFSEARCH_H

#include "OjfJet/OJFJets.h"


namespace OptimalJetFinder {


  
  class JetSearch {
    JetSearch( const JetSearch& );
    JetSearch& operator=( const JetSearch& );
  public:
    JetSearch( const Event* PP, 
	       double Radius,
	       unsigned Ntries = default_ntries );
    ~JetSearch();
    bool FindJetsForFixedNJets( unsigned Njets );
    unsigned FindJetsForOmegaCut( double OmegaCut );
    Jets* GetJets() const { return Q; }
    void SetNTries( unsigned Ntries );
    unsigned GetNTries() const { return ntries; }
    void SetMaxIter( unsigned MaxIter );
    unsigned GetMaxIter() const { return maxIter; }
    void SetNJetsBegin( unsigned nBegin );
    unsigned GetNJetsBegin() const { return nstart; }
    void SetNJetsEnd( unsigned nEnd );
    unsigned GetNJetsEnd() const { return nstop; }
  private:
    static const double INF;
    static const int default_max_iter;
    static const int default_ntries;
    static const int default_njets_start;
    static const int default_njets_stop;
    unsigned ntries;
    unsigned nstart, nstop;
    unsigned maxIter;
    double radius;
    const Event* P;
    Jets* Q;
  };

}

#endif //OJFSEARCH_H
