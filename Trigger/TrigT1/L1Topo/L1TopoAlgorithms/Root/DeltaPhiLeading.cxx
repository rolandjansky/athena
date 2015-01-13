//  DeltaPhiLeading.cpp
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoAlgorithms/DeltaPhiLeading.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(DeltaPhiLeading)

using namespace std;

namespace TCS {
  bool deltaPhi(int phi1, int phi2, int cut) {
    return abs(phi1-phi2) > cut;
  }
}

TCS::DeltaPhiLeading::DeltaPhiLeading(const std::string & name) : DecisionAlg(name)
{
  defineParameter("MaxTob", 1); // array index 0;
  defineParameter("DeltaPhiMin", 0);
  defineParameter("DeltaPhiMax", 6);
  defineParameter("MinET",1);
  setNumberOutputBits(1);
}

TCS::DeltaPhiLeading::~DeltaPhiLeading()
{}


TCS::StatusCode
TCS::DeltaPhiLeading::process(const std::vector<TCS::TOBArray const *> & input,
                              const std::vector<TCS::TOBArray *> & output,
                              Decision & decison)
{
   cout << "input size: " << input.size() << endl;
  
   if(input.size() == 1) {
      int count = 0;
      for(TCS::TOBArray::const_iterator tob1 = input[0]->begin(); tob1 != input[0]->end(); ++tob1) {
         cout << "Distance1: " << count << " " << std::distance(input[0]->begin(), tob1) << " Loc " << parameter("NumberLeading1").value() << endl;
         count++;
      
         // test array length
         if(std::distance(input[0]->begin(), tob1) > parameter("NumberLeading1").value()-1) continue; // 0 >= 0;

         // test EtMin
         cout << "     Et1: " << (*tob1)->Et() << endl;
         if( (*tob1)->Et() < parameter("EtMin").value() ) continue;
      
         TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
         int count2 = 0;
         for(; tob2 != input[0]->end(); ++tob2) {
            cout << "Distance2: " << count2 << " " << std::distance(input[0]->begin(), tob2) << " Loc " <<parameter("NumberLeading2").value()<< endl;
            count2++;
	
            // test array length
            if(std::distance(input[0]->begin(), tob2) > parameter("NumberLeading2").value()) continue;

            // test EtMin
            cout << "     Et2: " << (*tob2)->Et() << endl;
            if((*tob2)->Et() < parameter("EtMin").value() ) continue;
	
            // test DeltaPhiMin, DeltaPhiMax
            cout << "     DeltaPhi: " << (*tob1)->phi()-(*tob2)->phi() << endl;
            bool accept1 = deltaPhi((*tob1)->phi(), (*tob2)->phi(), parameter("DeltaPhiMin").value());
            bool accept2 = deltaPhi((*tob1)->phi(), (*tob2)->phi(), parameter("DeltaPhiMax").value());

            if(accept1 && !accept2) {
               output[0]->push_back(TCS::CompositeTOB(*tob1, *tob2));
            }

         }
      }

      decison.setBit(0, output.size()>0);  
    
   } else if( input.size() == 2) {
      for(TCS::TOBArray::const_iterator tob1 = input[0]->begin(); tob1 != input[0]->end(); ++tob1) {

         // test array length
         if(std::distance(input[0]->begin(), tob1) > parameter("NumberLeading1").value()-1) continue;

         // test EtMin
         cout << "     Et1: " << (*tob1)->Et() << endl;
         if( (*tob1)->Et() < parameter("EtMin").value() ) continue;

         TCS::TOBArray::const_iterator tob2 = input[1]->begin(); ++tob2;
         for(TCS::TOBArray::const_iterator tob2 = input[1]->begin(); tob2 != input[1]->end(); ++tob2) {

            // test array length
            if(std::distance(input[1]->begin(), tob2) > parameter("NumberLeading2").value()-1) continue;

            // test EtMin
            cout << "     Et2: " << (*tob2)->Et() << endl;
            if( (*tob2)->Et() < parameter("EtMin").value() ) continue;

            // test DeltaPhiMin, DeltaPhiMax		
            cout << "DeltaPhi: " << (*tob1)->phi()-(*tob2)->phi() << endl;
            bool accept1 = deltaPhi((*tob1)->phi(), (*tob2)->phi(), parameter("DeltaPhiMin").value());
            bool accept2 = deltaPhi((*tob1)->phi(), (*tob2)->phi(), parameter("DeltaPhiMax").value());
	
            if(accept1 && !accept2) {
               output[0]->push_back(TCS::CompositeTOB(*tob1, *tob2));
            }
         }
      }

      decison.setBit(0, output[0]->size()>0);

   } else {

      TCS_EXCEPTION("DeltaPhiLeading alg must have either 1 or 2 inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
