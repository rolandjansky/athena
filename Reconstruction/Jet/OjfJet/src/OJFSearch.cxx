/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OJFSearch.cxx,v 1.1.1.1 2007-05-10 09:02:22 seuster Exp $


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


// Implementation of a simple jet search based on
// the "core" of Optimal Jet Finder

// References:
// [1] D. Yu. Grigoriev, E. Jankowski, F. V. Tkachov,
//     Comput. Phys. Commun. 155, 42 (2003) [hep-ph/0301226].
// [2] S. Chumakov, E. Jankowski, F. V. Tkachov, 
//     the accompanying Comput. Phys. Commun. paper [paper.pdf].

     
#include "OjfJet/OJFSearch.h"
#include "OjfJet/OJFJets.h"
#include <cassert>
#include <vector>

#include <iostream>

//RS using namespace std;

//#define NDEBUG


namespace OptimalJetFinder {


  const double JetSearch::INF =  1.0E+100;
  const int JetSearch::default_max_iter = 1000;
  const int JetSearch::default_ntries = 10;
  const int JetSearch::default_njets_start = 1;
  const int JetSearch::default_njets_stop = 100;



  // This class represents a simple jet search.
  // The user may find it necessary to construct 
  // other classes performing similar tasks, using this one as an example
  // (e.g., to allow a non-random initial jet configuration).
  // PP points to the HEP event (where the initial data is).
  // Radius is the parameter R of eq. (20) in ref. [1].
  // Ntries is the number of times different random initial jet
  // configurations are tried. (The minimization algorithm is iterative
  // and requires some initial jet configuration. Here, the initial
  // configuration is taken completely random. In some cases, it may be
  // beneficial to run OJF on already pre-defined jet configuration,
  // e.g., to finetune the output of another jet finding algorithm. )

  JetSearch::JetSearch( const Event* PP, double Radius, unsigned Ntries ) 
    : ntries( Ntries ),
      nstart( default_njets_start ),
      nstop( default_njets_stop ),
      maxIter( default_max_iter ),
      radius( Radius ),
      P( PP ),
      Q( 0 ) {

    assert( ntries > 0 );
    assert( radius > 0 );
    assert( P );

  }



  JetSearch::~JetSearch() {

    if( Q ) delete Q;

  }


  
  // Sets the number of times different random initial jet
  // configurations are tried. (The minimization algorithm is iterative
  // and requires some initial jet configuration.
  // Here, the initial configuration is taken completely random.)
  // The default value is default_ntries = 10.

  void JetSearch::SetNTries( unsigned Ntries ) {

    assert( Ntries > 0 );
    ntries = Ntries;

  }



  // Sets the maximal number of iterations.
  // If a local minimum is not found within 'maxIter' iterations,
  // the minimization is terminated (for safety reasons).
  // The default value is default_max_iter = 2000.

  void JetSearch::SetMaxIter( unsigned MaxIter ) { 

    assert( MaxIter > 0 );
    maxIter = MaxIter;

  }



  // Sets the initial number of jets
  // for int JetSearch::FindJetsForOmegaCut( double omegaCut ).
  // See last paragraph of section 2.1 in ref. [1]
  // for more information how the final number of jets is determined.

  void JetSearch::SetNJetsBegin( unsigned nBegin ) { 

    assert( nBegin > 0 );
    nstart = nBegin;

  }



  // Sets the maximal number of jets 
  // for int JetSearch::FindJetsForOmegaCut( double omegaCut ).
  // See last paragraph of section 2.1 in ref. [1] for more information
  // how the final number of jets is determined.

  void JetSearch::SetNJetsEnd( unsigned nEnd ) { 

    assert( nEnd > 0 );
    nstop = nEnd;

  }



