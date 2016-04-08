/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ObjectMetadata.cxx 638777 2015-01-12 04:48:15Z ssnyder $

// Boost include(s):
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

// Gaudi/Athena include(s):
#include "GaudiKernel/System.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "D3PDMakerUtils/ObjectMetadata.h"

/// Private namespace for the local function(s)
namespace {

   /// Function checking if a type is a primitive or not
   /**
    * This function is just meant to simplify checking if a variable,
    * described by a type_info object is a primitive variable or not.
    * This is quite important for ROOT, and might be important for
    * other backends as well.
    *
    * @param ti The type_info of the object being investigated
    * @returns <code>true</code> if the object in question is a primitive,
    *          <code>false</code> if it's not
    */
   bool isPrimitive( const std::type_info& ti ) {

      if( ( ti == typeid( char* ) ) ||
          ( ti == typeid( char ) ) ||
          ( ti == typeid( unsigned char ) ) ||
          ( ti == typeid( short ) ) ||
          ( ti == typeid( unsigned short ) ) ||
          ( ti == typeid( int ) ) ||
          ( ti == typeid( unsigned int ) ) ||
          ( ti == typeid( float ) ) ||
          ( ti == typeid( double ) ) ||
          ( ti == typeid( long long ) ) ||
          ( ti == typeid( unsigned long long ) ) ||
          ( ti == typeid( bool ) ) ) {

         return true;
      }

      return false;
   }

} // namespace

namespace D3PD {

   // Set the value of the constant(s):
   const size_t          ObjectMetadata::RANDOM_NAME_POSTFIX_LENGTH = 6;
   const char*           ObjectMetadata::STRING_SEPARATOR           = "@";
   const unsigned int    ObjectMetadata::SERIALIZER_VERSION         = 1;
   const char* ObjectMetadata::Variable::STRING_SEPARATOR           = "$";

   // Initialize the static variable(s):
   int ObjectMetadata::m_objectCounter = 0;

   SG::unordered_map<std::string, size_t> ObjectMetadata::m_namecount;

   ObjectMetadata::ObjectMetadata()
      : m_variables(), m_name( "" ), m_prefix( "" ),
        m_container( false ) {

   }

   ObjectMetadata::ObjectMetadata( const ObjectMetadata& parent )
      : m_variables( parent.m_variables ), m_name( parent.m_name ),
        m_prefix( parent.m_prefix ), m_container( parent.m_container ) {

   }

   ObjectMetadata& ObjectMetadata::operator= ( const ObjectMetadata& parent )
   {
     if (&parent != this) {
       m_variables = parent.m_variables;
       m_name      = parent.m_name;
       m_prefix    = parent.m_prefix;
       m_container = parent.m_container;
     }

      return *this;
   }

   bool ObjectMetadata::operator== ( const ObjectMetadata& rhs ) const {

      return ( ( name() == rhs.name() ) && ( prefix() == rhs.prefix() ) &&
               ( container() == rhs.container() ) );
   }

   bool ObjectMetadata::operator< ( const ObjectMetadata& rhs ) const {

      if( prefix() != rhs.prefix() ) {
         return ( prefix() < rhs.prefix() );
      } else if( name() != rhs.name() ) {
         return ( name() < rhs.name() );
      } else {
         return ( container() < rhs.container() );
      }
   }

   /**
    * This function can be used to save the metadata about a D3PD variable.
    * Note that this class doesn't implement the full might of an ID3PD
    * object, so no new object is created as a result of this function
    * call. This means that it should always be followed by calling the
    * addVariable(...) function of a proper IAddVariable object.
    */
   StatusCode ObjectMetadata::addVariable( const std::string& name,
                                           const std::type_info& ti,
                                           void*& /*ptr*/,
                                           const std::string& docstring,
                                           const void* /*defval*/ ) {

      // Check that the variable has the correct prefix:
      if( ( m_prefix != "" ) &&
          ( name.find( m_prefix ) != 0 ) ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "ObjectMetadata" )
            << "Specified variable name (" << name << ") doesn't have the "
            << "expected prefix (" << m_prefix << ")";
         return StatusCode::RECOVERABLE;
      }

      // Create a new variable:
      Variable var;

      // Remove the prefix from the variable name:
      var.setName( name.substr( m_prefix.size(), name.npos ) );

