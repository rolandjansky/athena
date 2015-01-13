/*********************************
 * MinDPhiIncl1.cpp
 * Created by Veronica Sorin  on 14/8/14.
 *
 * @brief algorithm calculates the min phi-distance between one or two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include <algorithm>


#include "L1TopoAlgorithms/MinDPhiIncl1.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(MinDPhiIncl1)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::MinDPhiIncl1:     "




namespace {
   unsigned int
   calcDeltaPhi(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
      double dphi = fabs( tob1->phiDouble() - tob2->phiDouble() );
      if(dphi>M_PI)
         dphi = 2*M_PI - dphi;
      
      return round( 10 * dphi );
   }
}


TCS::MinDPhiIncl1::MinDPhiIncl1(const std::string & name) : DecisionAlg(name)
{
   defineParameter("NumberLeading1", 3);
   defineParameter("NumberLeading2", 3); 
   defineParameter("NumResultBits", 3);
   defineParameter("DeltaPhiMin",  0, 0);
   defineParameter("DeltaPhiMin",  0, 1);
   defineParameter("DeltaPhiMin",  0, 2);
   defineParameter("MinET1",1);
   defineParameter("MinET2",1);
   setNumberOutputBits(3);
}

TCS::MinDPhiIncl1::~MinDPhiIncl1(){}

TCS::StatusCode
TCS::MinDPhiIncl1::initialize() {
   p_NumberLeading1 = parameter("NumberLeading1").value();
   p_NumberLeading2 = parameter("NumberLeading2").value();
   for(int i=0; i<3; ++i) {
      p_DeltaPhiMin[i] = parameter("DeltaPhiMin", i).value();
   }
   p_MinET1 = parameter("MinET1").value();
   p_MinET2 = parameter("MinET2").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   for(int i=0; i<3; ++i) {
    TRG_MSG_INFO("DeltaPhiMin   : " << p_DeltaPhiMin[i]);
   }
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);

   TRG_MSG_INFO("number output : " << numberOutputBits());
 
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::MinDPhiIncl1::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decison )
{

   // mindphi 
   unsigned int mindphi = *min_element(begin(p_DeltaPhiMin),end(p_DeltaPhiMin));
   bool firstphi = true;


   // declare iterator for the tob with min dphi
   TCS::TOBArray::const_iterator tobmin1,tobmin2;  
	 
   if(input.size() == 1) {



      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
         {
            
            if( parType_t((*tob1)->Et()) <= p_MinET1 ) continue; // ET cut
            
            TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
            for( ;
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {

               if( parType_t((*tob2)->Et()) <= p_MinET2) continue; // ET cut

               // DeltaPhi cuts
               unsigned int deltaPhi = calcDeltaPhi( *tob1, *tob2 );

	       if (firstphi) {
		   mindphi = deltaPhi;
		   tobmin1=tob1;
                   tobmin2=tob2;
		   firstphi = false;
	       }

	       if ( deltaPhi < mindphi ) {
		  
		  mindphi = deltaPhi;
		  tobmin1=tob1;
		  tobmin2=tob2;

	       }


            }
         }

      bool accept[3];
      for(unsigned int i=0; i<numberOutputBits(); ++i) {
          accept[i] = mindphi > p_DeltaPhiMin[i] ;
          if( accept[i] ) {
             decison.setBit(i, true);
             output[i]->push_back(TCS::CompositeTOB(*tobmin1, *tobmin2));
          }
          TRG_MSG_DEBUG("Decision " << i << ": " << (accept[i]?"pass":"fail") << " mindphi = " << mindphi);

      }
      

   } else {

      TCS_EXCEPTION("MinDPhiIncl1 alg must have  1 inputs, but got " << input.size());

   }

   return TCS::StatusCode::SUCCESS;
}
