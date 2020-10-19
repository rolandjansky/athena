/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <algorithm>
#include <stdexcept>

// In "standalone mode" xAOD::IParticleContainer doesn't have a CLID
// defined for it. But this code requires one to be set.
//
// The following is incredibly ugly, but this is the best that I could
// come up with on short notice. Note that the CLID value is copy-pasted
// from the IParticleContainer.h header.
#include "xAODBase/IParticleContainer.h"
#ifdef XAOD_STANDALONE
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::IParticleContainer, 1241842700, 1 )
#endif // XAOD_STANDALONE

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

#include "TrigNavStructure/StringSerializer.h"

// Local include(s):
#include "xAODTrigger/versions/TrigComposite_v1.h"


namespace xAOD {

  const std::string TrigComposite_v1::s_collectionSuffix{"__COLL"};

  // Note: These definitions shadow those in TrigCompositeUtils.py
  const std::string TrigComposite_v1::s_initialRoIString{"initialRoI"};
  const std::string TrigComposite_v1::s_initialRecRoIString{"initialRecRoI"};
  const std::string TrigComposite_v1::s_roiString{"roi"};
  const std::string TrigComposite_v1::s_viewString{"view"};
  const std::string TrigComposite_v1::s_featureString{"feature"};
  const std::string TrigComposite_v1::s_seedString{"seed"};

  bool TrigComposite_v1::s_throwOnCopyError = false; 

  TrigComposite_v1::TrigComposite_v1() {
  }

  TrigComposite_v1::TrigComposite_v1( const TrigComposite_v1& parent ) : SG::AuxElement() {
    this->makePrivateStore( parent );
  }

  TrigComposite_v1& TrigComposite_v1::operator=( const TrigComposite_v1& rhs ) {
    if(this == &rhs) return *this;
    if( ( ! hasStore() ) && ( ! container() ) ) this->makePrivateStore();

    // Copy the auxiliary variables:
    SG::AuxElement::operator=( rhs );

    // Return this object:
    return *this;
  }

   /////////////////////////////////////////////////////////////////////////////
   //
   //                   Built in accessor functions
   //

  AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigComposite_v1, std::string,
                                      name, setName )

  AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigComposite_v1, std::vector<TrigCompositeUtils::DecisionID>,
                                      decisions, setDecisions )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //               Implementation for the link copy functions
   //

  void TrigComposite_v1::copyLinkInternal(const xAOD::TrigComposite_v1& other, const size_t index, const std::string& newName) {
    this->linkColNamesNC().push_back( newName );
    this->linkColKeysNC().push_back(    other.linkColKeys().at(    index ) );
    this->linkColIndicesNC().push_back( other.linkColIndices().at( index ) );
    this->linkColClidsNC().push_back(   other.linkColClids().at(   index ) );
  }

  bool TrigComposite_v1::copyLinkFrom(const xAOD::TrigComposite_v1& other, const std::string& name, std::string newName) {
    if (newName == "") {
      newName = name;
    }
    bool didCopy = false;
    // Check for the existence of single link
    std::vector<std::string>::const_iterator locationIt;
    locationIt = std::find(other.linkColNames().begin(), other.linkColNames().end(), name);
    if (locationIt != other.linkColNames().end()) {
      size_t index = std::distance(other.linkColNames().begin(), locationIt);
      if (this->hasObjectLink(newName)) {
        if (s_throwOnCopyError) throw std::runtime_error("Already have link with name " + newName);
      } else {
        copyLinkInternal(other, index, newName);
        didCopy = true;
      }
    }
    if (!didCopy && s_throwOnCopyError) throw std::runtime_error("Could not find link with name " + name);
    return didCopy;
  }

  bool TrigComposite_v1::copyLinkFrom(const xAOD::TrigComposite_v1* other, const std::string& name, std::string newName) {
    return copyLinkFrom(*other, name, newName);
  }

  bool TrigComposite_v1::copyLinkCollectionFrom(const xAOD::TrigComposite_v1& other, const std::string& name, std::string newName) {
    bool didCopy = false;
    // Check for the existence of a collection.
    if (newName == "") {
      newName = name;
    }
    const std::string mangledName = name + s_collectionSuffix;
    const std::string mangledNewName = newName + s_collectionSuffix;
    if (other.hasObjectLink(mangledName)) {
      if (this->hasObjectLink(mangledNewName)) {
        if (s_throwOnCopyError) throw std::runtime_error("Already have link collection with name " + newName);
      } else {
        // Copy all links in the collection. Just iterating through the source vector
        for (size_t index = 0; index < other.linkColNames().size(); ++index) {
          if (other.linkColNames().at(index) == mangledName) {
            copyLinkInternal(other, index, mangledNewName);
          }
        }
        didCopy = true;
      }
    }
    if (!didCopy && s_throwOnCopyError) throw std::runtime_error("Could not find link with name " + name);
    return didCopy;
  }
    
  bool TrigComposite_v1::copyLinkCollectionFrom(const xAOD::TrigComposite_v1* other, const std::string& name, std::string newName) {
    return copyLinkCollectionFrom(*other, name, newName);
  }

  bool TrigComposite_v1::copyAllLinksFrom(const xAOD::TrigComposite_v1& other) {
    bool didCopy = false;
    for (const std::string& name : other.linkColNames()) {
      // Check we don't have one (or more) entries with this raw name (raw = might be mangled).
      if (this->hasObjectLink(name)) continue;
      // Check if the link is for a single object or collection of objects by looking for the mangled suffix
      const bool isCollection = (name.size() > s_collectionSuffix.size() && 
                                 std::equal(s_collectionSuffix.rbegin(), s_collectionSuffix.rend(), name.rbegin()));
      if (isCollection) {
        // The copyLinkCollectionFrom call needs the un-mangled name as it is a public fn. It will re-mangle.
        const std::string unmangledName = name.substr(0, name.size() - s_collectionSuffix.size());
        copyLinkCollectionFrom(other, unmangledName);
      } else { // !isCollection
        copyLinkFrom(other, name);
      }
      didCopy = true;
    }
    return didCopy;
  }

  bool TrigComposite_v1::copyAllLinksFrom(const xAOD::TrigComposite_v1* other) {
    return copyAllLinksFrom(*other);
  }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //               Implementation for the link accessor functions
   //

   bool TrigComposite_v1::hasObjectLink( const std::string& name, const CLID clid ) const {

      // Since this function shouldn't throw exceptions too easily,
      // let's be super careful here...
      static const ConstAccessor< std::vector< std::string > > accNames( "linkColNames" );
      static const ConstAccessor< std::vector< uint32_t > >    accCLIDs( "linkColClids" );
      if( ! (accNames.isAvailable( *this ) || accCLIDs.isAvailable( *this) ) ) {
         return false;
      }

      // The check itself is pretty simple:
      const std::vector< std::string >& names = accNames( *this );
      const std::vector< uint32_t >&    clids = accCLIDs( *this );
      
      std::vector<std::string>::const_iterator vecIt = std::find( names.begin(), names.end(), name );
      if (vecIt == names.end()) {
         return false; // Could not find name
      }

      if (clid != CLID_NULL) { // Also check against clid
         const uint32_t storedCLID = clids.at( std::distance( names.begin(), vecIt ) );
         if (clid == ClassID_traits< xAOD::IParticleContainer >::ID()) {
            return derivesFromIParticle(storedCLID);
         } else if (storedCLID != clid) { // Otherwise we require the ID to match
            return false; // Type missmatch
         }
      }

      return true; // Satisfied
   }

   bool TrigComposite_v1::hasObjectCollectionLinks( const std::string& collectionName, const CLID clid ) const {
      const std::string mangledName = collectionName + s_collectionSuffix;
      return hasObjectLink( mangledName, clid );
   }


   bool TrigComposite_v1::hasObjectLinkExact(const std::string& name, const uint32_t key, const uint16_t index, const uint32_t clid) const {
      for (size_t i = 0; i < this->linkColNames().size(); ++i) {
         if (this->linkColNames().at(i) != name) continue;
         if (this->linkColKeysNoRemap().at(i) != key) continue;
         if (this->linkColIndicesNoRemap().at(i) != index) continue;
         if (this->linkColClids().at(i) != clid) continue;
         return true;
      } 
      return false;
   }

   bool TrigComposite_v1::derivesFromIParticle(const CLID /*clid*/) const {
      // It would be nice to include some logic here.
      return true; 
   }

   AUXSTORE_OBJECT_GETTER( TrigComposite_v1, std::vector< std::string >,
                           linkColNames )
   AUXSTORE_OBJECT_GETTER( TrigComposite_v1, std::vector< uint32_t >,
                           linkColClids )

   const std::vector< uint32_t >& TrigComposite_v1::linkColKeys() const {
      if (isRemapped()) {
        static const Accessor< std::vector< uint32_t > > acc_remap( "remap_linkColKeys" );
        return acc_remap( *this );
      }
      static const Accessor< std::vector< uint32_t > > acc_builtin( "linkColKeys" );
      return acc_builtin( *this );
   }

   const std::vector< uint16_t >& TrigComposite_v1::linkColIndices() const {
      if (isRemapped()) {
        static const Accessor< std::vector< uint16_t > > acc_remap( "remap_linkColIndices" );
        return acc_remap( *this );
      }
      static const Accessor< std::vector< uint16_t > > acc_builtin( "linkColIndices" );
      return acc_builtin( *this );
   }

   const std::vector< uint32_t >& TrigComposite_v1::linkColKeysNoRemap() const {
      static const Accessor< std::vector< uint32_t > > acc( "linkColKeys" );
      return acc( *this );
   }

   const std::vector< uint16_t >& TrigComposite_v1::linkColIndicesNoRemap() const {
      static const Accessor< std::vector< uint16_t > > acc( "linkColIndices" );
      return acc( *this );
   }

   ////////

   std::vector< std::string >& TrigComposite_v1::linkColNamesNC() {

      static const Accessor< std::vector< std::string > > acc( "linkColNames" );
      return acc( *this );
   }

   std::vector< uint32_t >& TrigComposite_v1::linkColKeysNC() {

      static const Accessor< std::vector< uint32_t > > acc( "linkColKeys" );
      return acc( *this );
   }

   std::vector< uint16_t >& TrigComposite_v1::linkColIndicesNC() {

      static const Accessor< std::vector< uint16_t > > acc( "linkColIndices" );
      return acc( *this );
   }

   std::vector< uint32_t >& TrigComposite_v1::linkColClidsNC() {

      static const Accessor< std::vector< uint32_t > > acc( "linkColClids" );
      return acc( *this );
   }

   void TrigComposite_v1::typelessSetObjectLink( const std::string& name, const uint32_t key, const uint32_t clid, const uint16_t beginIndex, const uint16_t endIndex ) {

     // Loop over collections
     if ( endIndex - beginIndex > 1 ) { // Adding a *collection* of links

       // Check uniqueness
       const std::string mangledName = name + s_collectionSuffix;
       const std::vector< std::string >& names = linkColNames();
       int oldStart = -1;
       int oldEnd = -1;
       for( size_t nameIndex = 0; nameIndex < names.size(); ++nameIndex ) {

         // Look for an existing collection with the same name
         if( names[ nameIndex ] == mangledName ) {
           oldEnd = nameIndex + 1;
           if ( oldStart == -1 ) oldStart = nameIndex;
         }
         else if ( oldStart != -1 ) {
           // If the start has been found, we must now be past the ned
           break;
         }
       }

       // Erase the old collection, if there was one
       if ( oldStart != -1 ) {

         this->linkColNamesNC().erase( this->linkColNamesNC().begin() + oldStart, this->linkColNamesNC().begin() + oldEnd );
         this->linkColKeysNC().erase( this->linkColKeysNC().begin() + oldStart, this->linkColKeysNC().begin() + oldEnd );
         this->linkColIndicesNC().erase( this->linkColIndicesNC().begin() + oldStart, this->linkColIndicesNC().begin() + oldEnd );
         this->linkColClidsNC().erase( this->linkColClidsNC().begin() + oldStart, this->linkColClidsNC().begin() + oldEnd );
       }

       // Append the new collection
       for ( unsigned int index = beginIndex; index < endIndex; ++index ) {

         this->linkColNamesNC().push_back( mangledName );
         this->linkColKeysNC().push_back( key );
         this->linkColIndicesNC().push_back( index );
         this->linkColClidsNC().push_back( clid );
       }
     }
     else { // Adding a *single* link

       // Check uniqueness
       if ( std::find( linkColNamesNC().begin(), linkColNamesNC().end(), name ) == linkColNamesNC().end() ) {

         this->linkColNamesNC().push_back( name );
         this->linkColKeysNC().push_back( key );
         this->linkColIndicesNC().push_back( beginIndex );
         this->linkColClidsNC().push_back( clid );
       }
       else {

         // Over-write an existing object
         const std::vector< std::string >& names = linkColNames();
         for( size_t nameIndex = 0; nameIndex < names.size(); ++nameIndex ) {

           if( names[ nameIndex ] == name ) {

             this->linkColKeysNC()[ nameIndex ] = key;
             this->linkColIndicesNC()[ nameIndex ] = beginIndex;
             this->linkColClidsNC()[ nameIndex ] = clid;
             break; // Names are unique, so stop once found
           }
         }
       }
     }
   }

   bool TrigComposite_v1::typelessGetObjectLink( const std::string& name, uint32_t& key, uint32_t& clid, uint16_t& index ) const {
      std::vector<std::string>::const_iterator it = std::find(linkColNames().begin(), linkColNames().end(), name);
      if (it == linkColNames().end()) {
         return false;
      }
      const size_t location = std::distance(linkColNames().begin(), it);
      key = linkColKeys().at(location);
      clid = linkColClids().at(location);
      index = linkColIndices().at(location);
      return true;
   }


   bool TrigComposite_v1::typelessGetObjectCollectionLinks( const std::string& name, 
     std::vector<uint32_t>& keyVec, std::vector<uint32_t>& clidVec, std::vector<uint16_t>& indexVec ) const
   {
      bool found = false;
      const std::string mangledName = name + s_collectionSuffix;
      for (size_t i = 0; i < this->linkColNames().size(); ++i) {
        if (linkColNames().at(i) != mangledName) {
          continue;
        }
        keyVec.push_back( linkColKeys().at(i) );
        clidVec.push_back( linkColClids().at(i) );
        indexVec.push_back( linkColIndices().at(i) );
        found = true;
      }
      return found;
   }


   bool TrigComposite_v1::isRemapped() const {
      static const Accessor< std::vector< uint32_t > > key_remap( "remap_linkColKeys" );
      static const Accessor< std::vector< uint16_t > > index_remap( "remap_linkColIndices" );
      size_t nDecorations = 0;
      if (key_remap.isAvailable( *this )) ++nDecorations;
      if (index_remap.isAvailable( *this )) ++nDecorations;
      if (nDecorations == 1) {
        throw std::runtime_error("TrigComposite_v1::isRemapped Only one of the 'remap_linkColKeys' and 'remap_linkColIndices' "
          "decorations were found on this object. This should never happen, a remapped element link must have both of these collections.");
      }
      return static_cast<bool>(nDecorations); //0=Fasle, 2=True
   }


   //
   /////////////////////////////////////////////////////////////////////////////


