/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigComposite_v1.cxx 784384 2016-11-15 16:37:49Z tamartin $

// System include(s):
#include <algorithm>
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

#include "TrigNavStructure/StringSerializer.h"

// Local include(s):
#include "xAODTrigger/versions/TrigComposite_v1.h"

namespace xAOD {

  const std::string TrigComposite_v1::s_collectionSuffix = "__COLL";
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

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigComposite_v1, std::string,
                                      name, setName )

   template<>
   bool TrigComposite_v1::hasDetail<uint32_t>( const std::string& name ) const {
     return hasDetail<int32_t>(name);
   }

   template<>
   bool TrigComposite_v1::hasDetail<std::vector<uint32_t>>( const std::string& name ) const {
     return hasDetail<std::vector<int32_t>>(name);
   }

   template<>
   bool TrigComposite_v1::hasDetail<std::vector<uint16_t>>( const std::string& name ) const {
     return hasDetail<std::vector<int32_t>>(name);
   }

   template<>
   bool TrigComposite_v1::hasDetail<std::string>( const std::string& name ) const {
     return hasDetail<std::vector<int32_t>>(name);
   }

   template<>
   bool TrigComposite_v1::hasDetail<std::vector<std::string>>( const std::string& name ) const {
     return hasDetail<std::vector<int32_t>>(name);
   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //                   Simple detail accessor functions
   //

   bool TrigComposite_v1::setDetail( const std::string& name, int32_t value ) {

      // It should be pretty strange if this should fail:
      try {
         auxdata< int32_t >( name ) = value;
      } catch(const std::exception& exc) {
         std::cerr << "xAOD::TrigComposite_v1::setDetail ERROR Internal logic error found in int32_t: [" << exc.what() << "]" << std::endl;
         return false;
      }

      // Return gracefully:
      return true;
   }

   bool TrigComposite_v1::setDetail( const std::string& name, uint32_t value ) {
     return setDetail(name, (int32_t)value); // place for conversion if needed
   }

   bool TrigComposite_v1::setDetail( const std::string& name, float value ) {

      // It should be pretty strange if this should fail:
      try {
         auxdata< float >( name ) = value;
      } catch(const std::exception& exc) {
         std::cerr << "xAOD::TrigComposite_v1::setDetail ERROR Internal logic error found in float: [" << exc.what() << "]" << std::endl;
         return false;
      }

      // Return gracefully:
      return true;
   }

   bool TrigComposite_v1::setDetail( const std::string& name,
                                     const std::vector< int32_t >& value ) {

      // It should be pretty strange if this should fail:
      try {
         auxdata< std::vector< int32_t > >( name ) = value;
      } catch(const std::exception& exc) {
         std::cerr << "xAOD::TrigComposite_v1::setDetail ERROR Internal logic error found in vector<int32_t>: [" << exc.what() << "]" << std::endl;
         return false;
      }

      // Return gracefully:
      return true;
   }


   bool TrigComposite_v1::setDetail( const std::string& name,
                                     const std::vector< uint32_t >& value ) {

      // It should be pretty strange if this should fail:
     std::vector<int32_t> temp(value.begin(), value.end()); //
      try {
         auxdata< std::vector< int32_t > >( name ) = temp;
      } catch(const std::exception& exc) {
         std::cerr << "xAOD::TrigComposite_v1::setDetail ERROR Internal logic error found in vector<uint32_t>: [" << exc.what() << "]" << std::endl;
         return false;
      }

      // Return gracefully:
      return true;
   }

   bool TrigComposite_v1::setDetail( const std::string& name,
                                     const std::vector< uint16_t >& value ) {

      std::vector<uint32_t> temp;
      temp.reserve( value.size() / 2 );

      // Pack shorts for space efficiency
      for (size_t i = 0; i < value.size(); i += 2) {
        const uint16_t a = value.at(i);
        const uint16_t b = (i + 1 < value.size() ? value.at(i + 1) : std::numeric_limits<uint16_t>::max() );
        const uint32_t combine = ( (b << 16) | (a & 0xffff) );
        temp.push_back( combine );
      }

      return setDetail(name, temp);
   }

   bool TrigComposite_v1::setDetail( const std::string& name,
                                     const std::vector< float >& value ) {

      // It should be pretty strange if this should fail:
      try {
         auxdata< std::vector< float > >( name ) = value;
      } catch(const std::exception& exc) {
         std::cerr << "xAOD::TrigComposite_v1::setDetail ERROR Internal logic error found in vector<float>: [" << exc.what() << "]" << std::endl;
         return false;
      }

      // Return gracefully:
      return true;
   }

   bool TrigComposite_v1::setDetail( const std::string& name, const std::string& value ) {

      std::vector<uint32_t> serialForm;
      HLT::StringSerializer::serialize(value, serialForm);
      return setDetail(name, serialForm);
   }

   bool TrigComposite_v1::setDetail( const std::string& name, const std::vector< std::string >& value ) {

      std::vector<uint32_t> serialForm;
      HLT::StringSerializer::serialize(value, serialForm);
      return setDetail(name, serialForm);
   }

   bool TrigComposite_v1::getDetail( const std::string& name,
                                     int32_t& value ) const {

      // Object used to access the auxiliary data:
      Accessor< int32_t > acc( name );

      // Enable the check once it will work correctly:
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }


   bool TrigComposite_v1::getDetail( const std::string& name,
                                     uint32_t& value ) const {
     int32_t v = 0;
     const bool status = getDetail(name, v); 
     value = v; // place for cast
     return status;
     
   }

   bool TrigComposite_v1::getDetail( const std::string& name,
                                     float& value ) const {

      // Object used to access the auxiliary data:
      Accessor< float > acc( name );

      // Enable the check once it will work correctly:
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }


   bool TrigComposite_v1::getDetail( const std::string& name,
                                     std::vector< int32_t >& value ) const {

      // Object used to access the auxiliary data:
      Accessor< std::vector< int32_t > > acc( name );

      // Enable the check once it will work correctly:
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }

   bool TrigComposite_v1::getDetail( const std::string& name,
                                     std::vector< uint32_t >& value ) const {

     std::vector<int32_t> temp;
     const bool status = getDetail(name, temp);
     
     value.reserve(temp.size());
     for ( int32_t i: temp )
       value.push_back(i);
     return status;
   }

   bool TrigComposite_v1::getDetail( const std::string& name,
                                     std::vector< uint16_t >& value ) const {

     std::vector<uint32_t> temp;
     const bool status = getDetail(name, temp);
     value.reserve(temp.size() * 2);

     // Unpack shorts
     static const uint32_t mask = std::numeric_limits<uint16_t>::max();
     for (size_t i = 0; i < temp.size(); ++i) {
       const uint32_t packed = temp.at(i);
       const uint16_t a = packed & mask;
       const uint16_t b = packed >> 16;
       value.push_back(a);
       // Use this to tell if the second half of the int was used or not
       if (b != std::numeric_limits<uint16_t>::max()) {
         value.push_back(b);
       }
     }

     return status;
   }


   bool TrigComposite_v1::getDetail( const std::string& name,
                                     std::vector< float >& value ) const {

      // Object used to access the auxiliary data:
      Accessor< std::vector< float > > acc( name );

      // Enable the check once it will work correctly:
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }

   bool TrigComposite_v1::getDetail( const std::string& name,
                                     std::string& value ) const {

      std::vector<uint32_t> temp;
      const bool status = getDetail(name, temp);

      HLT::StringSerializer::deserialize(temp.begin(), temp.end(), value);
      return status;
   }

   bool TrigComposite_v1::getDetail( const std::string& name,
                                     std::vector< std::string >& value ) const {

      std::vector<uint32_t> temp;
      const bool status = getDetail(name, temp);

      HLT::StringSerializer::deserialize(temp.begin(), temp.end(), value);
      return status;
   }

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
    if (newName == "self") {
      if (s_throwOnCopyError) throw std::runtime_error("Cannot copy the 'self' link in a logical way.");
      return false;
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
      // Cannot copy any 'self' links as *this does not know its own location in its parent container
      if (name == "self") continue;
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

   bool TrigComposite_v1::hasObjectLink( const std::string& name ) const {

      // Since this function shouldn't throw exceptions too easily,
      // let's be super careful here...
      static Accessor< std::vector< std::string > > accNames( "linkColNames" );
      if( ! accNames.isAvailable( *this ) ) {
         return false;
      }

      // The check itself is pretty simple:
      const std::vector< std::string >& names = accNames( *this );
      return ( std::find( names.begin(), names.end(), name ) != names.end() );
   }

   bool TrigComposite_v1::hasObjectCollectionLinks( const std::string& collectionName ) const {
      const std::string mangledName = collectionName + s_collectionSuffix;
      return hasObjectLink( mangledName );
   }

   AUXSTORE_OBJECT_GETTER( TrigComposite_v1, std::vector< std::string >,
                           linkColNames )
   AUXSTORE_OBJECT_GETTER( TrigComposite_v1, std::vector< uint32_t >,
                           linkColKeys )
   AUXSTORE_OBJECT_GETTER( TrigComposite_v1, std::vector< uint16_t >,
                           linkColIndices )
   AUXSTORE_OBJECT_GETTER( TrigComposite_v1, std::vector< uint32_t >,
                           linkColClids )

   std::vector< std::string >& TrigComposite_v1::linkColNamesNC() {

      static Accessor< std::vector< std::string > > acc( "linkColNames" );
      return acc( *this );
   }

   std::vector< uint32_t >& TrigComposite_v1::linkColKeysNC() {

      static Accessor< std::vector< uint32_t > > acc( "linkColKeys" );
      return acc( *this );
   }

   std::vector< uint16_t >& TrigComposite_v1::linkColIndicesNC() {

      static Accessor< std::vector< uint16_t > > acc( "linkColIndices" );
      return acc( *this );
   }

   std::vector< uint32_t >& TrigComposite_v1::linkColClidsNC() {

      static Accessor< std::vector< uint32_t > > acc( "linkColClids" );
      return acc( *this );
   }

   void TrigComposite_v1::typelessSetObjectLink( const std::string& name, const uint32_t key, const uint32_t clid, const uint16_t beginIndex, const uint16_t endIndex ) {

     // Loop over collections
     if ( endIndex - beginIndex > 1 ) {

       const std::string mangledName = name + s_collectionSuffix;
       for ( unsigned int index = beginIndex; index < endIndex; ++index ) {

         // Check uniqueness
         if ( std::find( linkColNamesNC().begin(), linkColNamesNC().end(), mangledName ) == linkColNamesNC().end() ) {

           this->linkColNamesNC().push_back( mangledName );
           this->linkColKeysNC().push_back( key );
           this->linkColIndicesNC().push_back( index );
           this->linkColClidsNC().push_back( clid );
         }
       }
     }
     else {

       // Check uniqueness
       if ( std::find( linkColNamesNC().begin(), linkColNamesNC().end(), name ) == linkColNamesNC().end() ) {

         this->linkColNamesNC().push_back( name );
         this->linkColKeysNC().push_back( key );
         this->linkColIndicesNC().push_back( beginIndex );
         this->linkColClidsNC().push_back( clid );
       }
     }
   }

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD

std::ostream& operator<<(std::ostream& os, const xAOD::TrigComposite_v1& tc) {
  os << "TrigComposite_v1 '" << tc.name() << "' link: name, key, index, CLID" << std::endl;
  for (size_t i=0; i<tc.linkColNames().size(); ++i){
    os << tc.linkColNames()[i] << ", ";
    os << tc.linkColKeys()[i] << ", ";
    os << tc.linkColIndices()[i] << ", ";
    os << tc.linkColClids()[i] << std::endl;
  }
  return os;
}
