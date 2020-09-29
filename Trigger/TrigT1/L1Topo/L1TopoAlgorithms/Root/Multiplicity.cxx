/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * Multiplicity.cpp
 * Created by Joerg Stelzer on 11/16/12.
 * Modified by V SOrin 2014
 *
 * @brief algorithm that check muon multiplicity and return 2 output lines, which will give information on how many muons were found
 * line 1: 0 or 1, line 1 and 2 : 2 or more, uses 2 bits
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/Multiplicity.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"

REGISTER_ALG_TCS(Multiplicity)


TCS::Multiplicity::Multiplicity(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("NumResultBits", 2);
   defineParameter("MinET",1);
   setNumberOutputBits(2);
}

TCS::Multiplicity::~Multiplicity(){}


TCS::StatusCode
TCS::Multiplicity::initialize() {
   p_NumberLeading1 = parameter("InputWidth").value();
   p_MinET = parameter("MinET").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);  // note that the reading of generic parameters doesn't work yet
   TRG_MSG_INFO("MinET          : " << p_MinET);
   TRG_MSG_INFO("number output : " << numberOutputBits());
   
   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::Multiplicity::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
	return process(input,output,decision);
}



TCS::StatusCode
TCS::Multiplicity::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

   if(input.size() == 1) {
      
     // loop over  muons
     unsigned int nLeading = p_NumberLeading1;

     // counter
     int nmuon = 0;
     // vector of tobs passing cuts
     std::vector<TCS::GenericTOB*> TOBvector;

     for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < nLeading;
           ++tob1) 
         {

          if( parType_t((*tob1)->Et())  <= p_MinET ) continue; // E_T cut
     //
          TRG_MSG_DEBUG("Muon : ET = " << parType_t((*tob1)->Et()));
          ++nmuon;
          // keep a list of all TOB that contributed
          TOBvector.push_back( *tob1 );
          
     }
     //
      


     if (nmuon == 0 or nmuon == 2) decision.setBit( 0, false );
     if (nmuon == 1 or nmuon > 2) decision.setBit( 0, true );
     if (nmuon < 2 ) decision.setBit( 1, false );
     if (nmuon > 1 ) decision.setBit( 1, true );

     if (nmuon == 1) output[0]->push_back(TCS::CompositeTOB(TOBvector));
     else if (nmuon > 1 )  output[1]->push_back(TCS::CompositeTOB(TOBvector));

   } else {

      TCS_EXCEPTION("Multiplicity alg must have  1 input, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;
}
