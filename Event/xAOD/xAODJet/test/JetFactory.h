// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODJET_TEST_JETFACTORY_H
#define XAODJET_TEST_JETFACTORY_H

// Package include(s).
#include "xAODJet/JetContainer.h"

// Framework include(s).
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CxxUtils/checker_macros.h"

// System include(s).
#include <memory>

namespace xAOD {
   namespace JetTests {

      /// Create and return an empty JetContainer
      ///
      /// JetContainer has an AuxContainer associated and is recorded in an
      /// event store so its elements can be used as ElementLink.
      ///
      /// The caller does not get ownership of the pointer!
      ///
      xAOD::JetContainer&
      createEmptyJetContainer ATLAS_NOT_THREAD_SAFE( const std::string& name );

      /// Fill input JetContainer with new jets which 4-momentum are given by
      /// jet4moms
      void fillJets( xAOD::JetContainer& jetCont,
                     const std::vector< xAOD::JetFourMom_t >& jet4moms );

      /// Fill input JetContainer with new jets which 4-momentum are given by
      /// jet4moms and with attributes as given by the map "jetAttMap".
      ///
      /// It is expected that vectors in this map have same lenght as jet4moms.
      ///
      void fillJets( xAOD::JetContainer& jetCont,
                     const std::vector< xAOD::JetFourMom_t >& jet4moms,
                     const std::map< std::string, std::vector< float > >& jetAttMapF,
                     const std::map< std::string, std::vector< int > >& jetAttMapI,
                     const std::map< std::string, std::vector< xAOD::JetFourMom_t > >& jetAttMap4M );

      /// Fill input JetContainer with a list of test jets
      void fillStandardTestJets( JetContainer& jetCont );

      /// Create and return an empty CaloClusterAuxContainer.
      ///
      /// CaloClusterAuxContainer has an AuxContainer associated and is
      /// recorded in an event store so its elements can be used as
      /// ElementLinks.
      ///
      xAOD::CaloClusterContainer&
      createEmptyCaloClusterContainer ATLAS_NOT_THREAD_SAFE( const std::string & name );

      /// Fill input CaloClusterAuxContainer with a list of test clusters
      /// (with only their default and Raw 4-momentum set)
      void fillStandardTestClusters( xAOD::CaloClusterContainer& clustCont );

      /// Shut down the "testing infrastructure"
      ///
      /// Unfortunately some of the infrastructure that's created behind the
      /// scenes by these functions can not correctly "disassemble" itself
      /// at the end of an application. Some objects need to be deleted before
      /// some of the ROOT infrastructure would become unavailable.
      ///
      /// Call this function before the very end of your test application!
      ///
      void shutDown ATLAS_NOT_THREAD_SAFE();

   } // namespace JetTests
} // namespace xAOD

#endif // not XAODJET_TEST_JETFACTORY_H
