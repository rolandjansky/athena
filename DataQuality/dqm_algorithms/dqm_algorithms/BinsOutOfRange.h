/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  BinsOutOfRange.h 
// Author: FENG TIAN (columbia university)
   Email: Feng.Tian@cern.ch
*/

#ifndef BinsOutOfRange_H
#define BinsOutOfRange_H

#include <dqm_core/Algorithm.h>
#include <vector>
#include <iostream>
#include <string>
class bin3;
namespace dqm_algorithms
{
	struct BinsOutOfRange: public dqm_core::Algorithm
        {
	  BinsOutOfRange();
	  
	  ~BinsOutOfRange();

	    //overwrites virtual functions
	  BinsOutOfRange * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	};
}

class bin3{
 public:
  double m_eta;
  double m_phi;
  int    m_ix;
  int    m_iy;
  double m_value;
} ;


#endif // BinsOutOfRange_H
