// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_TRIGCOMPOSITE_V1_H
#define XAODTRIGGER_VERSIONS_TRIGCOMPOSITE_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <string>
#include <vector>
#include <set>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "xAODBase/IParticleContainer.h"

#include "CxxUtils/checker_macros.h"

namespace TrigCompositeUtils{ 
   typedef unsigned int DecisionID;
   typedef std::set<DecisionID> DecisionIDContainer;
}

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

      /// Get positive HLT chain decisions associated with this TrigComposite. Navigation use
      const std::vector<TrigCompositeUtils::DecisionID>& decisions() const;
      /// Set positive HLT chain decisions associated with this TrigComposite. Navigation use
      void setDecisions( const std::vector<TrigCompositeUtils::DecisionID>& decisions );

      /// @}

      /// @name Functions for accessing basic properties on the object
      /// @{

      /// Check if a given type of detail is available
      template< typename TYPE >
      bool hasDetail( const std::string& name ) const;

      /// Set an TYPE detail on the object
      template< typename TYPE >
      bool setDetail( const std::string& name, const TYPE& value );

      /// Get an TYPE detail from the object
      template< typename TYPE >
      bool getDetail( const std::string& name, TYPE& value ) const;

      /// Get a detail by name, missing detail will result on std::runtime_error exception
      template<typename TYPE>
      TYPE getDetail( const std::string& name ) const;

      /// @}

      /// @name Functions for accessing links to component objects and object collections
      /// @{

      /// Set the link to an object
      template< class CONTAINER >
      bool setObjectLink( const std::string& name,
                          const ElementLink< CONTAINER >& link );
      /// Check if a link to an object with a given name and type exists. CLID_NULL to not check type.
      bool hasObjectLink( const std::string& name, const CLID clid = CLID_NULL ) const;
      /// Get the link with the requested name
      template< class CONTAINER >
      ElementLink< CONTAINER >
      objectLink( const std::string& name ) const;
      /// Get a bare pointer with the requested name
      template< class OBJECT >
      const OBJECT* object( const std::string& name ) const;

      /// Add a link to a single object within a collection. Performs de-duplication
      template< class CONTAINER >
      bool addObjectCollectionLink( const std::string& collectionName,
                                    const ElementLink< CONTAINER >& link );
      /// Add links to multiple objects within a collection.  Performs de-duplication
      template< class CONTAINER >
      bool addObjectCollectionLinks( const std::string& collectionName,
                                     const ElementLinkVector< CONTAINER >& links );
      /// Check if links exist to a collection of objects with given name and type. CLID_NULL to not check type.
      bool hasObjectCollectionLinks( const std::string& collectionName, const CLID clid = CLID_NULL) const;
      /// Get a vector of all element links from the collection
      template< class CONTAINER >
      ElementLinkVector< CONTAINER >
      objectCollectionLinks( const std::string& collectionName ) const;

      /// Add a link without type
      void typelessSetObjectLink( const std::string& name, 
                                  const uint32_t key, 
                                  const uint32_t clid, 
                                  const uint16_t beginIndex, 
                                  const uint16_t endIndex = 0 );

      /// Fetches a single link without type. Note: Will not work for collections of links, use typelessGetObjectCollectionLinks
      /// @param[in] name Name of the stored link
      /// @param[out] key Storegte key of the link-object's collection
      /// @param[out] clid Type of the object's collection
      /// @param[out] index Index within the collection of the link-object
      /// @return True if a link was found
      bool typelessGetObjectLink( const std::string& name, 
                                  uint32_t& key,
                                  uint32_t& clid, 
                                  uint16_t& index ) const;

      /// Fetches a collection of links without type.
      /// @param[in] name Name of the stored link
      /// @param[out] keyVec Storegte key of the link-objects will be pushed on to this vector
      /// @param[out] clidVec Type of the objects will be pushed on to this vector
      /// @param[out] indexVec Index of the objects within their collections  will be pushed on to this vector
      /// @return True if at least one link was found
      bool typelessGetObjectCollectionLinks( const std::string& name, 
                                  std::vector<uint32_t>& keyVec,
                                  std::vector<uint32_t>& clidVec, 
                                  std::vector<uint16_t>& indexVec ) const;


      /// Look up all links stored to objects of (container) type CONTAINER
      /// @return Vector of names to all links to objects
      template< class CONTAINER >
      std::vector<std::string> getObjectNames() const;

      /// Look up all links stored to collections objects from (container) type CONTAINER
      /// @return Vector of names to all collections of links to objects
      template< class CONTAINER >
      std::vector<std::string> getObjectCollectionNames() const;

      /// @}

      /// @name Functions for copying links between objects
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
      /// Raw access to the persistent link labels. Will use remapped data, if available.
      const std::vector< uint32_t >& linkColKeys() const;
      /// Raw access to the persistent link indices. Will use remapped data, if available.
      const std::vector< uint16_t >& linkColIndices() const;
      /// Raw access to the persistent link CLIDs
      const std::vector< uint32_t >& linkColClids() const;

      /// Information on if linkColKeys() and linkColIndices() are able to access remapped link data
      /// Remapping happens at the end of HLT execution when EDM objects are copied out of their per-EventView
      /// containers and into the global Trigger EDM containers.
      bool isRemapped() const;

      /// Raw access to the persistent link labels. Will not attempt to access remapped link data.
      const std::vector< uint32_t >& linkColKeysNoRemap() const;
      /// Raw access to the persistent link indices. Will not attempt to access remapped link data.
      const std::vector< uint16_t >& linkColIndicesNoRemap() const;

      /// @}

      /// For use in validation, when copying element links from one object to another
      static bool s_throwOnCopyError ATLAS_THREAD_SAFE;

      /// Constant used to identify an initial ROI from L1
      static const std::string s_initialRoIString;
      /// Constant used to identify an initial HLT ROI derived from L1
      static const std::string s_initialRecRoIString;
      /// Constant used to identify an (explicitly) updated HLT ROI
      static const std::string s_roiString;
      /// Constant used to identify a view
      static const std::string s_viewString;
      /// Constant used to identify a feature
      static const std::string s_featureString;
      /// Constant used to identify a seed (parent)
      static const std::string s_seedString;

   private:
      /// Raw access to the persistent link names (non-const)
      std::vector< std::string >& linkColNamesNC();
      /// Raw access to the persistent link labels (non-const)
      std::vector< uint32_t >& linkColKeysNC();
      /// Raw access to the persistent link indices (non-const)
      std::vector< uint16_t >& linkColIndicesNC();
      /// Raw access to the persistent link CLIDs (non-const)
      std::vector< uint32_t >& linkColClidsNC();

      // Helper function, check for exact link match
      bool hasObjectLinkExact(const std::string& name, const uint32_t key, const uint16_t index, const uint32_t clid) const;

      /// Helper function, copy one link into this object
      void copyLinkInternal(const xAOD::TrigComposite_v1& other, const size_t index, const std::string& newName);

      /// Helper function. Check if the requested type can be down cast to an IParticle transient interface
      bool derivesFromIParticle(const CLID clid) const;

      /// Helper function. Contains type logic check for use during actual link retrieval. Throws on error. 
      template< class CONTAINER >
      void checkTypes(const CLID storedCLID, const std::string& name) const;


      static const std::string s_collectionSuffix;

   }; // class TrigComposite_v1

/**
 * @brief print helper for TrigComposite
 */
std::ostream& operator<<(std::ostream& os, const xAOD::TrigComposite_v1& tc);


} // namespace xAOD

// Include the template implementation:
#include "TrigComposite_v1.icc"

#endif // XAODTRIGGER_VERSIONS_TRIGCOMPOSITE_V1_H
