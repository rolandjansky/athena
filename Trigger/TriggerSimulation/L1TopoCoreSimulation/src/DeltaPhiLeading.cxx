//
//  DeltaPhiLeading.cpp
//  TopoCore
//
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//

#include "L1TopoCoreSimulation/DeltaPhiLeading.h"
#include "L1TopoCoreSimulation/Exception.h"

REGISTER_ALG_NS(TCS,DeltaPhiLeading)

using namespace std;

namespace TCS {
   bool deltaPhi(int phi1, int phi2, int cut) {
      return abs(phi1-phi2) > cut;
   }
}

TCS::DeltaPhiLeading::DeltaPhiLeading(const std::string & name) : DecisionAlg(name)
{
   defineParameter("NumberLeading1", 2);
   defineParameter("NumberLeading2", 2);
   defineParameter("DeltaPhiMin", 13);
}



TCS::StatusCode
TCS::DeltaPhiLeading::process(const std::vector<TCS::TOBArray> & input,
                              TCS::TOBArray & output, TCS::Decision & decison)
{
   

   if(input.size() == 1) {
      for(TCS::TOBArray::const_iterator tob1 = input[0].begin(); tob1 != input[0].end(); ++tob1) {
         TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;
         for(; tob2 != input[0].end(); ++tob2) {
            bool accept = deltaPhi((*tob1)->phi(), (*tob2)->phi(), 13);
            cout << "Combining " << (*tob1)->Et() << " with " << (*tob2)->Et() << " cut " << accept << endl;
            
            if(accept) {
               output.push_back(TCS::CompositeTOB(*tob1, *tob2));
            }
         }
      }
      decison.setBit(0, output.size()>0);      
   } else if( input.size() == 2) {
      for(TCS::TOBArray::const_iterator tob1 = input[0].begin(); tob1 != input[0].end(); ++tob1) {
         TCS::TOBArray::const_iterator tob2 = input[1].begin(); ++tob2;
         for(TCS::TOBArray::const_iterator tob2 = input[1].begin(); tob2 != input[1].end(); ++tob2) {
            bool accept = deltaPhi((*tob1)->phi(), (*tob2)->phi(), 13);
            cout << "Combining " << (*tob1)->Et() << " with " << (*tob2)->Et() << " cut " << accept << endl;
            
            if(accept) {
               output.push_back(TCS::CompositeTOB(*tob1, *tob2));
            }
         }
      }
      decison.setBit(0, output.size()>0);
   } else {
      TCS_EXCEPTION("DeltaPhiLeading alg must have either 1 or 2 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
