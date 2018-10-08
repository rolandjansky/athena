// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODCORECNV_DYNVARTODECORATIONALG_H
#define XAODCORECNV_DYNVARTODECORATIONALG_H

// System include(s):
#include <string>
#include <vector>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// EDM include(s):
#include "AthContainersInterfaces/AuxTypes.h"

// Forward declaration(s):
namespace SG {
   class IAuxStore;
}

namespace xAODMaker {

   /// Algorithm for turning auxiliary variables into decorations
   ///
   /// This algorithm was written to handle ATLASRECTS-4499. Since I didn't find
   /// a good way of making @c pool::AuxStoreAPR forget about some variables
   /// entirely, the easiest seemed to be to just make it treat them as a
   /// decoration.
   ///
   /// This allows variables, that were mistakenly written into the primary
   /// AOD, to be overwritten during the derivation.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class DynVarToDecorationAlg : public AthAlgorithm {

   public:
      /// Algorithm constructor
      DynVarToDecorationAlg( const std::string& name, ISvcLocator* svcLoc );

      /// @name Function(s) implementing the @c IAlgorithm interface
      /// @{

      /// Function initialising the algorithm
      StatusCode initialize() override;
      /// Function executing the algorithm
      StatusCode execute() override;

      /// @}

   private:
      /// Function making the chosen variables dynamic
      StatusCode updateVars( SG::IAuxStore* store );

      /// @name Algorithm properties
      /// @{

      /// The name of the container to remove auxiliary variables from
      std::string m_containerName;
      /// The list of auxiliary variables to remove from the container
      std::vector< std::string > m_auxVariableNames;

      /// @}

      /// Flag showing whether the auxiliary ID set is initialised already
      bool m_auxIDsInitialized = false;
      /// Auxiliary IDs for the variables to be removed from the container
      SG::auxid_set_t m_auxIDs;

   }; // class DynVarToDecorationAlg

} // namespace xAODMaker

#endif // XAODCORECNV_DYNVARTODECORATIONALG_H
