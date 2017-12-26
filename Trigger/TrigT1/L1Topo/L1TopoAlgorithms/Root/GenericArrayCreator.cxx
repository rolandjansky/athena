//  GenericArrayCreator.cxx
//  L1TopoCoreSimulation
//  Created by Joerg Stelzer on 12/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoAlgorithms/GenericArrayCreator.h"
#include "L1TopoEvent/ClusterTOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/JetTOB.h"
#include "L1TopoEvent/GenericTOB.h"
#include "L1TopoCommon/Exception.h"

#include <typeinfo>

#include "boost/foreach.hpp"
#define foreach BOOST_FOREACH

REGISTER_ALG_TCS(GenericArrayCreator)


// constructor
TCS::GenericArrayCreator::GenericArrayCreator(const std::string & name) : SortingAlg(name)
   , m_jetsize(JetTOB::JS1)
{}


TCS::StatusCode
TCS::GenericArrayCreator::sort(const InputTOBArray & input, TOBArray & output) {

   // plain copy
   try {
      const ClusterTOBArray & in = dynamic_cast<const ClusterTOBArray &>(input);
      foreach(const ClusterTOB * cluster, in) {
         output.push_back( GenericTOB(*cluster) );
      }
   }
   catch(const std::bad_cast& bc) {
      try {
         const JetTOBArray & in = dynamic_cast<const JetTOBArray &>(input);
         foreach(const JetTOB * jet, in) {
            output.push_back( GenericTOB(*jet, m_jetsize) );
         }
         return StatusCode::SUCCESS;
      }
      catch(const std::bad_cast& bc) {
         TCS_EXCEPTION("GenericArrayCreator: neither ClusterArray nor JetArray")
      }
   }
   return StatusCode::SUCCESS;
}
