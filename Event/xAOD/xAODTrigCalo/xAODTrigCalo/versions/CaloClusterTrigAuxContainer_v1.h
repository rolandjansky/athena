// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterAuxContainer_v2.h 628097 2014-11-13 09:24:54Z krasznaa $
#ifndef XAODTRIGCALO_VERSIONS_CALOCLUSTERTRIGAUXCONTAINER_V1_H
#define XAODTRIGCALO_VERSIONS_CALOCLUSTERTRIGAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary container for calorimeter cluster containers
   ///
   /// This is a fixed implementation of the calorimeter cluster
   /// auxiliary store. Defining every possible property that
   /// calorimeter clusters can have.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revsision$
   /// $Date: 2014-11-13 10:24:54 +0100 (Thu, 13 Nov 2014) $
   ///
   class CaloClusterTrigAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      CaloClusterTrigAuxContainer_v1();
      /// Destructor
      ~CaloClusterTrigAuxContainer_v1() {}

   private:
     /// @name Basic variables
     ///@ {
     std::vector<float> time;
     std::vector<unsigned> clusterSize;
     std::vector<float> eta0;
     std::vector<float> phi0;
     std::vector<float> rawE;
     std::vector<float> rawEta;
     std::vector<float> rawPhi;
     std::vector<float> rawM;
     std::vector<float> altE;
     std::vector<float> altEta;
     std::vector<float> altPhi;
     std::vector<float> altM;

     std::vector<float> calE;
     std::vector<float> calEta;	
     std::vector<float> calPhi;	
     std::vector<float> calM; 
     ///@}

     /// @name Energies, Eta and Phi in various samplings
     /// @{
     std::vector <std::vector< float > > e_sampl;
     std::vector <std::vector< float > > eta_sampl;
     std::vector <std::vector< float > > phi_sampl;
     /// @}

     std::vector< float > ETACALOFRAME;
     std::vector< float > ETA1CALOFRAME;
     std::vector< float > ETA2CALOFRAME;

     std::vector< float > PHICALOFRAME;
     std::vector< float > PHI1CALOFRAME;
     std::vector< float > PHI2CALOFRAME;

     std::vector< float > CENTER_LAMBDA;
     std::vector< float > FIRST_ENG_DENS;
     std::vector< float > EM_PROBABILITY;
     std::vector< float > SECOND_LAMBDA;
     std::vector< float > SECOND_R;

   }; // class CaloClusterTrigAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CaloClusterTrigAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGCALO_VERSIONS_CALOCLUSTERTRIGAUXCONTAINER_V1_H
