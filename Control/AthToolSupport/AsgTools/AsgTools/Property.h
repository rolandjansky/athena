// Dear emacs, this is -*- c++ -*-
// $Id: Property.h 612908 2014-08-21 16:19:03Z krasznaa $
#ifndef ASGTOOLS_PROPERTY_H
#define ASGTOOLS_PROPERTY_H

// System include(s):
#include <string>
#include <vector>

/// Support class for PropertyMgr
///
/// Base class for all property types that can be set on a tool.
///
/// @author David Adams <dladams@bnl.gov>
///
/// $Revision: 612908 $
/// $Date: 2014-08-21 18:19:03 +0200 (Thu, 21 Aug 2014) $
///
class Property {

public:
   /// Property type enumeration
   enum Type {
      UNKNOWNTYPE,
      BOOL,
      INT,
      FLOAT,
      DOUBLE,
      STRING,
      INTVECTOR,
      FLOATVECTOR,
      STRINGVECTOR,
      TOOLHANDLE,
      TOOLHANDLEARRAY
   }; // enum Type

   /// Convenience type declaration for an integer vector
   typedef std::vector< int >         IntVector;
   /// Convenience type declaration for a float vector
   typedef std::vector< float >       FloatVector;
   /// Convenience type declaration for a string vector
   typedef std::vector< std::string > StringVector;

   /// Function returning a user-readable name for a property type
   static const std::string& name( Type type );

   /// Default constructor
   Property();
   /// Constructor with a type
   Property( Type type );
   /// Destructor
   virtual ~Property() {}

   /// Return if this is a valid property.
   /// Must have a known type.
   bool isValid() const;

   /// Return the type index.
   Type type() const;

   /// Return the type name.
   const std::string& typeName() const;

   // Set the value from another property.
   // Returns 0 for success.
   // The implementation here fails with error -1.
   virtual int setFrom( const Property& rhs );

private:
   /// The type of the property
   Type m_type;

}; // class Property

#endif // ASGTOOLS_PROPERTY_H
