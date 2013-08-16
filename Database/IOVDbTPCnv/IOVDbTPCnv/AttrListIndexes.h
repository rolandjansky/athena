/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBTPCNV_ATTRLISTINDEXES_H
# define IOVDBTPCNV_ATTRLISTINDEXES_H

/** @class AttrListIndexes 
 *
 * @brief Indexes to attribute name, type name and object value for
 * the persistent storage of an attribute list. The attribute and type
 * names are stored in individual vectors, but the object value is
 * stored in one of serveral type-specific vectors.
 *
 * @author RD Schaffer
 * $Id: AttrListIndexes.h,v 1.2 2008-10-13 15:12:05 schaffer Exp $
 **/

class AttrListIndexes 
{
public:
    AttrListIndexes();
    AttrListIndexes(unsigned short nameIndex, unsigned short typeIndex, unsigned short objIndex);
    AttrListIndexes(unsigned short typeIndex, unsigned short objIndex);
    AttrListIndexes(unsigned int combinedIndex);

    /// Index into attribute name vector
    unsigned short nameIndex()     const;

    /// Index into the type name vector
    unsigned short typeIndex()     const;

    /// Index into the type-specific value vectors - type index
    /// identifies which one
    unsigned short objIndex()     const;

    /// Access to the bit-packed value
    unsigned int   combinedIndex() const;

    /// Uninitialized index is NOT valid
    bool           isValid()      const;
        
    void setCombinedIndex(unsigned int combinedIndex);
    void setAttrIndex(unsigned short nameIndex);
    void setTypeIndex(unsigned short typeIndex);
    void setObjIndex(unsigned short index);

private:
    unsigned short m_typeIndex;
    unsigned short m_objIndex;
};



// inline definitions

inline 
AttrListIndexes::AttrListIndexes() 
        :
	m_typeIndex(0xFFFF),
        m_objIndex(0xFFFF)
{}

inline 
AttrListIndexes::AttrListIndexes(unsigned short nameIndex, unsigned short typeIndex, unsigned short objIndex)
        :
        // m_typeIndex: 10 bits nameIndex + 6 bits typeIndex:
	m_typeIndex( ((nameIndex & 0x03FF) << 6) | (typeIndex & 0x003F)),
        m_objIndex(objIndex)
{}


inline 
AttrListIndexes::AttrListIndexes(unsigned short typeIndex, unsigned short objIndex)
        :
	m_typeIndex(typeIndex & 0x003F),
        m_objIndex(objIndex)
{}

inline 
AttrListIndexes::AttrListIndexes(unsigned int combinedIndex)
{
    setCombinedIndex(combinedIndex);
}


inline
unsigned short 
AttrListIndexes::nameIndex()     const
{
    return ((m_typeIndex >> 6) & 0x03FF);
}

inline 
unsigned short
AttrListIndexes::typeIndex() const
{
    return (m_typeIndex & 0x003F);
}

inline 
unsigned short
AttrListIndexes::objIndex() const
{
    return (m_objIndex);
}

inline 
unsigned int   
AttrListIndexes::combinedIndex() const
{
    // Concatenate type and obj indexes
    unsigned int result = (m_typeIndex << 16) + m_objIndex;
    return (result);
}
        
inline 
bool
AttrListIndexes::isValid()      const
{
    // is valid if both type and obj indexes are not default values
    return (m_typeIndex != 0xFFFF && m_objIndex != 0xFFFF);
}


inline 
void
AttrListIndexes::setCombinedIndex(unsigned int combinedIndex)
{
    // Extract type and obj indexes
    m_typeIndex = combinedIndex >> 16;
    m_objIndex = combinedIndex & 0xFFFF;
}

inline
void 
AttrListIndexes::setAttrIndex(unsigned short index)
{
    // add in only the attribute index, keeping the type index
    m_typeIndex = ((index & 0x03FF) << 6) | (m_typeIndex & 0x003F);
}


inline 
void
AttrListIndexes::setTypeIndex(unsigned short index)
{
    // add in only the type index, keeping the attribute index
    m_typeIndex = (m_typeIndex & 0x03FF) | (index & 0x003F);
}

inline 
void
AttrListIndexes::setObjIndex(unsigned short index)
{
    m_objIndex = index;
}

#endif
