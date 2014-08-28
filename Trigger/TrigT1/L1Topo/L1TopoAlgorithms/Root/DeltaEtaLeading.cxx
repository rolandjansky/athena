//  DeltaEtaLeading.cpp
//  TopoCore

#include "L1TopoAlgorithms/DeltaEtaLeading.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(DeltaEtaLeading)

using namespace std;

namespace TCS {
  bool deltaEta(int eta1, int eta2, int cut) {
    return abs(eta1-eta2) > cut;
  }
}

TCS::DeltaEtaLeading::DeltaEtaLeading(const std::string & name) : DecisionAlg(name)
{
  defineParameter("MaxTob", 1); // array index 0;
  defineParameter("DeltaEtaMin", 0);
  defineParameter("DeltaEtaMax", 20);
  defineParameter("EtaMax", 10);
  defineParameter("MinET",1);
  setNumberOutputBits(1);
}


TCS::StatusCode
TCS::DeltaEtaLeading::initialize() {
   // TODO: implement parameters
   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::DeltaEtaLeading::process( const std::vector<TCS::TOBArray const *> & input,
                               const std::vector<TCS::TOBArray *> & output,
                               Decision & decison ) {

   cout << "input size: " << input.size() << endl;
  
   if(input.size() == 1) {
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
	
            bool accept1 = deltaEta((*tob1)->eta(), (*tob2)->eta(), parameter("DeltaEtaMin").value());
            //cout << "Combining " << (*tob1)->eta() << " with " << (*tob2)->eta() << " cut " << accept1 << endl;
            bool accept2 = deltaEta((*tob1)->eta(), (*tob2)->eta(), parameter("DeltaEtaMax").value());
            //cout << "Combining " << (*tob1)->eta() << " with " << (*tob2)->eta() << " cut " << accept2 << endl;
	
            // accept if tob1,2 greater than DeltaEtaMin, tob1,2 less than DeltaEtaMax
            if(accept1 && !accept2) {
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
	
            bool accept1 = deltaEta((*tob1)->eta(), (*tob2)->eta(), parameter("DeltaEtaMin").value());
            //cout << "Combining " << (*tob1)->eta() << " with " << (*tob2)->eta() << " cut " << accept1 << endl;
            bool accept2 = deltaEta((*tob1)->eta(), (*tob2)->eta(), parameter("DeltaEtaMax").value());
            //cout << "Combining " << (*tob1)->eta() << " with " << (*tob2)->eta() << " cut " << accept2 << endl;
	
            if(accept1 && !accept2) {
               output[0]->push_back(TCS::CompositeTOB(*tob1, *tob2));
            }
         }
      }

      decison.setBit(0, output[0]->size()>0);

   } else {

      TCS_EXCEPTION("DeltaEtaLeading alg must have either 1 or 2 inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