std::ostream& operator<<(std::ostream& os, const xAOD::TrigComposite_v1& tc) {
  os << "TrigComposite_v1 name:'" << tc.name() << "'" << std::endl;
  const bool isRemapped = tc.isRemapped();
  os << "  N Links:" << tc.linkColNames().size() << ", isRemapped:" << (isRemapped ? "YES" : "NO");
  for (size_t i=0; i<tc.linkColNames().size(); ++i){
    if (!i) os << std::endl;
    os << "    Link Name:"  << tc.linkColNames()[i];
    os << ", Key:"   << tc.linkColKeys()[i];
    if (isRemapped) os << ", OldKey:"   << tc.linkColKeysNoRemap()[i];
    os << ", Index:" << tc.linkColIndices()[i];
    if (isRemapped) os << ", OldIndex:" << tc.linkColIndicesNoRemap()[i];
    os << ", CLID:"  << tc.linkColClids()[i];
    if (i != tc.linkColNames().size() - 1) os << std::endl;
  }
  if (tc.decisions().size()) {
    os << std::endl << "  N Decisions:" << tc.decisions().size() << std::endl << "    ";
    for (const TrigCompositeUtils::DecisionID id : tc.decisions()) os << id << ", ";
  }
  return os;
}

} // namespace xAOD
