/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  KillBinsByStrip.h
    Author: Olivier Simard (CEA-Saclay)
    Email:  Olivier.Simard@cern.ch
*/

#ifndef DQM_ALGORITHMS_KILLBINSBYSTRIP_H
#define DQM_ALGORITHMS_KILLBINSBYSTRIP_H

#include <dqm_core/Algorithm.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <string>
class bin2;

namespace dqm_algorithms
{
        struct KillBinsByStrip : public dqm_core::Algorithm
        {
          KillBinsByStrip();
         
          ~KillBinsByStrip();

          // overwrites virtual functions
          KillBinsByStrip* clone();
          dqm_core::Result* execute(const std::string&,const TObject&,const dqm_core::AlgorithmConfig&);
          //void MakeTag(bin2,std::string&); // not yet implemented
          using dqm_core::Algorithm::printDescription;
          void printDescription(std::ostream& out);
        };
}
class bin2 {
 public:
  double m_eta;
  double m_phi;
  int m_ix;
  int m_iy;
  double m_value;
  double m_deviation;
};
#endif // DQM_ALGORITHMS_KILLBINSBYSTRIP_H
