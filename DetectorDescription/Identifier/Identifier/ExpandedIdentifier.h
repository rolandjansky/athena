/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDENTIFIER_EXPANDEDIDENTIFIER_H
#define IDENTIFIER_EXPANDEDIDENTIFIER_H

#include <vector>
#include <string>

//-----------------------------------------------
//
//
//  
//  ExpandedIdentifier :
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
//  ExpandedIdentifier ()                         : default constructor
//
//  ExpandedIdentifier (const std::string& text)  : build an identifier from a textual
//                                                  description following the syntax 
//        <value> [ "/" <value> ... ]
//
//  ExpandedIdentifier (const ExpandedIdentifier& other)  : copy constructor
//
//  ExpandedIdentifier (const ExpandedIdentifier& other, 
//                      size_type start)          : subset constructor
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
//  ExpandedIdentifier& operator << (element_type value)   : appends a numeric value to
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
//  ----------------------------------------------------
//
//  Example of how to use an identifier :
//
//  #include <ExpandedIdentifier.h>
//
//  ExpandedIdentifier id;
//
//  id << 125 << 236 << 306 << 2222;
//
//  for (size_type i = 0; i < id.fields (); ++i)
//    {
//      cout << "id[" << i << "] = " << id[i] << endl;
//    }
//
//-----------------------------------------------
class ExpandedIdentifier
{
public:


    //----------------------------------------------------------------
    // Define public typedefs
    //----------------------------------------------------------------
  typedef ExpandedIdentifier 		id_type;
  typedef int  				element_type;
  typedef std::vector<element_type> 	element_vector;
  typedef std::vector<element_type>::size_type 	size_type;

  typedef enum
    {
      max_value = 0x3FFFFFFF
    } max_value_type;

    //----------------------------------------------------------------
    // Defaulted members
    //----------------------------------------------------------------
  ExpandedIdentifier() = default;
  ExpandedIdentifier(const ExpandedIdentifier& other) = default;
  ExpandedIdentifier(ExpandedIdentifier&& other) = default;
  ExpandedIdentifier& operator=(const ExpandedIdentifier& other) = default;
  ExpandedIdentifier& operator=(ExpandedIdentifier&& other) = default;
  ~ExpandedIdentifier() = default;

    //----------------------------------------------------------------
    // Constructor from a subset of another ExpandedIdentifier
    //----------------------------------------------------------------
  ExpandedIdentifier (const ExpandedIdentifier& other, size_type start);

    //----------------------------------------------------------------
    // Constructor from a textual description
    //----------------------------------------------------------------
  ExpandedIdentifier (const std::string& text);

    //----------------------------------------------------------------
    // Modifications
    //----------------------------------------------------------------

    //----------------------------------------------------------------
    // Append a value into a new field.
    //----------------------------------------------------------------
  void add (element_type value);
  ExpandedIdentifier& operator << (element_type value);
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
  
  int operator == (const ExpandedIdentifier& other) const;
  int operator != (const ExpandedIdentifier& other) const;
  int operator < (const ExpandedIdentifier& other) const;
  int operator > (const ExpandedIdentifier& other) const;
  int prefix_less (const ExpandedIdentifier& other) const;

  /**
   *    Test if the shorter of two ids is identical
   *    to the equivalent sub-id extracted from the longer
   */
  int match (const ExpandedIdentifier& other) const;

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
};
//-----------------------------------------------


#include "Identifier/ExpandedIdentifier.icc"

#endif
