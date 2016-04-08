/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4Identifier.hh
// 01-Jul-2002 Bill Seligman

// This is code copied (dare I use the word stolen?) from the Athena
// Identifier class, as implemented by RD Schaffer and Christian
// Arnault.  It was slightly modified by Bill Seligman for ROOT/G4
// compatibility.

// If you're an Athena user, you're probably wondering: why steal the
// code?  Why not just include the files from the Athena libraries?
// The answer is that I still want the LArG4Sim application to run in
// stand-alone mode, without any access to the Athena libraries.  If
// this ever becomes an Athena-only application, we can make the
// switch.

#ifndef __LArG4Identifier_h__
#define __LArG4Identifier_h__

//  LArG4Identifier :
//  
//  Stores a set of numbers.
//  
//  ---------------------------------------------------
//  
//  ------------------------
//  Possible operations :
//  ------------------------
//
//  ------------------------
//  Constructors:
//  ------------------------
//
//  LArG4Identifier ()                         : default constructor
//
//  LArG4Identifier (const std::string& text)  : build an identifier from a textual
//                                          description following the syntax 
//        <value> [ "/" <value> ... ]
//
//  LArG4Identifier (const LArG4Identifier& other)  : copy constructor
//
//  LArG4Identifier (const LArG4Identifier& other, 
//              size_type start)          : subset constructor
//
//  ------------------------
//  Initialisations:
//  ------------------------
//
//  void clear ()                         : clears up the identifier
//
//  void set (const std::string& text)    : set from a textual description
//
//  ------------------------
//  Modifications:
//  ------------------------
//
//  void add (element_type value)         : appends a numeric value to
//                                          an identifier (adds a field).
//
//  LArG4Identifier& operator << (element_type value)   : appends a numeric value to
//                                          an identifier (adds a field).
//
//  ------------------------
//  Accessors:
//  ------------------------
//
//  size_type fields ()                   : returns the number of fields 
//                                          currently stored into the 
//                                          identifier.
//
//  element_type operator [] (size_type field) : gets the value stored at the
//                                          specified field number.
//
//  ------------------------
//  Comparison operators:
//  ------------------------
//
//  operator < (id_type& other)          : absolute comparison, 
//
//                                         e.g. :
//
//                                       /1/2 < /1/3
//                                       /1   < /1/3
//                                       /1 is implicitly /1/0...
//  
//  prefix_less (id_type& other)         : comparison on the equal length 
//                                         prefix parts of two ids, 
//
//                                         e.g. :
//
//                                       /1/2 < /1/3,
//                                          but now 
//                                       /1 == /1/3  
//
//
//  error_code last_error ()             : returns the last error code 
//                                         produced by the most recent 
//                                         identifier operation. The possible
//                                         values are :
//
//                                             none
//                                             bad_parameter
//                                             field_not_found
//
//  const std::string last_error_text () : returns a text describing the 
//                                         last error.
//
//  ----------------------------------------------------
//
//  Example of how to use an identifier :
//
//  #include <LArG4Identifier.h>
//
//  LArG4Identifier id;
//
//  id << 125 << 236 << 306 << 2222;
//  if (id.last_error () != LArG4Identifier::none)
//    {
//      cout << "Error : " << id.last_error_text () << endl;
//    }
//
//  for (size_type i = 0; i < id.fields (); ++i)
//    {
//      cout << "id[" << i << "] = " << id[i] << endl;
//    }
//
//-----------------------------------------------

#include <vector>
#include <string>
#include <limits.h>

class LArG4Identifier
{
public:


    //----------------------------------------------------------------
    // Define public typedefs
    //----------------------------------------------------------------
  typedef short  			element_type;      // WGS
  typedef std::vector<element_type> 	element_vector;
  typedef std::vector<element_type>::size_type 	size_type;

  typedef enum
    {
      max_value = SHRT_MAX
    } max_value_type;

    //----------------------------------------------------------------
    // Possible errors that may occur in operations.
    //----------------------------------------------------------------
  typedef enum
    {
      none,                 // success
      bad_parameter,        // bad value for any method parameter
      field_not_found,      // the field number is not found
      
      errors,
      get
    } error_code;

    //----------------------------------------------------------------
    // Constructors
    //----------------------------------------------------------------

    //----------------------------------------------------------------
    // Default constructor
    //----------------------------------------------------------------
  LArG4Identifier ();

    //----------------------------------------------------------------
    // Copy constructor
    //----------------------------------------------------------------
  LArG4Identifier (const LArG4Identifier& other);

    //----------------------------------------------------------------
    // Constructor from a subset of another LArG4Identifier
    //----------------------------------------------------------------
  LArG4Identifier (const LArG4Identifier& other, size_type start);

    //----------------------------------------------------------------
    // Constructor from a textual description
    //----------------------------------------------------------------
  LArG4Identifier (const std::string& text);

  virtual ~LArG4Identifier() {};

    //----------------------------------------------------------------
    // Modifications
    //----------------------------------------------------------------
LArG4Identifier& operator=(const LArG4Identifier&);//coverity issue fix.

    //----------------------------------------------------------------
    // Append a value into a new field.
    //----------------------------------------------------------------
  void add (element_type value);
  LArG4Identifier& operator << (element_type value);
  element_type& operator [] (size_type index);

    //----------------------------------------------------------------
    // build from a textual description
    //----------------------------------------------------------------
  void set (const std::string& text);

    //----------------------------------------------------------------
    // Erase all fields.
    // All previously stored data is lost.
    //----------------------------------------------------------------
  void clear ();

    //----------------------------------------------------------------
    // Accessors
    //----------------------------------------------------------------

    //----------------------------------------------------------------
    // Get the value stored into the specified field.
    //----------------------------------------------------------------
  element_type operator [] (size_type index) const;

    //----------------------------------------------------------------
    // Count the number of fields.
    //----------------------------------------------------------------
  size_type fields () const;

    //----------------------------------------------------------------
    // Comparison operators
    //----------------------------------------------------------------
  
  int operator == (const LArG4Identifier& other) const;
  int operator != (const LArG4Identifier& other) const;
  int operator < (const LArG4Identifier& other) const;
  int operator > (const LArG4Identifier& other) const;
  int prefix_less (const LArG4Identifier& other) const;

  /**
   *    Test if the shorter of two ids is identical
   *    to the equivalent sub-id extracted from the longer
   */
  int match (const LArG4Identifier& other) const;

    //----------------------------------------------------------------
    // Error management
    //----------------------------------------------------------------

    //----------------------------------------------------------------
    // Return the error produced in the last operation
    // The value identifier::none represents the successful condition.
    //----------------------------------------------------------------
  error_code last_error () const;

    //----------------------------------------------------------------
    // Return a textual equivalent to the error produced
    // in the last operation
    //----------------------------------------------------------------
  const std::string last_error_text () const;

    //----------------------------------------------------------------
    // Utilities
    //----------------------------------------------------------------

    //----------------------------------------------------------------
    // Send a textual representation of the identifier using the input format
    //----------------------------------------------------------------
  operator std::string () const;

  void show () const;

private:


    //----------------------------------------------------------------
    // The actual identifier data.
    //----------------------------------------------------------------
  element_vector m_fields;

    //----------------------------------------------------------------
    // Maintains the last error code (shared by all objects).
    //----------------------------------------------------------------
  error_code set_last_error (error_code code = get) const;


};

#endif // __LArG4Identifier_h__
