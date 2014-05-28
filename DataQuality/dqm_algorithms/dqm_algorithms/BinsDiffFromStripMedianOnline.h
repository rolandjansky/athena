/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  BinsDiffFromStripMedianOnline.h 
// Author: FENG TIAN (columbia university)
   Email: Feng.Tian@cern.ch
*/

#ifndef BinsDiffFromStripMedianOnline_H
#define BinsDiffFromStripMedianOnline_H

#include <dqm_core/Algorithm.h>
#include "dqm_algorithms/BinsDiffFromStripMedian.h"
#include <vector>
#include <iostream>
#include <string>

//class binOnline;
//class colorbinOnline;
//class colorclusterOnline;

namespace dqm_algorithms
{
	struct BinsDiffFromStripMedianOnline : public dqm_core::Algorithm
        {
          class binOnline;
          class colorbinOnline;
          class colorclusterOnline;

	  BinsDiffFromStripMedianOnline();
	  
	  ~BinsDiffFromStripMedianOnline();

	    //overwrites virtual functions
	  BinsDiffFromStripMedianOnline * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          void FindStripMedianOnline(const std::string & ,std::vector<double> onestrip,std::vector<double>& stripsMedian);
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
          colorclusterOnline MakeClusterOnline(const std::string &  name,const int r0,const int r2,binOnline & onebin, std::vector<std::vector<colorbinOnline> > & ColorBinMap);

          void AddToList(const int r0,const int r2,int i,int j,std::vector<std::vector<colorbinOnline> > & ColorBinMap, std::vector<colorbinOnline>& LookAtList);
          double CalEta(std::vector<colorbinOnline>& LookAtList);
          double CalPhi(std::vector<colorbinOnline>& LookAtList);
          double CalVal(std::vector<colorbinOnline>& LookAtList);
          double CalR(std::vector<colorbinOnline>& LookAtList,double eta, double phi);

          class binOnline{
           public:
           double m_eta;
           double m_phi;
           int    m_ix;
           int    m_iy;
           double m_value;
           double m_outstandingRatio;
           friend bool operator<(const binOnline &left, const binOnline &right) {return left.m_outstandingRatio > right.m_outstandingRatio;}
           } ;

           class colorbinOnline{
            public:
            double m_eta;
            double m_phi;
            int    m_ix;
            int    m_iy;
            double m_value;
            color  m_color;
            bool   m_status;  // true: not in cluster  ; false: in cluster
          };

          class colorclusterOnline{
           public:
            double m_eta;
            double m_phi;
            double m_radius;
            double m_value;
            color m_color;
            int    m_size;
          };

	};
}
#endif // BinsDiffFromStripMedianOnline_H
