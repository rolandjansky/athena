/*********************************
 * JetSimpleCone.cpp
 *
 * @brief algorithm calculates the sum of jets ET around each jet within radius R, makes cut on leading sum ET
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

#include "L1TopoAlgorithms/JetSimpleCone.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"


REGISTER_ALG_TCS(JetSimpleCone)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << name() << ":     "


TCS::JetSimpleCone::JetSimpleCone(const std::string & name) : DecisionAlg(name)
{  
   defineParameter("InputWidth", 0);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits",6);
   defineParameter("NumRegisters", 2); 
   defineParameter("Radius",10);
   defineParameter("MinET",0);
   defineParameter("MinEta",0);
   defineParameter("MaxEta",31);
   defineParameter("MinHt",0,0);
   defineParameter("MinHt",0,1);
   defineParameter("MinHt",0,2);
   defineParameter("MinHt",0,3);
   defineParameter("MinHt",0,4);
   defineParameter("MinHt",0,5);
   setNumberOutputBits(6);
}

TCS::JetSimpleCone::~JetSimpleCone()
{}


TCS::StatusCode
TCS::JetSimpleCone::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   if(parameter("MaxTob").value() > 0) p_NumberLeading1 = parameter("MaxTob").value();
   p_R = parameter("Radius").value();
   p_MinET  = parameter("MinET").value();
   p_EtaMin = parameter("MinEta").value();
   p_EtaMax = parameter("MaxEta").value();

   TRG_MSG_INFO("MaxTob          : " << p_NumberLeading1);
   TRG_MSG_INFO("Radius         : " << p_R);
   TRG_MSG_INFO("MinET          : " << p_MinET);
   TRG_MSG_INFO("EtaMin         : " << p_EtaMin);
   TRG_MSG_INFO("EtaMax         : " << p_EtaMax);
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_SimpleCone[i] = parameter("MinHt", i).value();
      TRG_MSG_INFO("SimpleCone " << i << " : " << p_SimpleCone[i]);
   }
   TRG_MSG_INFO("number output : " << numberOutputBits());

   // create strings for histogram names
   vector<ostringstream> MyAcceptHist(numberOutputBits());
   vector<ostringstream> MyRejectHist(numberOutputBits());
   
   for (unsigned int i=0;i<numberOutputBits();i++) {
     MyAcceptHist[i] << "Accept" << p_SimpleCone[i] << "SimpleCone"; 
     MyRejectHist[i] << "Reject" << p_SimpleCone[i] << "SimpleCone";
   }

   for (unsigned int i=0; i<numberOutputBits();i++) {

     const std::string& MyTitle1 = MyAcceptHist[i].str();
     const std::string& MyTitle2 = MyRejectHist[i].str();
     
     registerHist(m_histAcceptSimpleCone[i] = new TH1F(MyTitle1.c_str(),MyTitle1.c_str(),100,0,p_SimpleCone[i]*2));
     registerHist(m_histRejectSimpleCone[i] = new TH1F(MyTitle2.c_str(),MyTitle2.c_str(),100,0,p_SimpleCone[i]*2));
   }


   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::JetSimpleCone::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
	return process(input,output,decision);
}


TCS::StatusCode
TCS::JetSimpleCone::process( const std::vector<TCS::TOBArray const *> & input,
			     const std::vector<TCS::TOBArray *> & output,
			     Decision & decision )
{

  if(input.size()!=1) {
    TCS_EXCEPTION("JetSimpleCone alg must have exactly 1 input list, but got " << input.size());
  }

  unsigned int leadingET = 0;

  // loop over  jets
  for( TOBArray::const_iterator tob = input[0]->begin(); 
       tob != input[0]->end() && distance(input[0]->begin(), tob) < p_NumberLeading1;
       ++tob) {

    if( parType_t(fabs((*tob)->eta())) > p_EtaMax ) continue; // Eta cut
    if( parType_t(fabs((*tob)->eta())) < p_EtaMin ) continue; // Eta cut
    if( parType_t((*tob)->Et()) <= p_MinET ) continue; // E_T cut

    TRG_MSG_DEBUG("Jet : ET = " << (*tob)->Et());      
      
    unsigned int tmp_SumET = (*tob)->Et();
      
    for( TOBArray::const_iterator tob1 = input[0]->begin();
	 tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
	 ++tob1) {
	  
      if( tob1 == tob ) continue; // Avoid double counting of central jet 
      if( parType_t(fabs((*tob1)->eta())) > p_EtaMax ) continue; // Eta cut                                                                                           
      if( parType_t(fabs((*tob1)->eta())) < p_EtaMin ) continue; // Eta cut                                                                                              
      if( parType_t((*tob1)->Et()) <= p_MinET ) continue; // E_T cut    
      
      double deta = ( (*tob)->etaDouble() - (*tob1)->etaDouble() );
      double dphi = fabs( (*tob)->phiDouble() - (*tob1)->phiDouble() );
      if(dphi>M_PI)
	dphi = 2*M_PI - dphi;
      
      if ( 100 * ((dphi)*(dphi) + (deta)*(deta) ) > p_R*p_R) continue; // Exclude jets outside cone
      
      tmp_SumET += (*tob1)->Et();
    }

    if (tmp_SumET > leadingET) leadingET = tmp_SumET; 
  }

  for(unsigned int i=0; i<numberOutputBits(); ++i) {

    bool accept = leadingET > p_SimpleCone[i];
    
    decision.setBit( i, accept );
    
    if(accept) {
      output[i]->push_back( CompositeTOB( GenericTOB::createOnHeap( GenericTOB(leadingET,0,0) ) ));
      m_histAcceptSimpleCone[i]->Fill(leadingET);
    }
    else
      m_histRejectSimpleCone[i]->Fill(leadingET);
    
    
    TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " SimpleCone = " << leadingET);
    
   }
  
   return TCS::StatusCode::SUCCESS;
}
