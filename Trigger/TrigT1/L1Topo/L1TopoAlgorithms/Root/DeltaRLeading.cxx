//  DeltaRLeading.cpp
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoAlgorithms/DeltaRLeading.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include <math.h>

REGISTER_ALG_TCS(DeltaRLeading)

using namespace std;

namespace TCS {
  bool deltaR2(int phi1, int phi2, int eta1, int eta2, int cut) {
    return ( (phi1-phi2)*(phi1-phi2) + (eta1-eta2)*(eta1-eta2) ) > cut;
  }
}

TCS::DeltaRLeading::DeltaRLeading(const std::string & name) : 
   DecisionAlg(name)
{
   defineParameter("NumberLeading1", 1); // array index 0;
   defineParameter("NumberLeading2", 1); 
   defineParameter("DeltaR2Min", 0);
   defineParameter("DeltaR2Max", 30);
   defineParameter("DeltaPhiMin", 0);
   defineParameter("DeltaPhiMax", 30);
   defineParameter("DeltaEtaMin", 0);
   defineParameter("DeltaEtaMax", 30);
   defineParameter("EtaMax", 30);
   defineParameter("EtMin",1);
   setNumberOutputBits(1);
}




TCS::StatusCode
TCS::DeltaRLeading::process(const std::vector<TCS::TOBArray const *> & input,
                              const std::vector<TCS::TOBArray *> & output,
                              Decision & decison)
{
   cout << "input size: " << input.size() << endl;
  
   if(input.size() == 1) {
      cout << "here" << endl;
      int count = 0;
      for(TCS::TOBArray::const_iterator tob1 = input[0]->begin(); tob1 != input[0]->end(); ++tob1) {
         cout << "Distance1: " << count << " " << std::distance(input[0]->begin(), tob1) << " Loc " <<parameter("NumberLeading1").value()<< endl;
         count++;
      
         // test array length
         if(std::distance(input[0]->begin(), tob1) > parameter("NumberLeading1").value()-1) continue; // 0 >= 0;
      
         // test EtMin
         if((*tob1)->Et() < ((int) parameter("EtMin").value())) continue;
      
         // test EtaMax
         if((*tob1)->eta() > ((int) parameter("EtaMax").value())) continue;
      
         TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
         int count2 = 0;
         for(; tob2 != input[0]->end(); ++tob2) {
            cout << "Distance2: " << count2 << " " << std::distance(input[0]->begin(), tob2) << " Loc " <<parameter("NumberLeading2").value()<< endl;
            count2++;
	
            // test array length
            if(std::distance(input[0]->begin(), tob2) > parameter("NumberLeading2").value()) continue;

            // test EtMin
            if((*tob2)->Et() < ((int) parameter("EtMin").value())) continue;
	
            // test EtaMax
            if((*tob2)->eta() > ((int) parameter("EtaMax").value())) continue;

            // test deltaPhi
            //bool accept1 = deltaPhi((*tob1)->phi(), (*tob2)->phi(), parameter("DeltaPhiMin").value());
            //bool accept2 = deltaPhi((*tob1)->phi(), (*tob2)->phi(), parameter("DeltaPhiMax").value());
            //if( ! (accept1 && !accept2) ) continue; // accept1 = true, !accept2 = false

            // test deltaEta
            //bool accept3 = deltaEta((*tob1)->eta(), (*tob2)->eta(), parameter("DeltaEtaMin").value());
            //bool accept4 = deltaEta((*tob1)->eta(), (*tob2)->eta(), parameter("DeltaEtaMax").value());
            //if(! (accept3 && !accept4) ) continue;
	
            // test deltaR
            bool accept5 = deltaR2((*tob1)->phi(), (*tob2)->phi(), (*tob1)->eta(), (*tob2)->eta(), parameter("DeltaR2Min").value());
            bool accept6 = deltaR2((*tob1)->phi(), (*tob2)->phi(), (*tob1)->eta(), (*tob2)->eta(), parameter("DeltaR2Max").value());
	
            //cout << "Combining " << (*tob1)->Et() << " with " << (*tob2)->Et() << " cut " << accept << endl;
	
            if(accept5 && !accept6) {
               output[0]->push_back(TCS::CompositeTOB(*tob1, *tob2));
            }
         }
      }
    
      decison.setBit(0, output[0]->size()>0);  
    
   } else if( input.size() == 2) {
      for(TCS::TOBArray::const_iterator tob1 = input[0]->begin(); tob1 != input[0]->end(); ++tob1) {
      
         // test array length
         if(std::distance(input[0]->begin(), tob1) > parameter("NumberLeading1").value()-1) continue;
      
         // test EtMin
         if((*tob1)->Et() < ((int) parameter("EtMin").value())) continue;
      
         // test EtaMax
         if((*tob1)->eta() > ((int) parameter("EtaMax").value())) continue;
      
         TCS::TOBArray::const_iterator tob2 = input[1]->begin(); ++tob2;
         for(TCS::TOBArray::const_iterator tob2 = input[1]->begin(); tob2 != input[1]->end(); ++tob2) {
	
            // test array length
            if(std::distance(input[1]->begin(), tob2) > parameter("NumberLeading2").value()-1) continue;
	
            // test EtMin
            if((*tob2)->Et() < ((int) parameter("EtMin").value())) continue;
	
            // test EtaMax
            if((*tob2)->eta() > ((int) parameter("EtaMax").value())) continue;
	
            // test deltaR
            bool accept5 = deltaR2((*tob1)->phi(), (*tob2)->phi(), (*tob1)->eta(), (*tob2)->eta(), parameter("DeltaR2Min").value());
            bool accept6 = deltaR2((*tob1)->phi(), (*tob2)->phi(), (*tob1)->eta(), (*tob2)->eta(), parameter("DeltaR2Max").value());
	
            //cout << "Combining " << (*tob1)->Et() << " with " << (*tob2)->Et() << " cut " << accept << endl;

            if(accept5 && !accept6) {
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
