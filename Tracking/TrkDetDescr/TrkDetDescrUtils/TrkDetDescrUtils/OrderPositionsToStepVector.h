/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinUtility.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_ORDERPOSITIONSTOSTEPVECTOR_H
#define TRKDETDESCRUTILS_ORDERPOSITIONSTOSTEPVECTOR_H

#include <vector>
#include <algorithm>

namespace Trk {

  /** @class OrderPositionsToStepVector

    Class inheriting from std::vector<double> to transform 1-dim
    un-aequidistant positions (x,y,z,r-values) into stepvectors
    for BinUtility1D usage.

    The constructor takes r/z values as a std::vector<double>,
    a mimimum and a maximum boundary.

    @author Andreas.Salzburger@cern.ch

    */

    class OrderPositionsToStepVector : public std::vector<double> {
      public:
        /**Constructor with arguments*/
        OrderPositionsToStepVector(std::vector<double>& rzvalues, double rzmin, double rzmax);
        /**Destructor*/
        ~OrderPositionsToStepVector(){}

      private:
        /** Forbidden constructor */
        OrderPositionsToStepVector(){}

    };

inline OrderPositionsToStepVector::OrderPositionsToStepVector(std::vector<double>& values, double min, double max) :
         std::vector<double>(values.size())
   {
        unsigned int vectorSize = values.size();
        // sort the vector
        sort(values.begin(), values.end());
        // first step - necessary because of correction of parsecLayerRmin
        double firstStep = fabs(0.5*(values[1]+values[0])-min);
        double lastValue = min + firstStep;
        push_back(firstStep);
        // all steps between second and one before last
        for (unsigned int istep=2; istep<vectorSize; ++istep){
             double currentStep = fabs(0.5*(values[istep]+values[istep-1])-lastValue);
             push_back(currentStep);
             lastValue += currentStep;
        }
        // last step
        push_back(fabs(max -0.5*(values[vectorSize-2]+values[vectorSize-1])));
     }


}

#endif


