// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterAuxContainer_v2.h 628097 2014-11-13 09:24:54Z krasznaa $
#ifndef XAODCALOEVENT_VERSIONS_CALOCLUSTERAUXCONTAINER_V2_H
#define XAODCALOEVENT_VERSIONS_CALOCLUSTERAUXCONTAINER_V2_H

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
   class CaloClusterAuxContainer_v2 : public AuxContainerBase {

   public:
      /// Default constructor
      CaloClusterAuxContainer_v2();
      /// Destructor
      ~CaloClusterAuxContainer_v2() {}

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
     /// @}

   }; // class CaloClusterAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CaloClusterAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODCALOEVENT_VERSIONS_CALOCLUSTERAUXCONTAINER_V1_H
