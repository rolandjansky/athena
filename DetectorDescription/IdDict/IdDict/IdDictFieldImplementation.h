/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict package
 ***************************************************************************/

//<doc><file>	$Id: IdDictFieldImplementation.h,v 1.2 2007-11-08 21:21:47 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IDDICT_IDDICTFIELDIMPLEMENTATION_H
# define IDDICT_IDDICTFIELDIMPLEMENTATION_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "Identifier/Identifier.h" 
#include "Identifier/Range.h" 

class IdDictRange; 

/**   IdDictFieldImplementation is used to capture the specification
 **   of a single field of an Identifier. This class is primarily used
 **   to extract or set the field of an identifier:
 **
 **   int         unpack        (Identifier id) const;  // access to field value
 **   size_type   unpackToIndex (Identifier id) const;  // access to field index, i.e. from 0
 **   void        pack          (int value, Identifier& id) const; 
 **   void        reset         (Identifier& id) const; // reset field bits 
 **
 **   This class is normally filled during the parsing of the
 **   identifier dictionary. The parsing uses the set_ored_field
 **   method where the the internal variables are set to optimize the
 **   calculations. One may also build a new object by simply using:
 **
 **   void  set_bits         (size_type bits, size_type bits_offset);
 **
 **   This as well sets all internal variables.
 **
 **   The meaning of the varibles is:
 **
 **   size_type           bits()         - number of bits for this field
 **   size_type           bits_offset()  - bits offset relative to 1st field 
 **   size_type           mask()         - mask: (xxx >> shift) & mask
 **   value_type          zeroing_mask() - used to reset field to 0
 **   size_type           shift()        - shift: (xxx >> shift) & mask
 **   bool                decode_index() - true == need to lookup value
 **
 **   // Primarily for internal use:
 **   const IdDictRange*  range()        - points to the generating IdDictRange
 **   Range::field        field()   	 - derived from IdDictRange
 **   Range::field        ored_field()   - "expanded" field to account
 **                                        for other overlapping fields
 */


class IdDictFieldImplementation 
{ 
public: 

    typedef Identifier::value_type  value_type;
    typedef Identifier::size_type   size_type;

    typedef enum 
    { 
        NBITS   = sizeof(value_type) * 8, // bits per byte
        MAX_BIT = (static_cast<value_type>(1) << (NBITS - 1)),
        ALL_BITS  = ~(static_cast<value_type>(0))
    } bit_defs; 

    IdDictFieldImplementation();

    /// Identifier manipulation methods
    int         unpack        (Identifier id) const;  // access to field value
    size_type   unpackToIndex (Identifier id) const;  // access to field index, i.e. from 0
    void        pack          (int value, Identifier& id) const; 
    void        reset         (Identifier& id) const; // reset field bits 

    /// Accessors
    const IdDictRange*  range()        const; 
    const Range::field& field()        const; 
    const Range::field& ored_field()   const; 
    size_type           bits()         const; 
    size_type           bits_offset()  const; 
    size_type           mask()         const; 
    value_type          zeroing_mask() const; 
    size_type           shift()        const; 
    bool                decode_index() const;

    /// Modifiers
    void                set_range        (const IdDictRange* range);
    void                set_field        (const Range::field&  field);
    void                set_ored_field   (const Range::field& ored_field);
    void                set_bits_offset  (size_type bits_offset);
    void                set_bits         (size_type bits, size_type bits_offset);
    void                set_decode_index (bool decode_index);
    void                optimize         (void);

    // Printout
    void                show             (void) const;
    std::string         show_to_string   (void) const;
    
private:
    
    // Initialize derived fields
    void  initialize(); 
    
    const IdDictRange*  m_range; 
    Range::field        m_field; 
    Range::field        m_ored_field; 
    size_type           m_bits; 
    size_type           m_bits_offset; 
    size_type           m_mask; 
    value_type          m_zeroing_mask; 
    size_type           m_shift; 
    bool                m_decode_index;
    
}; 
 

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

//-----------------------------------------------------------------
inline IdDictFieldImplementation::IdDictFieldImplementation () 
//-----------------------------------------------------------------
    :
    m_range(0), 
    m_field(0), 
    m_ored_field(0), 
    m_bits(0), 
    m_bits_offset(0), 
    m_mask(0), 
    m_zeroing_mask(0), 
    m_shift(0), 
    m_decode_index(0)
{}


/** 
 **  Unpack a single field of the bits32 id. One specifies the
 **  pointer to the field implementation to be used
 **  for decoding. The first_field_index is normally 0. It is
 **  non-zero when fields 0 to first_field_index -1 are missing
 **  from the compact 
 */
