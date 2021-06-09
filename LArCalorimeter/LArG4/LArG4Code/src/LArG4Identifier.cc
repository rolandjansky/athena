/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "LArG4Code/LArG4Identifier.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <iostream>
#include <iomanip>


#ifndef LARG4NOROOT
// This class can be used with ROOT's dictionary generation, built-in
// I/O, and RTTI (Run-Time Type Identification).  To enable this
// functionality, this class must be derived from class TObject, all
// its members must also be derived from TObject, and the following
// macro (defined in TObject.h) must be included.
ClassImp(LArG4Identifier)
#endif


//-----------------------------------------------
static void show_vector (const LArG4Identifier::element_vector& v)
{
  LArG4Identifier::element_vector::const_iterator it;
  bool first = true;

  std::cout << "[";
  for (it = v.begin (); it != v.end (); ++it)
    {
      if (first) first = false;
      else std::cout << ".";

      LArG4Identifier::element_type value = *it;
      std::cout << value;

        // if (value >= 0) std::cout << value;
        // else std::cout << "*";
    }
  std::cout << "]";
}

  // Constructors
//-----------------------------------------------
LArG4Identifier::LArG4Identifier ()
{
}

//-----------------------------------------------
LArG4Identifier::LArG4Identifier (const LArG4Identifier& other) :
#ifndef LARG4NOROOT
  TObject(),
#endif
    m_fields(other.m_fields)

{
}

//-----------------------------------------------
LArG4Identifier::LArG4Identifier (const LArG4Identifier& other, size_type start)
{
  if (start < other.fields ())
    {
      element_vector::const_iterator it = other.m_fields.begin ();
      it += start;

      m_fields.insert (m_fields.end (),
                       it, 
                       other.m_fields.end ());
    }
}

//-----------------------------------------------
LArG4Identifier::LArG4Identifier (const std::string& text)
{
  set (text);
}

  // Modifications

//-----------------------------------------------
LArG4Identifier& LArG4Identifier::operator=(const LArG4Identifier& other)
{
 if(this != &other)
   {
     m_fields = other.m_fields;
   }
 return *this;
}

//-----------------------------------------------
void LArG4Identifier::add (element_type value)
{
  m_fields.push_back (value);
}

//-----------------------------------------------
LArG4Identifier& LArG4Identifier::operator << (element_type value)
{
  m_fields.push_back (value);

  return (*this);
}

//-----------------------------------------------
LArG4Identifier::element_type & LArG4Identifier::operator [] (size_type index)
{
  return m_fields.at(index);
}

//-----------------------------------------------
void LArG4Identifier::set (const std::string& text)
{
  clear ();
  if (text.size () == 0) return;
  const char* ctext = text.c_str ();

  for (;;)
    {

      const char* sep;

      sep = strchr (ctext, '/');

      int value = 0;
      //int items;

      /*items =*/ sscanf (ctext, "%d", &value);

      add ((element_type) value);

      if (sep == 0) break;
      
      ctext = sep + 1;
    }
}

//-----------------------------------------------
void LArG4Identifier::clear ()
{
  m_fields.clear ();
}

  // Accessors
//-----------------------------------------------
LArG4Identifier::element_type LArG4Identifier::operator [] (size_type index) const
{
  return m_fields.at(index);
}

//-----------------------------------------------
LArG4Identifier::size_type LArG4Identifier::fields () const
{
  return (m_fields.size ());
}

  // Comparison operators

//----------------------------------------------------------------
int LArG4Identifier::operator == (const LArG4Identifier& other) const
{
  const LArG4Identifier& me = *this;
  const size_type my_fields = fields ();
  const size_type other_fields = other.fields ();
  
  if (my_fields != other_fields) return (0);
  
  size_type field = 0;
  for (; field < my_fields; ++field) 
    {
      if (me[field] != other[field]) return (0);
    }

  return (1);
}

//----------------------------------------------------------------
int LArG4Identifier::operator != (const LArG4Identifier& other) const
{
  const LArG4Identifier& me = *this;

  return (!(me == other));
}

//-----------------------------------------------
int LArG4Identifier::operator < (const LArG4Identifier& other) const
{
  const LArG4Identifier& me = *this;
  const size_type my_fields = fields ();
  const size_type other_fields = other.fields ();

  size_type field = 0;
  for (;;)
    {
      if ((field == my_fields) ||
          (field == other_fields))
        {
            // Someone has run out of fields. And up to now my_id ==
            // other_id. If the lengths are different, the following
            // then defines the "shorter" one to be "less than". If
            // the lengths are the same, then the two are NOT "less
            // than".
          return (my_fields < other_fields);
        }

      element_type my_field = me[field];
      element_type other_field = other[field];

      if (my_field < other_field) return (1);
      if (my_field > other_field) return (0);

      field++;
    }

  return (0);
}

//-----------------------------------------------
int LArG4Identifier::operator > (const LArG4Identifier& other) const
{
  const LArG4Identifier& me = *this;

  return (other < me);
}

//----------------------------------------------------------------
int LArG4Identifier::match (const LArG4Identifier& other) const
{
  const LArG4Identifier& me = *this;
  const size_type my_fields = fields ();
  const size_type other_fields = other.fields ();

  const size_type fs = (my_fields < other_fields) ? my_fields : other_fields;
  
  for (size_type field = 0; field < fs; ++field) 
    {
      if (me[field] != other[field]) return (0);
    }

  return (1);
}

//-----------------------------------------------
int LArG4Identifier::prefix_less (const LArG4Identifier& other) const
{
  const LArG4Identifier& me = *this;
  const size_type my_fields = fields ();
  const size_type other_fields = other.fields ();
  
    //
    // Scan fields up to the less common field number
    //
  size_type field = 0;
  while ((field < my_fields) && 
         (field < other_fields))
    {
      element_type my_field = me[field];
      element_type other_field = other[field];
      
      if (my_field < other_field) return (1);
      if (my_field > other_field) return (0);
      
      field++;
    }
  
  return (0);
}

//-----------------------------------------------
LArG4Identifier::operator std::string () const
{
  std::string result;
  char temp[20];

  size_type my_fields = m_fields.size ();

  if (my_fields == 0) return (result);

    // print fields one by one.
  for (size_type field_number = 0; field_number < my_fields; field_number++)
    {
      element_type value = m_fields[field_number];

      if (field_number > 0) result += "/";

      sprintf (temp, "%d", value);
      result += temp;
    }

  return (result);
}

//-----------------------------------------------
void LArG4Identifier::show () const
{
  show_vector (m_fields);
}