  // Finds the final jet configuration with
  // a fixed number of jets equal to njets.
  // Returns true if successful and false otherwise (how original!).
  // For each "try", it starts with a random initial jet configuration
  // and finds a local minimum of Omega function, eq. (20) in ref. [1].
  // After some number of tries (parameter ntries),
  // the best jet configuration is chosen,
  // i.e. the one that gives the smallest value of Omega function
  // (the deepest local minimum). 

  bool JetSearch::FindJetsForFixedNJets( unsigned njets ) {

    assert ( njets > 0 );
    
    double omegaBest = INF;
    
    Q = 0;

    bool zaj_copy_created = false;

    std::vector<Position*> vpos;

    for( unsigned i = 1; i<=ntries; i++ ) {

      Jets* Qtemp = new Jets( njets, P, radius );

      Qtemp->Randomize();

      bool success = Qtemp->Minimize( maxIter );

      if( success ) {

	double omega = Qtemp->GetOmega();

	if( omega < omegaBest ) {

	  omegaBest = omega;

	  if( Q ) delete Q;
	  Q = Qtemp;

	  if( ntries > 1 ) {
 
	    if( ! zaj_copy_created ) {
	      zaj_copy_created = true;
	      Particle* a = P->GetFirst();
	      while( a ) {
		vpos.push_back( new Position( a->GetPosition() ) );
		a = a->GetNext();
	      }

	    } //if( ! zaj_copy_created )

	    else {
	      assert ( vpos.size() == P->GetNumber() ); 
	      Particle* a = P->GetFirst();
	      std::vector<Position*>::iterator vi = vpos.begin();

		while( a ) {
		  (*vi)->Copy( a->GetPosition() );
		  a = a->GetNext();
		  ++vi;
		}

	    } //else if( ! zaj_copy_created )
 
	  } //end: copy the recombination matrix
	} //if ( omega < omegaBest)
      } //if ( Qtemp->Minimize( maxIter ) )

      if( Qtemp != Q ) delete Qtemp;


    } //end of the loop over tries



    //copy the combination matrix back

    if( zaj_copy_created ) { 
      assert ( vpos.size() == static_cast<unsigned>( P->GetNumber() ) ); 
      Particle* a = P->GetFirst();
      std::vector<Position*>::iterator vi = vpos.begin();
      while( a ) {
	a->GetPosition()->Copy( *vi );
	delete *vi;
	a = a->GetNext();
	++vi;
      }
    } //if ( zaj_copy_created )

 
    if( Q ) return true;
    return false;
    
  }



  // Finds the final jet configuration for omega_cut 
  // of relation (21) in ref. [1] and returns the number of jets
  // in the final jet configuration or 0 if the search is not successful.
  // It runs "JetSearch::FindJetsForFixedNJets(unsigned njets)"
  // increasing the number of jets between the values set
  // by "JetSearch::SetNJetsBegin( unsigned nBegin )"
  // and "JetSearch::SetNJetsEnd( unsigned nEnd )".
  // The final jet configuration is the one with the smallest number of jets
  // for which the value of Omega function (eq. (20) in ref. [1])
  // is smaller than omegaCut parameter.  

  unsigned JetSearch::FindJetsForOmegaCut( double omegaCut ) {

    assert ( omegaCut > 0 );
    assert (  nstart > 0   &&   nstop > 0   &&   nstart <= nstop   );
    
    unsigned njets = nstart;

    while( njets <= nstop ) {
      
      bool success = FindJetsForFixedNJets( njets );

      if(  ! success  ) {
	if( Q ) {
	  delete Q;
	  Q = 0;
	}
	return 0;
      }
      
      if( Q && Q->GetOmega() < omegaCut ) {
	return njets;
      }
      
      // std::cout << "Rolf : OJF : njets = " << njets << " omega " << Q->GetOmega() << "\n";
      njets++;
      
      if( Q ) {
	delete Q;
	Q = 0;
      }

    } //while( njets <= nstop ) {

    if( Q ) {
      delete Q;
      Q = 0;
    }
    return 0;

  }


}
