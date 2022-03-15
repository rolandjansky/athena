/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <iostream>
#include <iterator>

// Local include(s):
#include "xAODTracking/versions/NeutralParticleAuxContainer_v1.h"
 
namespace xAOD {
 
   NeutralParticleAuxContainer_v1::NeutralParticleAuxContainer_v1()
      : AuxContainerBase() {
 
      AUX_VARIABLE( d0 );
      AUX_VARIABLE( z0 );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( theta );
      AUX_VARIABLE( oneOverP );

      AUX_VARIABLE( definingParametersCovMatrix );

      AUX_VARIABLE( vx );
      AUX_VARIABLE( vy );
      AUX_VARIABLE( vz );      
   
   }
   
   void NeutralParticleAuxContainer_v1::dump() const {
     std::cout<<" Dumping NeutralParticleAuxContainer_v1"<<std::endl;
     std::cout<<"d0:";
     std::copy(d0.begin(), d0.end(),
       std::ostream_iterator<float>(std::cout, ", "));
     std::cout<<"z0:";
     std::copy(z0.begin(), z0.end(),
       std::ostream_iterator<float>(std::cout, ", "));
     std::cout<<"phi0:";
     std::copy(phi.begin(), phi.end(),
       std::ostream_iterator<float>(std::cout, ", "));
     std::cout<<"theta0:";
     std::copy(theta.begin(), theta.end(),
       std::ostream_iterator<float>(std::cout, ", "));
     std::cout<<"oneOverP:";
     std::copy(oneOverP.begin(), oneOverP.end(),
       std::ostream_iterator<float>(std::cout, ", "));
     std::cout<<"definingParametersCovMatrix: ";
     for (unsigned int i=0; i<definingParametersCovMatrix.size();++i){
     std::copy(definingParametersCovMatrix[i].begin(), definingParametersCovMatrix[i].end(),
       std::ostream_iterator<float>(std::cout, ", "));
        std::cout<<std::endl;
     }
   }

} // namespace xAOD
