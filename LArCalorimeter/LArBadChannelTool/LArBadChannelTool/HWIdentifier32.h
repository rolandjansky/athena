/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//<doc><file>	$Id: HWIdentifier32.h,v 1.3 2004-02-24 13:52:15 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IDENTIFIER_HWIDENTIFIER32_H
# define IDENTIFIER_HWIDENTIFIER32_H

#include "Identifier/Identifier.h"
#include "Identifier/Identifier32.h"

class HWIdentifier32 : public Identifier32 {

public:

    /// Default constructor
    HWIdentifier32 ();

    /// Constructor from value_type
    explicit HWIdentifier32(value_type value);
 
    /// Constructor from Identifier32
    explicit HWIdentifier32(const Identifier32& old);

    /// Constructor from a long value
    explicit HWIdentifier32(unsigned long long value);
 
    /// Constructor from Identifier
    explicit HWIdentifier32(const Identifier& old);
};


inline HWIdentifier32::HWIdentifier32()
    : Identifier32::Identifier32()
{}

inline HWIdentifier32::HWIdentifier32(value_type value)
    : Identifier32::Identifier32(value)
{}

inline HWIdentifier32::HWIdentifier32(const Identifier32& old)
    : Identifier32::Identifier32(old)
{}

inline HWIdentifier32::HWIdentifier32(unsigned long long value)
    : Identifier32::Identifier32()
{
          bool hi = (value >> 32);
          bool lo = (value << 32);
          if (hi && lo) return; // full 64-bit identifier, so return invalid
          //if (hi) m_id = (static_cast<value_type>(value >> 32));
          //else if (lo) m_id = (static_cast<value_type>(value));
          if (hi) {
             const HWIdentifier32 myid(value >> 32);
             this->operator=(myid);
          } else if (lo) {
             const HWIdentifier32 myid(value);
             this->operator=(myid);
          } else {
             const HWIdentifier32 myid(static_cast<value_type>(0));
             this->operator=(myid);
          }
}

inline HWIdentifier32::HWIdentifier32(const Identifier& old)
    : Identifier32::Identifier32(old.get_identifier32().get_compact())
{ }

#endif // IDENTIFIER_HWIDENTIFIER32_H
