/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  GenericListGenerator.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Modified by V Sorin 2014
//  algorithm to generate the all lists from TOBs

#include "L1TopoAlgorithms/GenericListGenerator.h"
#include "L1TopoEvent/ClusterTOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>
#include <typeinfo>

REGISTER_ALG_TCS(GenericListGenerator)

// constructor
TCS::GenericListGenerator::GenericListGenerator(const std::string & name)
: SortingAlg(name),
  m_jetsize(JetTOB::JS1)
{
   defineParameter( "JetSize", 0 );
}


// destructor
TCS::GenericListGenerator::~GenericListGenerator()
{}

TCS::StatusCode
TCS::GenericListGenerator::sort(const InputTOBArray & input, TOBArray & output) {
   // because fw seems to have differnt notation, 2 means for now JS1 8x8
   m_jetsize = parameter("JetSize").value()==2?JetTOB::JS1:JetTOB::JS2; 
 

   try {
      const ClusterTOBArray & clusters = dynamic_cast<const ClusterTOBArray&>(input);
      for(ClusterTOBArray::const_iterator cl = clusters.begin(); cl!= clusters.end(); ++cl ) {
         output.push_back( GenericTOB(**cl) );
      }
      return TCS::StatusCode::SUCCESS;
   }
   catch(const std::bad_cast &) {}

   try {
      const JetTOBArray & jets = dynamic_cast<const JetTOBArray&>(input);
      for(JetTOBArray::const_iterator jet = jets.begin(); jet!= jets.end(); ++jet ) {
         output.push_back( GenericTOB(**jet, m_jetsize) );
      }
      return TCS::StatusCode::SUCCESS;
   }
   catch(const std::bad_cast &) {}

   return TCS::StatusCode::SUCCESS;
}

