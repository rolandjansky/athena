// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigComposite_v1.h 761887 2016-07-14 13:16:16Z tbold $
#ifndef XAODTRIGGER_VERSIONS_TRIGCOMPOSITE_V1_H
#define XAODTRIGGER_VERSIONS_TRIGCOMPOSITE_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <string>
#include <vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

namespace xAOD {

   /// Class used to describe composite objects in the HLT
   ///
   /// This is a generic class for describing the output of high-level
   /// HLT algorithms that combine lower-level objects into one.
   ///
   /// @author Tomasz Bold <Tomasz.Bold@cern.ch>
   /// @author Camille Belanger-Champagne <Camille.Belanger-Champagne@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 761887 $
   /// $Date: 2016-07-14 15:16:16 +0200 (Thu, 14 Jul 2016) $
   ///
   class TrigComposite_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      TrigComposite_v1();

      /// @name Basic properties
      /// @{

      /// Get a human-readable name for the object
      const std::string& name() const;
      /// Set a human-readable name for the object
      void setName( const std::string& name );

      /// @}

      /// @name Functions for accessing basic properties on the object
      /// @{

      /// Check if a given type of detail is available
      template< typename TYPE >
      bool hasDetail( const std::string& name ) const;

      /// Set an integer detail on the object
      bool setDetail( const std::string& name, int value );
      /// Set an unsigned detail on the object
      bool setDetail( const std::string& name, unsigned int value );
      /// Set a floating point detail on the object
      bool setDetail( const std::string& name, float value );
      /// Set a vector<int> detail on the object
      bool setDetail( const std::string& name,
                      const std::vector< int >& value );
      /// Set a vector<unsigned int> detail on the object 
      bool setDetail( const std::string& name,
                      const std::vector< unsigned int >& value );
      /// Set a vector<float> detail on the object
      bool setDetail( const std::string& name,
                      const std::vector< float >& value );

      /// Get an integer detail from the object
      bool getDetail( const std::string& name, int& value ) const;

      /// Get an unsigned integer detail from the object
      bool getDetail( const std::string& name, unsigned int& value ) const;

      /// Get a floating point detail from the object
      bool getDetail( const std::string& name, float& value ) const;

      /// Get a vector<int> detail from the object
      bool getDetail( const std::string& name,
                      std::vector< int >& value ) const;

      /// Get a vector<insigned int> detail from the object
      bool getDetail( const std::string& name,
                      std::vector< unsigned int>& value ) const;     
      /// Get a vector<float> detail from the object
      bool getDetail( const std::string& name,
                      std::vector< float >& value ) const;


      /// Get a detail by name, missing detail will result on std::runtime_error exception
      template<typename T>
      T getDetail( const std::string& name ) const;

      /// @}

      /// @name Functions for accessing links to component objects
      /// @{

      /// Set the link to an object
      template< class CONTAINER >
      bool setObjectLink( const std::string& name,
                          const ElementLink< CONTAINER >& link );
      /// Check if a link to an object with a given name exists
      bool hasObjectLink( const std::string& name ) const;
      /// Get the link with the requested name
      template< class CONTAINER >
      ElementLink< CONTAINER >
      objectLink( const std::string& name ) const;
      /// Get a bare pointer with the requested name
      template< class OBJECT >
      const OBJECT* object( const std::string& name ) const;

      /// Raw access to the persistent link names
      const std::vector< std::string >& linkColNames() const;
      /// Raw access to the persistent link labels
      const std::vector< uint32_t >& linkColKeys() const;
      /// Raw access to the persistent link indices
      const std::vector< uint16_t >& linkColIndices() const;
      /// Raw access to the persistent link CLIDs
      const std::vector< uint32_t >& linkColClids() const;

      /// @}

   private:
      /// Raw access to the persistent link names (non-const)
      std::vector< std::string >& linkColNamesNC();
      /// Raw access to the persistent link labels (non-const)
      std::vector< uint32_t >& linkColKeysNC();
      /// Raw access to the persistent link indices (non-const)
      std::vector< uint16_t >& linkColIndicesNC();
      /// Raw access to the persistent link CLIDs (non-const)
      std::vector< uint32_t >& linkColClidsNC();

   }; // class TrigComposite_v1

} // namespace xAOD

// Include the template implementation:
#include "TrigComposite_v1.icc"

#endif // XAODTRIGGER_VERSIONS_TRIGCOMPOSITE_V1_H
