//  GenericListGenerator.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoAlgorithms/GenericListGenerator.h"
#include "L1TopoEvent/ClusterTOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(GenericListGenerator)

// constructor
TCS::GenericListGenerator::GenericListGenerator(const std::string & name) : SortingAlg(name)
{}


// destructor
TCS::GenericListGenerator::~GenericListGenerator()
{}

TCS::StatusCode
TCS::GenericListGenerator::sort(const InputTOBArray & input, TOBArray & output) {
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
         output.push_back( GenericTOB(**jet, JetTOB::JS1) );
      }
      return TCS::StatusCode::SUCCESS;
   }
   catch(const std::bad_cast &) {}

   return TCS::StatusCode::SUCCESS;
}

