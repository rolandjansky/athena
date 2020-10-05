// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DynVarFixerAlg.h 803546 2017-04-25 09:44:26Z krasznaa $
#ifndef XAODCORECNV_DYNVARFIXERALG_H
#define XAODCORECNV_DYNVARFIXERALG_H

// System include(s):
#include <vector>
#include <string>
#include <typeinfo>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthContainersInterfaces/AuxTypes.h"

// Forward declaration(s):
class TClass;

namespace xAODMaker {

   /// Algorithm for fixing dynamic variable corruption on xAOD containers
   ///
   /// This is a general algorithm for fixing problems where a dynamic auxiliary
   /// variable of a container is not the right size. Which can happen when the
   /// algorithm creating the container doesn't consistently create dynamic
   /// variables for all objects. (Not creating some variables if some selection
   /// fails for instance.)
   ///
   /// This is a generalisation of algorithms like
   /// <code>xAODMaker::TrackLinkFixerAlg</code> and
   /// <code>xAODMaker::BTaggingAuxFixerAlg</code>.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 803546 $
   /// $Date: 2017-04-25 11:44:26 +0200 (Tue, 25 Apr 2017) $
   ///
   class DynVarFixerAlg : public AthAlgorithm {

   public:
      /// Standard Algorithm constructor
      DynVarFixerAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      StatusCode initialize() override;
      /// Function executing the algorithm
      StatusCode execute() override;

   private:
      /// Get the dictionary describing an auxiliary vector variable
      ::TClass* getClass( SG::auxid_t auxid );

      /// The names of the containers to massage
      std::vector< std::string > m_containers;
      /// Cache of the dictionaries used
      std::vector< ::TClass* > m_dicts;
      /// Flag showing which variables don't have a dictionary for them
      std::vector< bool > m_noDict;

   }; // class DynVarFixerAlg

} // namespace xAODMaker

#endif // XAODCORECNV_DYNVARFIXERALG_H
