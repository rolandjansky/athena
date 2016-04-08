// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxStoreWrapper.h 583869 2014-02-18 11:31:09Z krasznaa $
#ifndef XAODCORECNV_AUXSTOREWRAPPER_H
#define XAODCORECNV_AUXSTOREWRAPPER_H

// System include(s):
#include <vector>
#include <set>
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// Forward declaration(s):
namespace SG {
   class IAuxStore;
   class DataProxy;
}

namespace xAODMaker {

   /**
    *  @short Algorithm preparing auxiliary store objects for slimming
    *
    *         This algorithm can be used to update some or all of the auxiliary
    *         store objects in StoreGate to save all of their auxiliary
    *         variables as separate branches in the output TTree.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 583869 $
    * $Date: 2014-02-18 12:31:09 +0100 (Tue, 18 Feb 2014) $
    */
   class AuxStoreWrapper : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      AuxStoreWrapper( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// Create a new container store
      StatusCode changeContainer( SG::IAuxStore* store,
                                  const SG::DataProxy* proxy  );
      /// Create a new element store
      StatusCode changeElement( SG::IAuxStore* store,
                                const SG::DataProxy* proxy );

      /// StoreGate keys of the store objects to be wrapped
      std::vector< std::string > m_keys;
      /// Keys of the objects to be wrapped, in a set container
      std::set< std::string > m_keysSet;

   }; // class AuxStoreWrapper

} // namespace xAODMaker

#endif // XAODCORECNV_AUXSTOREWRAPPER_H
