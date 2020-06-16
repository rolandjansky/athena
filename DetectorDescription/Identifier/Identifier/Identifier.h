/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDENTIFIER_IDENTIFIER_H
#define IDENTIFIER_IDENTIFIER_H


#define IDENTIFIER_TYPE unsigned long long
#define IDENTIFIER_DIFF_TYPE long long
#define IDENTIFIER_PCODE "ll"

#include "GaudiKernel/MsgStream.h"
#include "Identifier/Identifier32.h"
#include <boost/io/ios_state.hpp>
#include <vector>
#include <string>
#include <iostream>
/**
 **-----------------------------------------------
 **
 **  Identifier is a simple type-safe 64 bit unsigned integer. An
 **  Identifier relies on other classes - IdHelpers - to encode and
 **  decode its information.
 **  
 **  The default constructor created an Identifier an invalid state
 **  which can be check with the "is_valid" method to allow some error
 **  checking.
 **  
 **-----------------------------------------------
 */
class Identifier
{
public:

    ///----------------------------------------------------------------
    /// Define public typedefs
    ///----------------------------------------------------------------
    typedef Identifier                  id_type;
    typedef IDENTIFIER_TYPE             value_type;
    typedef IDENTIFIER_DIFF_TYPE        diff_type;
    typedef IDENTIFIER_TYPE             size_type;

    typedef enum
    {
        NBITS = sizeof(value_type) * 8, // bits per byte
        MAX_BIT = (static_cast<value_type>(1) << (NBITS - 1)),
        ALL_BITS = ~(static_cast<value_type>(0))
    } bit_defs;

    ///----------------------------------------------------------------
    /// Constructors
    ///----------------------------------------------------------------

    /// Default constructor
    Identifier() = default;
    /// Default Copy constructor
    Identifier(const Identifier& other) = default;
    /// Default Move constructor
    Identifier(Identifier&& other) = default;
    /// Default Assignment operators
    Identifier& operator=(const Identifier& old) = default;
    ///  Default Move Assignment operator
    Identifier& operator=(Identifier&& old) = default;
    /// Default dtor
    ~Identifier() = default;

    ///Additional ctors
    /// Constructor from value_type
    explicit Identifier (value_type value);

    /// Constructor from Identifier32
    Identifier (const Identifier32& other);

    /// Constructor from 32-bit value_type and int
    /// (to avoid common implicit conversions)
    explicit Identifier (Identifier32::value_type value);
    explicit Identifier (int value);

 

    ///----------------------------------------------------------------
    /// Modifications
    ///----------------------------------------------------------------
   
    /// Assignment operators overloads
    Identifier& operator = (const Identifier32& old);
    Identifier& operator = (value_type value);
    /// Assignment to avoid common implicit conversions and shift properly
    Identifier& operator = (Identifier32::value_type value);
    Identifier& operator = (int value);

private:
    /// Bitwise operations 
    Identifier& operator |= (value_type value);
    Identifier& operator &= (value_type value);

public:

    /// build from a string form - hexadecimal
    void set (const std::string& id);

    /// Reset to invalid state
    void clear ();

    /// Set literal value
    Identifier& set_literal(value_type value);

    ///----------------------------------------------------------------
    /// Accessors
    ///----------------------------------------------------------------

    /// Get the 32-bit version Identifier, will be invalid if >32 bits
    /// needed
    Identifier32 get_identifier32  (void) const;

    /// Get the compact id
    value_type   get_compact  (void) const;

    /// A get_compact functional for use in STL algorithms
    class get_compact_func
    {
    public:
        value_type operator() (const Identifier& id)
            {
                return id.get_compact();
            }
    };

    ///----------------------------------------------------------------
    /// Comparison operators
    ///----------------------------------------------------------------
  
    bool operator ==    (const Identifier& other) const;
    bool operator !=    (const Identifier& other) const;
    bool operator <     (const Identifier& other) const;
    bool operator >     (const Identifier& other) const;
    bool operator <=    (const Identifier& other) const;
    bool operator >=    (const Identifier& other) const;

    /// Comparison operators with value_type.
    /// This is a hack, only because GeoAdaptors/GeoMuonHits wants to
    /// to compare explicitly with 0 as a test of whether the identifier
    /// has been constructed properly.  But is_valid() here compares
    /// with max_value, not 0, since presumably it is possible to have
    /// a zero value - just not in muons.
    bool operator ==    (value_type other) const;
    bool operator !=    (value_type other) const;
    bool operator ==    (Identifier32::value_type other) const;
    bool operator ==    (int other) const;
    bool operator !=    (Identifier32::value_type other) const;
    bool operator !=    (int other) const;

    ///----------------------------------------------------------------
    /// Error management
    ///----------------------------------------------------------------

    /// Check if id is in a valid state
    bool is_valid () const;

    ///----------------------------------------------------------------
    /// Utilities
    ///----------------------------------------------------------------

    /// Provide a string form of the identifier - hexadecimal
    std::string  getString() const;

    /// Print out in hex form
    void show () const;

private:

    /// extract field from identifier (shift first, then mask)
    value_type extract(size_type shift, size_type mask) const;

    /// extract field(s) by masking first, then shifting
    value_type mask_shift(value_type mask, size_type shift) const;

    /// extract field, no mask
    value_type extract(size_type shift) const;

    // allow IdDict access to the following private methods
    friend class IdDictDictionary;
    friend class IdDictFieldImplementation;
    friend class AtlasDetectorID;
    friend class PixelID;
    
    typedef enum {
        //max_value = 0xFFFFFFFFFFFFFFFFULL
        max_value = ~(static_cast<value_type>(0))
    } max_value_type;

    //----------------------------------------------------------------
    // The compact identifier data.
    //----------------------------------------------------------------
    value_type m_id = max_value;

};
//-----------------------------------------------


// Define a hash functional

namespace std {
template<>
struct hash<Identifier>
{
  size_t operator()(const Identifier& id) const
  {
    return static_cast<size_t>(id.get_compact());
  }
};
}

#include "Identifier/Identifier.icc"

#endif
