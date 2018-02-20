// Dear emacs, this is -*- c++ -*-
// $Id: Property.h 802972 2017-04-15 18:13:17Z krumnack $
#ifndef ASGTOOLS_PROPERTY_H
#define ASGTOOLS_PROPERTY_H

// System include(s):
#include <string>
#include <vector>

class StatusCode;

/// Support class for PropertyMgr
///
/// Base class for all property types that can be set on a tool.
///
/// @author David Adams <dladams@bnl.gov>
///
/// $Revision: 802972 $
/// $Date: 2017-04-15 20:13:17 +0200 (Sat, 15 Apr 2017) $
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


  /// \brief get the property as a string
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   no string conversion available
  virtual StatusCode getString (std::string& result) const;

  /// \brief get the property as a string I can pas to \ref setString
  ///
  /// the main difference to \ref getString is that this will *not*
  /// quote strings, i.e. it will assume strings are already properly
  /// quoted if they need to be and won't be otherwise.  otherwise
  /// there will be an issue when setting non-strings from strings
  /// the user provided
  ///
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   no string conversion available
  virtual StatusCode getCastString (std::string& result) const;

  /// \brief set the property from a string
  /// \par Guarantee
  ///   basic
  /// \par Failures
  ///   no string conversion available\n
  ///   format errors
  virtual StatusCode setString (const std::string& value);


private:
   /// The type of the property
   Type m_type;

}; // class Property

#endif // ASGTOOLS_PROPERTY_H
