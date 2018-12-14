// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigComposite_v1.h 784388 2016-11-15 17:08:58Z tamartin $
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
#include "AthLinks/ElementLinkVector.h"

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
   /// $Revision: 784388 $
   /// $Date: 2016-11-15 18:08:58 +0100 (Tue, 15 Nov 2016) $
   ///
   class TrigComposite_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      TrigComposite_v1();

       /// Copy constructor
       TrigComposite_v1( const TrigComposite_v1& parent );

       /// Assignment operator
       TrigComposite_v1& operator=( const TrigComposite_v1& rhs );

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
      bool setDetail( const std::string& name, int32_t value );
      /// Set an unsigned detail on the object
      bool setDetail( const std::string& name, uint32_t value );
      /// Set a floating point detail on the object
      bool setDetail( const std::string& name, float value );
      /// Set a string detail on the object
      bool setDetail( const std::string& name,
                      const std::string& value );
      /// Set a vector<int> detail on the object
      bool setDetail( const std::string& name,
                      const std::vector< int32_t >& value );
      /// Set a vector<unsigned int> detail on the object 
      bool setDetail( const std::string& name,
                      const std::vector< uint32_t >& value );
      /// Set a vector<unsigned short> detail on the object. Note: For an even vector, the final entry cannot be (2^16)-1=65535
      bool setDetail( const std::string& name,
                      const std::vector< uint16_t >& value );
      /// Set a vector<float> detail on the object
      bool setDetail( const std::string& name,
                      const std::vector< float >& value );
      /// Set a vector<string> detail on the object
      bool setDetail( const std::string& name,
                      const std::vector< std::string >& value );

      /// Get an integer detail from the object
      bool getDetail( const std::string& name, int32_t& value ) const;

      /// Get an unsigned integer detail from the object
      bool getDetail( const std::string& name, uint32_t& value ) const;

      /// Get a floating point detail from the object
      bool getDetail( const std::string& name, float& value ) const;

      /// Get a string detail from the object
      bool getDetail( const std::string& name, std::string& value ) const;

      /// Get a vector<int> detail from the object
      bool getDetail( const std::string& name,
                      std::vector< int32_t >& value ) const;

      /// Get a vector<insigned int> detail from the object
      bool getDetail( const std::string& name,
                      std::vector< uint32_t >& value ) const;

      /// Get a vector<unsigned short> detail from the object
      bool getDetail( const std::string& name,
                      std::vector< uint16_t >& value ) const;

      /// Get a vector<float> detail from the object
      bool getDetail( const std::string& name,
                      std::vector< float >& value ) const;

      /// Get a vector<string> detail from the object
      bool getDetail( const std::string& name,
                      std::vector< std::string >& value ) const;

      /// Get a detail by name, missing detail will result on std::runtime_error exception
      template<typename T>
      T getDetail( const std::string& name ) const;

      /// Get a detail by name, will not throw. Name change to solve disambiguation
      template<typename T>
      std::pair<bool, T> returnDetail( const std::string& name ) const;

      /// @}

      /// @name Functions for accessing links to component objects and object collections
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

      /// Add a link to a single object within a collection
      template< class CONTAINER >
      bool addObjectCollectionLink( const std::string& collectionName,
                                    const ElementLink< CONTAINER >& link );
      /// Add links to multiple objects within a collection
      template< class CONTAINER >
      bool addObjectCollectionLinks( const std::string& collectionName,
                                     const ElementLinkVector< CONTAINER >& links );
      /// Check if links exist to a collection of objects with given name
      bool hasObjectCollectionLinks( const std::string& collectionName ) const;
      /// Get a vector of all element links from the collection
      template< class CONTAINER >
      ElementLinkVector< CONTAINER >
      objectCollectionLinks( const std::string& collectionName ) const;

      /// @}

      /// @name Functions for accessing links to component objects and object collections
      /// @{

      /// Copy one named link from another object.
      /// Will not overwrite an existing link
      /// @param other TrigComposite object to copy the link from
      /// @param name Name of link to copy
      /// @param newName Optional parameter to rename the link during the copy
      /// @return True on successful copy
      bool copyLinkFrom(const xAOD::TrigComposite_v1& other, const std::string& name, std::string newName = "");
      bool copyLinkFrom(const xAOD::TrigComposite_v1* other, const std::string& name, std::string newName = "");

      /// Copy one named link collection from another object.
      /// Will not overwrite or append to an existing collection of links
      /// @param other TrigComposite object to copy the link collection from
      /// @param name Name of link collection
      /// @param newName Optional parameter to rename the link collection during the copy
      /// @return True on successful copy
      bool copyLinkCollectionFrom(const xAOD::TrigComposite_v1& other, const std::string& name, std::string newName = "");
      bool copyLinkCollectionFrom(const xAOD::TrigComposite_v1* other, const std::string& name, std::string newName = "");

      /// Copy all single links and collections of links from another object.
      /// Will not overwrite existing links, or link collections, or append to existing link collections.
      /// @param other TrigComposite object to copy all links and link collections from, which do not already exist.
      /// @return True on successful copy of at least one link
      bool copyAllLinksFrom(const xAOD::TrigComposite_v1& other);
      bool copyAllLinksFrom(const xAOD::TrigComposite_v1* other);

      /// @}

      /// @name Functions for accessing raw data used to construct ElementLinks
      /// @{

      /// Raw access to the persistent link names
      const std::vector< std::string >& linkColNames() const;
      /// Raw access to the persistent link labels
      const std::vector< uint32_t >& linkColKeys() const;
      /// Raw access to the persistent link indices
      const std::vector< uint16_t >& linkColIndices() const;
      /// Raw access to the persistent link CLIDs
      const std::vector< uint32_t >& linkColClids() const;

      /// @}

      /// For use in validation, when copying element links from one object to another
      static bool s_throwOnCopyError;

   private:
      /// Raw access to the persistent link names (non-const)
      std::vector< std::string >& linkColNamesNC();
      /// Raw access to the persistent link labels (non-const)
      std::vector< uint32_t >& linkColKeysNC();
      /// Raw access to the persistent link indices (non-const)
      std::vector< uint16_t >& linkColIndicesNC();
      /// Raw access to the persistent link CLIDs (non-const)
      std::vector< uint32_t >& linkColClidsNC();

      /// Helper function, copy one link into this object
      void copyLinkInternal(const xAOD::TrigComposite_v1& other, const size_t index, const std::string& newName);

      static const std::string s_collectionSuffix;

   }; // class TrigComposite_v1

} // namespace xAOD

// Include the template implementation:
#include "TrigComposite_v1.icc"

/**
 * @brief print helper for TrigComposite
 */
std::ostream& operator<<(std::ostream& os, const xAOD::TrigComposite_v1& tc);


#endif // XAODTRIGGER_VERSIONS_TRIGCOMPOSITE_V1_H