//-----------------------------------------------------------------
inline int IdDictFieldImplementation::unpack (Identifier id) const
//-----------------------------------------------------------------
{
    // Unpack field
    size_type index = id.extract(m_shift, m_mask);
    int field = index;
    if (m_decode_index) field = m_ored_field.get_value_at (index); 
    return (field);
}


/** 
 **  Unpack a single field index of the id. The value unpacked above
 **  may differ from the index if the values are enumerated rather
 **  than simply 0 to N-1.
 */
//-----------------------------------------------------------------
inline Identifier::size_type   IdDictFieldImplementation::unpackToIndex (Identifier id) const
//-----------------------------------------------------------------
{
    // Unpack field
    size_type index = id.extract(m_shift, m_mask);
    return (index);
}

//-----------------------------------------------------------------
inline void IdDictFieldImplementation::pack (int value, Identifier& id) const
//-----------------------------------------------------------------
{
    // Pack value into this field
    size_type index = (size_type)value;
    if (m_decode_index) index = m_ored_field.get_value_index (value); 
    id |= (index << m_shift); 
}

//-----------------------------------------------------------------
inline void IdDictFieldImplementation::reset (Identifier& id) const
//-----------------------------------------------------------------
{
    // Reset field to 0
    id &= m_zeroing_mask;
}

//-----------------------------------------------------------------
inline const Range::field&  IdDictFieldImplementation::field() const
//-----------------------------------------------------------------
{ return (m_field); } 

//-----------------------------------------------------------------
inline const Range::field&  IdDictFieldImplementation::ored_field() const
//-----------------------------------------------------------------
{ return (m_ored_field); } 

//-----------------------------------------------------------------
inline IdDictFieldImplementation::size_type     
IdDictFieldImplementation::bits() const
//-----------------------------------------------------------------
{ return (m_bits); } 

//-----------------------------------------------------------------
inline IdDictFieldImplementation::size_type     
IdDictFieldImplementation::bits_offset() const
//-----------------------------------------------------------------
{ return (m_bits_offset); } 

//-----------------------------------------------------------------
inline IdDictFieldImplementation::size_type     
IdDictFieldImplementation::mask() const
//-----------------------------------------------------------------
{ return (m_mask); } 

//-----------------------------------------------------------------
inline IdDictFieldImplementation::value_type    
IdDictFieldImplementation::zeroing_mask() const
//-----------------------------------------------------------------
{ return (m_zeroing_mask); } 

//-----------------------------------------------------------------
inline IdDictFieldImplementation::size_type     
IdDictFieldImplementation::shift() const
//-----------------------------------------------------------------
{ return (m_shift); } 

//-----------------------------------------------------------------
inline bool          IdDictFieldImplementation::decode_index() const
//-----------------------------------------------------------------
{ return (m_decode_index); }

//-----------------------------------------------------------------
inline void  IdDictFieldImplementation::initialize()
//-----------------------------------------------------------------
{
    // Initialize masks and shift 
    m_mask = (static_cast<size_type>(1) << m_bits) - 1;
    m_shift = IdDictFieldImplementation::NBITS - m_bits - m_bits_offset;

    // Shifting by NBITS is undefined behavior.
    if (m_shift < IdDictFieldImplementation::NBITS)
      m_zeroing_mask = (m_mask << m_shift) ^ IdDictFieldImplementation::ALL_BITS;
    else
      m_zeroing_mask = IdDictFieldImplementation::ALL_BITS;
}

//-----------------------------------------------------------------
inline void  IdDictFieldImplementation::set_field        (const Range::field&  field)
//-----------------------------------------------------------------
{
    m_field = field;
}


//-----------------------------------------------------------------
inline void  IdDictFieldImplementation::set_ored_field   (const Range::field& ored_field)
//-----------------------------------------------------------------
{
    // Set ored field and bits, and init
    m_ored_field = ored_field;
    m_bits       = m_ored_field.get_bits();
    initialize();
}

//-----------------------------------------------------------------
inline void  IdDictFieldImplementation::set_bits_offset  (size_type bits_offset)
//-----------------------------------------------------------------
{
    // Set offset and init
    m_bits_offset = bits_offset;
    initialize();
}

//-----------------------------------------------------------------
inline void  IdDictFieldImplementation::set_bits         (size_type bits, size_type bits_offset)
//-----------------------------------------------------------------
{
    // Set bits, offset and init
    m_bits        = bits;
    m_bits_offset = bits_offset;
    initialize();
}

//-----------------------------------------------------------------
inline void  IdDictFieldImplementation::set_decode_index (bool decode_index)
//-----------------------------------------------------------------
{
    m_decode_index = decode_index;
}

inline void  IdDictFieldImplementation::optimize         (void)
{
    m_ored_field.optimize(); // optimize for decoding
}

#endif // IDDICT_IDDICTFIELDIMPLEMENTATION_H
