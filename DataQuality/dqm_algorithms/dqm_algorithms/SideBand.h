/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SideBand.h file declares the dqm_algorithms::SideBand  class.
 * \author Andrea Dotti
*/

#ifndef DQM_ALGORITHMS_SIDEBAND_H
#define DQM_ALGORITHMS_SIDEBAND_H

#include <string>
#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
  /*! \class tile_dq::SideBand
   * SideBand DQ algorithm: check number of entries outside range
   * This test can be used to chek, for a 1D histogram, the entries
   * outside a region.
   * The test is perfrormed evaluating the integral of the histogram
   * thus real numbers as bin content are accepted.
   * The parameters that can be set are:
   *     UseUnderFlow : if != 0 include also underflow bin
   *     UseOverFlow  : if != 0 include also overflow bin
   *     Min          : Specify Lower limit (-999999 means full range)
   *     Max          : Specify Upper limit (-999999 means full range)
   * Thresholds are specified through the key-word "Threshold"
   * Detailed example in workbench macro: TestSideBand.C
   *     
   * There are two variants of this algorithms that you can select with 
   * Constructor parameter name:
   *  "SideBand_Absolute" in which the integral is checked in value
   *  "SideBand_Relative" in which the integral is checked as ratio with 
   *                      respect to total integral.
   */
  class SideBand : public dqm_core::Algorithm
    {
    public:
      SideBand(const std::string & name );
      virtual SideBand* clone();
      virtual dqm_core::Result* execute(const std::string &,
					const TObject & obj, 
					const dqm_core::AlgorithmConfig & conf );
      virtual ~SideBand() {};
      using dqm_core::Algorithm::printDescription;
      void printDescription(std::ostream& out);
    private:
      std::string m_name;
    };

}

#endif // DQM_ALGORITHMS_SIDEBAND_H
