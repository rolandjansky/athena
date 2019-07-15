// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKEXAMPLE_FLOATCOMPRESSIONALG_H
#define DERIVATIONFRAMEWORKEXAMPLE_FLOATCOMPRESSIONALG_H

// System include(s):
#include <vector>
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// EDM include(s).
#include "xAODCore/tools/FloatCompressor.h"

// Forward declaration(s):
namespace SG {
   class IConstAuxStore;
}

namespace DerivationFramework {

   /**
    *  @short Algorithm for float compression of AUX containers or single AUX variables
    *
    *         This algorithm is based on ElementLink resetting algorithm
    *         in Event/xAOD/xAODCoreCnv/src/ElementLinkResetAlg.h
    *         It allows a configurable compression of float and vector<float> in StoreGate
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    * @author Kerim Suruliz <Kerim.Suruliz@cern.ch>
    * @author Johannes Elmsheuser <Kerim.Suruliz@cern.ch>
    */
   class FloatCompressionAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      FloatCompressionAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// Function reseting all the ElementLinks in one specific container
      StatusCode reset( const SG::IConstAuxStore& store,
                        const std::string& key );
      StatusCode resetVars( const SG::IConstAuxStore& store,
			    const std::map<std::string, std::vector<std::pair<std::string,int>>> key );

      /// StoreGate keys of the auxiliary objects to be processed
      std::vector< std::string > m_keys;

      /// StoreGate keys of the auxiliary objects to be processed
      std::vector< std::string > m_vars;

      /// Helper class for caching auxiliary ID types
      class AuxIDType {
      public:
         AuxIDType();
         bool isSet; ///< Flag for whether this type was already set up
  	 bool isFloat; ///< True if the type is an ElementLink
	 bool isFloatVec; ///< True of the type is an ElementLink vector
      };

      /// Cached types of the auxiliary IDs
      std::vector< AuxIDType > m_typeCache;
      /// Float compressors used by the algorithm
      std::vector< xAOD::FloatCompressor > m_floatCompressors;

   }; // class FloatCompressionAlg

} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKEXAMPLE_FLOATCOMPRESSIONALG_H
