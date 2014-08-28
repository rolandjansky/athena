/*********************************
 * JetHT.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Copyright (c) 2012 Joerg Stelzer. All rights reserved.
 *
 * @brief algorithm calculates the phi-distance between one or two lists and applies delta-phi criteria
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/JetHT.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

#include "L1TopoEvent/MetTOB.h"

REGISTER_ALG_TCS(JetHT)

using namespace std;

// not the best solution but we will move to athena where this comes for free
#define LOG cout << "TCS::JetHT:     "


TCS::JetHT::JetHT(const std::string & name) : DecisionAlg(name)
{
   defineParameter("MinET",0);
   setNumberOutputBits(1);
}

TCS::JetHT::~JetHT(){}


TCS::StatusCode
TCS::JetHT::initialize() {
   p_MinET = parameter("MinET").value();
   LOG << "MinET          : " << p_MinET << endl;
   LOG << "nummber output : " << numberOutputBits() << endl;
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::JetHT::process( const std::vector<TCS::TOBArray const *> & input,
                      const std::vector<TCS::TOBArray *> & output,
                      Decision & decision )
{

   if(input.size()!=1) {
      TCS_EXCEPTION("JetHT alg must have exactly 1 input list, but got " << input.size());
      return TCS::StatusCode::FAILURE;
   }

   bool accept{false};

   for( TCS::TOBArray::const_iterator tob = input[0]->begin(); tob != input[0]->end(); ++tob) {

      if( parType_t((*tob)->Et()) < p_MinET) continue; // ET cut

      accept = true;

      output[0]->push_back(TCS::CompositeTOB(*tob));

      LOG << "      Jet " << distance(input[0]->begin(), tob) << " ET = " << (*tob)->Et() << endl;
   }

   decision.setBit( 0, accept );

   return TCS::StatusCode::SUCCESS;
}
