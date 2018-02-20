/*********************************
 * MinDeltaPhiIncl2.cpp
 * Created by Veronica Sorin  on 14/8/14.
 *
 * @brief algorithm calculates the min phi-distance between  two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "TH1F.h"


#include "L1TopoAlgorithms/MinDeltaPhiIncl2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"

REGISTER_ALG_TCS(MinDeltaPhiIncl2)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::MinDeltaPhiIncl2:     "

TCS::MinDeltaPhiIncl2::MinDeltaPhiIncl2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0); 
   defineParameter("NumResultBits", 3);
   defineParameter("DeltaPhiMin",  0, 0);
   defineParameter("DeltaPhiMin",  0, 1);
   defineParameter("DeltaPhiMin",  0, 2);
   defineParameter("MinET1",1);
   defineParameter("MinET2",1);
   setNumberOutputBits(3);
}

TCS::MinDeltaPhiIncl2::~MinDeltaPhiIncl2(){}


TCS::StatusCode
TCS::MinDeltaPhiIncl2::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();

   for(unsigned int i=0; i< numberOutputBits(); ++i) {
      p_DeltaPhiMin[i] = parameter("DeltaPhiMin", i).value();
   }
   p_MinET1 = parameter("MinET1").value();
   p_MinET2 = parameter("MinET2").value();
   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   for(unsigned int i=0; i< numberOutputBits(); ++i) {
      TRG_MSG_INFO("DeltaPhiMin"<<i<<"   : " << p_DeltaPhiMin[i]);
   }
   TRG_MSG_INFO("MinET1         : " << p_MinET1);
   TRG_MSG_INFO("MinET2         : " << p_MinET2);
   TRG_MSG_INFO("nummber output : " << numberOutputBits());

   // create strings for histogram names
   std::vector<std::ostringstream> MyAcceptHist(numberOutputBits());
   std::vector<std::ostringstream> MyRejectHist(numberOutputBits());
   
   for (unsigned int i=0;i< numberOutputBits();i++) {
     MyAcceptHist[i] << "Accept" << p_DeltaPhiMin[i]  << "MinDPhi2"; 
     MyRejectHist[i] << "Reject" << p_DeltaPhiMin[i]  << "MinDPhi2";
   }

   for (unsigned int i=0; i<numberOutputBits();i++) {
     const std::string& MyTitle1 = MyAcceptHist[i].str();
     const std::string& MyTitle2 = MyRejectHist[i].str();
       
     registerHist(m_histAcceptMinDPhi2[i] = new TH1F(MyTitle1.c_str(),MyTitle1.c_str(),100,0,3.5));
     registerHist(m_histRejectMinDPhi2[i] = new TH1F(MyTitle2.c_str(),MyTitle2.c_str(),100,0,3.5));
   }

   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::MinDeltaPhiIncl2::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                            const std::vector<TCS::TOBArray *> & output,
                            Decision & decision )
{

   // mindphi 
   unsigned int mindphi = *min_element(begin(p_DeltaPhiMin),end(p_DeltaPhiMin));
   bool firstphi = true;

   // declare iterator for the tob with min dphi
   TCS::TOBArray::const_iterator tobmin1,tobmin2;  
      
   if (input.size() == 2) {
   
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {

            if( parType_t((*tob1)->Et()) <= p_MinET1) continue; // ET cut

            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {

               if( parType_t((*tob2)->Et()) <= p_MinET2) continue; // ET cut

               // test DeltaPhiMin, DeltaPhiMax
               unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhiBW( *tob1, *tob2 );

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

      for(unsigned int i=0; i<numberOutputBits(); ++i) {
          bool accept = mindphi > p_DeltaPhiMin[i] ;
          const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
          const bool fillReject = fillHistos() and not fillAccept;
          const bool alreadyFilled = decision.bit(i);
          if( accept ) {
              decision.setBit(i, true);
              output[i]->push_back(TCS::CompositeTOB(*tobmin1, *tobmin2));
          }
          if(fillAccept and not alreadyFilled){
              m_histAcceptMinDPhi2[i]->Fill((float)mindphi*0.10);
          } else if(fillReject){
              m_histRejectMinDPhi2[i]->Fill((float)mindphi*0.10);
          }
          TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail"));
      } // for(i)
   } else {
      TCS_EXCEPTION("MinDeltaPhiIncl2 alg must have 2 inputs, but got " << input.size());
   }

   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::MinDeltaPhiIncl2::process( const std::vector<TCS::TOBArray const *> & input,
                            const std::vector<TCS::TOBArray *> & output,
                            Decision & decision )
{

   // mindphi 
   unsigned int mindphi = *min_element(begin(p_DeltaPhiMin),end(p_DeltaPhiMin));
   bool firstphi = true;

   // declare iterator for the tob with min dphi
   TCS::TOBArray::const_iterator tobmin1,tobmin2;  
      
   if (input.size() == 2) {
   
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {

            if( parType_t((*tob1)->Et()) <= p_MinET1) continue; // ET cut

            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {

               if( parType_t((*tob2)->Et()) <= p_MinET2) continue; // ET cut

               // test DeltaPhiMin, DeltaPhiMax
               unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhi( *tob1, *tob2 );

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

      for(unsigned int i=0; i<numberOutputBits(); ++i) {
          bool accept = mindphi > p_DeltaPhiMin[i] ;
          const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
          const bool fillReject = fillHistos() and not fillAccept;
          const bool alreadyFilled = decision.bit(i);
          if( accept) {
              decision.setBit(i, true);
              output[i]->push_back(TCS::CompositeTOB(*tobmin1, *tobmin2));
          }
          if(fillAccept and not alreadyFilled){
              m_histAcceptMinDPhi2[i]->Fill((float)mindphi*0.10);
          } else if(fillReject) {
              m_histRejectMinDPhi2[i]->Fill((float)mindphi*0.10);
          }
          TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail"));
      }
   } else {
      TCS_EXCEPTION("MinDeltaPhiIncl2 alg must have 2 inputs, but got " << input.size());
   }

   return TCS::StatusCode::SUCCESS;
}
