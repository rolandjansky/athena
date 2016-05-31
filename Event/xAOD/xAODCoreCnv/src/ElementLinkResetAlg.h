// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElementLinkResetAlg.h 751107 2016-05-31 11:23:23Z krasznaa $
#ifndef XAODCORECNV_ELEMENTLINKRESETALG_H
#define XAODCORECNV_ELEMENTLINKRESETALG_H

// System include(s):
#include <vector>
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// Forward declaration(s):
namespace SG {
   class IConstAuxStore;
}

namespace xAODMaker {

   /**
    *  @short Algorithm reseting ElementLink objects to their default state
    *
    *         This algorithm can be used to make sure that during/after an
    *         AODFix-like configuration EDM objects are not left with cached
    *         pointers to no longer existing objects.
    *
    *         The algorithm can make sure that every ElementLink in some
    *         selected container(s), or all containers that are in StoreGate,
    *         are cleared of all cached information.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 751107 $
    * $Date: 2016-05-31 13:23:23 +0200 (Tue, 31 May 2016) $
    */
   class ElementLinkResetAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      ElementLinkResetAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// Function reseting all the ElementLinks in one specific container
      StatusCode reset( const SG::IConstAuxStore& store );

      /// StoreGate keys of the auxiliary objects to be processed
      std::vector< std::string > m_keys;

      /// Helper class for caching auxiliary ID types
      class AuxIDType {
      public:
         AuxIDType();
         bool isSet; ///< Flag for whether this type was already set up
         bool isEL; ///< True if the type is an ElementLink
         bool isELVec; ///< True of the type is an ElementLink vector
      };

      /// Cached types of the auxiliary IDs
      std::vector< AuxIDType > m_typeCache;

   }; // class ElementLinkResetAlg

} // namespace xAODMaker

#endif // XAODCORECNV_ELEMENTLINKRESETALG_H
