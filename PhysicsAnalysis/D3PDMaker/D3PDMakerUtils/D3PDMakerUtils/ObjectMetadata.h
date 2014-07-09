// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ObjectMetadata.h 475045 2011-12-26 18:49:56Z ssnyder $
#ifndef D3PDMAKERUTILS_OBJECTMETADATA_H
#define D3PDMAKERUTILS_OBJECTMETADATA_H

// STL include(s):
#include <string>
#include <set>

// D3PD include(s):
#include "D3PDMakerInterfaces/IAddVariable.h"

#include "CxxUtils/unordered_map.h"

namespace D3PD {

   /**
    *  @short D3PD variable metadata handling class
    *
    *         This class is used to store information in D3PD files about the
    *         variables that were created using a single D3PDObject in the job.
    *         The created metadata can be used to create D3PDReader classes
    *         later on, independent of the storage technology and the language
    *         used.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 475045 $
    * $Date: 2011-12-26 19:49:56 +0100 (Mon, 26 Dec 2011) $
    */
   class ObjectMetadata : public IAddVariable {

   public:
      /// Default constructor
      ObjectMetadata();
      /// Copy constructor
      ObjectMetadata( const ObjectMetadata& parent );

      /// Assignment operator
      ObjectMetadata& operator= ( const ObjectMetadata& parent );
      /// Equality operator
      bool operator== ( const ObjectMetadata& rhs ) const;
      /// Operator needed to use such objects in ordered STL containers
      bool operator<  ( const ObjectMetadata& rhs ) const;

      /// Make the template implementation from @c IAddVariable visible
      using IAddVariable::addVariable;
      using IAddVariable::addDimensionedVariable;

      ////////////////////////////////////////////////////////////////////////
      //
      //               Functions inherited from IAddVariable

      virtual StatusCode addVariable( const std::string& name,
                                      const std::type_info& ti,
                                      void*& ptr,
                                      const std::string& docstring = "",
                                      const void* defval = 0 );

      virtual StatusCode addDimensionedVariable( const std::string& name,
                                                 const std::type_info& ti,
                                                 void*& ptr,
                                                 const std::string& dim,
                                                 const std::string& docstring = "",
                                                 const void* defval = 0 );

      //
      ////////////////////////////////////////////////////////////////////////

      /// Get the name of the D3PDObject that this object describes
      const std::string& name() const;
      /// Set the name of the D3PDObject that this object describes
      void setName( const std::string& name );

      /// Get the name for the metadata object that should be created
      std::string metadataName() const;
      /// Get the D3PDObject's name from the name of the metadata object
      static std::string objectName( const std::string& metaName );

      /// Get the prefix given to variables in this D3PDObject
      const std::string& prefix() const;
      /// Set the prefix given to variables in this D3PDObject
      void setPrefix( const std::string& prefix );

      /// Get whether the D3PDObject describes a container or not
      bool container() const;
      /// Set whether the D3PDObject describes a container or not
      void setContainer( bool container );

      /// Function "serializing" the stored information into a string
      std::string toString() const;
      /// Function "de-serializing" the stored information from a string
      StatusCode read( const std::string& data );

      /// Function clearing the object
      void clear();

      /// Operator merging the contents of two objects
      ObjectMetadata& operator+= ( const ObjectMetadata& obj );
      /// Function merging the contents of two objects
      ObjectMetadata& merge( const ObjectMetadata& obj );

      /// Length of the random string appended to the object name
      static const size_t RANDOM_NAME_POSTFIX_LENGTH;
      /// Character separating parts of the object's metadata
      static const char* STRING_SEPARATOR;
      /// "Version number" of the serialized information
      /**
       * While I don't plan to change this class any time soon, we should
       * be able to clearly decide later on if the format of the metadata
       * serialization changed, which version was used to write the
       * metadata found in a given file.
       */
      static const unsigned int SERIALIZER_VERSION;

      /**
       *  @short Internal class keeping track of a single variable
       *
       *         This class is used internally by the class to keep all the
       *         important properties of a D3PD variable. It is mainly used
       *         by the D3PDReader project to store and retrieve information
       *         about the D3PDObject-s that produced a D3PD.
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 475045 $
       * $Date: 2011-12-26 19:49:56 +0100 (Mon, 26 Dec 2011) $
       */
      class Variable {
      public:
         /// Default constructor
         Variable();
         /// Type name of the variable
         const std::string& type() const;
         /// Name of the variable without its prefix
         const std::string& name() const;
         /// Description of the variable
         const std::string& doc() const;
         /// Flag showing whether the variable is a primitive
         bool primitive() const;
         /// Set the type name of the variable
         void setType( const std::string& type );
         /// Set the name of the variable
         void setName( const std::string& name );
         /// Set the description of the variable
         void setDoc( const std::string& doc );
         /// Set whether the variable is a primitive
         void setPrimitive( bool primitive );
         /// Function "serializing" the variable information into a string
         std::string toString() const;
         /// Read data from a "serialized" string
         StatusCode read( const std::string& data );
         /// Operator checking the equivalence of two variables
         bool operator== ( const Variable& var ) const;
         /// Operator needed to use such objects in STL containers
         bool operator< ( const Variable& var ) const;
         /// Character separating parts of the variable's metadata
         static const char* STRING_SEPARATOR;
      private:
         std::string m_type; ///< Full type name of the variable
         std::string m_name; ///< Name of the variable without its prefix
         std::string m_doc; ///< Description of the variable
         bool m_primitive; ///< Flag showing whether the variable is a primitive
      }; // class Variable

      /// Function for accessing all the variables of the D3PDObject
      const std::set< Variable >& variables() const;

   protected:
      std::set< Variable > m_variables; ///< The list of variables created by a D3PDObject
      std::string m_name; ///< Name of the D3PDObject that this object describes
      std::string m_prefix; ///< Prefix used by the D3PDObject
      bool m_container; ///< The D3PDObject describes a container

      static int m_objectCounter; ///< Variable used to give names to unnamed D3PDObject-s
      /// Count of the number of times a given name was used, 
      /// in order to assign them a unique suffix.
      static SG::unordered_map<std::string, size_t> m_namecount;

   private:
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
     static std::string genSuffix (const std::string& name, size_t length);

   }; // class ObjectMetadata

   /// Operator for creating the sum of two metadata objects
   ObjectMetadata operator+ ( const ObjectMetadata& obj1,
                              const ObjectMetadata& obj2 );

} // namespace D3PD

#endif // D3PDMAKERUTILS_OBJECTMETADATA_H
