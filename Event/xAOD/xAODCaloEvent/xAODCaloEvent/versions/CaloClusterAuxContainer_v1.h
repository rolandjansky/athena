// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterAuxContainer_v1.h 617648 2014-09-19 12:05:41Z wlampl $
#ifndef XAODCALOEVENT_VERSIONS_CALOCLUSTERAUXCONTAINER_V1_H
#define XAODCALOEVENT_VERSIONS_CALOCLUSTERAUXCONTAINER_V1_H

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
   /// $Date: 2014-09-19 14:05:41 +0200 (Fri, 19 Sep 2014) $
   ///
   class CaloClusterAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      CaloClusterAuxContainer_v1();
      /// Destructor
      ~CaloClusterAuxContainer_v1() {}

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
     //std::vector <std::vector< float > > eta_sampl;
     //std::vector <std::vector< float > > phi_sampl;
      /// @}

   }; // class CaloClusterAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloClusterAuxContainer_v1, 1302486236, 1 )
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CaloClusterAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODCALOEVENT_VERSIONS_CALOCLUSTERAUXCONTAINER_V1_H
