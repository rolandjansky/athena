//  SumEtLeading.cpp
//  TopoCore

#include "L1TopoAlgorithms/SumEtLeading.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(SumEtLeading)

using namespace std;

TCS::SumEtLeading::SumEtLeading(const std::string & name) :
   DecisionAlg(name)
{
   defineParameter("NumberLeading",1); // array index 0;
   defineParameter("EtMin",3);
   defineParameter("HtMin",20);
   defineParameter("EtaMax",6);
   setNumberOutputBits(1);
}


TCS::StatusCode
TCS::SumEtLeading::initialize() {
   p_NumberLeading = parameter("NumberLeading").value();
   p_EtMin = parameter("EtMin").value();
   p_HtMin = parameter("HtMin").value();
   p_EtaMax = parameter("EtaMax").value();
   return StatusCode::SUCCESS;
}



// input is a vector of tob arrays, which means each element of input is an array of tob obejects
TCS::StatusCode
TCS::SumEtLeading::process( const std::vector<TCS::TOBArray const *> & input,
                            const std::vector<TCS::TOBArray *> & output,
                            Decision & decison )
{

   double sumET = 0;

   // declare tobvector to be filled with tobs (from the currently tobarray) that pass cuts
   vector<TCS::GenericTOB*> TOBvector;

   // declare iterators
   TCS::TOBArray::const_iterator tob;
  
   // run over each TOBArray in input
   for( TOBArray::const_iterator tob = input[0]->begin();
        tob != input[0]->end() && distance( input[0]->begin(), tob) < p_NumberLeading;
        ++tob ) {

      if( (uint32_t)(*tob)->Et() < p_EtMin ) continue; // test ETMin
      
      if( abs((*tob)->eta()) > p_EtaMax ) continue; // test EtaMax
      
      // sum up Et of each TOB in TOBArray (event?)
      sumET += (*tob)->Et();

      // keep a list of all TOB that contributed
      TOBvector.push_back( *tob );
   }
  
   cout << "sumET: " << sumET << endl;
   // compare sumET to Htmin and fill output
   if ( sumET > p_HtMin ) {
      output[0]->push_back(TCS::CompositeTOB(TOBvector));
   }

   decison.setBit(0, output.size()>0);  
  
   return TCS::StatusCode::SUCCESS;
}
