/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file GraphTest.h file declares the dqm_algorithms::GraphTest  class.
 * \author Haleh Hadavand
*/
#ifndef DQM_ALGORITHMS_GRAPHTEST_H
#define DQM_ALGORITHMS_GRAPHTEST_H
#include <string>
#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
  /*! \class tile_dq::GraphTest
   * GraphTest DQ algorithm: check validity of a TGraph (or TH1)
   * This test can be used to perform a test on a TGraph with (or without)
   * error bars, in case asymmmetric.
   * The algorithm accepts also TH1 as input, in such a case
   * the histogram with be translated into a TGraph object (without error bars)
   * It performs two tests:
   * 1- It checks that each point in the TGraph has "small" error bars
   * 2- It checks that each point is "close" to a reference value
   * For test 2 a reference TGraph has to be provided (in this way
   * each point may have a different reference value). The (x,y)
   * coordinates of the point are compared with the reference point
   * coordinates (x_r,y_r) and REFERENCE point error bars defining
   * the maximum allowed distance. i.e. 
   * the point is good if:
   *     x_r - f*x_err < x < x_r + f*x_err
   *                    and
   *     y_r - f*y_err < y < y_r + f*y_err
   * "f" is a factor that can be used to specify different limits for Green and Red 
   * thresholds.
   * If, for a specific reference point, the error bars along one coordinate are set to
   * zero the test 2 is NOT performed for that point (this can be used to exclude, for
   * example, a dead channel through the reference graph).
   * The algorithms can work also with TGraphAsymmErrors object, so high and low errors
   * can always be specified separately.
   * The parameters that can be set, separetely, for Green and Red thresholds are:
   *    XErrHigh   : Maximum (higher) error bar length x coord.
   *    XErrLow    : Maximum (lower) error bar length x coord.
   *    YErrHigh   : Maximum (higher) error bar length y coord.
   *    YErrLow    : Maximum (lower) error bar length y coord.
   *    DistFactor : The factor "f" used in second part of the test
   * Detailed example in workbench macro: TestGraphTest.C
   */
  class GraphTest : public dqm_core::Algorithm
    {
    public:
      GraphTest();
      virtual GraphTest* clone();
      virtual dqm_core::Result* execute(const std::string &,
					const TObject & obj, 
					const dqm_core::AlgorithmConfig & conf );
      virtual ~GraphTest();
      using dqm_core::Algorithm::printDescription;
      void printDescription(std::ostream& out);
    };
}

#endif // DQM_ALGORITHMS_GRAPHTEST_H