      // Check if it's already amongst the existing variables:
      if( m_variables.find( var ) != m_variables.end() ) {
         return StatusCode::SUCCESS;
      }

      // Set the type of the variable:
      var.setType( System::typeinfoName( ti.name() ) );

      // Set whether the variable is a primitive:
      var.setPrimitive( isPrimitive( ti ) );

      // Set the documentation string for the variable:
      var.setDoc( docstring );

      // Remember the variable:
      m_variables.insert( var );

      return StatusCode::SUCCESS;
   }

   /**
    * The object doesn't support dimensioned variables at the moment, like most
    * of the D3PDMaker code doesn't. Still, we might implement something like
    * this later on.
    */
   StatusCode
   ObjectMetadata::addDimensionedVariable( const std::string& /*name*/,
                                           const std::type_info& /*ti*/,
                                           void*& /*ptr*/,
                                           const std::string& /*dim*/,
                                           const std::string& /*docstring*/,
                                           const void* /*defval*/ ) {

      REPORT_MESSAGE_WITH_CONTEXT( MSG::FATAL, "ObjectMetadata" )
         << "addDimensionedVariable(...) not implemented";

      return StatusCode::FAILURE;
   }

   const std::string& ObjectMetadata::name() const {

      return m_name;
   }

   void ObjectMetadata::setName( const std::string& name ) {

      m_name = name;
      return;
   }

   /**
    * When saving the "serialized" version of the object as metadata into a
    * D3PD file, this function should be used to construct the name of the
    * metadata object. The name receives a "random component" to make sure
    * that when a single job uses the same D3PDObject multiple times, possibly
    * with different detail levels, we would save the variable definitions for
    * both into the output file.
    *
    * @returns A unique name, containing the D3PDObject's name
    */
   std::string ObjectMetadata::metadataName() const {

      std::string name = m_name;
      if (name.empty()) {
        ++m_objectCounter;
        name = "D3PDObject" + boost::lexical_cast< std::string >( m_objectCounter );
      }
      return name + "_" + genSuffix (name, RANDOM_NAME_POSTFIX_LENGTH);
   }

   /**
    * This function should be used by the client code to "decode" the name
    * of the original D3PDObject from the name of the metadata object.
    * This way the client code doesn't have to know how this class works
    * internally.
    *
    * @param metaName The name of the metadata object in the D3PD file
    * @returns The name of the original D3PDObject that the metadata object
    *          describes
    */
   std::string ObjectMetadata::objectName( const std::string& metaName ) {

      return metaName.substr( 0, metaName.size() -
                              ( RANDOM_NAME_POSTFIX_LENGTH + 1 ) );
   }

   const std::string& ObjectMetadata::prefix() const {

      return m_prefix;
   }

   void ObjectMetadata::setPrefix( const std::string& prefix ) {

      m_prefix = prefix;
      return;
   }

   bool ObjectMetadata::container() const {

      return m_container;
   }

   void ObjectMetadata::setContainer( bool container ) {

      m_container = container;
      return;
   }

   /**
    * This function translates the contents of the entire object into
    * a single string. The different parts of the configuration string are
    * separated by "@" character. (So variable names and variable descriptions
    * can't have this character in them.)
    *
    * @returns The configuration of this object "serialized" into a string
    */
   std::string ObjectMetadata::toString() const {

      // Save the simple part of the information
      std::string result =
         boost::lexical_cast< std::string >( SERIALIZER_VERSION ) +
         STRING_SEPARATOR + m_prefix +
         STRING_SEPARATOR + ( m_container ? "1" : "0" );

      // Save the serialized version of all the variables:
      std::set< Variable >::const_iterator itr = m_variables.begin();
      std::set< Variable >::const_iterator end = m_variables.end();
      for( ; itr != end; ++itr ) {
         result += STRING_SEPARATOR + itr->toString();
      }

      return result;
   }

   /**
    * This function can be used to read in information from a serialized form.
    * Note that the function doesn't clear the contents of the object. So it
    * can be used to merge information from multiple metadata objects / files.
    *
    * @param data The string that should be decoded
    * @returns <code>StatusCode::SUCCESS</code> if the operation was successful,
    *          <code>StatusCode::FAILURE</code> otherwise
    */
   StatusCode ObjectMetadata::read( const std::string& data ) {

      //
      // Tokenize the string using Boost:
      //
      boost::char_separator< char > separator( STRING_SEPARATOR, "",
                                               boost::keep_empty_tokens );
      boost::tokenizer< boost::char_separator< char > > tokens( data,
                                                                separator );
      boost::tokenizer< boost::char_separator< char > >::const_iterator itr = tokens.begin();
      boost::tokenizer< boost::char_separator< char > >::const_iterator end = tokens.end();

      // Check that we didn't reach the last token yet:
      if( itr == end ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::FATAL, "ObjectMetadata" )
            << "The received data can not be parsed successfully: "
            << data;
         return StatusCode::FAILURE;
      }

      // Check that the metadata was saved using the same version of the
      // serializer code. Later on we might want to introduce backward
      // compatibility, but for now this simple check should be enough.
      if( atoi(itr->c_str()) != SERIALIZER_VERSION ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::FATAL, "ObjectMetadata" )
            << "Version mismatch! The metadata was saved with a different "
            << "serialization version (" << *itr << ") than the code used ("
            << SERIALIZER_VERSION << ")";
         return StatusCode::FAILURE;
      }

      ++itr;

      // Check that we didn't reach the last token yet:
      if( itr == end ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::FATAL, "ObjectMetadata" )
            << "The received data can not be parsed successfully: "
            << data;
         return StatusCode::FAILURE;
      }

      // Extract the prefix from the metadata:
      m_prefix = *itr; ++itr;

      // Check that we didn't reach the last token yet:
      if( itr == end ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::FATAL, "ObjectMetadata" )
            << "The received data can not be parsed successfully: "
            << data;
         return StatusCode::FAILURE;
      }

      // Extract whether this is a container that the variables describe:
      if( *itr == "0" ) {
         m_container = false;
      } else if( *itr == "1" ) {
         m_container = true;
      } else {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::FATAL, "ObjectMetadata" )
            << "The received data can not be parsed successfully: "
            << data;
         return StatusCode::FAILURE;
      }

      ++itr;

      // Finally, let's extract all the variable metadata:
      for( ; itr != end; ++itr ) {
         Variable var;
         CHECK( var.read( *itr ) );
         m_variables.insert( var );
      }

      return StatusCode::SUCCESS;
   }

   void ObjectMetadata::clear() {

      // Clear/reset all the variables:
      m_variables.clear();
      m_name = "";
      m_prefix = "";
      m_container = false;

      return;
   }

   ObjectMetadata& ObjectMetadata::operator+= ( const ObjectMetadata& obj ) {

      return this->merge( obj );
   }

   /**
    * This merging function was implemented in the anticipation that it might
    * come in handy later on. But no guarantees...
    *
    * @param obj The object that should be merged with this one
    * @returns A reference to the current object
    */
   ObjectMetadata& ObjectMetadata::merge( const ObjectMetadata& obj ) {

      // Only objects with the same "name" should be merged:
      if( name() != obj.name() ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::WARNING, "ObjectMetadata" )
            << "Can't merge object with name \"" << obj.name()
            << "\" into another object with name \"" << name() << "\"";
         return *this;
      }

      // Now merge all the variable definitions:
      std::set< Variable >::const_iterator itr = obj.variables().begin();
      std::set< Variable >::const_iterator end = obj.variables().end();
      for( ; itr != end; ++itr ) {
         m_variables.insert( *itr );
      }

      return *this;
   }

   /**
    * @brief Generate a unique suffix for a metadata object name.
    * @param name The metadata object name.
    * @param length The length of the suffix to add.
    *
    * We used to do this by generating a random string.
    * However, this is very bad for regression testing, as adding or removing
    * one variable can then change the names of all subsequent variables.
    * Instead, we generate the suffix based on a count of the number of times
    * we've seen a particular name.
    */
   std::string ObjectMetadata::genSuffix (const std::string& name,
                                          size_t length)
   {
     size_t count = ++m_namecount[name];
     std::ostringstream os;
     os << std::setw(length) << std::setprecision(length) << std::setfill('0')
        << count;
     return os.str();
   }


   ObjectMetadata::Variable::Variable()
      : m_type( "" ), m_name( "" ), m_doc( "" ), m_primitive( true ) {

   }

   const std::string& ObjectMetadata::Variable::type() const {

      return m_type;
   }

   const std::string& ObjectMetadata::Variable::name() const {

      return m_name;
   }

   const std::string& ObjectMetadata::Variable::doc() const {

      return m_doc;
   }

   bool ObjectMetadata::Variable::primitive() const {

      return m_primitive;
   }

   void ObjectMetadata::Variable::setType( const std::string& type ) {

      m_type = type;
      return;
   }

   void ObjectMetadata::Variable::setName( const std::string& name ) {

      m_name = name;
      return;
   }

   void ObjectMetadata::Variable::setDoc( const std::string& doc ) {

      m_doc = doc;
      return;
   }

   void ObjectMetadata::Variable::setPrimitive( bool primitive ) {

      m_primitive = primitive;
      return;
   }

   /**
    * This function translates the contents of the variable into
    * a single string. The different parts of the configuration string are
    * separated by "$" character. (So variable names and variable descriptions
    * can't have this character in them.)
    *
    * @returns The configuration of this variable "serialized" into a string
    */
   std::string ObjectMetadata::Variable::toString() const {

      return m_name + STRING_SEPARATOR + m_type + STRING_SEPARATOR +
         m_doc + STRING_SEPARATOR + ( m_primitive ? "1" : "0" );
   }

   /**
    * This function can be used to read in information from a serialized form.
    * Note that the function is meant to reset all stored properties of the
    * object. (Unlike the function in D3PD::ObjectMetadata!) This is just
    * because merging doesn't make sense for this class.
    *
    * @param data The string that should be decoded
    * @returns <code>StatusCode::SUCCESS</code> if the operation was successful,
    *          <code>StatusCode::FAILURE</code> otherwise
    */
   StatusCode ObjectMetadata::Variable::read( const std::string& data ) {

      //
      // Tokenize the string using Boost:
      //
      boost::char_separator< char > separator( STRING_SEPARATOR, "",
                                               boost::keep_empty_tokens );
      boost::tokenizer< boost::char_separator< char > > tokens( data,
                                                                separator );
      boost::tokenizer< boost::char_separator< char > >::const_iterator itr = tokens.begin();

      // Check that we didn't reach the last token yet:
      if( itr == tokens.end() ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::FATAL, "ObjectMetadata::Variable" )
            << "The received data can not be parsed successfully: "
            << data;
         return StatusCode::FAILURE;
      }

      // Extract the name of the variable:
      m_name = *itr; ++itr;

      // Check that we didn't reach the last token yet:
      if( itr == tokens.end() ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::FATAL, "ObjectMetadata::Variable" )
            << "The received data can not be parsed successfully: "
            << data;
         return StatusCode::FAILURE;
      }

      // Extract the type of the variable:
      m_type = *itr; ++itr;

      // Check that we didn't reach the last token yet:
      if( itr == tokens.end() ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::FATAL, "ObjectMetadata::Variable" )
            << "The received data can not be parsed successfully: "
            << data;
         return StatusCode::FAILURE;
      }

      // Extract the documentation string of the variable:
      m_doc = *itr; ++itr;

      // Check that we didn't reach the last token yet:
      if( itr == tokens.end() ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::FATAL, "ObjectMetadata::Variable" )
            << "The received data can not be parsed successfully: "
            << data;
         return StatusCode::FAILURE;
      }

      // Extract the information of whether this variable is a primitive or not:
      if( *itr == "0" ) {
         m_primitive = false;
      } else if( *itr == "1" ) {
         m_primitive = true;
      } else {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::FATAL, "ObjectMetadata::Variable" )
            << "The received data can not be parsed successfully: "
            << data;
         return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
   }

   bool ObjectMetadata::Variable::operator== ( const ObjectMetadata::Variable& var ) const {

      return ( ( name() == var.name() ) &&
               ( type() == var.type() ) );
   }

   bool ObjectMetadata::Variable::operator< ( const ObjectMetadata::Variable& var ) const {

      if( name() != var.name() ) {
         return ( name() < var.name() );
      } else {
         return ( type() < var.type() );
      }
   }

   const std::set< ObjectMetadata::Variable >& ObjectMetadata::variables() const {

      return m_variables;
   }

   ObjectMetadata operator+ ( const ObjectMetadata& obj1,
                              const ObjectMetadata& obj2 ) {

      ObjectMetadata result( obj1 );
      return result += obj2;
   }

} // namespace D3PD
