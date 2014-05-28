/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  BinsDiffFromStripMedian.h 
// Author: FENG TIAN (columbia university)
   Email: Feng.Tian@cern.ch
*/

#ifndef BinsDiffFromStripMedian_H
#define BinsDiffFromStripMedian_H

#include <dqm_core/Algorithm.h>
#include <vector>
#include <iostream>
#include <string>
enum color{green,yellow,red};
class bin;
class colorbin;
class colorcluster;

namespace dqm_algorithms
{
	struct BinsDiffFromStripMedian : public dqm_core::Algorithm
        {
	  BinsDiffFromStripMedian();
	  
	  ~BinsDiffFromStripMedian();

	    //overwrites virtual functions
	  BinsDiffFromStripMedian * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          void FindStripMedian(std::vector<double> onestrip,std::vector<double>& stripsMedian);
       //   void MakeTag(colorbin,std::string&);
          colorcluster MakeCluster(const int r0,const int r2,bin & onebin, std::vector<std::vector<
colorbin> > & ColorBinMap);
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	};
}

class bin{
 public:
  double m_eta;
  double m_phi;
  int    m_ix;
  int    m_iy;
  double m_value;
  double m_outstandingRatio;
} ;

class colorbin{
 public:
  double m_eta;
  double m_phi;
  int    m_ix;
  int    m_iy;
  double m_value;
  color  m_color;
  bool   m_status;  // true: not in cluster  ; false: in cluster
};
class colorcluster{
 public:
  double m_eta;
  double m_phi;
  double m_radius;
  double m_value;
  double m_color;
  int    m_size;
};


#endif // BinsDiffFromStripMedian_H
