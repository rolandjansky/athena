/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Identifier package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: IdentifierHash.h,v 1.4 2003-11-26 12:28:21 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IDENTIFIER_IDENTIFIERHASH_H
# define IDENTIFIER_IDENTIFIERHASH_H

#include "GaudiKernel/MsgStream.h"


/**
 **
 **  ---------------------------------------------------
 **  
 **  IdentifierHash :
 **  	
 **  This is a "hash" representation of an Identifier. This encodes a
 **  32 bit index which can be used to look-up "Identifiable"s stored
 **  in a simple vector. It is intended to be a continuous hash,
 **  i.e. it runs from 0 to N-1, where there are N different possible
 **  values for an Identifier(32) within a specific context.
 **
 **  IdentifierHashes are created by default in an invalid state which
 **  can be checked with "is_valid" method. This allows some error
 **  checking.
 **  
 **  ---------------------------------------------------
 */  
class IdentifierHash
{
public:


    ///----------------------------------------------------------------
    /// Define public typedefs
    ///----------------------------------------------------------------
    typedef unsigned int	value_type;

    ///----------------------------------------------------------------
    /// Constructors
    ///----------------------------------------------------------------

    /// Default methods
    IdentifierHash () = default;
    IdentifierHash (const IdentifierHash& other) =default;
    IdentifierHash (IdentifierHash&& other) =default;
    IdentifierHash& operator=(const IdentifierHash& other) = default;
    IdentifierHash& operator=(IdentifierHash&& other) = default;
    ~IdentifierHash () = default;

    /// Initialization with value
    IdentifierHash (value_type value);

    ///----------------------------------------------------------------
    /// Accessors
    ///----------------------------------------------------------------

    /// Get the value 
    operator unsigned int	(void) const;
    unsigned int value          (void) const;

    ///----------------------------------------------------------------
    /// Error management
    ///----------------------------------------------------------------

    /// Check if id is in a valid state
    bool is_valid () const;

    ///----------------------------------------------------------------
    /// Modifications
    ///----------------------------------------------------------------

    /// Assignment operators
    IdentifierHash& operator = (value_type value);
    IdentifierHash& operator += (unsigned int value);
    IdentifierHash& operator -= (unsigned int value);

private:

    typedef enum {
        max_value = 0xFFFFFFFF
    } max_value_type;

    //----------------------------------------------------------------
    // The actual identifier data.
    //----------------------------------------------------------------
    value_type m_value = max_value;
};
//-----------------------------------------------



// Define a hash functional

namespace std {
template<>
struct hash<IdentifierHash>
{
  size_t operator()(const IdentifierHash& id) const
  {
    return static_cast<size_t>(id.value());
  }
};
}

#include "Identifier/IdentifierHash.icc"
#endif // IDENTIFIER_IDENTIFIERHASH_H
